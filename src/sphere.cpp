//#include "stdafx.h"

#include "sphere.h"

point_set_t* sphere(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;

	if (dim <= 5)
		return sphereWSGeoeIncremental(point_set, k);
	else
		return sphereWSLP(point_set, k);
}

point_set_t* sphereNothing(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;



	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(k);

	double* b_value = new double[dim];

	for (int j = 0; j < dim; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			if (point_set->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = point_set->points[i]->coord[j];
				result->points[j] = point_set->points[i];
			}
		}
	}
	int count = dim;

	if (k < 2 * dim)
	{
		for (int j = count; j < k; ++j)
			result->points[j] = result->points[0];
		return result;
	}

	//first, constrict set I
	point_set_t* I;
	I = construct_I(dim, k - dim);

	//for each point in I, query the nearest facet in the data set 
	for (int i = 0; i < I->numberOfPoints; i++)
	{
		point_set_t* basis = search_basis(point_set, I->points[i]);
		//result->points[i] = facet->points[0];


		for (int j = 0; j < basis->numberOfPoints; j++)
		{
			//include the facet into the final solution
			bool isNew = true;
			for (int p = 0; p < count; p++)
			{
				if (basis->points[j]->id == result->points[p]->id)
				{
					isNew = false;
					break;
				}
			}

			if (isNew)
				result->points[count++] = basis->points[j];
		}

		basis->numberOfPoints = dim + 1;
		release_point_set(basis, false);
		//print_point_set(facet);
	}


	// fill in any remaining points with the first point
	for (int j = count; j < k; ++j)
		result->points[j] = result->points[0];

	release_point_set(I, true);
	//print_point_set(result);
	return result;
}


point_set_t* sphereWSLP(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;

	double R = 2 * sqrt(dim * 1.0);
	//first, constrict set I
	point_set_t* I;
	I = construct_I(dim, k);

	for(int i = 0; i < I->numberOfPoints; i++)
		I->points[i] = scale(R / calc_len(I->points[i]), I->points[i]);

	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(k);

	int count = 0;

	//for each point in I, query the nearest facet in the data set 
	for (int i = 0; i < I->numberOfPoints; i++)
	{
		point_set_t* basis = search_basis(point_set, I->points[i]);
		//result->points[i] = facet->points[0];


		for (int j = 0; j < basis->numberOfPoints; j++)
		{
				//include the facet into the final solution
				bool isNew = true;
				for (int p = 0; p < count; p++)
				{
					if (basis->points[j]->id == result->points[p]->id)
					{
						isNew = false;
						break;
					}
				}

				if (isNew)
					result->points[count++] = basis->points[j];
		}

		basis->numberOfPoints = dim + 1;
		release_point_set(basis, false);
		//print_point_set(facet);
	}

	double *v = new double[dim];
	double epsilon = 0.0000001;
	double worst, w;
	point_t* max;
	int* active = new int[point_set->numberOfPoints];

	for (int i = 0; i< point_set->numberOfPoints; i++)
		active[i] = 1;
	//printf("%i ", count);
	while (count < k)
	{
		// Find a point with maximum regret
		max = point_set->points[0];
		worst = worstDirection(count, result, point_set->points[0], v);
		for (int i = 1; i < point_set->numberOfPoints; i++)
			if (active[i])
			{
				w = worstDirection(count, result, point_set->points[i], v);
				if (worst<w)
				{
					worst = w;
					max = point_set->points[i];
				}
				if (w <= 0.0 + epsilon)
					active[i] = 0;
			}

		// Add a point if regret > 0. Otherwise, stop.
		if (worst >= 0.0 + epsilon)
			result->points[count++] = max;
		else
			break;
	}


	// fill in any remaining points with the first point
	for (int j = count; j < k; ++j)
		result->points[j] = result->points[0];

	delete[] v;
	delete[] active;
	release_point_set(I, true);
	//print_point_set(result);
	return result;
}

point_set_t* sphereWSImpLP(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;
	int N = point_set->numberOfPoints;

	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(k);

	double* b_value = new double[dim];

	for (int j = 0; j < dim; j++)
	{
		b_value[j] = -1;
		result->points[j] = point_set->points[0];
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			if (point_set->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = point_set->points[i]->coord[j];
				result->points[j] = point_set->points[i];
			}
		}
	}
	
	int count = dim;

	if (k >= 2 * dim)
	{

		double R = 2 * sqrt(dim * 1.0);
		//first, constrict set I
		point_set_t* I;
		I = construct_I(dim, k);

		for(int i = 0; i < I->numberOfPoints; i++)
			I->points[i] = scale(R / calc_len(I->points[i]), I->points[i]);

		//for each point in I, query the nearest facet in the data set 
		for (int i = 0; i < I->numberOfPoints; i++)
		{
			point_set_t* basis = search_basis(point_set, I->points[i]);
			//result->points[i] = facet->points[0];


			for (int j = 0; j < basis->numberOfPoints; j++)
			{
				//include the facet into the final solution
				bool isNew = true;
				for (int p = 0; p < count; p++)
				{
					if (basis->points[j]->id == result->points[p]->id)
					{
						isNew = false;
						break;
					}
				}

				if (isNew)
					result->points[count++] = basis->points[j];
			}

			basis->numberOfPoints = dim + 1;
			release_point_set(basis, false);
			//print_point_set(facet);
		}

		release_point_set(I, true);
	}

	if(count == k)
		return result;

	double epsilon = 0.0000001;
	double worst, w;
	point_t* max;
	int* active = new int[point_set->numberOfPoints];

	point_t* lastRound_max;
	point_set_t* directions = alloc_point_set(N);
	double *rr = new double[N];

	max = NULL;
	worst = 0;
	for (int i = 0; i< N; i++)
	{
		active[i] = 1;

		directions->points[i] = alloc_point(dim);

		rr[i] = worstDirection(count, result, point_set->points[i], directions->points[i]->coord);

		w = rr[i];
		if (worst<w)
		{
			worst = w;
			max = point_set->points[i];
		}
		if (rr[i] <= 0.0 + epsilon)
			active[i] = 0;
	}


	if(max == NULL)
	{
		result->numberOfPoints = count;
		return result;

	}

	result->points[count++] = max;
	lastRound_max = max;

	while (count < k)
	{
		// Find a point with maximum regret
		max = NULL;
		worst = 0;

		for (int i = 0; i < N; i++)
		{
			if (worst > rr[i])
			{
				directions->points[i]->id = -2;
				continue;
			}
			else if (directions->points[i]->id == -2 || dot_prod(sub(point_set->points[i], lastRound_max), directions->points[i]) < rr[i])
			{
				rr[i] = worstDirection(count, result, point_set->points[i], directions->points[i]->coord);
				directions->points[i]->id = -1;
			}

			w = rr[i];
			if (worst<w)
			{
				worst = w;
				max = point_set->points[i];
			}
			if (w <= 0.0 + epsilon)
				active[i] = 0;
		}

		// Add a point if regret > 0. Otherwise, stop.
		if (worst >= 0.0 + epsilon)
		{
			result->points[count++] = max;
			lastRound_max = max;
		}
		else
		{
			break;
		}
	}
	//printf("skipCount: %i %f\n", skipCount, 1.0 * skipCount / totalCount);

	// fill in any remaining points with the first point
	//for (int j = count; j < k; ++j)
	//	result->points[j] = result->points[0];
	result->numberOfPoints = count;

	//print_point_set(result);

	delete[] rr;
	delete[] active;
	delete[] b_value;
	
	release_point_set(directions, true);
	return result;
}

point_set_t* sphereWSGeoRebuild(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;

	//first, constrict set I
	point_set_t* I;
	I = construct_I(dim, k);

	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(k);

	int count = 0;

	//for each point in I, query the nearest facet in the data set 
	for (int i = 0; i < I->numberOfPoints; i++)
	{
		point_set_t* basis = search_basis(point_set, I->points[i]);
		//result->points[i] = facet->points[0];


		for (int j = 0; j < basis->numberOfPoints; j++)
		{
			//include the facet into the final solution
			bool isNew = true;
			for (int p = 0; p < count; p++)
			{
				if (basis->points[j]->id == result->points[p]->id)
				{
					isNew = false;
					break;
				}
			}

			if (isNew)
				result->points[count++] = basis->points[j];
		}

		basis->numberOfPoints = dim + 1;
		release_point_set(basis, false);
		//print_point_set(facet);
	}

	geoAddPoint_rebuild(result, count, point_set);

	
	release_point_set(I, true);
	//print_point_set(result);
	return result;
}

point_set_t* sphereWSGeoeIncremental(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;

	//first, constrict set I
	point_set_t* I;
	I = construct_I(dim, k);

	//point_set_t* result = alloc_point_set(k);
	point_set_t* result = alloc_point_set(k);

	int count = 0;

	//for each point in I, query the nearest facet in the data set 
	for (int i = 0; i < I->numberOfPoints; i++)
	{
		point_set_t* basis = search_basis(point_set, I->points[i]);
		//result->points[i] = facet->points[0];


		for (int j = 0; j < basis->numberOfPoints; j++)
		{
			//include the facet into the final solution
			bool isNew = true;
			for (int p = 0; p < count; p++)
			{
				if (basis->points[j]->id == result->points[p]->id)
				{
					isNew = false;
					break;
				}
			}

			if (isNew)
				result->points[count++] = basis->points[j];
		}

		basis->numberOfPoints = dim + 1;
		release_point_set(basis, false);
		//print_point_set(facet);
	}

	geoAddPoint(result, count, point_set);


	release_point_set(I, true);
	//print_point_set(result);
	return result;
}





