#include "adaptive.h"

//skyline?
/* AdaptiveLP: Add the "worst point" to the solution set */

point_set_t* Greedy(int K, point_set_t *p)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;

    int i, j;
    double *v = new double[D]; 

	point_set_t* sol = alloc_point_set(K); 
    double epsilon = 0.0000001;
    double worst, w;
    point_t* max;

    if (K < 1)
    {
        printf("AdaptiveLP requires K to be greater than 0.\n");
        exit;
    }

	double* b_value = new double[D];

	for (int j = 0; j < D; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < p->numberOfPoints; i++)
		{
			if (p->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = p->points[i]->coord[j];
				sol->points[j] = p->points[i];
			}
		}
	}
	delete[] b_value;

    // add more points
    int index = D;
    int nextIndex;
    while (index < K) 
    {
          // Find a point with maximum regret
		  max = p->points[0]; 
		  worst = worstDirection(index, sol, p->points[0], v); 
          for (i = 1; i < N; i++) 
          {
			  w = worstDirection(index, sol, p->points[i], v);
              if (worst<w) 
              {
                  worst = w;
				  max = p->points[i]; 
				  nextIndex = i;
              }
          }
		  //printf("%d\n", nextIndex);

          // Add a point if regret > 0. Otherwise, stop.
          if (worst >= 0.0 + epsilon) 
			  sol->points[index++]=max; 
          else 
              break;  
    }
    
    // fill in any remaining points with the first point
    for(j = index; j < K; ++j)
		sol->points[j] = sol->points[0];

	delete [] v;

	return sol;
}

point_set_t* Greedy_2RMS(int K, point_set_t *p)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;

    int i, j;
    double *v = new double[D]; 
	bool *active = new bool[N];

	point_set_t* sol = alloc_point_set(K); 
    double epsilon = 0.0000001;
    double worst, w;
    point_t* max;
	
    if (K < 1)
    {
        printf("Greedy_2RMS requires K to be greater than 0.\n");
        exit;
    }

	for(int i = 0; i < N; i++)
		active[i] = true;

	double* b_value = new double[D];

	for (int j = 0; j < D; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < p->numberOfPoints; i++)
		{
			if (p->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = p->points[i]->coord[j];
				sol->points[j] = p->points[i];
			}
		}
	}
	delete[] b_value;

	for(int j = 0; j < D; j++)
	{
		for(int i = 0; i < p->numberOfPoints; i++)
		{
			if(p->points[i] == sol->points[j])
			{
				active[i]= false;
				break;
			}
		}
	}

    // add more points
    int index = D;
    int nextIndex;
    while (index < K) 
    {
		  //for (int i = 0; i < N; i++)
			 // printf("%d ", active[i]);
		  //printf("\n");

          // Find a point with maximum regret
		  max = NULL; 
		  nextIndex = -1;
		  worst = 0;
          for (i = 0; i < N; i++) 
          {
			  if(!active[i])
				  continue;

			  //printf("%d\n", i);
			  w = worstDirection(index, sol, p, active, p->points[i], v);
              if (worst<w) 
              {
                  worst = w;
				  max = p->points[i]; 
				  nextIndex = i;
              }
          }
		  //printf("%d\n", nextIndex);

          // Add a point if regret > 0. Otherwise, stop.
          if (nextIndex >= 0 && worst >= 0.0 + epsilon) 
		  {
			  sol->points[index++]=max; 
			  active[nextIndex] = false;
		  }
          else 
              break;  
    }
    
    // fill in any remaining points with the first point
    for(j = index; j < K; ++j)
		sol->points[j] = sol->points[0];

	delete [] v;
	delete [] active;

	return sol;
}


point_set_t* adaptiveLPfast(int K, point_set_t *p, bool selectDim)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;

    int i, j;
    double *v = new double[D]; 

	point_set_t* sol = alloc_point_set(K); 

    double epsilon = 0.0000001;
    double worst, w;
    point_t* max;
    int* active = new int[N];
    

    if (K < 1)
    {
        printf("AdaptiveLP requires K to be greater than 0.\n");
        exit(0);
    }
    
    for (i=0; i<N; i++) 
      active[i]=1;
    
    // pick max in the first coordinate
	/*
	sol->points[0] = p->points[0]; 
	for(i = 1; i < N; ++i)
		if (p->points[i]->coord[0] > sol->points[0]->coord[0])
			sol->points[0]=p->points[i]; 
	*/

	int dim = p->points[0]->dim;
	int* maxIndex = (int*) malloc(dim*sizeof(int));
	for(int i = 0; i < dim; i++)
	{
		maxIndex[i] = 0;
	}

	for(int i = 1; i < p->numberOfPoints; i++){

		for(int j = 0; j < dim; j++){
			if(p->points[i]->coord[j] > p->points[maxIndex[j]]->coord[j])
				maxIndex[j] = i;
		}
	}

	int index;
	if(selectDim)
		index = dim;
	else
		index = 1;

	for(int i = 0; i < index; i++)
	{
		sol->points[i] = p->points[maxIndex[i]];
	}
        
    // add more points
	//int index = 1;
   
    
    while (index < K) 
    {
          // Find a point with maximum regret
		  max = p->points[0]; 
		  worst = worstDirection(index, sol, p->points[0], v); 
          for (int i = 1; i < N; i++) 
            if (active[i])
            {
				w = worstDirection(index, sol, p->points[i], v);
                if (worst<w) 
                {
                    worst = w;
					max = p->points[i]; 
                }
                if (w<=0.0+epsilon) 
                    active[i]=0;               
            }

          // Add a point if regret > 0. Otherwise, stop.
          if (worst >= 0.0 + epsilon) 
		  {
			  sol->points[index++]=max; 
		  }
          else 
              break;  
    }
    
    
    // fill in any remaining points with the first point
    for(j = index; j < K; ++j)
		sol->points[j] = sol->points[0];

	delete[] v;
	delete[] active;

	return sol;
}

//index
point_set_t* ImpGreedy2(int K, point_set_t *p)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;


    double *v = new double[D]; 

	point_set_t* sol = alloc_point_set(K); 

    double epsilon = 0.0000001;
    double worst, w;
    point_t* max;
    int* active = new int[N];
  	double *rr = new double [N];

	point_set_t* directions = alloc_point_set(N);

    if (K < 1)
    {
        printf("AdaptiveLP requires K to be greater than 0.\n");
        exit(0);
    }
    
 
    // pick max in the first coordinate
	int maxIndex = 0;
	int dim = p->points[0]->dim;

	sol->points[0] = p->points[0]; 
	for(int i = 1; i < N; ++i)
	{
		if (p->points[i]->coord[0] > sol->points[0]->coord[0])
		{
			sol->points[0]=p->points[i]; 
			maxIndex = i;
		}
	}

	int index = 1;
	sol->points[0] = p->points[maxIndex];
	
        
    // add more points

	point_t* lastRound_max;
	int skipCount = 0;
	int totalCount = 0;

	max = NULL; 
	worst = 0;
	for (int i=0; i<N; i++) 
	{
		active[i]=1;

		directions->points[i] = alloc_point(D);

		double temprr = 0;
		int tempdim = 0;

		//double frac = p->points[maxIndex]->coord[0] / p->points[i]->coord[0];
		//int fracdim = 0;
		for(int j = 0; j < dim; j++)
		{
			
			double rr = 1 / p->points[i]->coord[j]  * (p->points[i]->coord[j] - p->points[maxIndex]->coord[j]);
			if(rr > temprr)
			{
				temprr = rr;
				tempdim = j;
			}
			
			/*
			if(frac > p->points[maxIndex]->coord[j] / p->points[i]->coord[j])
			{
				frac = p->points[maxIndex]->coord[j] / p->points[i]->coord[j];
				fracdim = j;
			}*/
		}

		rr[i] = temprr;
		for(int j = 0; j < dim; j++)
		{
			if(j == tempdim)
				directions->points[i]->coord[j] = 1 / p->points[i]->coord[j];
			else
				directions->points[i]->coord[j] = 0;
		}

		skipCount++;
		totalCount++;

		w = rr[i];
		if (worst<w) 
        {
            worst = w;
			max = p->points[i]; 
        }
        if (w<=0.0+epsilon) 
            active[i]=0; 
	}


	sol->points[index++]=max; 
	lastRound_max = max;

    while (index < K) 
    {
          // Find a point with maximum regret
		  max = NULL; 
		  worst = 0; 

          for (int i = 0; i < N; i++) 
		  {	

            if (active[i])
            {

				//if(temp > 0)
					//printf("gub");
				if(1)
				{
					
					if(worst > rr[i])
					{
						directions->points[i]->id = -2;
						skipCount++;
						totalCount++;
						continue;
					}
					else if(directions->points[i]->id == -2 || dot_prod(sub(p->points[i], lastRound_max) ,directions->points[i]) < rr[i])
					{
						rr[i]  = worstDirection(index, sol, p->points[i], directions->points[i]->coord);
						directions->points[i]->id = -1;
						//skipCount++;
						totalCount++;
					}
					else
					{
						skipCount++;
						totalCount++;
					}
				}
				else
				{
					double temp = dot_prod(sub(p->points[i], lastRound_max) ,directions->points[i]);
					if(temp < rr[i])
					{
						rr[i]  = worstDirection(index, sol, p->points[i], directions->points[i]->coord);
						totalCount++;
					}
					else
					{
						skipCount++;
						totalCount++;
					}

				}

				w = rr[i];
				if (worst<w) 
				{
					worst = w;
					max = p->points[i]; 
				}
				if (w<=0.0+epsilon) 
					active[i]=0;
            }
		  }

          // Add a point if regret > 0. Otherwise, stop.
          if (worst >= 0.0 + epsilon) 
		  {
			  sol->points[index++]=max; 
			  lastRound_max = max;
		  }
          else 
		  {
              break;  
		  }
    }
    //printf("skipCount: %i %f\n", skipCount, 1.0 * skipCount / totalCount);
    
    // fill in any remaining points with the first point
    for(int j = index; j < K; ++j)
		sol->points[j] = sol->points[0];

	delete[] v;
	delete[] active;
	delete[] rr;

	release_point_set(directions, true);
	return sol;
}

point_set_t* ImpGreedy(int k, point_set_t* point_set)
{
	int dim = point_set->points[0]->dim;
	int N = point_set->numberOfPoints;

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

	if (count == k)
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
	for (int j = count; j < k; ++j)
		result->points[j] = result->points[0];

	delete[] rr;
	delete[] active;
	delete[] b_value;

	release_point_set(directions, true);
	return result;
}

point_set_t* IncGreedy(int k, point_set_t* point_set, point_set_t* selected)
{
	point_set = skyline_point(point_set);
	int dim = point_set->points[0]->dim;
	int N = point_set->numberOfPoints;

	point_set_t* result = alloc_point_set(k + selected->numberOfPoints);
	for(int i = 0; i < selected->numberOfPoints; i++)
		result->points[i] = selected->points[i];

	int count = selected->numberOfPoints;

	if(count == 0)
	{
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

		delete[] b_value;
		count = dim;
	}


	if (count == k + selected->numberOfPoints)
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


	if (worst < 0.0 + epsilon)
	{
		result->numberOfPoints = count;
		return result;
	}
		

	result->points[count++] = max;
	lastRound_max = max;

	while (count < result->numberOfPoints)
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

	delete[] rr;
	delete[] active;


	release_point_set(directions, true);
	release_point_set(point_set, false);
	return result;
}


struct ordered_point_t
{
	point_t* p;
	double mrr_ub;
	int j;
	double mrr_j;
	point_t* u_j;
};

struct APtrCmp
{
	bool operator()(const ordered_point_t* lhs, const ordered_point_t* rhs) const 
	{ 
		if (isZero(lhs->mrr_ub - rhs->mrr_ub))
			return lhs->p->id < rhs->p->id;

		return lhs->mrr_ub > rhs->mrr_ub;
	}
};

struct APtrCmp_reverse
{
	bool operator()(const ordered_point_t* lhs, const ordered_point_t* rhs) const 
	{ 
		if (isZero(lhs->mrr_ub - rhs->mrr_ub))
			return lhs->p->id < rhs->p->id;

		return lhs->mrr_ub < rhs->mrr_ub;
	}
};

