/*
 * algorithm.cpp
 *
 *  Created on: 14Mar.,2017
 *      Author: jianzhongq
 */

#include "algorithm.h"
#include "defs.h"
#include <ctime>
#include <cstdlib>
using namespace std;

CAlgorithm::CAlgorithm() {
	// TODO Auto-generated constructor stub

}

CAlgorithm::~CAlgorithm() {
	// TODO Auto-generated destructor stub
}

// run algorithm
bool CAlgorithm::run(set<int>& answers, CDataPoint* pPoints, int numPoints,
		int numDimensions, int k) {
	return true;
}

// print algorithm name
void CAlgorithm::printName() {
	printf(NAME_PRINT_HEADING, this->m_strName.c_str());
}

// set algorithm name
void CAlgorithm::setName(string strName) {
	m_strName = strName;
}

// fill up answer set with randomly select points if not full
void CAlgorithm::fillUpAnswerSet(set<int>& answers, int k, int numPoints) {
	int i;
	// printf("fillUpAnswerSet start\n");
	// fflush(stdout);
	while (answers.size() < k) {
		i = rand() % numPoints;
		while (answers.find(i) != answers.end()) {
			i = (i+1) % numPoints;
		}
		answers.insert(i);
	}
	// printf("fillUpAnswerSet done\n");
	// fflush(stdout);
}
