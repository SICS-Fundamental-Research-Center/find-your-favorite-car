#include "graphDP.h"

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

void computeConv(vector<point_t*> P, vector<int>& idxs)
{
	idxs.clear();

	int n = P.size();
	int dim = P[0]->dim;


	int curlong, totlong; /* used !qh_NOmem */
	int exitcode;
	boolT ismalloc = True;

	coordT *points;
	//temp_points = new coordT[(orthNum * S->numberOfPoints + 1)*(dim)];
	points = qh temp_malloc = (coordT*)qh_malloc( (n+dim+1)*(dim)*sizeof(coordT));



	for (int j = 0; j < dim; j++)
		points[j] = 0;
	int count = 1;

	for(int i = 0; i < dim; i++)
	{
		double max = 0;
		for(int j = 0; j < P.size(); j++)
		{
			if(max < P[j]->coord[i])
				max = P[j]->coord[i];
		}

		for (int j = 0; j < dim; j++)
			points[count*dim + j] = 0;
		points[count*dim + i] = max;
		
		count++;
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < dim; j++)
			points[count*dim + j] = P[i]->coord[j];
		count++;
	}


	//printf("# of points: %d\n", count);
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

		vertexT *vertex;
		

		FORALLvertices
		{
			int index;
			getPoint(P, vertex->point, index);
			
			//for(int i = 0; i < dim; i++)
			//	printf("%\lf ", vertex->point[i]);
			//printf("\n");

			if(index >= 0)
				idxs.push_back(index);
		}

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
}

struct skylineCmp
{
	bool operator()(const point_t* lhs, const point_t* rhs) const
	{
		return lhs->coord[0] < rhs->coord[0];
	}
};

double computeWeight(vector<point_t*> skyline, int idx_i, int idx_j, vector<double> angle, vector<int> conv_idxs)
{
	if(idx_i == 0)
		return skyline[1]->coord[1] - skyline[idx_j]->coord[1];
	else if (idx_j == skyline.size()-1)
		return skyline[skyline.size() - 1]->coord[0] - skyline[idx_i]->coord[0];


	int a, b;
	//a = conv_idxs[std::lower_bound(conv_idxs.begin(), conv_idxs.end(), idx_i) - conv_idxs.begin()];
	//b = conv_idxs[std::lower_bound(conv_idxs.begin(), conv_idxs.end(), idx_j) - conv_idxs.begin()];

	a = idx_i;
	b = idx_j;

	double alpha = acos((skyline[b]->coord[0] - skyline[a]->coord[0])/ calc_len(sub(skyline[b], skyline[a])));


	int idx = std::lower_bound(angle.begin(), angle.end(), alpha) - angle.begin();
	int k = conv_idxs[idx];

	
	if(a < k && k < b)
	{
		point_t* u = alloc_point(2);
		u->coord[0] = tan(alpha);
		u->coord[1] = 1;

		
		point_t* t_k = skyline[k];

		double w1 = (dot_prod(u, t_k) - dot_prod(u, skyline[a])) / dot_prod(u, t_k);
		double w2 = (dot_prod(u, t_k) - dot_prod(u, skyline[b])) / dot_prod(u, t_k);

		return w1 < w2 ? w1: w2;
	}

	return 0;
}

double computeWeight_LP(point_set_t* skyline, vector<point_t*> sortedSkyline, int idx_i, int idx_j)
{
	if(idx_i > idx_j)
	{
		printf("wrong index\n");
		exit(0);
	}

	point_set_t* S = alloc_point_set(sortedSkyline.size() - (idx_j - idx_i - 1) - 2);
	int count = 0;
	for(int i = 1; i <= idx_i; i++)
		S->points[count++] = sortedSkyline[i];
	//print_point_set(S);
	for(int i = idx_j; i < sortedSkyline.size() - 1; i++)
		S->points[count++] = sortedSkyline[i];

	//for(int i = 0; i < sortedSkyline.size(); i++)
	//	printf("%d ", sortedSkyline[i]->id);
	//printf("\n");
	//print_point_set(S);

	int dummy = 0;

	double w = evaluateLP(skyline, S, dummy);
	release_point_set(S, false);
	//printf("2: %lf\n", w);

	return w;
}


double computeWeight_Geo(point_set_t* skyline, vector<point_t*> sortedSkyline, int idx_i, int idx_j)
{
	if(idx_i > idx_j)
	{
		printf("wrong index\n");
		exit(0);
	}

	point_set_t* S = alloc_point_set(sortedSkyline.size() - (idx_j - idx_i - 1) - 2);
	int count = 0;
	for(int i = 1; i <= idx_i; i++)
		S->points[count++] = sortedSkyline[i];
	//print_point_set(S);
	for(int i = idx_j; i < sortedSkyline.size() - 1; i++)
		S->points[count++] = sortedSkyline[i];

	//for(int i = 0; i < sortedSkyline.size(); i++)
	//	printf("%d ", sortedSkyline[i]->id);
	//printf("\n");
	//print_point_set(S);

	int dummy = 0;

	double w = evaluateGeo(skyline, S, dummy);
	release_point_set(S, false);
	//printf("mrr: %lf\n\n", w);

	return w;
}


double computeWeight_Geo_tmp(point_set_t* skyline, vector<point_t*> sortedSkyline, int idx_i, int idx_j)
{

	if(idx_i > idx_j)
	{
		printf("wrong index\n");
		exit(0);
	}

	if(idx_j - idx_i == 1)
		return 0;

	point_t* p1;
	point_t* p2;

	if(idx_i == 0)
	{
		p1 = alloc_point(2);
		p1->coord[0] = 0;
		p1->coord[1] = sortedSkyline[idx_j]->coord[1];
	}
	else
		p1 = sortedSkyline[idx_i];

	if(idx_j != sortedSkyline.size() - 1)
	{
		p2 = sortedSkyline[idx_j];	


		double a = (p2->coord[1] -  p1->coord[1])/ (p2->coord[0] - p1->coord[0]);
		double b = p2->coord[1] - a * p2->coord[0];

		double CR = 1;
		for(int i = idx_i + 1; i < idx_j; i++)
		{
			point_t* q = sortedSkyline[i];

			double newCR = b/ (q->coord[1] - q->coord[0]* a);
			if(newCR < CR)
				CR = newCR;
		}

		return 1- CR;
	}
	else
	{
		double CR = 1;
		for(int i = idx_i + 1; i < idx_j; i++)
		{
			point_t* q = sortedSkyline[i];

			double newCR = p1->coord[0]/q->coord[0];
			if(newCR < CR)
				CR = newCR;
		}

		return 1- CR;

	}

	//point_set_t* S = alloc_point_set(2);
	//S->points[0] = p1;
	//S->points[1] = p2;

	//point_set_t* P = alloc_point_set(idx_j - idx_i - 1);
	//for(int i = idx_i + 1; i < idx_j; i++)
	//	P->points[i - (idx_i + 1)] = sortedSkyline[i];

	//int dummy = 0;

	//double w = evaluateGeo(P, S, dummy);
	////printf("mrr: %lf\n\n", w);

	//return w;
}

point_set_t* graphDP(point_set_t* P, int k)
{
	point_set_t* skyline;
	skyline = skyline_point(P);

	vector<point_t*> sortedSkyline;
	for(int i = 0; i < skyline->numberOfPoints; i++)
		sortedSkyline.push_back(skyline->points[i]);
	
	//vector<int> idxs;
	//computeConv(sortedSkyline, idxs);
	//for(int i = 0; i < idxs.size(); i++)
	//	printf("%d ", sortedSkyline[idxs[i]]->id);
	//printf("\n");

	for(int i = 0; i < 2; i++)
	{
		double max = 0;
		for(int j = 0; j < sortedSkyline.size(); j++)
		{
			if(sortedSkyline[j]->coord[i] > max)
				max = sortedSkyline[j]->coord[i];
		}

		point_t* b_i = alloc_point(2);
		b_i->coord[0] = 0;
		b_i->coord[1] = 0;

		b_i->coord[i]= max;
		sortedSkyline.push_back(b_i);
	}

	std::sort(sortedSkyline.begin(), sortedSkyline.end(), skylineCmp());


	//computeWeight(sortedSkyline, 2, 10);
	//vector<int> conv_idxs;
	//computeConv(sortedSkyline, conv_idxs);
	//std::sort(conv_idxs.begin(), conv_idxs.end());

	//vector<double> angle;
	//for(int i = 0; i < conv_idxs.size()-1; i++)
	//{
	//	double v = acos((sortedSkyline[conv_idxs[i+1]]->coord[0] - sortedSkyline[conv_idxs[i]]->coord[0])/ calc_len(sub(sortedSkyline[conv_idxs[i+1]], sortedSkyline[conv_idxs[i]])));
	//	angle.push_back(v);
	//}


	double** DP = new double*[sortedSkyline.size()];
	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		DP[i] = new double[k+2];
	}

	double** weight = new double*[sortedSkyline.size()];
	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		weight[i] = new double[sortedSkyline.size()];
	}
	for(int i = 0; i <sortedSkyline.size(); i++)
	{
		for(int j = 0; j < sortedSkyline.size(); j++)
			weight[i][j] = -1;
	}

	int** prev = new int*[sortedSkyline.size()];
	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		prev[i] = new int[k+2];
	}


	for(int i = 1; i <= sortedSkyline.size() - 2;i++)
	{
		//weight[0][i] = computeWeight(sortedSkyline, i, j, angle, conv_idxs);

		weight[i][sortedSkyline.size() - 1] = computeWeight_Geo_tmp(skyline, sortedSkyline, i, sortedSkyline.size() - 1);

		//DP[i][1] = weight[0][i] > weight[i][sortedSkyline.size() - 1]?weight[0][i]: weight[i][sortedSkyline.size() - 1];
		DP[i][1] = weight[i][sortedSkyline.size() - 1];
		//printf("DP[%d][1]: %lf\n", i, DP[i][1]);
	}

	for(int len = 2; len <= k+1; len++)
	{
		for(int i = 0; i <= sortedSkyline.size() - 2 ; i++)
		{
			int low = i+1, high = sortedSkyline.size() - 2;

			double min = 1;
			int min_idx = low;

			for(int j = low; j <= high; j++)
			{
				if(weight[i][j] < 0)
					weight[i][j] = computeWeight_Geo_tmp(skyline, sortedSkyline, i, j);

				double newMax = weight[i][j] > DP[j][len - 1]?  weight[i][j]: DP[j][len - 1];
				if(min > newMax)
				{
					min = newMax;
					min_idx = j;
				}

				//if(weight[i][j] > DP[j][len - 1])
					//break;
			}


			DP[i][len] = min;
			prev[i][len] = min_idx;

			//while(low <= high)
			//{
			//	//printf("%d, %d, %d\n", i, low, high);
			//	if(low == high)
			//	{
			//		weight[i][low] = computeWeight_LP(skyline, sortedSkyline, i, low);
			//		DP[i][len] = weight[i][low] > DP[low][len-1]? weight[i][low]: DP[low][len-1];
			//		break;
			//	}

			//	int mid = (low + high) / 2;
			//	if(weight[i][mid] < 0)
			//		weight[i][mid] = computeWeight_LP(skyline, sortedSkyline, i, mid);
			//	if(weight[i][mid] >= DP[mid][len - 1] || isZero(weight[i][mid] -DP[mid][len - 1]) )
			//		high = mid;
			//	else
			//		low = mid + 1;
			//}		

		}
		//if(len == 2)
		//	printf("DP[32][2]: %lf\n", DP[33][2]);
		//if(len == 3)
		//{
		//	printf("DP[8][3]: %lf\n", DP[9][3]);
		//	//printf("w[8][32]: %lf\n", weight[9][33]);
		//}
		//if(len == 4)
		//{
		//	printf("DP[2][4]: %lf\n", DP[3][4]);
		//	//printf("w[8][32]: %lf\n", weight[9][33]);
		//}
		//if(len == 5)
		//{
		//	printf("DP[0][5]: %lf\n", DP[0][5]);
		//	//printf("w[8][32]: %lf\n", weight[9][33]);
		//}

	}


	double rr = DP[0][k+1];
	//printf("rr: %lf\n", rr);
	
	int count = 0;
	point_set_t* S = alloc_point_set(k);
	int node = 0;
	for(int i = k+1; i>1; i--)
	{
		//printf("%d ", prev[node][i]);
		node = prev[node][i];
		S->points[count++] = sortedSkyline[node];
	}

	S->numberOfPoints = count;
	//for(int i = count; i < k; i++)
		//S->points[i] = S->points[0];

	////////////////////////////////////check correctness
	//point_set_t* S = alloc_point_set(2);
	//rr = 1;
	//for(int i = 1; i < sortedSkyline.size() - 1; i++)
	//{
	//	for(int j = i + 1; j < sortedSkyline.size() - 1; j++)
	//	{
	//		S->points[0] = sortedSkyline[i];
	//		S->points[1] = sortedSkyline[j];

	//		double newRR = evaluateLP(skyline, S, 0);
	//		if(rr > newRR)
	//		{
	//			rr = newRR;
	//			printf("i: %d, j: %d\n", i, j);
	//		}
	//	}
	//}
	//printf("bruteforce min rr: %lf\n", rr);


	//for(int i = 1; i < sortedSkyline.size()-1; i++)
	//{
	//	printf("DP[%i][2]: %lf\n", i, DP[i][2]);
	//}

	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		delete [] DP[i];
	}
	delete[] DP;

	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		delete [] weight[i];
	}
	delete [] weight;

	for(int i = 0; i < sortedSkyline.size(); i++)
	{
		delete [] prev[i];
	}
	delete[] prev;


	release_point_set(skyline, false);
	return S;
}