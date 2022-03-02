/*
 * cubeAlgorithm.cpp
 *
 *  Created on: 16Mar.,2017
 *      Author: jianzhongq
 */

#include "cubeAlgorithm.h"
#include <cstdio>
#include <cmath>
#include <cstring>
using namespace std;

CCUBEAlgorithm::CCUBEAlgorithm() {
	// TODO Auto-generated constructor stub
	m_strName = "CUBE";
}

CCUBEAlgorithm::~CCUBEAlgorithm() {
	// TODO Auto-generated destructor stub
}

// run algorithm
bool CCUBEAlgorithm::run(set<int>& answers, CDataPoint* pPoints, int numPoints,
		int numDimensions, int k) {

	// d must be greater than k
	if (k < numDimensions) {
		printf("Number of dimensions not greater than k.\n");
		return false;
	}

	// if not enough points, return all
	if (numPoints <= k) {
		for (int i = 0; i < numPoints; i++) {
			answers.insert(i);
		}
		return true;
	}

	double* maxCoordinate = new double[numDimensions];
	int* maxCoordinateId = new int[numDimensions];

	// get the point with the largest coordinate on dimensions 1 to d-1
	// insert into answer set
	computeMaxPoints(answers, pPoints, numPoints, numDimensions, k,
			maxCoordinate, maxCoordinateId);

	// divide the space into buckets and take data points from the buckets
	computePointsInBuckets(answers, pPoints, numPoints, numDimensions, k,
			maxCoordinate);

	// fill up answer set with randomly select points if not full
	fillUpAnswerSet(answers, k, numPoints);

	delete[] maxCoordinate;
	delete[] maxCoordinateId;
	return true;
}

// get the point with the largest coordinate on dimensions 1 to d-1
void CCUBEAlgorithm::computeMaxPoints(set<int>& answers, CDataPoint* pPoints,
		int numPoints, int numDimensions, int k, double* maxCoordinate,
		int* maxCoordinateId) {
	int i = 0, j;
	for (j = 0; j < numDimensions; j++) {
		maxCoordinateId[j] = i;
		maxCoordinate[j] = pPoints[i].m_pCoordinates[j];
	}
	for (i = 1; i < numPoints; i++) {
		for (j = 0; j < numDimensions - 1; j++) {
			if (pPoints[i].m_pCoordinates[j] > maxCoordinate[j]) {
				maxCoordinateId[j] = i;
				maxCoordinate[j] = pPoints[i].m_pCoordinates[j];
			}
		}
	}
	for (j = 0; j < numDimensions - 1; j++) {
		answers.insert(maxCoordinateId[j]);
	}
}

// divide the space into buckets and take data points from the buckets
void CCUBEAlgorithm::computePointsInBuckets(set<int>& answers,
		CDataPoint* pPoints, int numPoints, int numDimensions, int k,
		double* maxCoordinate) {

	// t = floor((k-d+1)^(1/(d-1)))
	int t = floor(pow(k - numDimensions + 1, 1.0 / (numDimensions - 1)));

	int i, j;
	// take points in each bucket
	int numBuckets = pow(t, (double) numDimensions - 1);
	// printf("%d", numBuckets);
	double* maxBucketCoordinate = new double[numBuckets + 1]; // +1 so that delete[] is always valid
	int* maxBucketCoordinateId = new int[numBuckets + 1];
	int bucketId;

	for (i = 0; i < numBuckets; i++) {
		maxBucketCoordinate[i] = 0;
		maxBucketCoordinateId[i] = -1;
	}
	for (i = 0; i < numPoints; i++) {
		// for each point, compute the bucket that it belongs to
		j = 0;
		bucketId = 0;
		while (j < numDimensions - 1) {
			if (pPoints[i].m_pCoordinates[j] >= maxCoordinate[j]) {
				bucketId += pow(t, (double)j) * (t - 1);
			} else {
				bucketId += pow(t, (double)j)
						* ((int) (pPoints[i].m_pCoordinates[j]
								/ (maxCoordinate[j] / t))); // break each dimension evenly into t buckets
			}
			j++;
		}
		//if (bucketId >= numBuckets || i % 1000 == 0) {
		// printf("bucketId >= numBuckets! %d %d\n", bucketId, i);
		//}
		if (pPoints[i].m_pCoordinates[numDimensions - 1]
				> maxBucketCoordinate[bucketId]) {
			maxBucketCoordinate[bucketId] =
					pPoints[i].m_pCoordinates[numDimensions - 1];
			maxBucketCoordinateId[bucketId] = i;
		}
	}

	for (i = 0; i < numBuckets; i++) {
		if (maxBucketCoordinateId[i] > 0) {
			answers.insert(maxBucketCoordinateId[i]);
			//printf("Bucket: %d; max point: %d\n", i, maxBucketCoordinateId[i]);
		}
	}

	delete[] maxBucketCoordinate;
	maxBucketCoordinate = NULL;
	delete[] maxBucketCoordinateId;
	maxBucketCoordinateId = NULL;
}

