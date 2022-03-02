/*
 * defs.h
 *
 *  Created on: 14Mar.,2017
 *      Author: jianzhongq
 */

#ifndef DEFS_H_
#define DEFS_H_

// print heading for test result
#define NAME_PRINT_HEADING "========== %s ==========\n"

// parameter b for CES function
#define CES_B_DEFAULT 0.5

// parameter g precision bound for MinWidth algorithm
#define MIN_WIDTH_G_EPS 0.0001

// precision bound for MinWidth algorithm coverage in find break points
#define MIN_WIDTH_EPS 0.000001

// parameter bucket size increment for MinVar algorithm, default 0.001
#define MIN_VAR_BUCKET_INCREMENT 0.001  // 0.01, 0.005, 0.001, 0.0005, 0.0001; default 0.001

// parameter maximum number of iterations to find points in buckets for MinVarRF algorithm, default 11
#define MAX_MIN_VAR_RF_ITR 11 // 1, 2, 3, 4, 5; default 11

// parameter for data generator: to generate random data
#define RANDOM_DATA 0

// control if the tests are run on sklyine points only
// #define RUN_ON_SKYLINE

#endif /* DEFS_H_ */
