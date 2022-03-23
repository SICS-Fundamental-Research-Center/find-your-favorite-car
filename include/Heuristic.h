#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "data_struct.h"
#include "operation.h"
#include "construct.h"
#include "lp.h"
#include "evaluate.h"
#include "search.h"

point_set_t* geoGreedy(point_set_t* p, double eps);

point_set_t* Greedy(point_set_t* p, double eps);

point_set_t* ImpGreedy(point_set_t* point_set, double eps);

#endif
