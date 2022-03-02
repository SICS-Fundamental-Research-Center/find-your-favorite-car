#include "perform.h"

void performSingleAlg(point_set_t* point_set, int k, double eps, stat_t*& stat, int algID)
{
	double time, mrr;
	point_set_t* S;
	char* algName;
#ifndef WIN32
	struct timeval start, end;
#else
	std::clock_t    start, end;
#endif


#ifndef WIN32
	gettimeofday(&start, NULL);
#else
	start = std::clock();
#endif

	if (algID == 0)
	{
		S = sweepDP(point_set, k);
		algName = "SweepDP";
	}
	else if (algID == 1)
	{
		S = biSearch(point_set, k);
		algName = "BiSearch";
	}
	else if (algID == 2)
	{
		S = graphDP(point_set, k);
		algName = "GraphDP";
	}
	else if (algID == 3)
	{
		S = Greedy(k, point_set);
		algName = "Greedy";
	}
	else if (algID == 4)
	{
		S = geoGreedy(k, point_set);
		algName = "GeoGreedy";
	}
	else if (algID == 5)
	{
		S = ImpGreedy(k, point_set);
		algName = "ImpGreedy";
	}
	else if (algID == 6)
	{
		S = cube(k, point_set);
		algName = "Cube";
	}

	else if (algID == 8)
	{
		S = sphereWSImpLP(point_set, k);
		algName = "Sphere";
	}

	else if (algID == 10)
	{
		S = DMM(point_set, k);
		algName = "DMM";
	}

	else if (algID == 11)
	{
		S = Greedy(point_set, eps);
		algName = "Greedy";
	}
	else if (algID == 12)
	{
		S = geoGreedy(point_set, eps);
		algName = "GeoGreedy";
	}
	else if (algID == 13)
	{
		S = ImpGreedy(point_set, eps);
		algName = "ImpGreedy";
	}

	else if (algID == 16)
	{
		S = DMM(point_set, eps);
		algName = "DMM";
	}

	else if (algID == 21)
	{
		S = DMM(point_set, k, 2);
		algName = "DMM (k=2)";
	}
	else if (algID == 22)
	{
		S = DMM(point_set, k, 3);
		algName = "DMM (k=3)";
	}
	else if (algID == 23)
	{
		S = DMM(point_set, k, 4);
		algName = "DMM (k=4)";
	}
	else if (algID == 24)
	{
		S = DMM(point_set, k, 5);
		algName = "DMM (k=5)";
	}
	else if (algID == 24)
	{
		S = DMM(point_set, k, 5);
		algName = "DMM (k=5)";
	}/*
	else if (algID == 25)
	{
		S = minWidth(k, point_set);
		algName = "MinWidth";
	}
	else if (algID == 26)
	{
		S = AreaGreedy(k, point_set);
		algName = "AreaGreedy";
	}
	else if (algID == 27)
	{
		S = angle(k, point_set);
		algName = "Angle";
	}

	else if (algID == 28)
	{
		S = minVar(point_set, k);
		algName = "MinVar";
	}
	else if (algID == 29)
	{
		S = maxDif(point_set, k);
		algName = "MaxDif";
	}
	*/

#ifndef WIN32
	gettimeofday(&end, NULL);
	time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
#else
	end = std::clock();
	time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
#endif
	//print_point_set(S);
	
	
	int dummy;
	
	mrr = evaluateGeo(point_set, S, dummy);
	
	//mrr = evaluateLP(point_set, S, 0);
	//mrr	= evaluateCES(point_set, S, 0);
	//mrr = evaluateCVX(point_set, S, 0);
	//mrr = evaluateCAV(point_set, S, 0);
	//mrr = evaluateMUF(point_set, S, 0);

	if((algID >= 21 && algID <= 24))
		mrr = evaluateKRMS(point_set, S, algID - 19);

	printf("%15s%15lf%15lf%15d\n", algName, time, mrr, S->numberOfPoints);
	stat = alloc_stat(algName, time, mrr, S->numberOfPoints);
	release_point_set(S, false);
}

double getValue(stat_t* stat, int i)
{
	if(i == 0)
		return stat->time;
	else if (i == 1)
		return stat->mrr;
	else if (i == 2)
		return stat->r;
}

void plotdata(vector< vector<stat_t*> > statistic, vector<int> idx, char* name)
{
	if(idx.size() != statistic.size())
	{
		printf("size not matcha!\n");
		exit(0);
	}

	int measure_count = 3;
	char* measurements[] = {"time", "mrr", "size"};

	for(int i = 0; i < measure_count; i++)
	{
		char filename[MAX_FILENAME_LENG];

		FILE *wPtr;
		sprintf(filename, "output/%s%s.dat", name, measurements[i]);
		wPtr = (FILE *)fopen(filename, "w");

		for(int j = 0; j < idx.size(); j++)
		{
			fprintf(wPtr, "%d\t", idx[j]);

			for(int k = 0; k < statistic[j].size(); k++)
			{
				if(k == statistic[j].size() - 1)
					fprintf(wPtr, "%lf\n", getValue(statistic[j][k], i));
				else
					fprintf(wPtr, "%lf\t", getValue(statistic[j][k], i));
			}
		}

		fclose(wPtr);
	}
}

void plotdata(vector< vector<stat_t*> > statistic, vector<double> idx, char* name)
{
	if(idx.size() != statistic.size())
	{
		printf("size not matcha!\n");
		exit(0);
	}

	int measure_count = 3;
	char* measurements[] = {"time", "mrr", "size"};

	for(int i = 0; i < measure_count; i++)
	{
		char filename[MAX_FILENAME_LENG];

		FILE *wPtr;
		sprintf(filename, "output/%s%s.dat", name, measurements[i]);
		wPtr = (FILE *)fopen(filename, "w");

		for(int j = 0; j < idx.size(); j++)
		{
			fprintf(wPtr, "%lf\t", idx[j]);

			for(int k = 0; k < statistic[j].size(); k++)
			{
				if(k == statistic[j].size() - 1)
					fprintf(wPtr, "%lf\n", getValue(statistic[j][k], i));
				else
					fprintf(wPtr, "%lf\t", getValue(statistic[j][k], i));
			}
		}

		fclose(wPtr);
	}
}


void varyR()
{
	FILE* rPtr;

	if ((rPtr = fopen("varyR.config", "r")) == NULL)
	{
		fprintf(stderr, "Cannot open varyK.config \n");
		exit(0);
	}

	char input[MAX_FILENAME_LENG];
	char output[MAX_FILENAME_LENG];

	fscanf(rPtr, "%s", input);
	fscanf(rPtr, "%s", output);

	int num_of_idx;
	fscanf(rPtr, "%i", &num_of_idx);
	vector<int> k;
	for(int i = 0; i < num_of_idx; i++)
	{
		int nextK;
		fscanf(rPtr, "%i", &nextK);
		k.push_back(nextK);
	}

	int num_of_alg;
	fscanf(rPtr, "%i", &num_of_alg);
	vector<int> alg;
	for(int i = 0; i < num_of_alg; i++)
	{
		int nextAlg;
		fscanf(rPtr, "%i", &nextAlg);
		alg.push_back(nextAlg);
	}

	fclose(rPtr);


	/***************************************************/

	point_set_t* P = read_points(input);
	point_set_t* skyline = skyline_point(P);
	
	int dim = P->points[0]->dim;
	
	printf("Vary R ...\n");
	printf("Input: input/%s\n", input);

	vector< vector<stat_t*> > statistic;
	for(int i = 0; i < k.size(); i++)
	{
		vector<stat_t*> statistic_for_one;
		printf("-----------------------------k = %d------------------------------------\n", k[i]);
		printf("%15s%15s%15s%15s\n", "Algorithm", "time", "mrr", "size");

		for(int j = 0; j < alg.size(); j++)
		{
			stat_t* new_stat;
			
			performSingleAlg(skyline, k[i], 1, new_stat, alg[j]);
			statistic_for_one.push_back(new_stat);
		}
		statistic.push_back(statistic_for_one);
	}

	plotdata(statistic, k, output);

	release_point_set(skyline, false);
	release_point_set(P, true);
}

void varyNorD()
{
	FILE* rPtr;

	if ((rPtr = fopen("varyNorD.config", "r")) == NULL)
	{
		fprintf(stderr, "Cannot open varyNorD.config \n");
		exit(0);
	}

	char output[MAX_FILENAME_LENG];

	fscanf(rPtr, "%s", output);

	int num_of_idx;
	fscanf(rPtr, "%i", &num_of_idx);
	vector<char*> inputfiles;
	vector<int> idx;
	for(int i = 0; i < num_of_idx; i++)
	{
		int next_idx;
		fscanf(rPtr, "%i", &next_idx);
		idx.push_back(next_idx);

		char* input = new char[MAX_FILENAME_LENG];
		fscanf(rPtr, "%s", input);
		inputfiles.push_back(input);
	}

	int num_of_alg;
	fscanf(rPtr, "%i", &num_of_alg);
	vector<int> alg;
	for(int i = 0; i < num_of_alg; i++)
	{
		int nextAlg;
		fscanf(rPtr, "%i", &nextAlg);
		alg.push_back(nextAlg);
	}
	
	int k;
	fscanf(rPtr, "%i", &k);
	double epsilon;
	fscanf(rPtr, "%lf", &epsilon);

	fclose(rPtr);


	/***************************************************/

	point_t* u;

	printf("Vary N or D...\n");
	printf("Input: ");
	for(int i = 0; i < idx.size(); i++)
	{
		printf("input/%s ", inputfiles[i]);
	}
	printf("\nk = %d, epsilone = %lf\n", k, epsilon);

	vector< vector<stat_t*> > statistic;
	for(int itr = 0; itr < idx.size(); itr++)
	{
		char* input = inputfiles[itr];

		point_set_t* P = read_points(input);
		point_set_t* skyline = skyline_point(P);
	
		int dim = P->points[0]->dim;

		if(itr == 0)
		{
			u = alloc_point(dim);
			for (int i = 0; i < dim; i++)
				u->coord[i] = ((double)rand()) / RAND_MAX;
		}


		vector<stat_t*> statistic_for_one;
		printf("-----------------------------%s------------------------------------\n", input);
		printf("%15s%15s%15s%15s\n", "Algorithm", "time", "mrr", "size");

		for(int j = 0; j < alg.size(); j++)
		{
			stat_t* new_stat;
			performSingleAlg(skyline, k, epsilon, new_stat, alg[j]);
			statistic_for_one.push_back(new_stat);
		}
		statistic.push_back(statistic_for_one);

		release_point_set(skyline, false);
		release_point_set(P, true);
	}

	plotdata(statistic, idx, output);
}

void varyMRR()
{
	FILE* rPtr;

	if ((rPtr = fopen("varyMRR.config", "r")) == NULL)
	{
		fprintf(stderr, "Cannot open varyMRR.config \n");
		exit(0);
	}

	char input[MAX_FILENAME_LENG];
	char output[MAX_FILENAME_LENG];

	fscanf(rPtr, "%s", input);
	fscanf(rPtr, "%s", output);

	int num_of_idx;
	fscanf(rPtr, "%i", &num_of_idx);
	vector<double> epsilon;
	for(int i = 0; i < num_of_idx; i++)
	{
		double nextEps;
		fscanf(rPtr, "%lf", &nextEps);
		epsilon.push_back(nextEps);
	}

	int num_of_alg;
	fscanf(rPtr, "%i", &num_of_alg);
	vector<int> alg;
	for(int i = 0; i < num_of_alg; i++)
	{
		int nextAlg;
		fscanf(rPtr, "%i", &nextAlg);
		alg.push_back(nextAlg);
	}

	fclose(rPtr);
	/***************************************************/

	point_set_t* P = read_points(input);
	point_set_t* skyline = skyline_point(P);
	
	int dim = P->points[0]->dim;

	printf("Vary MRR ...\n");
	printf("Input: input/%s\n", input);

	vector< vector<stat_t*> > statistic;
	for(int i = 0; i < epsilon.size(); i++)
	{
		vector<stat_t*> statistic_for_one;
		printf("-----------------------------epsilon = %lf------------------------------------\n", epsilon[i]);
		printf("%15s%15s%15s%15s\n", "Algorithm", "time", "mrr", "size");

		for(int j = 0; j < alg.size(); j++)
		{
			stat_t* new_stat;
			performSingleAlg(skyline, 30, epsilon[i], new_stat, alg[j]);
			statistic_for_one.push_back(new_stat);
		}
		statistic.push_back(statistic_for_one);
	}

	plotdata(statistic, epsilon, output);

	release_point_set(skyline, false);
	release_point_set(P, true);
}
