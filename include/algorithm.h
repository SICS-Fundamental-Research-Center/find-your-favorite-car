/*
 * algorithm.h
 *
 *  Created on: 14Mar.,2017
 *      Author: jianzhongq
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "defs.h"
#include "dataPoint.h"
#include <string>
#include <cstdio>
#include <set>
using namespace std;

// base class for k-regret query algorithms
class CAlgorithm {
public:
	CAlgorithm();
	virtual ~CAlgorithm();

	// run algorithm
	virtual bool run(set<int>& answers, CDataPoint* pPoints, int numPoints,
			int numDimensions, int k);

	// fill up answer set with randomly selected points if not full
	void fillUpAnswerSet(set<int>& answers, int k, int numPoints);

	// print algorithm name
	void printName();

	// set algorithm name
	void setName(string strName);
protected:
	// name of the algorithm
	string m_strName;
};

#endif /* ALGORITHM_H_ */
