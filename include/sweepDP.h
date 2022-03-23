#ifndef SWEEPDP_H
#define SWEEPDP_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include "evaluate.h"
#include "read_write.h"

#include <vector>
#include <algorithm>
#include <queue>

point_set_t* sweepDP(point_set_t* P, int k);

#endif