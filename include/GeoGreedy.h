#ifndef _geogreedy_h_ 
#define _geogreedy_h_ 

#include "data_struct.h"
#include "operation.h"
#include "construct.h"
#include "lp.h"
#include "evaluate.h"
#include "search.h"

#include <fstream>
using namespace std;


double evaluateGeo(point_set_t *p, point_set_t* S, int &nextIndex);
double evaluateGeoRefl(point_set_t *p, point_set_t*S, int &nextIndex);
double evaluateGeoWithLP(point_set_t *p, point_set_t* S, int &nextIndex);

point_set_t* geoGreedy(int K, point_set_t *p);
point_set_t* geoGreedy_rebulid(int K, point_set_t *p);
point_set_t* geoGreedy_rebulid_refl(int K, point_set_t *p);
point_set_t* geoGreedy_refl(int K, point_set_t *p);
point_set_t* geoGreedy_rebulid_highD(int K, point_set_t *p);

void geoAddPoint(point_set* &S, int currentK, point_set_t *p);
void geoAddPoint_rebuild(point_set* &S, int currentK, point_set_t *p);

point_set_t* search_faceGeo(point_set_t* p, point_t* query);
//point_set_t* search_face_weightGeo(point_set_t* point_set_v, point_t* query, double* &weight);

#endif