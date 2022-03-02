/*
 * dataPoint.h
 *
 *  Created on: 15Mar.,2017
 *      Author: jianzhongq
 */

#ifndef DATAPOINT_H_
#define DATAPOINT_H_

// class of a single data point
class CDataPoint {
public:
	CDataPoint();
	virtual ~CDataPoint();

	int m_nId;
	double* m_pCoordinates;
};

#endif /* DATAPOINT_H_ */
