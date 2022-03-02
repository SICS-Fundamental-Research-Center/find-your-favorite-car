//#include "stdafx.h"

#include "search.h"

bool violationTest(point_t* x, point_t* NN, point_t* e)
{
	return isViolated(x, NN, e);
}

//X must have size dim + 1
void basisComputation(point_set_t*& X, point_t*& NN, point_t* p, point_t* x)
{
	if (!violationTest(x, NN, p))
		return;

	if (X->numberOfPoints == 0)
	{
		X->points[0] = p;
		X->numberOfPoints++;

		NN = copy(p);
		return;
	}

	int d = p->dim;

	X->points[X->numberOfPoints] = p;
	X->numberOfPoints++;
	
	//printf("X:\n");
	//print_point_set(X);
	//printf("NN:\n");
	//print_point(NN);
	//printf("p:\n");
	//print_point(p);
	//printf("x:\n");
	//print_point(x);

	//outer loop
	while (true)
	{
		//if (!violationTest(x, NN, p))
			//return;

		point_t* q_X = projectPointsOntoAffineSpace(X, x);

		//print_point_set(X);

		//print_point(q_X);

 		int m = X->numberOfPoints - 1;

		if (m == 0)
			return;


		//d * (m+1) matrix

		vector<double> line(m + 1, 0);
		vector< vector<double> > A(d, line);
		point_set_t* X_tmp = alloc_point_set(m);


		double maxLambda = -1;
		int maxT = -1;
		//printf("------------------\n");
		//m bounding hyperplanes
		for (int t = 0; t < X->numberOfPoints; t++)
		{
			

			//initialize X_tmp
			for (int i = 0; i < m; i++)
			{
				if(i < t)
					X_tmp->points[i] = X->points[i];
				else
					X_tmp->points[i] = X->points[i+1];
			}

			//print_point_set(X_tmp);

			//set matrix
			for (int i = 0; i < d; i++) {
				for (int j = 0; j < m - 1; j++) {
					A[i][j] = X_tmp->points[j + 1]->coord[i] - X_tmp->points[0]->coord[i];
				}

				A[i][m - 1] = NN->coord[i] - q_X->coord[i];
				A[i][m] =  NN->coord[i] - X_tmp->points[0]->coord[i];
			}

			//print_point(q_X);

			//solve system of linear equations
			vector<double> sol(m);
			sol = gaussNtimesD(A);

			double lambda = sol[m - 1];

			//find the minimum non-negative lambda
			if ((isZero(lambda) || lambda > 0) && maxLambda < -0.5)
			{
				maxLambda = lambda;
				maxT = t;
			}
			else if (maxLambda > -0.5 && lambda > 0  && !isZero(lambda))
			{
				if (lambda < maxLambda)
				{
					maxLambda = lambda;
					maxT = t;
				}
			}
			
		}

		release_point_set(X_tmp, false);

		if (maxLambda > 1 && !isZero(maxLambda - 1))
		{
			// X is indeed the basis
			release_point(NN);
			NN = q_X;
			//print_point(NN);
			return;
		}
		
		//set new X
		for (int i = 0; i < m; i++)
		{
			if (i < maxT)
				X->points[i] = X->points[i];
			else
				X->points[i] = X->points[i + 1];
		}
		X->numberOfPoints--;
		//printf("new X:\n");
		//print_point_set(X);

		//set new NN: NN = NN + (q_X - NN) * lambda 
		//printf("lambda: %lf", maxLambda);
		
		for (int i = 0; i < d; i++)
			NN->coord[i] = NN->coord[i] + (q_X->coord[i] - NN->coord[i]) * maxLambda;
		//print_point(NN);
		//printf("new NN:\n");
		//print_point(NN);

		release_point(q_X);
	}
}

void search_basis(point_set_t* P, point_set_t* T, point_t* NN_T, point_t* x, point_set_t*& X, point_t*& NN_X)
{
	int dim = P->points[0]->dim;
	int count;

	point_set_t* Q = alloc_point_set(P->numberOfPoints);

	Q->numberOfPoints = T->numberOfPoints;
	for (int i = 0; i < T->numberOfPoints; i++)
		Q->points[i] = T->points[i];	
	count = Q->numberOfPoints;

	X = alloc_point_set(dim + 1);
	X->numberOfPoints = T->numberOfPoints;
	for (int i = 0; i < T->numberOfPoints; i++)
		X->points[i] = T->points[i];
	NN_X = copy(NN_T);

	for (int i = 0; i < P->numberOfPoints; i++)
	{
		point_t* p = P->points[i];

		bool isPointInT = false;
		for (int j = 0; j < T->numberOfPoints; j++)
		{
			if (p == T->points[j])
			{
				isPointInT = true;
				break;
			}
		}
		if (isPointInT)
			continue;

		Q->points[count] = p;
		count++;
		Q->numberOfPoints++;

		if (violationTest(x, NN_X, p))
		{
			//if (Q->numberOfPoints == 26 && p->id == 730)
			//{
			//	printf("before:\n");
			//	print_point(NN_X);
			//	print_point_set(X);

			//}


			
			basisComputation(X, NN_X, p, x);
			//if (Q->numberOfPoints == 26 && p->id == 730)
			//{
			//	printf("after:\n");
			//	print_point(NN_X);
			//	print_point_set(X);
			//}
			
			point_set_t* newX;
			point_t* newNN_X;


			search_basis(Q, X, NN_X, x, newX, newNN_X);

			X = newX;
			NN_X = newNN_X;
		}

	}

	T->numberOfPoints = dim + 1;
	release_point_set(T, false);
	release_point(NN_T);

	release_point_set(Q, false);
}

point_set_t* search_basis(point_set_t* point_set_v, point_t* query)
{

	point_set_t* P = point_set_v;
	point_t* x = query;

	int dim = P->points[0]->dim;

	point_set_t* T = alloc_point_set(dim + 1);
	T->numberOfPoints = 1;
	T->points[0] = P->points[0];

	point_t* NN_T = copy(P->points[0]);

	point_set_t* X;
	point_t* NN_X;

	search_basis(P, T, NN_T, x, X, NN_X);

	
	release_point(NN_X);
	

	return X;
}