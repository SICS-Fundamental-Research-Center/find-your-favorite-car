#include "Heuristic.h"


#ifdef WIN32
#ifdef __cplusplus 
extern "C" {
#endif 
#endif

	//#include "data_utility.h"

#include "mem.h"
#include "qset.h"
#include "libqhull.h"
#include "qhull_a.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#if __MWERKS__ && __POWERPC__
#include <SIOUX.h>
#include <Files.h>
#include <console.h>
#include <Desk.h>

#elif __cplusplus
	extern "C" {
		int isatty(int);
	}

#elif _MSC_VER
#include <io.h>
#define isatty _isatty
	int _isatty(int);

#else
	int isatty(int);  /* returns 1 if stdin is a tty
					  if "Undefined symbol" this can be deleted along with call in main() */
#endif

#ifdef WIN32
#ifdef __cplusplus 
}
#endif
#endif

bool insert_pt(double* pt, int start, int dim)
{
	if (start == dim)
		return false;

	for (int i = start; i < dim; i++)
	{
		coordT *point_to_add;
		point_to_add = qh temp_malloc = (coordT*)qh_malloc(dim*sizeof(coordT));
		for (int j = 0; j < dim; j++)
			point_to_add[j] = pt[j];
		point_to_add[i] = 0;

		boolT isoutside;
		realT bestdist;

		qh_findbestfacet(point_to_add, !qh_ALL, &bestdist, &isoutside);
		if (isoutside)
		{
			qh_partitionpoint(point_to_add, qh facet_list);
			insert_pt(point_to_add, i + 1, dim);
		}
	}

	return true;
}

point_set_t* geoGreedy(point_set_t* p, double eps)
{
	int size = p->numberOfPoints;
	int dim = p->points[0]->dim;

	int orthNum = pow(2.0, dim) - 1;
	//bool* isSatisfied = new bool[size];

	coordT *points;
	points = qh temp_malloc = (coordT*)qh_malloc((orthNum * dim + 1)*(dim)*sizeof(coordT));
	int count = 1;

	for (int i = 0; i < dim; i++)
		points[i] = 0;
	
	point_set_t *S = alloc_point_set(size);
	int current = dim;
	
	int* maxIndex = new int[dim];

	for (int j = 0; j < dim; j++)
	{
		maxIndex[j] = 0;
		for (int i = 1; i < size; i++) {
			if (p->points[i]->coord[j] > p->points[maxIndex[j]]->coord[j])
				maxIndex[j] = i;
		}
		insertOrth(points, count, p->points[maxIndex[j]]);
		S->points[j] = p->points[maxIndex[j]];
		//dataIndex[maxIndex[j]]->pruned = true;
	}

	int curlong, totlong; /* used !qh_NOmem */
	int exitcode;
	boolT ismalloc = True;

	qh_init_A(stdin, stdout, stderr, 0, NULL);  /* sets qh qhull_command */
	exitcode = setjmp(qh errexit); /* simple statement for CRAY J916 */

	if (!exitcode) {

		qh_initflags(qh qhull_command);
		qh_init_B(points, count, dim, ismalloc);
		qh_qhull();
		qh_check_output();



		if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
			qh_check_points();
		exitcode = qh_ERRnone;

		facetT *facet;


		int nextIndex;
		double global_minCR;
		do
		{
			nextIndex = -1;
			global_minCR = 1;

			for (int i = 0; i < size; i++)
			{
				double minCR = 1;
				if (1)//!isSatisfied[i])
				{
					bool update = true;

					FORALLfacets{
						if (!isZero(facet->offset))
						{
					
							double newCR = -facet->offset / dot_prod(p->points[i],  facet->normal);
							if (minCR > newCR) {
								minCR = newCR;
							}
							if (global_minCR > newCR)
							{
								global_minCR = newCR;
								nextIndex = i;
							}
						}
					}

					//if (minCR > alpha || isZero(minCR - alpha))
						//isSatisfied[i] = true;
				}
			}

			if (eps < 1 - global_minCR && !isZero(1 - global_minCR - eps))
			{
				coordT *point;
				facetT *nearfacet;
				boolT isoutside;
				realT bestdist;
				pointT *furthest;

				point = (coordT*)qh_malloc(((dim)*sizeof(coordT)));
				for (int i = 0; i < dim; i++)
					point[i] = p->points[nextIndex]->coord[i];
				

				qh_partitionpoint(point, qh facet_list);

				insert_pt(point, 0, dim);


				qh facet_next = qh facet_list;      /* advance facet when processed */
				while ((furthest = qh_nextfurthest(&facet))) {
					qh num_outside--;  /* if ONLYmax, furthest may not be outside */
					if (!qh_addpoint(furthest, facet, qh ONLYmax))
						break;
				}

				if (1)
				{
					int numoutside;
					if (qh MERGEexact || (qh hull_dim > qh_DIMreduceBuild && qh PREmerge))
						qh_postmerge("First post-merge", qh premerge_centrum, qh premerge_cos, (qh POSTmerge ? False : qh TESTvneighbors));
					else if (!qh POSTmerge && qh TESTvneighbors)
						qh_postmerge("For testing vertex neighbors", qh premerge_centrum, qh premerge_cos, True);
					if (qh POSTmerge)
						qh_postmerge("For post-merging", qh postmerge_centrum, qh postmerge_cos, qh TESTvneighbors);
					if (qh visible_list == qh facet_list) { /* i.e., merging done */
						qh findbestnew = True;
						qh_partitionvisible(/*visible_list, newfacet_list*/ !qh_ALL, &numoutside);
						qh findbestnew = False;
						qh_deletevisible(/*qh visible_list*/);
						qh_resetlists(False, qh_RESETvisible /*qh visible_list newvertex_list newfacet_list */);
					}
				}

				S->points[current++] = p->points[nextIndex];
			}

		} while (eps < 1 - global_minCR);

		
	}
	qh NOerrexit = True;  /* no more setjmp */
#ifdef qh_NOmem
	qh_freeqhull(True);
#else
	qh_freeqhull(False);
	qh_memfreeshort(&curlong, &totlong);
	if (curlong || totlong)
		fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
			totlong, curlong);
#endif

	//delete[] isSatisfied;

	S->numberOfPoints = current;

	return S;
}

point_set_t* Greedy(point_set_t* p, double eps)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;

	int i, j;
	double *v = new double[D];

	point_set_t* sol = alloc_point_set(N);
	double worst, w;
	point_t* max;

	//bool* isSatisfied = new bool[N];

	double* b_value = new double[D];

	for (int j = 0; j < D; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < p->numberOfPoints; i++)
		{
			if (p->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = p->points[i]->coord[j];
				sol->points[j] = p->points[i];
			}
		}
	}
	delete[] b_value;
	

	// add more points
	int index = D;
	int nextIndex;
	worst = 1;
	while (worst > eps)
	{
		// Find a point with maximum regret
		max = p->points[0];
		worst = worstDirection(index, sol, p->points[0], v);
		for (i = 0; i < N; i++)
		{
			if (1)//!isSatisfied[i])
			{
				w = worstDirection(index, sol, p->points[i], v);

				//if (1- w > alpha)
					//isSatisfied[i] = true;

				if (worst < w)
				{
					worst = w;
					max = p->points[i];
					nextIndex = i;
				}
			}
		}
		//printf("%d\n", nextIndex);

		if (worst > eps)
			sol->points[index++] = max;
	}

	
	sol->numberOfPoints = index;

	delete[] v;
	//delete[] isSatisfied;

	return sol;
}

point_set_t* ImpGreedy(point_set_t* point_set, double eps)
{
	int dim = point_set->points[0]->dim;
	int N = point_set->numberOfPoints;

	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(N);

	double* b_value = new double[dim];

	for (int j = 0; j < dim; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			if (point_set->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = point_set->points[i]->coord[j];
				result->points[j] = point_set->points[i];
			}
		}
	}

	int count = dim;

	double threshold = 0.0000001;
	double worst, w;
	point_t* max;
	int* active = new int[point_set->numberOfPoints];

	point_t* lastRound_max;
	point_set_t* directions = alloc_point_set(N);
	double *rr = new double[N];

	max = NULL;
	worst = 0;
	for (int i = 0; i< N; i++)
	{
		active[i] = 1;

		directions->points[i] = alloc_point(dim);

		rr[i] = worstDirection(count, result, point_set->points[i], directions->points[i]->coord);

		w = rr[i];
		if (worst<w)
		{
			worst = w;
			max = point_set->points[i];
		}
		if (rr[i] <= 0.0 + threshold)
			active[i] = 0;
	}


	result->points[count++] = max;
	lastRound_max = max;

	while (worst > eps)
	{
		// Find a point with maximum regret
		max = NULL;
		worst = 0;

		for (int i = 0; i < N; i++)
		{
			if (worst > rr[i])
			{
				directions->points[i]->id = -2;
				continue;
			}
			else if (directions->points[i]->id == -2 || dot_prod(sub(point_set->points[i], lastRound_max), directions->points[i]) < rr[i])
			{
				rr[i] = worstDirection(count, result, point_set->points[i], directions->points[i]->coord);
				directions->points[i]->id = -1;
			}

			w = rr[i];
			if (worst<w)
			{
				worst = w;
				max = point_set->points[i];
			}
			if (w <= 0.0 + threshold)
				active[i] = 0;
		}

		// Add a point if regret > 0. Otherwise, stop.
		if (worst > eps)
		{
			result->points[count++] = max;
			lastRound_max = max;
		}
		else
		{
			break;
		}
	}
	//printf("skipCount: %i %f\n", skipCount, 1.0 * skipCount / totalCount);

	delete[] rr;
	delete[] active;
	delete[] b_value;

	release_point_set(directions, true);

	result->numberOfPoints = count;
	return result;
}
