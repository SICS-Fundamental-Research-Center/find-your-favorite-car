#ifndef GRAPHDP_H
#define GRAPHDP_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include "read_write.h"
#include "evaluate.h"
#include "GeoGreedy.h"

#include <algorithm>
#include <vector>

point_set_t* graphDP(point_set_t* P, int k);

#endif