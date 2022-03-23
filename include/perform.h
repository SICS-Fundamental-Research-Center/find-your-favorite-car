#ifndef PERFORM_H
#define PERFORM_H

#include "sphere.h"
#include "adaptive.h"
#include "cube.h"
#include "GeoGreedy.h"
#include "DMM.h"
#include "graphDP.h"
#include "sweepDP.h"
#include "biSearch.h"

#include "Heuristic.h"

#include "evaluate.h"
#include "read_write.h"

#ifndef WIN32
#include <sys/time.h>
#endif

void performSingleAlg(point_set_t* point_set, int k, double eps, stat_t*& stat, int algID);

void performAlgs(point_set* point_set, int k, stat_list_t* &stat_list, int numberOfAlg, int* alg);

void varyR();

void varyNorD();

void varyMRR();

//void varyK(config_t* config_v);

//void varyOther(config_t* config_v);

#endif
