#ifndef SEARCH_H
#define SEARCH_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"

void basisComputation(point_set_t*& X, point_t*& NN, point_t* p, point_t* x);
point_set_t* search_basis(point_set_t* point_set_v, point_t* query);


#endif