#ifndef SPHERE_H
#define SPHERE_H

#include "data_struct.h"
#include "data_utility.h"
#include "construct.h"
#include "search.h"
#include "operation.h"

#include "GeoGreedy.h"

#include "lp.h"

point_set_t* sphere(point_set_t* point_set, int k);

point_set_t* sphereNothing(point_set_t* point_set, int k);

point_set_t* sphereWSLP(point_set_t* point_set, int k);

point_set_t* sphereWSImpLP(point_set_t* point_set, int k);

point_set_t* sphereWSGeoRebuild(point_set_t* point_set, int k);

point_set_t* sphereWSGeoeIncremental(point_set_t* point_set, int k);

#endif