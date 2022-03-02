#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include "read_write.h"

#include "lp.h"
#include <set>
#include <vector>
#include <algorithm>

point_set_t* adaptiveLPfast(int K, point_set_t *p, bool selectDim);

point_set_t* ImpGreedy(int K, point_set_t *p);

point_set_t* Greedy(int K, point_set_t *p);

point_set_t* Greedy_2RMS(int K, point_set_t *p);

point_set_t* IncGreedy(int k, point_set_t* point_set, point_set_t* selected);

#endif