#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H


#include "data_struct.h"

// allocate/release memory for points/hyperplanes 
point_t* alloc_point(int dim);
point_t* alloc_point(int dim, int id);
void release_point( point_t* &point_v);
point_t* rand_point(int dim);
point_set_t* alloc_point_set(int numberOfPoints);
void release_point_set(point_set_t* &point_set_v, bool clear);
hyperplane_t* alloc_hyperplane(point_t* normal, double offset);
void release_hyperplane(hyperplane_t* &hyperplane_v);

// print informaiton
void print_point(point_t* point_v);
void print_point_set(point_set_t* point_set_v);

#endif