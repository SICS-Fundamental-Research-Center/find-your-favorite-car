#include "DMM.h"

#define DMM_PRECISION 100.0

point_set_t* discretize(int gamma, int dim)
{
	int size = pow(gamma, dim - 1.0);
	point_set_t* F = alloc_point_set(size);
	double alpha = PI / 2 / gamma;

	int *theta = new int[dim - 1];
	for (int i = 0; i < dim - 1; i++)
	{
		theta[i] = 1;
	}

	for (int i = 0; i < size; i++)
	{
		//for (int i = 0; i < dim - 1; i++)
		//{
		//	printf("%d ", theta[i]);
		//}
		//printf("\n");
		double r = 1;
		point_t* pt = alloc_point(dim);

		for (int j = dim - 1; j >= 1; j--)
		{
			double angle = (theta[j - 1] - 0.5) * alpha ;
			pt->coord[j] = r * cos(angle);
			double tmp = sin(angle);
			r = r*tmp;
		}
		pt->coord[0] = r;

		F->points[i] = pt;

		for (int j = 0; j < dim - 1; j++)
		{
			if (theta[j] < gamma)
			{
				theta[j]++;
				break;
			}
			else
				theta[j] = 1;
		}

	}

	delete[] theta;
 
	return F;
}

point_set_t* discretize2(int gamma, int dim)
{
	int size = pow(gamma, dim - 1.0);
	point_set_t* F = alloc_point_set(size);
	double alpha = PI / 2 / gamma;

	int *theta = new int[dim - 1];
	for (int i = 0; i < dim - 1; i++)
	{
		theta[i] = 0;
	}

	for (int i = 0; i < size; i++)
	{
		//for (int i = 0; i < dim - 1; i++)
		//{
		//	printf("%d ", theta[i]);
		//}
		//printf("\n");
		double r = 1;
		point_t* pt = alloc_point(dim);

		for (int j = dim - 1; j >= 1; j--)
		{
			double angle = (theta[j - 1]) * alpha ;
			pt->coord[j] = r * cos(angle);
			double tmp = sin(angle);
			r = r*tmp;
		}
		pt->coord[0] = r;

		F->points[i] = pt;

		for (int j = 0; j < dim - 1; j++)
		{
			if (theta[j] < gamma)
			{
				theta[j]++;
				break;
			}
			else
				theta[j] = 0;
		}

	}

	delete[] theta;
 
	return F;
}
double computeRR(point_set_t* point_set, point_t* f, point_t* pt)
{
	double max = 0;
	for (int i = 0; i < point_set->numberOfPoints; i++)
	{
		double temp = dot_prod(f, point_set->points[i]);
		if (temp > max)
			max = temp;
	}

	return 1 - dot_prod(f, pt) / max;
}

bool isSame(int* v1, vector<int> v2)
{
	int m = v2.size();

	for (int i = 0; i < m; i++)
	{
		if (v1[i] != v2[i])
			return false;
	}
	return true;
}

point_set_t* MRST_oracle(double **M, int n, int m, double eps, point_set_t* point_set)
{
	int** M_tmp = new int*[n];
	for (int i = 0; i < n; i++)
	{
		M_tmp[i] = new int[m];
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			M_tmp[i][j] = (M[i][j] < eps + 1/DMM_PRECISION || isZero(M[i][j]-eps-1/DMM_PRECISION))? 1:0;
			//printf("%d ", M_tmp[i][j]);
		}
		//printf("\n");
	}

	vector< vector<int> > M_binary;
	vector<int> candidate;
	for (int i = 0; i < n; i++)
	{
		bool add = true;
		for (int j = 0; j < M_binary.size(); j++)
		{
			if (isSame(M_tmp[i], M_binary[j]))
				add = false;
		}
		if (add)
		{
			vector<int> newRow(m);
			for (int j = 0; j < m; j++)
			{
				newRow[j] = M_tmp[i][j];
			}
			M_binary.push_back(newRow);
			candidate.push_back(i);
		}

	}

	int* count = new int[M_binary.size()];

	for (int i = 0; i < M_binary.size(); i++)
	{
		count[i] = 0;
		for (int j = 0; j < M_binary[i].size(); j++)
			count[i] += M_binary[i][j];

		//printf("%d\n", count[i]);
	}
	int totalCount = m;
	vector<point_t*> selected;
	while (totalCount > 0)
	{
		int maxCount = 0;
		int maxI;
		for (int i = 0; i < candidate.size(); i++)
		{
			if (maxCount < count[i])
			{
				maxI = i;
				maxCount = count[i];
			}
		}

		totalCount -= maxCount;
		//count[maxI] = 0;
		selected.push_back(point_set->points[candidate[maxI]]);

		for (int i = 0; i < m; i++)
		{
			if (M_binary[maxI][i] == 1)
			{
				for (int j = 0; j < candidate.size(); j++)
				{
					if (M_binary[j][i] == 1)
					{
						M_binary[j][i] = 0;
						count[j]--;
					}
				}
			}
		}

	}

	point_set_t* S = alloc_point_set(selected.size());

	for (int i = 0; i < selected.size(); i++)
	{
		S->points[i] = selected[i];
	}

	delete[] count;
	for (int i = 0; i < n; i++)
	{
		delete[] M_tmp[i];
	}
	delete[] M_tmp;

	return S;
}

point_set_t* MRST_oracle2(double **M, int n, int m, double eps, point_set_t* point_set)
{
	int** M_tmp = new int*[n];
	for (int i = 0; i < n; i++)
	{
		M_tmp[i] = new int[m];
	}

	int* count = new int[n];
	//printf("----eps: %lf----\n", eps);
	for (int i = 0; i < n; i++)
	{
		count[i] = 0;
		for (int j = 0; j < m; j++)
		{
			M_tmp[i][j] = (M[i][j] < eps + 1/DMM_PRECISION || isZero(M[i][j]-eps-1/DMM_PRECISION))? 1:0;
			//printf("%d ", M_tmp[i][j]);
			count[i]+= M_tmp[i][j];
		}
		//printf("\n");
	}

	
	int totalCount = m;
	vector<point_t*> selected;
	while (totalCount > 0)
	{
		//for (int i = 0; i < n; i++)
		//{

		//	for (int j = 0; j < m; j++)
		//	{
		//		printf("%d ", M_tmp[i][j]);
		//	}
		//	printf("\n");
		//}

		int maxCount = 0;
		int maxI;
		for (int i = 0; i < n; i++)
		{
			if (maxCount < count[i])
			{
				maxI = i;
				maxCount = count[i];
			}
		}
		//printf("\n");

		totalCount -= maxCount;
		//count[maxI] = 0;
		selected.push_back(point_set->points[maxI]);

		for (int i = 0; i < m; i++)
		{
			if (M_tmp[maxI][i] == 1)
			{
				for (int j = 0; j < n; j++)
				{
					if (M_tmp[j][i] == 1)
					{
						M_tmp[j][i] = 0;
						count[j]--;
					}
				}
			}
		}

	}

	point_set_t* S = alloc_point_set(selected.size());

	for (int i = 0; i < selected.size(); i++)
	{
		S->points[i] = selected[i];
	}

	delete[] count;
	for (int i = 0; i < n; i++)
	{
		delete[] M_tmp[i];
	}
	delete[] M_tmp;

	return S;
}
point_set_t* DMM(point_set_t* point_set, int k)
{
	//std::clock_t    start, end;
	//double time;

	//start = std::clock();

	int dim = point_set->points[0]->dim;
	int gamma = 4;

	point_set_t*F = discretize(gamma, dim);
	//print_point_set(F);

	int n = point_set->numberOfPoints;
	int m = F->numberOfPoints;

	double** M = new double*[n];
	for (int i = 0; i < n; i++)
	{
		M[i] = new double[m];
	}

	for (int j = 0; j < m; j++)
	{
		double max = 0;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			double temp = dot_prod(F->points[j], point_set->points[i]);
			if (temp > max)
				max = temp;
		}

		for (int i = 0; i < n; i++)
		{
			
			M[i][j] = 1 - dot_prod(F->points[j], point_set->points[i]) / max;
			//printf("%lf ", M[i][j]);
		}
		//printf("\n");
	}

	//for (int i = 0; i < n; i++)
	//{
	//	for (int j = 0; j < m; j++)
	//		printf("%lf ", M[i][j]);
	//	printf("\n");
	//}
	//end = std::clock();
	//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
	//printf("init matrix: %lf\n", time);

	//start = std::clock();

	std::set<int> sortedV_set;
	std::set<int>::iterator it;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			double value = M[i][j];
			double temp2 = M[i][j] * DMM_PRECISION;
			sortedV_set.insert( M[i][j] * DMM_PRECISION);
		}
	}
	vector<double> sortedV;
	for (it = sortedV_set.begin(); it != sortedV_set.end(); ++it)
	{
		sortedV.push_back((*it) / DMM_PRECISION);
	}
	//printf("\n");

	/*vector<double> sortedV;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			double value = M[i][j];
			bool add = true;
			for (int k = 0; k < sortedV.size(); k++)
			{
				if (isZero(value - sortedV[k]))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				sortedV.push_back(value);
			}
		}
	}*/
	/*std::sort(sortedV.begin(), sortedV.begin() + sortedV.size());*/
	//end = std::clock();
	//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
	//printf("sort: %lf\n", time);

	//start = std::clock();
	//for (int i = 0; i < sortedV.size(); i++)
	//	printf("%lf ", sortedV[i]);
	//printf("\n");

	int low = 0, high = sortedV.size();
	while (low < high)
	{
		
		int mid = (low + high) / 2;

		point_set_t* tmp = MRST_oracle2(M, n, m, sortedV[mid], point_set);

		if (tmp->numberOfPoints <= k)
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
		//release_point_set(tmp, false);
		//end = std::clock();
		//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
		//printf("1 itr: %lf\n", time);

		//start = std::clock();
	}
	point_set_t* result = MRST_oracle2(M, n, m, sortedV[low], point_set);

	for (int i = 0; i < n; i++)
	{
		delete[] M[i];
	}
	delete[] M;

	//printf("Size: %d\n", result->numberOfPoints);
	//print_point_set(result);
	
	return result;
}

point_set_t* DMM(point_set_t* point_set, int k, int kRank)
{
	//std::clock_t    start, end;
	//double time;

	//start = std::clock();

	int dim = point_set->points[0]->dim;
	int gamma = 4;

	point_set_t*F = discretize(gamma, dim);
	//print_point_set(F);

	int n = point_set->numberOfPoints;
	int m = F->numberOfPoints;

	double** M = new double*[n];
	for (int i = 0; i < n; i++)
	{
		M[i] = new double[m];
	}

	for (int j = 0; j < m; j++)
	{
		vector<double> kRankV;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			double temp = dot_prod(F->points[j], point_set->points[i]);
			kRankV.push_back(temp);
			sort(kRankV.begin(), kRankV.end(), greater<double>() );
			if(kRankV.size() > kRank)
				kRankV.pop_back();
		}

		for (int i = 0; i < n; i++)
		{
			
			M[i][j] = 1 - dot_prod(F->points[j], point_set->points[i]) / kRankV[kRankV.size() -1];
			M[i][j] = M[i][j] < 0? 0: M[i][j];
			//printf("%lf ", M[i][j]);
		}
		//printf("\n");
	}

	//for (int i = 0; i < n; i++)
	//{
	//	for (int j = 0; j < m; j++)
	//		printf("%lf ", M[i][j]);
	//	printf("\n");
	//}
	//end = std::clock();
	//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
	//printf("init matrix: %lf\n", time);

	//start = std::clock();

	std::set<int> sortedV_set;
	std::set<int>::iterator it;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			double value = M[i][j];
			sortedV_set.insert( M[i][j] * DMM_PRECISION);
		}
	}
	vector<double> sortedV;
	for (it = sortedV_set.begin(); it != sortedV_set.end(); ++it)
	{
		sortedV.push_back((*it) / DMM_PRECISION);
		//printf("%lf ", (*it) / 100.0);
	}
	//printf("\n");

	/*vector<double> sortedV;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			double value = M[i][j];
			bool add = true;
			for (int k = 0; k < sortedV.size(); k++)
			{
				if (isZero(value - sortedV[k]))
				{
					add = false;
					break;
				}
			}
			if (add)
			{
				sortedV.push_back(value);
			}
		}
	}*/
	/*std::sort(sortedV.begin(), sortedV.begin() + sortedV.size());*/
	//end = std::clock();
	//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
	//printf("sort: %lf\n", time);

	//start = std::clock();
	//for (int i = 0; i < sortedV.size(); i++)
	//	printf("%lf ", sortedV[i]);
	//printf("\n");

	int low = 0, high = sortedV.size();
	while (low < high)
	{
		
		int mid = (low + high) / 2;

		point_set_t* tmp = MRST_oracle2(M, n, m, sortedV[mid], point_set);

		if (tmp->numberOfPoints <= k)
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
		//release_point_set(tmp, false);
		//end = std::clock();
		//time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
		//printf("1 itr: %lf\n", time);

		//start = std::clock();
	}
	point_set_t* result = MRST_oracle2(M, n, m, sortedV[low], point_set);

	for (int i = 0; i < n; i++)
	{
		delete[] M[i];
	}
	delete[] M;

	//printf("Size: %d\n", result->numberOfPoints);
	//print_point_set(result);
	
	return result;
}


point_set_t* DMM(point_set_t* point_set, double eps)
{
	//std::clock_t    start, end;
	//double time;

	//start = std::clock();

	int dim = point_set->points[0]->dim;
	int gamma = 4;

	point_set_t*F = discretize(gamma, dim);
	//print_point_set(F);

	int n = point_set->numberOfPoints;
	int m = F->numberOfPoints;

	double** M = new double*[n];
	for (int i = 0; i < n; i++)
	{
		M[i] = new double[m];
	}

	for (int j = 0; j < m; j++)
	{
		double max = 0;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			double temp = dot_prod(F->points[j], point_set->points[i]);
			if (temp > max)
				max = temp;
		}

		for (int i = 0; i < n; i++)
		{
			
			M[i][j] = 1 - dot_prod(F->points[j], point_set->points[i]) / max;
			//printf("%lf ", M[i][j]);
		}
		//printf("\n");
	}

	point_set_t* result = MRST_oracle2(M, n, m, eps, point_set);

	for (int i = 0; i < n; i++)
	{
		delete[] M[i];
	}
	delete[] M;

	//printf("Size: %d\n", result->numberOfPoints);
	//print_point_set(result);
	
	return result;
}

point_set_t* DMM_Greedy(point_set_t* point_set, int k)
{
	int dim = point_set->points[0]->dim;
	int gamma = 4;

	point_set_t*F = discretize(gamma, dim);
	//print_point_set(F);

	int n = point_set->numberOfPoints;
	int m = F->numberOfPoints;

	double** M = new double*[n];
	for (int i = 0; i < n; i++)
	{
		M[i] = new double[m];
	}

	for (int j = 0; j < m; j++)
	{
		double max = 0;
		for (int i = 0; i < point_set->numberOfPoints; i++)
		{
			double temp = dot_prod(F->points[j], point_set->points[i]);
			if (temp > max)
				max = temp;
		}

		for (int i = 0; i < n; i++)
		{
			
			M[i][j] = 1 - dot_prod(F->points[j], point_set->points[i]) / max;
			//printf("%lf ", M[i][j]);
		}
		//printf("\n");
	}




	if(k < dim)
	{
		printf("k < dim. \n");
		exit(0);
	}

	point_set_t* result = alloc_point_set(k);

	int* activeI = new int[n];
	for(int i = 0; i < n; i++)
		activeI[i] = 1;
	int* activeJ = new int[m];
	for(int j = 0; j < m; j++)
		activeI[j] = 1;

	double* b_value = new double[dim];
	int* b_index = new int[dim];

	for (int j = 0; j < dim; j++)
	{
		b_value[j] = 0;
		for (int i = 0; i < n; i++)
		{
			if (point_set->points[i]->coord[j] > b_value[j])
			{
				b_value[j] = point_set->points[i]->coord[j];
				b_index[j] = i;
			}
		}
	}

	for(int i = 0; i < dim; i++)
	{
		result->points[i] = point_set->points[b_index[i]];
		activeI[b_index[i]] = 0;
	}
	delete[] b_value;
	delete[] b_index;

	int count = dim;


	//printf("-----------------\n");
	//for(int i = 0; i < m; i++)
	//{
	//	printf("%8d ", i);
	//}
	//printf("\n");
	//for(int i = 0; i < n; i++)
	//{
	//	for(int j = 0; j < m; j++)
	//	{
	//		//if(activeJ[j])
	//		//	continue;
	//		printf("%lf ", M[i][j]);
	//	}
	//	printf("\n");
	//}
	//printf("-----------------\n");

	while(count < k)
	{
		
		//for(int i = 0; i < n; i++)
		//{
		//	if(activeI[i])
		//		continue;
		//	for(int j = 0; j < m; j++)
		//	{
		//		//if(activeJ[j])
		//		//	continue;
		//		printf("%lf ", M[i][j]);
		//	}
		//	printf("\n");
		//}
		//printf("Min values;\n");
		double max = -1;
		int maxJ = -1;
		for(int j = 0; j < m; j++)
		{
			//if(activeJ[j])
			//	continue;

			int minI = -1;
			double min = INF;
			for(int i = 0; i < n; i++)
			{
				if(activeI[i])
					continue;

				if(M[i][j] < min)
				{
					min = M[i][j];
					minI = i;
				}
			}

			//printf("%lf ", min);
			
			if(max < min)
			{
				max = min;
				maxJ = j;
			}
		}
		//printf("\n maxJ: %d, max: %lf\n", maxJ, max);



		double selectMin = INF;
		int selectI = -1;
		for(int i = 0; i < n; i++)
		{
			if(!activeI[i])
				continue;

			if(selectMin > M[i][maxJ])
			{
				selectMin = M[i][maxJ];
				selectI = i;
			}
		}

		//printf("selectMin: %lf, selectI: %d\n", selectMin, selectI);

		result->points[count++] = point_set->points[selectI];
		activeI[selectI] = 0;
		activeJ[maxJ] = 0;



		//double max = -1;
		//int maxI = -1, maxIJ = -1;

		//for(int i = 0; i < n; i++)
		//{
		//	if(!activeI[i])
		//		continue;

		//	double min = INF;
		//	int minJ = -1;
		//	for(int j = 0; j < m; j++)
		//	{
		//		if(!activeJ[j])
		//			continue;

		//		if(min > M[i][j])
		//		{
		//			min = M[i][j];
		//			minJ = j;
		//		}
		//	}

		//	if(max < min)
		//	{
		//		max = min;
		//		maxI = i;
		//		maxIJ = minJ;
		//	}
		//}

		//for(int i = 0; i < )

		//result->points[count++] = point_set->points[maxI];
		//activeI[maxI] = 0;
		//activeJ[maxIJ] = 0;
	}


	return result;
}