/*
 * skylineAlgorithm.cpp
 *
 *  Created on: 23Mar.,2017
 *      Author: jianzhongq
 */

#include "skylineAlgorithm.h"
#include <ctime>
#include <cstdio>
using namespace std;

set<int> CSkylineAlgorithm::m_skylinePoints;

CSkylineAlgorithm::CSkylineAlgorithm() {
	// TODO Auto-generated constructor stub

}

CSkylineAlgorithm::~CSkylineAlgorithm() {
	// TODO Auto-generated destructor stub
}

// tests whether pPoint1 dominates pPoint2
bool CSkylineAlgorithm::isDominating(CDataPoint* pPoint1, CDataPoint* pPoint2,
		int numDimensions) {
	bool hasLarger = false;
	int i;
	for (i = 0; i < numDimensions; i++) {
		if (pPoint1->m_pCoordinates[i] < pPoint2->m_pCoordinates[i]) {
			return false;
		}
		if (pPoint1->m_pCoordinates[i] > pPoint2->m_pCoordinates[i]) {
			hasLarger = true;
		}
	}
	return hasLarger;
}

// compute the skyline points give a set of points
void CSkylineAlgorithm::computeSkyline(CDataPoint* pPoints, int numPoints,
		int numDimensions, set<int>& skylinePoints) {
	// use simply scans to find the skyline points

	clock_t start;	// timer
	double duration;

	start = clock();

	if (m_skylinePoints.size() > 0) {
		skylinePoints = m_skylinePoints;

	} else {

		int i, j;
		bool isSkyline;
		for (i = 0; i < numPoints; i++) {
			isSkyline = true;
			for (j = 0; j < numPoints; j++) {
				if (i != j
						&& isDominating(&(pPoints[j]), &(pPoints[i]),
								numDimensions)) {
					isSkyline = false;
					break;
				}
			}
			if (isSkyline) {
				skylinePoints.insert(i);
			}
		}
		m_skylinePoints = skylinePoints;
	}

	duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	//printf("Skyline (%d points) computation time: %f\n", m_skylinePoints.size(), duration);
}
