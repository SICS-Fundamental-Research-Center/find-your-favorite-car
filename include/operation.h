#ifndef OPERATION_H
#define OPERATION_H

#include "data_struct.h"
#include "data_utility.h"
#include <cmath>
#include <vector>

using namespace std;

#include "QuadProg++.hh"
using namespace QuadProgPP;

#define EQN_EPS 1e-9

// Basic operaitons
int isZero(double x);
DIST_TYPE calc_l1_dist( point_t* point_v1, point_t* point_v2);
DIST_TYPE calc_len(point_t* point_v);
point_t* copy(point_t * point_v2);
double dot_prod(point_t* point_v1, point_t* point_v2);
double dot_prod(point_t* point_v1, double* v);
double dot_prod(double* v1, double* v2, int dim);
point_t* sub(point_t* point_v1, point_t* point_v2);
point_t* add(point_t* point_v1, point_t* point_v2);
point_t* scale(double c, point_t* point_v);
double compute_intersection_len(hyperplane_t *hp, point_t* s);
int guassRank(vector< point_t* > P);
bool linearInd(point_t* p, point_t* q);

point_t* find_nearest_inCH(point_set_t* facet, point_t* query, double &dist, double* &weight);
bool isViolated(point_t* normal_q, point_t* normal_p, point_t* e);
point_t* projectPointsOntoAffineSpace(point_set_t* space, point_t* p);
vector<double> gaussNtimesD(vector< vector<double> > A);
DIST_TYPE calc_dist( point_t* point_v1, point_t* point_v2);
void insertOrth(double* &points, int &count, point_t* v);
point_t* getPoint(point_set_t* p, double* point);
point_t* getPoint(vector<point_t*> P, double* p, int &index);
point_t* maxPoint(point_set_t* p, double *v);
float rand_f( float min_v, float max_v);
point_t* findFavorite(point_set_t* p, point_t* u);


/*
*	build the input for catersian product (in the form of vector of vector of double)
*  t is the number of hyperplanes in each family that are used to divide the facet of hypercube (t >= 0)
*  e.g. when dim = 3, t = 0, means no division and each facet has 1 hypercube
*		when dim = 3, t = 1, means each 2-d facet is divided by 2 hyperplan, one for each dimension, resulting in 4 hypercubes on each facet
*/
Vvi build_input(int t, int dim);

/*
*  codes adapted from stack over flow.
*  use recursion to compue the catersian product in the format of vector of vector of double.
*/
void cart_product(
	Vvi& rvvi,  // final result
	Vi&  rvi,   // current result 
	Vvi::const_iterator me, // current input
	Vvi::const_iterator end); // final input;

/*
*	For debug purpose, output the content of a vector of double
*/
std::ostream& operator<<(std::ostream& os, const Vi& vi);

/*
*	For debug purpose, output the content of a vector of vector of double
*/
std::ostream& operator<<(std::ostream& os, const Vvi& vvi);


#endif