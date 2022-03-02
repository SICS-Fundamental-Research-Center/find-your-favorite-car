/*
 * cubeAlgorithm.h
 *
 *  Created on: 16Mar.,2017
 *      Author: jianzhongq
 */

#ifndef CUBEALGORITHM_H_
#define CUBEALGORITHM_H_

 #include "algorithm.h"

class CCUBEAlgorithm: public CAlgorithm {
public:
	CCUBEAlgorithm();
	virtual ~CCUBEAlgorithm();

	// run algorithm
	bool run(set<int>& answers, CDataPoint* pPoints, int numPoints,
			int numDimensions, int k);

	// get the point with the largest coordinate on dimensions 1 to d-1
	virtual void computeMaxPoints(set<int>& answers, CDataPoint* pPoints, int numPoints,
			int numDimensions, int k, double* maxCoordinate,
			int* maxCoordinateId);

	// divide the space into buckets and take data points from the buckets
	virtual void computePointsInBuckets(set<int>& answers, CDataPoint* pPoints,
			int numPoints, int numDimensions, int k, double* maxCoordinate);

};

#endif /* CUBEALGORITHM_H_ */
