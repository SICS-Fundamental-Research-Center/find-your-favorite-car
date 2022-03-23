//#include "stdafx.h"

#include "data_utility.h"


/*
 *	Allocate memory for a point in dim-dimensional space
 */
point_t* alloc_point( int dim)
{
	point_t* point_v;

	point_v = ( point_t*)malloc( sizeof( point_t));
	memset( point_v, 0, sizeof( point_t));

	point_v->dim = dim;

	point_v->coord = ( COORD_TYPE*)malloc( dim * sizeof( COORD_TYPE));
	memset( point_v->coord, 0, dim * sizeof( COORD_TYPE));

	point_v->id = -1;

	return point_v;
}

/*
 *	Allocate memory for a point in dim-dimensional space with id
 */
point_t* alloc_point(int dim, int id)
{
	point_t* point_v = alloc_point(dim);

	point_v->id = id;

	return point_v;
}

/*
 *	Generate a random point in dim-dimensional space
 */
point_t* rand_point(int dim)
{
	point_t* point_v = alloc_point(dim);

	for (int i = 0; i < dim; i++)
		point_v->coord[i] = rand() * 1.0 / RAND_MAX;

	//print_point(point_v);

	return point_v;
}

/*
 *	Release memory for a point
 */
void release_point( point_t* &point_v)
{
	if(point_v == NULL)
		return;
	
	if(point_v->coord != NULL)
	{
		free( point_v->coord);
		point_v->coord = NULL;
	}

	free( point_v);
	point_v = NULL;
}

/*
 *	Allocate memory for a hyperplane in dim-dimensional space
 */
hyperplane_t* alloc_hyperplane(point_t* normal, double offset)
{
	hyperplane_t* hyperplane_v;

	hyperplane_v = (hyperplane_t*)malloc(sizeof(hyperplane_t));
	memset(hyperplane_v, 0, sizeof(hyperplane_t));

	hyperplane_v->normal = normal;
	hyperplane_v->offset = offset;

	return hyperplane_v;
}

/*
 *	Release memory for a point in dim-dimensional space
 */
void release_hyperplane(hyperplane_t* &hyperplane_v)
{
	if (hyperplane_v == NULL)
		return;

	free(hyperplane_v);
	hyperplane_v = NULL;
}

/*
 *	Allocate memory for numberOfPoints number of points
 */
point_set_t* alloc_point_set(int numberOfPoints)
{
	point_set_t* point_set_v;

	point_set_v = ( point_set_t*)malloc( sizeof( point_set_t));
	memset( point_set_v, 0, sizeof( point_set_t));

	point_set_v->numberOfPoints = numberOfPoints;

	point_set_v->points = (point_t **) malloc(numberOfPoints * sizeof(point_t*));
	memset(point_set_v->points, 0, numberOfPoints * sizeof(point_t*));

	return point_set_v;
}

/*
 *	Release memory for numberOfPoints number of points
 */
void release_point_set(point_set_t* &point_set_v, bool clear)
{
	if(point_set_v == NULL)
		return;

	if(point_set_v->points != NULL)
	{
		if(clear)
		{
			for(int i = 0; i < point_set_v->numberOfPoints; i++)
				release_point( point_set_v->points[i]);
		}

		free(point_set_v->points);
		point_set_v->points = NULL;
	}

	free(point_set_v);
	point_set_v = NULL;
}


inter_point_t* alloc_inter_point(point_t* pt, linked_pt* i, linked_pt* j)
{
	inter_point_t* inter_point_v;

	inter_point_v = ( inter_point_t*)malloc( sizeof( inter_point_t));
	memset( inter_point_v, 0, sizeof( inter_point_t));

	inter_point_v->linked_pt_i = i;
	inter_point_v->linked_pt_j = j;
	inter_point_v->pt = pt;

	return inter_point_v;
}

void release_inter_point(inter_point_t* &inter_point_v)
{
	if(inter_point_v == NULL)
		return;

	free(inter_point_v);
	inter_point_v = NULL;
}

linked_pt_t* alloc_linked_pt(point_t* pt)
{
	linked_pt_t* linked_pt_v;

	linked_pt_v = ( linked_pt_t*)malloc( sizeof( linked_pt_t));
	memset( linked_pt_v, 0, sizeof( linked_pt_t));

	linked_pt_v->pt = pt;

	return linked_pt_v;
}

void release_linked_pt(linked_pt_t* &linked_pt_v)
{
	if(linked_pt_v == NULL)
		return;

	free(linked_pt_v);
	linked_pt_v = NULL;
}

/*
*	For degug purpose, print the coordinates for a given point
*/
void print_point(point_t* point_v)
{
	//printf("printing point:\n");
	//printf("%d ", point_v->id);
	for (int i = 0; i < point_v->dim; i++)
		printf("%f ", point_v->coord[i]);
	printf("\n");
}

/*
 *	For debug purose, print the contents of a given point set
 */
void print_point_set(point_set_t* point_set_v)
{
	for(int i = 0; i < point_set_v->numberOfPoints; i++)
		//printf("%i ", point_set_v->points[i]->id);
		print_point(point_set_v->points[i]);

	printf("\n");
}


stat_t* alloc_stat(char* algName, double time, double mrr, int r)
{
	stat_t* stat_v;

	stat_v = ( stat_t*)malloc( sizeof( stat_t));
	memset( stat_v, 0, sizeof( stat_t));

	//stat_v->algName = (char*)malloc( sizeof(char) * MAX_FILENAME_LENG);
	//memset( stat_v->algName, 0, sizeof(char) * MAX_FILENAME_LENG);

	stat_v->algName = algName;
	stat_v->mrr = mrr;
	stat_v->time = time;
	stat_v->r = r;

	return stat_v;
}

void release_stat(stat_t* &stat_v)
{
	//free(stat_v->algName);

	free(stat_v);
	stat_v = NULL;
}

stat_list_t* alloc_stat_list(int numberOfAlg)
{
	stat_list_t* stat_list_v;

	stat_list_v = ( stat_list_t*)malloc( sizeof( stat_list_t));
	memset( stat_list_v, 0, sizeof( stat_list_t));

	stat_list_v->numberOfalg = numberOfAlg;

	stat_list_v->alg_list = (stat_t **) malloc(numberOfAlg * sizeof(stat_t*));
	memset(stat_list_v->alg_list, 0, numberOfAlg * sizeof(stat_t*));

	return stat_list_v;
}

void release_stat_list(stat_list_t* &stat_list_v)
{
	if(stat_list_v->alg_list != NULL)
	{
		for(int i = 0; i < stat_list_v->numberOfalg; i++)
			release_stat( stat_list_v->alg_list[i]);

		free(stat_list_v->alg_list);
		stat_list_v->alg_list = NULL;
	}

	free(stat_list_v);
	stat_list_v = NULL;
}

stat_result_t* alloc_stat_result(int numberOfK)
{
	stat_result_t* stat_result_v;

	stat_result_v = ( stat_result_t*)malloc( sizeof( stat_result_t));
	memset( stat_result_v, 0, sizeof( stat_result_t));

	stat_result_v->numberOfQueries = numberOfK;

	stat_result_v->stats = (stat_list_t **) malloc(numberOfK * sizeof(stat_list_t*));
	memset(stat_result_v->stats, 0, numberOfK * sizeof(stat_list_t*));

	return stat_result_v;
}

void release_stat_result(stat_result_t* &stat_result)
{
	if(stat_result->stats != NULL)
	{
		for(int i = 0; i < stat_result->numberOfQueries; i++)
			release_stat_list( stat_result->stats[i]);

		free(stat_result->stats);
		stat_result->stats = NULL;
	}
	
	free(stat_result);
	stat_result = NULL;

}