//#include "stdafx.h"

#include "evaluate.h"

double evaluateLP(point_set_t *p, point_set_t* S, int VERBOSE)
{
	p = skyline_point(p);

	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;

	int i, j;
	double maxRegret = 0.0, maxK, maxN;
	double* v = new double[D];

	for(i = 0; i < N; ++i)
	{ 
		double w = worstDirection(S, p->points[i], v);

		maxN = dot_prod(maxPoint(p, v), v);
		maxK = dot_prod(maxPoint(S, v), v);

		//printf("%d, %lf, %lf\n", i, w, 1.0 - maxK/maxN);

		if (1.0 - maxK/maxN > maxRegret)
		{
            maxRegret = 1.0 - maxK/maxN;
			//printf("%d\n", p->points[i]->id);
			//print_point_set(p);
			//print_point_set(S);
			//printf("%lf\n", w);
		}
	}		


	release_point_set(p, false);

	if (VERBOSE)
        printf("LP max regret ratio = %lf\n", maxRegret);

	delete []v;
	return maxRegret;

}

double evaluateKRMS(point_set_t *p, point_set_t* S, int K)
{
	int N = 10000;
	int D = p->points[0]->dim;

	double mrr = 0;
	point_t* u = alloc_point(D);
	for(int i = 0; i < N; i++)
	{
		double rr = 0;

		for(int j = 0; j < D; j++)
			u->coord[j] = rand_f(0, 1);

		vector<double> utilities;
		for(int j = 0; j < p->numberOfPoints; j++)
			utilities.push_back( dot_prod(u, p->points[j]) );
		sort(utilities.begin(), utilities.end());
		double kScore = utilities[utilities.size() - K];
		

		double localMax = 0;
		for(int j = 0; j < S->numberOfPoints; j++)
		{
			double temp =  dot_prod(u, S->points[j]);
			if(temp > localMax)
				localMax = temp;
		}

		rr = localMax > kScore? 0: 1 - localMax/kScore;

		if(rr > mrr)
			mrr = rr;
	}
	release_point(u);
	return mrr;
}

double evaluateLP_pruned(point_set_t *p, point_set_t* S, int VERBOSE)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;

	int i, j;
	double maxRegret = 0.0, maxK, maxN;
	double* v = new double[D];
	int count = 0;


	int size =  K * K * D;
	double threshold = 0.1;


	point_set_t* stored = alloc_point_set(size);
	stored->numberOfPoints = 0;
	
	for(int i = 0; i < K; i++)
	{
		bool add = true;
		if(!isZero(threshold))
		{
			for(int j = 0; j < D; j++)
			{
				if(S->points[i]->coord[j] < threshold)
				{
					add = false;
					break;
				}
			}
		}
		if(add)
		{
			stored->points[stored->numberOfPoints] = copy(S->points[i]);
			stored->numberOfPoints++;
		}
	}

	for(i = 0; i < N; ++i)
	{ 
		double ub = 1;
		for(j = 0; j < stored->numberOfPoints; j++)
		{
			double r = 1;
			for(int x = 0; x < D; x++)
			{
				if( r > stored->points[j]->coord[x] / p->points[i]->coord[x])
					r = stored->points[j]->coord[x] / p->points[i]->coord[x];
			}

			if(ub > 1 - r)
			{
				ub = 1 - r;
			}
			if(ub < maxRegret)
				break;
		}

		if(ub < maxRegret)
		{
			count++;
			continue;
		}

		worstDirection(S, p->points[i], v);

		maxN = dot_prod(maxPoint(p, v), v);
		maxK = dot_prod(maxPoint(S, v), v);

		double mrr = 1.0 - maxK/maxN;
		if(stored->numberOfPoints < size)
		{

			point_t* pt = scale(1 - mrr, p->points[i]);
			//stored->numberOfPoints++;

			bool add = true;
			if(!isZero(threshold))
			{
				for(int j = 0; j < D; j++)
				{
					if(pt->coord[j] < threshold)
					{
						add = false;
						break;
					}
				}
			}
			if(add)
			{
				stored->points[stored->numberOfPoints] = pt;
				stored->numberOfPoints++;
			}
			else
				release_point(pt);
		}


		if (1.0 - maxK/maxN > maxRegret)
            maxRegret = 1.0 - maxK/maxN;
	}		
	//print_point_set(stored);

	if (VERBOSE)
	{
        printf("LP max regret ratio = %lf\n", maxRegret);
		printf("%lf skipped\n", 1.0 * count / N);
	}

	delete []v;
	release_point_set(stored, true);
	return maxRegret;
}

double evaluateLP(point_set_t *p, point_set_t* S, int VERBOSE, int& nextIndex)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;

	int i, j;
	double maxRegret = 0.0, maxK, maxN;
	double* v = new double[D];
	double worst, w;
    point_t* max;

	int index;

	index = S->numberOfPoints;
/*
	for(i = 0; i < N; ++i)
	{ 
		if(i == 1 || i == 1069)
			printf("bug\n");
		worstDirection(S, p->points[i], v);

		maxN = dot_prod(maxPoint(p, v), v);
		maxK = dot_prod(maxPoint(S, v), v);

		if (1.0 - maxK/maxN > maxRegret)
		{
            maxRegret = 1.0 - maxK/maxN;
			nextIndex = i;
		}
	}		
*/
	max = p->points[0]; 
	worst = worstDirection(index, S , p->points[0], v); 
	nextIndex = 0;
    for (i = 1; i < N; i++) 
    {
		w = worstDirection(index, S, p->points[i], v);
        if (worst<w) 
        {
            worst = w;
			max = p->points[i]; 
			nextIndex = i;
        }
    }
	//printf("%d\n", nextIndex);

	w = worstDirection(S, p->points[nextIndex], v);

	maxN = dot_prod(maxPoint(p, v), v);
	maxK = dot_prod(maxPoint(S, v), v);

    maxRegret = 1.0 - maxK/maxN;


	if (VERBOSE)
        printf("LP max regret ratio = %lf\n", maxRegret);

	delete []v;
	return maxRegret;
}


double CES(int D, float b, point_t* evalPoint)
{
	double runTot = 0;
	for (int i = 0; i < D; i++)
	{
		runTot += pow(evalPoint->coord[i], (double) b);
	}

	runTot = pow(runTot, (double)(1 / b));
	return runTot;
}

double evaluateCES(point_set_t *p, point_set_t* S, int VERBOSE)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;


	double maxRegretTotal = 0;
	double runningRegretFunc = 0;
	double maxRegretFunc = 0;
	double runningGainSky = 0;
	double maxGainSky = 0;
	double runningGainIn = 0;
	double maxGainIn = 0;
	float b;

	for (int i = 0; i < 1000; i++)
	{

		//Create the random function
		b = (1.0 / ((rand() % 10) + .001));

		if ((b < .001) || (b > 1))
			continue;
	
		runningRegretFunc = 0;
		maxGainSky = 0;
		maxGainIn = 0;
		for (int j = 0; j < N; j++)
		{	
			runningGainSky = CES(D, b, p->points[j]);
			if (runningGainSky > maxGainSky)
			{
				maxGainSky = runningGainSky;
			}
		}
		
		for (int z = 0; z < K; z++)
		{
			runningGainIn = CES(D, b, S->points[z]);
			if (runningGainIn > maxGainIn)
			{
				maxGainIn = runningGainIn;
			}
		}

		runningRegretFunc = ((maxGainSky - maxGainIn) / maxGainSky);
		if (runningRegretFunc > maxRegretFunc)
			maxRegretFunc = runningRegretFunc;

	}

	for (int i = 0; i < 10000; i++)
	{

		//Create the random function
		b = (rand() % 1000) + 1;
	
		runningRegretFunc = 0;
		maxGainSky = 0;
		maxGainIn = 0;

		for (int j = 0; j < N; j++)
		{	
			runningGainSky = CES(D, b, p->points[j]);
			if (runningGainSky > maxGainSky)
			{
				maxGainSky = runningGainSky;
			}
		}
		
		for (int z = 0; z < K; z++)
		{
			runningGainIn = CES(D, b, S->points[z]);
			if (runningGainIn > maxGainIn)
			{
				maxGainIn = runningGainIn;
			}
		}

		runningRegretFunc = ((maxGainSky - maxGainIn) / maxGainSky);
		if (runningRegretFunc > maxRegretFunc)
			maxRegretFunc = runningRegretFunc;

	}

	if (VERBOSE)
       		cout << "CES max regret ratio = " << maxRegretFunc << endl;

	return maxRegretFunc;
}

double exponents(int D, float b, point_t* evalPoint)
{
	double runTot = 0;
	for (int i = 0; i < D; i++)
	{
		runTot += pow(evalPoint->coord[i],(double) b);
	}

	return runTot;	
}

double evaluateCVX(point_set_t *p, point_set_t *S, int VERBOSE)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;


	double maxRegretTotal = 0;
	double runningRegretFunc = 0;
	double maxRegretFunc = 0;
	double runningGainSky = 0;
	double maxGainSky = 0;
	double runningGainIn = 0;
	double maxGainIn = 0;
	float b;
	
	for (int i = 0; i < 10000; i++)
	{

		//Create the random function
		b = (rand() % 10) + 1;
		//b = (rand() % 1000) + 1;
		
		runningRegretFunc = 0;
		maxGainSky = 0;
		maxGainIn = 0;
		for (int j = 0; j < N; j++)
		{	
			runningGainSky = exponents(D, b, p->points[j]);
			if (runningGainSky > maxGainSky)
			{
				maxGainSky = runningGainSky;
			}
		}
		
		for (int z = 0; z < K; z++)
		{
			runningGainIn = exponents(D, b, S->points[z]);
			if (runningGainIn > maxGainIn)
			{
				maxGainIn = runningGainIn;
			}
		}

		runningRegretFunc = ((maxGainSky - maxGainIn) / maxGainSky);
		if (runningRegretFunc > maxRegretFunc)
			maxRegretFunc = runningRegretFunc;

	}


	if (VERBOSE)
       		cout << "Convex max regret ratio = " << maxRegretFunc << endl;

	return maxRegretFunc;
}

double evaluateCAV(point_set_t *p, point_set_t* S, int VERBOSE)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;

	double maxRegretTotal = 0;
	double runningRegretFunc = 0;
	double maxRegretFunc = 0;
	double runningGainSky = 0;
	double maxGainSky = 0;
	double runningGainIn = 0;
	double maxGainIn = 0;
	float b;
	
	for (int i = 0; i < 10000; i++)
	{

		//Create the random function
		b = (1.0 / ((rand() % 100) + .001));

		if ((b < .001) || (b > 1))
			continue;
	
		runningRegretFunc = 0;
		maxGainSky = 0;
		maxGainIn = 0;
		for (int j = 0; j < N; j++)
		{	
			runningGainSky = exponents(D, b, p->points[j]);
			if (runningGainSky > maxGainSky)
			{
				maxGainSky = runningGainSky;
			}
		}
		
		for (int z = 0; z < K; z++)
		{
			runningGainIn = exponents(D, b, S->points[z]);
			if (runningGainIn > maxGainIn)
			{
				maxGainIn = runningGainIn;
			}
		}

		runningRegretFunc = ((maxGainSky - maxGainIn) / maxGainSky);
		if (runningRegretFunc > maxRegretFunc)
			maxRegretFunc = runningRegretFunc;

	}

	if (VERBOSE)
       		cout << "Concave max regret ratio = " << maxRegretFunc << endl;

	return maxRegretFunc;
}

// evaluate CD utility function value given a data point and function weights
double mulplication(point_t* pt, point_t* u) {
	int numDimensions = pt->dim;

	double value = 1;
	int i;
	for (i = 0; i < numDimensions; i++) {
		value *= pow(pt->coord[i], u->coord[i]);
	}
	return value;
}

double evaluateMUF(point_set_t *p, point_set_t *S, int VERBOSE)
{
	int D = p->points[0]->dim;
	int N = p->numberOfPoints;
	int K = S->numberOfPoints;


	double maxRegretTotal = 0;
	double runningRegretFunc = 0;
	double maxRegretFunc = 0;
	double runningGainSky = 0;
	double maxGainSky = 0;
	double runningGainIn = 0;
	double maxGainIn = 0;
	float b;
	
	point_t* u = alloc_point(D);
	for (int i = 0; i < 10000; i++)
	{

		for(int j = 0; j < D; j++)
			u->coord[j] = rand_f(0,1);

		
		runningRegretFunc = 0;
		maxGainSky = 0;
		maxGainIn = 0;
		for (int j = 0; j < N; j++)
		{	
			runningGainSky = mulplication(p->points[j],u);
			if (runningGainSky > maxGainSky)
			{
				maxGainSky = runningGainSky;
			}
		}
		
		for (int z = 0; z < K; z++)
		{
			runningGainIn = mulplication(S->points[z], u);
			if (runningGainIn > maxGainIn)
			{
				maxGainIn = runningGainIn;
			}
		}

		runningRegretFunc = ((maxGainSky - maxGainIn) / maxGainSky);
		if (runningRegretFunc > maxRegretFunc)
			maxRegretFunc = runningRegretFunc;

	}

	release_point(u);

	if (VERBOSE)
       		cout << "MUF max regret ratio = " << maxRegretFunc << endl;

	return maxRegretFunc;
}