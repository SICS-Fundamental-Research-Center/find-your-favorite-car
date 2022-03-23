//#include "stdafx.h"

#include "cube.h"

/* Algorithm using the cube "strips" method */

point_set_t* cube(int K, point_set_t *p) 
{
	int N = p->numberOfPoints;
	int D = p->points[0]->dim;

	int i, j, t, index, L = D - 1, inCube, distinct;

	point_set_t* c = alloc_point_set(D); // maximal points in each direction
	point_set_t* answer = alloc_point_set(K); // why k+1?

	// compute the maximal points in each of the D directions
	for(i = 0; i < D; ++i)
		c->points[i] = p->points[0]; // initialize with first point
	for(i = 0; i < N; ++i)
		for(j = 0; j < D; ++j)
			if (p->points[i]->coord[j] > c->points[j]->coord[j])
				c->points[j] = p->points[i];

	// initialize t as in the cube algorithm
	t = (int)pow(K - D + 1.0, 1.0/(D - 1.0));
	
	if (0)
	{
		// keep looping until we find at least K distinct points 
		do
		{
			distinct = cubealgorithm(D, N, K, p, L, t, c, answer);
			t++;
		} while (distinct < K && distinct < N);

		if (distinct > K)
			cubealgorithm(D, N, K, p, L, t - 2, c, answer);
	}
	else
		cubealgorithm(D, N, K, p, L, t, c, answer);

	release_point_set(c, false);

	return answer;
}

int cubealgorithm(int D, int N, int K, point_set_t *p, int L, int t, point_set_t *c, point_set_t *answer)
{
	int i, j, index, inCube, distinct, cubeBestIndex, done, seenBefore;
	int* boundary = new int[D];

	index = 0;
	// first list the maximal points in the directions {1,...,D}\L
	for(i = 0; i < D; ++i)
		if (i != L)
			answer->points[index++] = c->points[i];
	
	/*** Try all 0 <= j_1, j_2, \ldots < t ***/
	
	// initialize to all zeros
	for(i = 0; i < D; ++i)
		boundary[i] = 0;
	
	done = 0;
	while(!done)
	{
		// pick the maximal point in current cube
		cubeBestIndex = -1;
		for(i = 0; i < N; ++i)
		{
			// determine if p[i] is in this cube
			inCube = 1;
			for(j = 0; j < D && inCube; ++j)
				if (j != L)  // not the excluded dimension
					inCube = (boundary[j] * c->points[j]->coord[j] <= t * p->points[i]->coord[j]) && (t * p->points[i]->coord[j] < (boundary[j] + 1) * c->points[j]->coord[j]);
			
			if (inCube)  // check if it is maximal in the missing dimension
				if (cubeBestIndex < 0)
					cubeBestIndex = i;  // none seen yet, set to i
				else if(p->points[i]->coord[L] > p->points[cubeBestIndex]->coord[L])
						cubeBestIndex = i; // replace if larger in dimension L
		}
		
		// If there is a point in this cube and it is distinct from earlier ones, add to list
		if (cubeBestIndex >= 0)
		{
			seenBefore = 0;
			for(i = 0; i < index && !seenBefore; ++i)
				if(answer->points[i]->id == p->points[cubeBestIndex]->id)
					seenBefore = 1;
				
			if (!seenBefore)
				answer->points[index++] = p->points[cubeBestIndex];
		}
		
		
		// increment t-ary counter
		if (L == 0) // if we're skipping 0th dimension, start at 1
			j = 1; 
		else 
			j = 0; 
		boundary[j]++; 
		while(boundary[j] == t && !done)
		{
			boundary[j] = 0;  // reset position 
			
			if (j == L - 1) j += 2;  // if we reach the skipped dimension, skip it
			else            j += 1;  // else increment by 1

			if (j < D)
				boundary[j]++; // if not at the end, do the carry
			else 
				done = 1;      // else indicate that all combinations tried  
		}
	}

	// fill in any remaining positions with the first point found
	//for(i = index; i < K; ++i)
	//	answer->points[i] = answer->points[0];
	answer->numberOfPoints = index;

	delete [] boundary;

	return index;
}