/*
 * skylineAlgorithm.h
 *
 *  Created on: 23Mar.,2017
 *      Author: jianzhongq
 */

#ifndef SKYLINEALGORITHM_H_
#define SKYLINEALGORITHM_H_

#include "dataPoint.h"
#include <set>
using namespace std;

class CSkylineAlgorithm {
public:
	CSkylineAlgorithm();
	virtual ~CSkylineAlgorithm();

	// compute the skyline points give a set of points
	static void computeSkyline(CDataPoint* pPoints, int numPoints, int numDimensions,
			set<int>& skylinePoints);

	// tests whether pPoint1 dominates pPoint2
	static bool isDominating(CDataPoint* pPoint1, CDataPoint* pPoint2, int numDimensions);

private:
	static set<int> m_skylinePoints;
};

#endif /* SKYLINEALGORITHM_H_ */
