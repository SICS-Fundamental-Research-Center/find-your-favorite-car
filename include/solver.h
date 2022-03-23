/****************************************************************
*								*
* Polynomial root finder (polynoms up to degree 4)		*
* AUTHOR: Jochen SCHARZE (See 'Cubic & Quartic Roots' in	*
*			  'Graphics Gems 1', AP)		*
*								*
****************************************************************/


#ifndef SOLVER_H
#define SOLVER_H


#include<math.h>
#include<float.h>
#define EQN_EPS 1e-9

inline double cbrt(double x) {

  if (fabs(x) < DBL_EPSILON) return 0.0;

  if (x > 0.0) return pow(x, 1.0/3.0);

  return -pow(-x, 1.0/3.0);

}

int isZero(double x);
int solveLinear(double c[2], double s[1]);
int solveQuadric(double c[3], double s[2]);
int solveCubic(double c[4], double s[3]);
int solveQuartic(double c[5], double s[4]);


#endif