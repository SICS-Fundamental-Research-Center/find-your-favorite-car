#include "biSearch.h"

double precision = 100000;

typedef struct segment
{
	point_t* pt;

	double s;
	double t;
} segment_t;

segment_t* alloc_segment(point_t* pt)
{
	segment_t* segment_v;

	segment_v = ( segment_t*)malloc( sizeof( segment_t));
	memset( segment_v, 0, sizeof( segment_t));

	segment_v->pt = pt;

	return segment_v;
}

void release_segment(segment_t* &segment_v)
{
	if(segment_v == NULL)
		return;

	free(segment_v);
	segment_v = NULL;
}

struct zero_intersectCmp
{
	bool operator()(const point_t* lhs, const point_t* rhs) const
	{
		return lhs->coord[1] > rhs->coord[1];
	}
};

struct xCmp
{
	bool operator()(const inter_point_t* lhs, const inter_point_t* rhs) const
	{
		return lhs->pt->coord[0] > rhs->pt->coord[0];
	}
};

double compute_f_lambda(point_t* p, double lambda)
{

	return lambda * p->coord[0] + (1-lambda)*p->coord[1];
}

point_t* lineIntersection_biSearch(point_t* p, point_t* q)
{
	point_t* pt = alloc_point(2);

	pt->coord[0] = (q->coord[1] - p->coord[1])/(p->coord[0] - p->coord[1] - q->coord[0] + q->coord[1]);
	pt->coord[1] = compute_f_lambda(p, pt->coord[0]);

	return pt;
}


vector<segment_t*> compute_upper_envelope(point_set_t *P, std::set<int>& candidateTheta)
{
	candidateTheta.clear();

	point_set_t* skyline = alloc_point_set(P->numberOfPoints);
	for(int i = 0; i < P->numberOfPoints; i++)
	{
		skyline->points[i] = copy(P->points[i]);
		skyline->points[i]->id= i;
	}

	int n = skyline->numberOfPoints;

	vector<segment_t*> upper_envelope;

	vector<point_t*> L;
	for(int i = 0; i < skyline->numberOfPoints; i++)
		L.push_back(skyline->points[i]);
	std::sort(L.begin(), L.end(), zero_intersectCmp());

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

	segment_t* segment = alloc_segment(P->points[begin->pt->id]);
	segment->s = 0;
	upper_envelope.push_back(segment);

	int** isAdded = new int*[n];
	for(int i = 0; i < n; i++)
		isAdded[i] = new int[n];
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
			isAdded[i][j] = 0;
	}

	priority_queue<inter_point_t*, vector<inter_point_t*>, xCmp> heap;
	for(linked_pt* itr = begin; itr->next != NULL; itr = itr->next)
	{
		point_t* pt = lineIntersection_biSearch(itr->pt, itr->next->pt);
		heap.push(alloc_inter_point(pt, itr, itr->next));

		int i = itr->pt->id;
		int j = itr->next->pt->id;
		isAdded[i][j] = 1;
		isAdded[j][i] = 1;
	}

	while(!heap.empty())
	{
		inter_point_t* intersection = heap.top();
		heap.pop();

		//printf("lambda:%lf, ", intersection->pt->coord[0]);
		//for(linked_pt* itr = begin; itr->next != NULL; itr = itr->next)
		//{
		//	printf("%3d ", itr->pt->id);
		//}
		//printf("\n");


		linked_pt* point_i = intersection->linked_pt_i;
		linked_pt* point_j = intersection->linked_pt_j;

		//printf("swap: %d %d\n", point_i->pt->id, point_j->pt->id);

		//if(point_i->pt->id == 48 && point_j->pt->id == 79)
		//	printf("debug!\n");

		if(point_i->next != point_j || point_j->prev != point_i)
		{
			printf("link error.\n");
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
			point_t* pt = lineIntersection_biSearch(prev->pt, point_j->pt);

			if(pt->coord[0] > intersection->pt->coord[0] && (pt->coord[0] < 1|| isZero(1 - pt->coord[0])))
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
			point_t* pt = lineIntersection_biSearch(point_i->pt, next->pt);

			if(pt->coord[0] > intersection->pt->coord[0] && (pt->coord[0] < 1|| isZero(1 - pt->coord[0])))
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

		if(prev == NULL)
		{
			upper_envelope[upper_envelope.size() - 1]->t = intersection->pt->coord[0];
			
			begin = point_j;
			segment = alloc_segment(P->points[point_j->pt->id]);
			segment->s = intersection->pt->coord[0];
			upper_envelope.push_back(segment);


			for(int i = 0; i > P->numberOfPoints; i++)
			{
				point_t* pt = P->points[i];

				candidateTheta.insert( compute_f_lambda(pt, intersection->pt->coord[0]) / intersection->pt->coord[1] * precision);
			}

		}

		candidateTheta.insert( intersection->pt->coord[1] / compute_f_lambda(upper_envelope[upper_envelope.size() - 1]->pt, intersection->pt->coord[0]) * precision);

		release_point(intersection->pt);
		release_inter_point(intersection);
	}

	upper_envelope[upper_envelope.size() - 1]->t = 1;

	while(begin != NULL)
	{
		linked_pt* next = begin->next;

		release_linked_pt(begin);
		begin = next;
	}

	for(int i = 0; i < n; i++)
		delete[] isAdded[i];
	delete[] isAdded;

	release_point_set(skyline, true);


	for(int i = 0; i < P->numberOfPoints; i++)
	{
		point_t* pt = P->points[i];

		candidateTheta.insert( pt->coord[1] / compute_f_lambda(upper_envelope[0]->pt, 0) * precision);
		candidateTheta.insert( pt->coord[0] / compute_f_lambda(upper_envelope[upper_envelope.size() - 1]->pt, 1) * precision);
	}

	int cand[] = {687, 508, 925, 406, 442};
	point_set_t* sol = alloc_point_set(5);


	for(int i = 0; i < P->numberOfPoints; i++)
	{
		
		for(int j = 0; j < 5; j++)
		{
			if(P->points[i]->id == cand[j])
				sol->points[j] = P->points[i];
		}
	}
	
	return upper_envelope;
}



void computeInteval(vector<segment_t*> upper_envelope, double theta, point_t* pt, double& s, double& t)
{
	int lb, ub;

	s = -1;
	t = -1;
	//compute left intersection
	lb = 0;
	ub = upper_envelope.size();

	while(lb < ub)
	{
		int mid = (lb + ub) / 2;

		segment_t* segment = upper_envelope[mid];

		//printf("%d [%lf, %lf]\n", segment->pt->id, segment->s, segment->t);

		bool left_boundary_below = compute_f_lambda(segment->pt, segment->s) * theta < compute_f_lambda(pt, segment->s);
		bool right_boundary_below = compute_f_lambda(segment->pt, segment->t) * theta < compute_f_lambda(pt, segment->t);

		if(!left_boundary_below && right_boundary_below)
		{
			point_t* scaledPt = scale(theta, segment->pt);
			point_t* intersection = lineIntersection_biSearch(scaledPt, pt);
			s = intersection->coord[0];

			release_point(scaledPt);
			break;
		}
		else if(left_boundary_below && right_boundary_below)
			ub = mid;
		else if(left_boundary_below && !right_boundary_below)
			ub = mid;
		else
		{
			//check slope
			if(pt->coord[0] - pt->coord[1] < segment->pt->coord[0] -  segment->pt->coord[1])
				ub = mid;
			else
				lb = mid + 1;
		}
	}

	//compute right intersection
	lb = 0;
	ub = upper_envelope.size();

	while(lb < ub)
	{
		int mid = (lb + ub) / 2;


		segment_t* segment = upper_envelope[mid];

		//printf("%d [%lf, %lf]\n", segment->pt->id, segment->s, segment->t);
		bool left_boundary_below = compute_f_lambda(segment->pt, segment->s) * theta < compute_f_lambda(pt, segment->s);
		bool right_boundary_below = compute_f_lambda(segment->pt, segment->t) * theta < compute_f_lambda(pt, segment->t);

		if(left_boundary_below && !right_boundary_below)
		{
			point_t* scaledPt = scale(theta, segment->pt);
			point_t* intersection = lineIntersection_biSearch(scaledPt, pt);
			t = intersection->coord[0];
			release_point(scaledPt);
			break;
		}
		else if(left_boundary_below && right_boundary_below)
			lb = mid + 1;
		else if(!left_boundary_below && right_boundary_below)
			lb = mid + 1;
		else
		{
			//check slope
			if(pt->coord[0] - pt->coord[1] < segment->pt->coord[0] -  segment->pt->coord[1])
				ub = mid;
			else
				lb = mid + 1;
		}
	}

	if(s > 0 && t < -0.5)
		t = 1;
	if(s < -0.5 && t > 0)
		s = 0;
}

struct interval
{
	point_t* p;
	double leftAngle;
	double rightAngle;
};


struct intervalCmp
{
	bool operator()(const interval* lhs, const interval* rhs) const
	{
		if (isZero(lhs->leftAngle - rhs->leftAngle) && isZero(lhs->rightAngle - rhs->rightAngle))
			return lhs->p->id < rhs->p->id;
		else if (isZero(lhs->leftAngle - rhs->leftAngle))
			return lhs->rightAngle > rhs->rightAngle;
		else
			return lhs->leftAngle < rhs->leftAngle;
	}
};

point_set_t* dec_RMS(point_set_t* P, double theta, vector<segment_t*> upper_envelope)
{
	if(isZero(1 - theta))
	{
		point_set_t* S = alloc_point_set(upper_envelope.size());
		for(int i = 0; i < S->numberOfPoints; i++)
			S->points[i] = upper_envelope[i]->pt;

		return S;
	}

	vector<interval *> interval_list;
	//printf("-----\n");
	for(int i = 0; i < P->numberOfPoints; i++)
	{

		point_t* pt = P->points[i];

		//print_point(pt);
		double s, t;
		computeInteval(upper_envelope, theta, pt , s, t);
		//printf("%6d: [%lf, %lf]\n", pt->id, s, t);

		if(s > -0.5 && t > -0.5)
		{
			interval *itvl = (interval*)malloc(sizeof(interval));
			memset(itvl, 0, sizeof(interval));

			itvl->p = pt;
			itvl->leftAngle = s;
			itvl->rightAngle = t;

			interval_list.push_back(itvl);
		}
	}

	std::sort(interval_list.begin(), interval_list.end(), intervalCmp());

	//printf("------\n");
	//for(int i = 0; i < interval_list.size(); i++)
	//{
	//	printf("%6d [%lf, %lf]\n", interval_list[i]->p->id, interval_list[i]->leftAngle, interval_list[i]->rightAngle);
	//}

	// perfrom the interval cover algorithm
	double current;
	vector<interval*> selected;
	
	current = interval_list[0]->rightAngle;
	selected.push_back(interval_list[0]);


	double maxAngle = current;
	int maxI = -1;
	for (int i = 1; i < interval_list.size() && !isZero(current - 1); i++)
	{
		if (current >= interval_list[i]->leftAngle &&  current < interval_list[i]->rightAngle)
		{
			if (maxAngle < interval_list[i]->rightAngle)
			{
				maxAngle = interval_list[i]->rightAngle;
				maxI = i;
			}
		}

		if ((i + 1 < interval_list.size() && interval_list[i + 1]->leftAngle > current) || i == interval_list.size() - 1)
		{
			current = maxAngle;
			selected.push_back(interval_list[maxI]);

			//interval * itvl = interval_list[maxI];
			//printf("selected %d: %lf, %lf\n", itvl->p->id, itvl->leftAngle, itvl->rightAngle);
		}
	}

	point_set_t* S = alloc_point_set(selected.size());
	for (int i = 0; i < selected.size(); i++)
	{
		S->points[i] = selected[i]->p;
		//printf("%d ", selected[i]->p->id);
	}
	//printf("\n");

	for(int i = 0; i < interval_list.size(); i++)
	{
		free(interval_list[i]);
	}

	return S;
}

point_set_t* biSearch(point_set_t* P, int k)
{
	point_set_t* skyline = skyline_point(P);


	std::set<int> candidateTheta;
	vector<segment_t*> upper_envelope = compute_upper_envelope(skyline, candidateTheta);

	//for(int i = 0; i < upper_envelope.size(); i++)
	//{
	//	//print_point(upper_envelope[i]->pt);
	//	printf("%d [%lf, %lf]\n", upper_envelope[i]->pt->id, upper_envelope[i]->s, upper_envelope[i]->t);
	//	//printf("0.9*(%lfx+%lf)\n", (upper_envelope[i]->pt->coord[0] - upper_envelope[i]->pt->coord[1]), upper_envelope[i]->pt->coord[1]);
	//}

	std::set<int>::iterator it;

	vector<double> sortedTheta;
	for (it = candidateTheta.begin(); it != candidateTheta.end(); ++it)
	{
		sortedTheta.push_back((*it) / precision);
		
		//double v = 1 - (*it) / precision;
		//
		//if(v > 0.01 && v < 0.011)
		//	printf("%lf\n", 1 - (*it) / precision);
	}
	
	
	int low = 0;
	int high = sortedTheta.size();

	point_set_t* S;
	while(low != high - 1)
	{
		int mid = (low + high)/2;

		
		double theta = sortedTheta[mid];

		S = dec_RMS(skyline, theta, upper_envelope);

		if(S->numberOfPoints > k)
			high = mid;
		else
			low = mid;

		release_point_set(S, false);
	}

	S = dec_RMS(skyline, sortedTheta[low], upper_envelope);

	//double theta = 0.995;
	//point_set_t* S = dec_RMS(skyline, theta, upper_envelope);

	//double mrr = evaluateLP(skyline, S, 0);
	//printf("mrr = (%lf) < %lf \n", mrr, 1- theta);
	//if(mrr < 1- theta)
	//	printf("correct!\n");
	//else
	//	printf("wrong!\n");

	//print_point_set(S);

	//printf("---------------------------------------------------\n");
	//for(int i = 0; i < skyline->numberOfPoints; i++)
	//{
	//	point_t* pt = skyline->points[i];
	//	if(pt->id == 721 || pt->id == 701)
	//	{
	//		double s, t;
	//		computeInteval(upper_envelope, theta, pt, s, t);

	//		printf("%d [%lf, %lf]\n",pt->id, s, t);
	//		print_point(pt);
	//		printf("(%lfx+%lf)\n", (pt->coord[0] - pt->coord[1]), pt->coord[1]);
	//	}
	//}

	release_point_set(skyline, false);

	return S;

}