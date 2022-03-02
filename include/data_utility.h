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

inter_point_t* alloc_inter_point(point_t* pt, linked_pt* i, linked_pt* j);

void release_inter_point(inter_point_t* &inter_point_v);

linked_pt_t* alloc_linked_pt(point_t* pt);

void release_linked_pt(linked_pt_t* &linked_pt_v);

// print informaiton
void print_point(point_t* point_v);
void print_point_set(point_set_t* point_set_v);

stat_t* alloc_stat(char* algName, double time, double mrr, int r);

void release_stat(stat_t* &stat_v);

stat_list_t* alloc_stat_list(int numberOfAlg);

void release_stat_list(stat_list_t* &stat_list_v);

stat_result_t* alloc_stat_result(int numberOfK);

void release_stat_result(stat_result_t* &stat_result_t);


#endif