#ifndef EVALUATE_H
#define EVALUATE_H

#include "data_struct.h"
#include "data_utility.h"
#include "lp.h"
#include "operation.h"
#include "read_write.h"
#include <fstream>
#include <algorithm>
using namespace std;

double evaluateLP(point_set_t *p, point_set_t* S, int VERBOSE);
double evaluateLP_pruned(point_set_t *p, point_set_t* S, int VERBOSE);
double evaluateLP(point_set_t *p, point_set_t* S, int VERBOSE, int& nextIndex);
double evaluateKRMS(point_set_t *p, point_set_t* S, int K);
double evaluateCES(point_set_t *p, point_set_t* S, int VERBOSE);
double evaluateCVX(point_set_t *p, point_set_t *S, int VERBOSE);
double evaluateCAV(point_set_t *p, point_set_t* S, int VERBOSE);
double evaluateMUF(point_set_t *p, point_set_t *S, int VERBOSE);

#endif