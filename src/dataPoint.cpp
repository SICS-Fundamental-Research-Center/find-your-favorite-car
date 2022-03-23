/*
 * dataPoint.cpp
 *
 *  Created on: 15Mar.,2017
 *      Author: jianzhongq
 */

#include "dataPoint.h"
#include <cstdlib>
using namespace std;

CDataPoint::CDataPoint() {
	// TODO Auto-generated constructor stub
	m_nId = -1;
	m_pCoordinates = NULL;
}

CDataPoint::~CDataPoint() {
	// TODO Auto-generated destructor stub
	if (m_pCoordinates) {
		delete[] m_pCoordinates;
		m_pCoordinates = NULL;
	}
}

