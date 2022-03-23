#ifndef CUBE_H
#define CUBE_H

#include "data_struct.h"
#include "data_utility.h"

point_set_t* cube(int K, point_set_t *p);
int cubealgorithm(int D, int N, int K, point_set_t *p, int L, int t, point_set_t *c, point_set_t *answer);
#endif