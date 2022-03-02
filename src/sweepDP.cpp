#include "sweepDP.h"

struct xIntersectCmp
{
	bool operator()(const point_t* lhs, const point_t* rhs) const
	{
		return 1 / lhs->coord[0] < 1 / rhs->coord[0];
	}
};

double computeAngle(point_t* p)
{
	return atan(p->coord[1] / p->coord[0]);
}

struct angleCmp
{
	bool operator()(const inter_point_t* lhs, const inter_point_t* rhs) const
	{
		return atan(lhs->pt->coord[1]/lhs->pt->coord[0]) > atan(rhs->pt->coord[1]/rhs->pt->coord[0]);
	}
};

point_t* lineIntersection(point_t* p, point_t* q)
{
	point_t* pt = alloc_point(2);

	pt->coord[0] = (p->coord[1] - q->coord[1]) / (q->coord[0] * p->coord[1] - p->coord[0] * q->coord[1]);
	pt->coord[1] = (p->coord[0] - q->coord[0]) / (p->coord[0] * q->coord[1] - q->coord[0] * p->coord[1]);

	return pt;
}

double computeRatio(point_t* lower, point_t* p, point_t* direction)
{
	double t_lower = 1/(lower->coord[0]* direction->coord[0] + lower->coord[1]*direction->coord[1]);
	double t_p = 1 / (p->coord[0] * direction->coord[0] + p->coord[1] * direction->coord[1]);

	return (t_p - t_lower) / t_p;
}

point_set_t* sweepDP(point_set_t* P, int k)
{
	double eps = 0.0001;

	point_set_t* tmp = skyline_point(P);

	point_set_t* skyline = alloc_point_set(tmp->numberOfPoints);
	for(int i = 0; i < tmp->numberOfPoints; i++)
	{
		skyline->points[i] = copy(tmp->points[i]);
		skyline->points[i]->id= i;
	}

	int n = skyline->numberOfPoints;

	//for(int i = 0; i < skyline->numberOfPoints; i++)
	//	printf("%d \n", skyline->points[i]->id);

	vector<point_t*> lower_envelope;

	vector<point_t*> L;
	for(int i = 0; i < skyline->numberOfPoints; i++)
		L.push_back(skyline->points[i]);
	std::sort(L.begin(), L.end(), xIntersectCmp());
	
	//for(int i = 0; i < L.size(); i++)
	//{
	//	printf("%d ", i);
	//	print_point(L[i]);
	//}

	linked_pt* begin;
	linked_pt * prev_linked_pt = NULL;
	for(int i = 0; i < L.size(); i++)
	{
		linked_pt_t* linked_pt_v  = alloc_linked_pt(L[i]);;

		linked_pt_v->prev = prev_linked_pt;
		
		if(i == 0)
			begin = linked_pt_v;
		else
			prev_linked_pt->next = linked_pt_v;

		prev_linked_pt = linked_pt_v;
	}
	prev_linked_pt->next = NULL;

	//for(linked_pt* itr = begin; itr != NULL; itr = itr->next)
	//{
	//	printf("%d ", count++);
	//	print_point(itr->pt);
	//}
	lower_envelope.push_back(begin->pt);

	int** isAdded = new int*[n];
	for(int i = 0; i < n; i++)
		isAdded[i] = new int[n];
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
			isAdded[i][j] = 0;
	}
	
	priority_queue<inter_point_t*, vector<inter_point_t*>, angleCmp> heap;
	for(linked_pt* itr = begin; itr->next != NULL; itr = itr->next)
	{
		point_t* pt = lineIntersection(itr->pt, itr->next->pt);
		heap.push(alloc_inter_point(pt, itr, itr->next));

		int i = itr->pt->id;
		int j = itr->next->pt->id;
		isAdded[i][j] = 1;
		isAdded[j][i] = 1;
	}

	double** DP = new double*[n];
	for(int i = 0; i < n; i++)
		DP[i] = new double[k];

	vector<vector< vector<int> > > sol;
	sol.resize(n);
	for(int i = 0; i < n; i++)
		sol[i].resize(k);

	for(int i = 0; i < n; i++)
	{
		point_t* direction = alloc_point(2);
		direction->coord[0] = 1;
		direction->coord[1] = 0;

		DP[i][0] = computeRatio(lower_envelope[lower_envelope.size() - 1], skyline->points[i], direction);
		//printf("%d: %lf\n", i, DP[i][0]);
		for(int j = 0; j < k; j++)
		{
			if(j != 0)
				DP[i][j] = DP[i][j-1];

			sol[i][j].push_back(i);
		}

		release_point(direction);
	}

	while(!heap.empty())
	{
		//for(linked_pt* itr = begin; itr->next != NULL; itr = itr->next)
		//{
		//	printf("%3d ", itr->pt->id);
		//}
		//printf("\n");

		inter_point_t* intersection = heap.top();
		heap.pop();

		//printf("L(angle):%lf, ", computeAngle(intersection->pt));

		linked_pt* point_i = intersection->linked_pt_i;
		linked_pt* point_j = intersection->linked_pt_j;

		//printf("swap: %d %d\n", point_i->pt->id, point_j->pt->id);

		//if(point_i->pt->id == 48 && point_j->pt->id == 79)
		//	printf("debug!\n");

		if(point_i->next != point_j || point_j->prev != point_i)
		{
			printf("link error.\n");
			printf("intersection of %d %d:\n", point_i->pt->id, point_j->pt->id);
			print_point(intersection->pt);
			printf("angle: %lf\n",computeAngle(intersection->pt));

			point_t* pt;
			printf("intersection of %d %d:\n", point_i->pt->id, point_i->next->pt->id);
			pt = lineIntersection(point_i->pt, point_i->prev->pt);
			print_point(pt);
			printf("angle: %lf\n", computeAngle(pt));

			printf("intersection of %d %d:\n", point_j->pt->id, point_j->prev->pt->id);
			pt = lineIntersection(point_j->pt, point_j->next->pt);
			print_point(pt);
			printf("angle: %lf\n", computeAngle(pt));
		}

		linked_pt* prev = point_i->prev;
		linked_pt* next = point_j->next;

		if(prev != NULL)
			prev->next = point_j;
		
		point_i->prev = point_j;
		point_i->next = point_j->next;

		point_j->prev = prev;
		point_j->next = point_i;

		if(next != NULL)
			next->prev = point_i;

		
		if(prev != NULL)
		{
			point_t* pt = lineIntersection(prev->pt, point_j->pt);

			if(computeAngle(pt) > computeAngle(intersection->pt) && pt->coord[0] > -eps && pt->coord[1] > -eps)
			{
				int i = prev->pt->id < point_j->pt->id? prev->pt->id: point_j->pt->id;
				int j = prev->pt->id < point_j->pt->id? point_j->pt->id: prev->pt->id;

				if(!isAdded[i][j] && !isAdded[j][i])
				{
					heap.push(alloc_inter_point(pt, prev, point_j));
					isAdded[i][j] = 1;
					isAdded[j][i] = 1;
				}
				else
					release_point(pt);
				//if(prev->pt->id == 48 && point_j->pt->id == 79)
					//printf("add 48 79.\n");
			}
			else
				release_point(pt);
		}


		if(next != NULL)
		{
			point_t* pt = lineIntersection(point_i->pt, next->pt);

			if(computeAngle(pt) > computeAngle(intersection->pt) && pt->coord[0] > -eps && pt->coord[1] > -eps)
			{
				int i = point_i->pt->id < next->pt->id? point_i->pt->id: next->pt->id;
				int j = point_i->pt->id < next->pt->id? next->pt->id: point_i->pt->id;

				if(!isAdded[i][j] && !isAdded[j][i])
				{
					heap.push(alloc_inter_point(pt, point_i, next));
					isAdded[i][j] = 1;
					isAdded[j][i] = 1;
				}
				else
					release_point(pt);
				//if(point_i->pt->id == 48 && next->pt->id == 79)
					//printf("add 48 79.\n");
			}
			else
				release_point(pt);
		}

		//bool debug = false;
		if(prev == NULL)
		{
			//debug = true;
			begin = point_j;
			lower_envelope.push_back(begin->pt);


			for(int i = 0; i < n; i++)
			{
				double newRatio;
				newRatio = computeRatio(lower_envelope[lower_envelope.size() - 1], skyline->points[i], intersection->pt);

				for(int j = 0; j < k; j++)
				{
					if(DP[i][j] < newRatio)
						DP[i][j] = newRatio;
				}
			}

			//printf("|     |");
			//for(int i = 0; i < k; i++)
			//	printf("%7d|", i);
			//printf("\n--------");
			//for(int i = 0; i < k; i++)
			//	printf("-------");

			//for(int i = 0; i < n; i++)
			//{
			//	printf("\n|%5d|", i);
			//	for(int j = 0; j < k; j++)
			//	{
			//		printf("%7.3lf|", DP[i][j]);
			//	}
			//}
			//printf("\n--------");
			//for(int i = 0; i < k; i++)
			//	printf("-------");
			//printf("\n");
		}

		int i = point_i->pt->id;
		int j = point_j->pt->id;

		//if(debug && i == 9 && j == 4)
		//	printf("debug\n");

		for(int l = 0; l < k; l++)
		{
			// update i
			double newRatio;
			newRatio = computeRatio(lower_envelope[lower_envelope.size() - 1], point_i->pt, intersection->pt);
			if(newRatio > DP[i][l])		
				DP[i][l] = newRatio;

			//update j
			double v1, v2;

			if(l > 0)
			{
				v2 = computeRatio(lower_envelope[lower_envelope.size() - 1], point_j->pt, intersection->pt);
				if(DP[j][l] < DP[i][l-1])
				{
					v1 = DP[j][l];
					DP[j][l] = v1 > v2? v1: v2;
				}
				else
				{
					v1 =  DP[i][l-1];
					DP[j][l] = v1 > v2? v1: v2;

					

					//if(debug)
					//{
					//	printf("sol[%d][%d]:", i, l-1);
					//	for(int x = 0; x < sol[i][l-1].size(); x++)
					//		printf("%d ", sol[i][l-1][x]);
					//	printf("\n");
					//}

					sol[j][l].clear();
					for(int x = 0; x < sol[i][l-1].size(); x++)
						sol[j][l].push_back(sol[i][l-1][x]);
					sol[j][l].push_back(j);

					//if(debug)
					//{
					//	printf("sol[%d][%d]:", j, l);
					//	for(int x = 0; x < sol[j][l].size(); x++)
					//		printf("%d ", sol[j][l][x]);
					//	printf("\n");
					//}
					

				}				
			}
			else
			{
				newRatio = computeRatio(lower_envelope[lower_envelope.size() - 1], point_j->pt, intersection->pt);
				if(newRatio > DP[j][l])		
					DP[j][l] = newRatio;
			}

		}

		release_point(intersection->pt);
		release_inter_point(intersection);
	}

	//for(int i = 0; i < lower_envelope.size(); i++)
	//	printf("%d ", tmp->points[lower_envelope[i]->id]->id);
	//printf("\n");

	for(int i = 0; i < n; i++)
	{
		point_t* direction = alloc_point(2);
		direction->coord[0] = 0;
		direction->coord[1] = 1;

		double newRatio = computeRatio(lower_envelope[lower_envelope.size() - 1], skyline->points[i], direction);
		if(DP[i][k-1] < newRatio)
			DP[i][k-1] = newRatio;

		release_point(direction);
	}



	//printf("|     |");
	//for(int i = 0; i < k; i++)
	//	printf("%7d|", i);
	//printf("\n--------");
	//for(int i = 0; i < k; i++)
	//	printf("-------");

	//for(int i = 0; i < n; i++)
	//{
	//	printf("\n|%5d|", i);
	//	for(int j = 0; j < k; j++)
	//	{
	//		printf("%7.3lf|", DP[i][j]);
	//	}
	//}
	//printf("\n--------");
	//for(int i = 0; i < k; i++)
	//	printf("-------");
	//printf("\n");

	double min = 1;
	int minI;
	for(int i = 0; i < n; i++)
	{
		if(DP[i][k-1] < min)
		{
			min = DP[i][k-1];
			minI = i;
		}
	}
	//printf("rr: %lf\n", min);

	point_set_t* S = alloc_point_set(sol[minI][k-1].size());
	for(int i = 0; i < sol[minI][k-1].size(); i++)
	{
		//printf("%d ", tmp->points[sol[minI][k-1][i]]->id);
		S->points[i] = tmp->points[sol[minI][k-1][i]];
	}
	//printf("\n");

	//release
	while(begin != NULL)
	{
		linked_pt* next = begin->next;

		release_linked_pt(begin);
		begin = next;
	}

	for(int i = 0; i < n; i++)
		delete[] isAdded[i];
	delete[] isAdded;

	for(int i = 0; i < n; i++)
		delete[] DP[i];
	delete[] DP;



	//for(int i = 0; i < lower_envelope.size(); i++)
	//	printf("%d ", tmp->points[lower_envelope[i]->id]->id);
	//printf("\n");
	release_point_set(tmp, false);
	release_point_set(skyline, true);
	return S;
}