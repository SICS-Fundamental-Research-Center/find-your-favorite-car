#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include<math.h>
#include<float.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <vector>
#include <iostream>
#include <iterator>

#define COORD_TYPE			double
#define DIST_TYPE			double
#define PI					3.1415926
#define INF					100000000
#define	MAX_FILENAME_LENG	256

#define DEBUG 1


#define CONFIG_FILE	"rtree.config"
#define SAVE_RTREE_FILE "save_rtree_file"

#define FALSE    	0
#define TRUE     	1

#define RANGE_SEARCH 0
#define kNN_SEARCH 1
#define CHOICE kNN_SEARCH

#define ASC_NUM  	48
#define NO_ID	 	-1
#define FOUND		1
#define NOT_FOUND 	0

#define ROOT  0   
#define LEAF  1
#define NODE  2

//#define INFINITY  FLT_MAX
//#define INFINITY  1E+37 //FLT_MAX
#define UNDEFINED -3  // for id of entries in PR

//#define R_FLOAT
//#define R_TYPE int
#define R_TYPE double


#define RAMDOM_C 1
#define RAMDOM_P 3
#define HEURISTIC 2

// data structure for storing points.
typedef struct point
{
	int			dim;
	COORD_TYPE*	coord;

	int			id;
	
}	point_t;

// data structure for storing point set.
typedef struct point_set
{
	int numberOfPoints;
	point_t **points;
}	point_set_t;

// data structure for storing hyperplane.
typedef struct hyperplane
{
	point_t*	normal;
	double		offset;

}	hyperplane_t;

// data structure for storing hyperplane set.
typedef struct hyperplane_set
{
	int numberOfHyperplanes;
	hyperplane_t **hyperplanes;

}	hyperplane_set_t;


// R-tree related data structures
typedef struct rtree_info_s
{
	int m, M, dim, reinsert_p, no_histogram;
	int extra_level;
} rtree_info;

// R-tree related data structures
typedef struct node {
	R_TYPE *a;
	R_TYPE *b;
	int id;
	int attribute;
	int vacancy;
	struct node *parent;
	struct node **ptr;
	int dim;

}   node_type;

// R-tree related data structures
struct nodeCmp
{
	bool operator()(const node_type* lhs, const node_type* rhs) const
	{
		double dist1 = 0, dist2 = 0;
		for (int i = 0; i < lhs->dim; i++)
		{
			dist1 += pow(1 - lhs->b[i], 2);
			dist2 += pow(1 - rhs->b[i], 2);
		}

		return dist1 > dist2;
	}
};

// R-tree related data structures
typedef struct NN {
	double dist;
	int oid;
	struct node *pointer;
	int level;
	struct NN *next;
} NN_type;

// R-tree related data structures
typedef struct BranchArray {
	double min;
	node_type *node;
} ABL;

// R-tree related data structures
typedef struct config_rt {
	int dim;
	int m;
	int M;
	int reinsert_p;
	int no_histogram;
	//char nodefile[FILENAME_MAX];
	//char rootfile[FILENAME_MAX];
	char queryfile[FILENAME_MAX];
	char positionfile[FILENAME_MAX];
	char save_tree_file[FILENAME_MAX];
}   config_type;

// R-tree related data structures
struct setNode_s
{
	int noOfNode;
	node_type **elt;
	int *level;
};

typedef struct linked_pt
{
	point_t* pt;

	linked_pt* next;
	linked_pt* prev;
} linked_pt_t;

typedef struct inter_point
{
	point_t* pt;
	linked_pt_t* linked_pt_i;
	linked_pt_t* linked_pt_j;

} inter_point_t;

// date structure for computing catersian product
typedef std::vector<double> Vi;
typedef std::vector<Vi> Vvi;

typedef struct setNode_s setNode;


// data structure for configuration file
typedef struct stat_s
{	
	char*	algName;
	double 	time;
	double	mrr;
	int		r;
	
}	stat_t;

// data structure for configuration file
typedef struct stat_list
{	
	int			numberOfalg;
	stat_t** 	alg_list;
	
}	stat_list_t;

typedef struct stat_result
{	
	int				numberOfQueries;
	stat_list_t** 	stats;
	
}	stat_result_t;

#endif