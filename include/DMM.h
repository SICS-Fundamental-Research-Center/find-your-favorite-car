#ifndef DMM_H
#define DMM_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include <algorithm>

point_set_t* DMM(point_set_t* point_set, int k);

point_set_t* DMM(point_set_t* point_set, double eps);


point_set_t* DMM_Greedy(point_set_t* point, int k);


point_set_t* DMM(point_set_t* point_set, int k, int kRank);

#endif
