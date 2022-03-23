//#include "stdafx.h"

#include "operation.h"

// is x equal to 0 ?
int isZero(double x)
{
	return x > -EQN_EPS && x < EQN_EPS;
}

// compute L1 distance
DIST_TYPE calc_l1_dist( point_t* point_v1, point_t* point_v2)
{
	int i, dim;
	DIST_TYPE diff;

	diff = 0;
	
	dim = point_v1->dim;
	for( i=0; i<dim; i++)
	{
		diff += ( DIST_TYPE) abs( point_v1->coord[ i] - point_v2->coord[ i]);
	}

	return diff;
}

/*
 *	Calculate the norm of a given vector
 */
DIST_TYPE calc_len(point_t* point_v)
{
	int i, dim;
	DIST_TYPE diff;

	diff = 0;
	
	dim = point_v->dim;
	for( i=0; i<dim; i++)
	{
		diff += point_v->coord[i] * point_v->coord[i];
	}

	return ( DIST_TYPE)sqrt( diff);

}


/*
 *  Create a new point give the target point
 *  (useful when some point need to be freed)
 */
point_t* copy(point_t * point_v2)
{
	if(point_v2 == NULL)
		return NULL;

	point_t* point_v1 = alloc_point(point_v2->dim);

	for(int i = 0; i< point_v2->dim; i++)
		point_v1->coord[i] = point_v2->coord[i];

	return point_v1;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(point_t* point_v1, point_t* point_v2)
{
	int dim = point_v1->dim;
	double result = 0;

	int i;
	for(i = 0; i < dim; i++)
	{
		result += point_v1->coord[i]*point_v2->coord[i];
	}
	return result;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(point_t* point_v1, double* v)
{
	int dim = point_v1->dim;
	double result = 0;

	int i;
	for(i = 0; i < dim; i++)
	{
		result += point_v1->coord[i]*v[i];
	}
	return result;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(double* v1, double* v2, int dim)
{
	double result = 0;
	int i;
	for (i = 0; i < dim; i++)
	{
		result += v1[i] * v2[i];
	}
	return result;
}

/*
 *	Calculate the subtraction between two points.
 *  Remember to release the returned point to save memory.
 */
point_t* sub(point_t* point_v1, point_t* point_v2)
{
	point_t* result = alloc_point(point_v1->dim);

	int i;
	for(i = 0; i < point_v1->dim; i++)
	{
		result->coord[i] = point_v1->coord[i] - point_v2->coord[i];
	}
	return result;
}

/*
 *	Calculate the addition between two points.
 *  Remember to release the returned point to save memory.
 */
point_t* add(point_t* point_v1, point_t* point_v2)
{
	point_t* result = alloc_point(point_v1->dim);

	int i;
	for(i = 0; i < point_v1->dim; i++)
	{
		result->coord[i] = point_v1->coord[i] + point_v2->coord[i];
	}
	return result;
}

/*
 *	Scale the given point
 *  Remember to release the returned point to save memory.
 */
point_t* scale(double c, point_t* point_v)
{
	point_t* result = alloc_point(point_v->dim);

	int i;
	for(i = 0; i < point_v->dim; i++)
	{
		result->coord[i] = point_v->coord[i] * c;
	}

	return result;
}

// compute the rank of the guass matrix (used for computing frames)
int guassRank(vector< point_t* > P)
{
	int dim = P[0]->dim;

	std::vector<vector<double> > A;
	for(int i = 0; i < P.size(); i++)
	{
		vector<double> v;
		for(int j = 0; j < dim; j++)
		{
			v.push_back(P[i]->coord[j]);
		}
		A.push_back(v);
	}
	while(A.size() < dim)
	{
		vector<double> v;
		for(int j = 0; j < dim; j++)
		{
			v.push_back(0);
		}
		A.push_back(v);

	}
	
	int n = A.size();
	int d = A[0].size();

	//for (int i = 0; i < n; i++)
	//{
	//	for (int j = 0; j < d; j++)
	//		printf("%lf ", A[i][j]);
	//	printf("\n");
	//}
	//printf("\n");

	for (int i = 0; i< d; i++) {
		// Search for maximum in this column
		double maxEl = abs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k<n; k++) {
			if (abs(A[k][i]) > maxEl) {
				maxEl = abs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k<d; k++) {
			double tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}

		// Make all rows below this one 0 in current column
		for (int k = i + 1; k<n; k++) {
			double c = -A[k][i] / A[i][i];
			for (int j = i; j<d; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
	}

	int count = 0;
	for (int i = 0; i < n; i++)
	{
		bool allZero = true;
		for (int j = 0; j < d; j++)
		{
			if (!isZero(A[i][j]))
			{
				allZero = false;
				break;
			}
		}

		if (!allZero)
			count++;
	}
	return count;
}

// check if two vectors are linearly independent (used for computing frames)
bool linearInd(point_t* p, point_t* q)
{
	int dim = p->dim;

	bool pZero = true, qZero = true;
	double ratio;
	for(int i = 0; i < dim; i++)
	{
		if(!isZero(q->coord[i]))
		{
			qZero = false;
			ratio = p->coord[i] / q->coord[i];
		}
		if(!isZero(p->coord[i]))
			pZero = false;
	}

	if(pZero || qZero)
		return true;

	for(int i = 0; i < dim; i++)
	{
		if(!isZero(ratio - p->coord[i] / q->coord[i]))
			return true;
	}

	return false;
}

// compute the intersection betwee a hyperplane and a ray shooting in s
double compute_intersection_len(hyperplane_t *hp, point_t* s)
{
	return calc_len(s) * (hp->offset / (dot_prod(hp->normal, s)));
}

/*
 *	Check whether point e violates the hyperplane
 *  which has the vector (normal_q - normal_p) as normal vector and pass through normal_p
 */
bool  isViolated(point_t* normal_q, point_t* normal_p, point_t* e)
{
	if(isZero(calc_dist(normal_q,normal_p)))
	{
		system("pause");
		return true;
	}

	bool result;
	point_t* temp_normal = sub(normal_q, normal_p);
	point_t* temp = sub(e, normal_p);
	
	//use the dot product to determin the above/below relation
	//double v = dot_prod(temp_normal, temp);

	//printf("%lf", v);
	
	if(dot_prod(temp_normal, temp) > 0 && !isZero(dot_prod(temp_normal, temp)))
		result = true;
	else
		result = false;

	release_point(temp_normal);
	release_point(temp);

	return result;
}

point_t* projectPointsOntoAffineSpace(point_set_t* space, point_t* p)
{
	if (space->numberOfPoints < 1)
	{
		printf("ERROR in pt to affine\n");
		exit(0);
	}
	if (space->numberOfPoints == 1)
		return space->points[0];

	int dim = space->points[0]->dim;

	int n = space->numberOfPoints - 1;
	point_set_t* dirVecs = alloc_point_set(n);
	for (int i = 0; i < n; i++)
		dirVecs->points[i] = sub(space->points[i + 1], space->points[0]);

	//print_point_set(dirVecs);

	for (int i = 1; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			// p_i = p_i - sum_j( dot(p_i, p_j) / |p_j|^2 * p_j)
			double c_j = dot_prod(dirVecs->points[i], dirVecs->points[j]) / dot_prod(dirVecs->points[j], dirVecs->points[j]);

			for (int k = 0; k < dim; k++)
				dirVecs->points[i]->coord[k] = dirVecs->points[i]->coord[k] - c_j * dirVecs->points[j]->coord[k];
		}

	}
	//print_point_set(dirVecs);
	for (int i = 0; i < dirVecs->numberOfPoints; i++)
	{
		double norm = calc_len(dirVecs->points[i]);
		for (int j = 0; j < dim; j++)
			dirVecs->points[i]->coord[j] = dirVecs->points[i]->coord[j] / norm;
	}
	//print_point_set(dirVecs);
	//print_point(proj);

	point_t* tmp = sub(p, space->points[0]);

	point_t* coord = alloc_point(n);
	for (int i = 0; i < n; i++)
		coord->coord[i] = dot_prod(tmp, dirVecs->points[i]);

	for (int i = 0; i < dim; i++)
	{
		tmp->coord[i] = 0;
		for (int j = 0; j < n; j++)
			tmp->coord[i] += coord->coord[j] * dirVecs->points[j]->coord[i];
	}

	point_t* proj = add(tmp, space->points[0]);

	

	release_point(tmp);
	release_point(coord);
	release_point_set(dirVecs, true);

	return proj;
}

vector<double> gaussNtimesD(vector< vector<double> > A) {
	int n = A.size();
	int d = A[0].size() - 1;

	//for (int i = 0; i < n; i++)
	//{
	//	for (int j = 0; j < d + 1; j++)
	//		printf("%f ", A[i][j]);
	//	printf("\n");
	//}
	//printf("\n");

	for (int i = 0; i< d; i++) {
		// Search for maximum in this column
		double maxEl = abs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k<n; k++) {
			if (abs(A[k][i]) > maxEl) {
				maxEl = abs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k<d + 1; k++) {
			double tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}

		// Make all rows below this one 0 in current column
		for (int k = i + 1; k<n; k++) {
			double c = -A[k][i] / A[i][i];
			for (int j = i; j<d + 1; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
	}
/*	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < d + 1; j++)
			printf("%f ", A[i][j]);
		printf("\n");
	}
	printf("\n");
	*/
	//check validity
	int count = 0;
	for (int i = 0; i < n; i++)
	{
		bool allZero = true;
		for (int j = 0; j < d + 1; j++)
		{
			if (!isZero(A[i][j]))
			{
				allZero = false;
				break;
			}
		}

		if (!allZero)
			count++;
	}
	if (count != d)
	{
		//printf("No Solution.\n");
		vector<double> x(d);
		x[d - 1] = -1;
		return x;
	}

	// Solve equation Ax=b for an upper triangular matrix A
	vector<double> x(d);
	
	for (int i = d - 1; i >= 0; i--) {
		x[i] = A[i][d] / A[i][i];
		for (int k = i - 1; k >= 0; k--) {
			A[k][d] -= A[k][i] * x[i];
		}
	}
	
	return x;
}

/*
 *	Calculate the Euclidean distance between two points
 */
DIST_TYPE calc_dist( point_t* point_v1, point_t* point_v2)
{
	int i, dim;
	DIST_TYPE diff;

	diff = 0;
	
	dim = point_v1->dim;
	for( i=0; i<dim; i++)
	{
		double t = point_v1->coord[ i];
		double t2 = point_v2->coord[ i];
		diff += ( DIST_TYPE)pow( point_v1->coord[ i] - point_v2->coord[ i], 2);
	}

	return ( DIST_TYPE)sqrt( diff);
}

void insertOrth(double* &points, int &count, point_t* v)
{
	int dim = v->dim;
	int orthNum = pow(2.0, dim) - 1;

	for(int i = 0; i< dim; i++)
	{
		points[count * dim + i] = v->coord[i];
	}
	count++;



	int startEnumPow;
	for(int i=0; i< orthNum - 1; i++){
		startEnumPow = i+1; //from 1 to n-1
		for(int j=0; j<dim; j++){
			points[count * dim + j] = v->coord[j] * (startEnumPow % 2);
			startEnumPow /= 2;
			//printf("%lf ", startProjData[i][j]);
		}
		count++;
	}


}

point_t* getPoint(point_set_t* p, double* point)
{
	int dim = p->points[0]->dim;

	for(int i = 0; i < p->numberOfPoints; i++)
	{
		bool match = true;
		for(int j = 0; j < dim; j++)
		{
			if(!isZero(p->points[i]->coord[j] - point[j]))
			{
				match = false;
				break;
			
			}
		}
		if(match)
			return p->points[i];
	}
	return NULL;
}

point_t* getPoint(vector<point_t*> P, double* p, int &index)
{
	int dim = P[0]->dim;

	for (int i = 0; i < P.size(); i++)
	{
		bool match = true;
		for (int j = 0; j < dim; j++)
		{
			if (!isZero(P[i]->coord[j] - p[j]))
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			index = i;
			return P[i];
		}		
	}

	index = -1;
	return NULL;
}

// finds the point in p farthest in the direction v
point_t* maxPoint(point_set_t* p, double *v)
{
	int N = p->numberOfPoints;

	int i, maxIndex = 0;
	double max = 0.0;
	
	for(i = 0; i < N; ++i)
		if (dot_prod(p->points[i], v) > max)
		{
			maxIndex = i;
			max = dot_prod(p->points[i], v);
		}

	return p->points[maxIndex];
}

/*
 *	Generate a random float value in [min_v, max_v].
 */
float rand_f( float min_v, float max_v)
{
	float rand_v;

	if( min_v > max_v)
		return 0;

	//srand( time(NULL));

	rand_v = float( rand( )) / RAND_MAX; //rand_v falls in [0,1] randomly.

	return ( max_v - min_v) * rand_v + min_v;
}

/*
*	build the input for catersian product (in the form of vector of vector of double)
*  t is the number of hyperplanes in each family that are used to divide the facet of hypercube (t >= 0)
*  e.g. when dim = 3, t = 0, means no division and each facet has 1 hypercube
*		when dim = 3, t = 1, means each 2-d facet is divided by 2 hyperplan, one for each dimension, resulting in 4 hypercubes on each facet
*/
Vvi build_input(int t, int dim) {

	// the distance between concecutive parallel hyperplanes
	double dist_bet = 1.0 / (t + 1);

	Vvi vvi;
	Vi vi;
	for (int i = 0; i < t + 1; i++)
	{
		//make the points at the center of each hypercube
		vi.push_back(i * dist_bet + dist_bet / 2);
		//cout << i * dist_bet + dist_bet / 2 << endl;;
	}

	// the query points are put on the front facet of hypercubes
	// and therefore, there exists one dimension for each point with value 1.
	// we need another (dim - 1) number of points to define a point in dim-dimensional space
	// and each of this dimension are formed by the same set of value
	for (int i = 0; i < dim - 1; i++)
		vvi.push_back(vi);
	return vvi;
}

/*
*  codes adapted from stack over flow.
*  use recursion to compue the catersian product in the format of vector of vector of double.
*/
void cart_product(
	Vvi& rvvi,  // final result
	Vi&  rvi,   // current result 
	Vvi::const_iterator me, // current input
	Vvi::const_iterator end) // final input
{
	if (me == end) {
		// terminal condition of the recursion. We no longer have
		// any input vectors to manipulate. Add the current result (rvi)
		// to the total set of results (rvvvi).
		rvvi.push_back(rvi);
		return;
	}

	// need an easy name for my vector-of-ints
	const Vi& mevi = *me;
	for (Vi::const_iterator it = mevi.begin();
	it != mevi.end();
		it++) {
		// final rvi will look like "a, b, c, ME, d, e, f"
		// At the moment, rvi already has "a, b, c"
		rvi.push_back(*it);  // add ME
		cart_product(rvvi, rvi, me + 1, end); //add "d, e, f"
		rvi.pop_back(); // clean ME off for next round
	}
}

/*
*	For debug purpose, output the content of a vector of double
*/
std::ostream& operator<<(std::ostream& os, const Vi& vi)
{
	os << "(";
	std::copy(vi.begin(), vi.end(), std::ostream_iterator<double>(os, " "));
	os << ")";
	return os;
}

/*
*	For debug purpose, output the content of a vector of vector of double
*/
std::ostream& operator<<(std::ostream& os, const Vvi& vvi)
{
	os << "(\n";
	for (Vvi::const_iterator it = vvi.begin();
	it != vvi.end();
		it++) {
		os << "  " << *it << "\n";
	}
	os << ")";
	return os;
}

/*
 *	Given a polyhedra formed by d points in facet,
 *  we want to obtain the nearest point in this polyhedra given a query point.
 *  We formalize this problem as a quadratic optimization problem
 *  Remember to release the returned point to save memory.
 */
point_t* find_nearest_inCH(point_set_t* facet, point_t* query, double &dist)
{
	int dim = query->dim;
	double zero = 0.00000001;

	//Use QuadProg++ to solve the quadratic optimization problem
	Matrix<double> G, CE, CI;
	Vector<double> g0, ce0, ci0, x;
	int n, m, p;
	char ch;

	// Matrix G and g0 determine the objective function.
	// min 1/2 x^T*G*x + g0*x
	n = dim * 2;
	G.resize(n, n);
	for (int i = 0; i < n; i++)	
	{
		for (int j = 0; j < n; j++)
		{
				if(i == j && i < dim)
					G[i][j] = 2;
				else if(i == j)
					G[i][j] = zero;
				else
					G[i][j] = 0;
		}
	}
	/*
	std::cout << "G:" << std::endl;;
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			std::cout << G[i][j] << " ";
		}
		std::cout << std::endl;;
	}*/
	
	g0.resize(n);
	for(int i = 0; i < n; i++)
	{
		if(i < dim)
		{
			g0[i] = -2 * query->coord[i];
		}
		else
			g0[i] = 0;
	}
	/*
	std::cout << "g0:" << std::endl;;
	for(int i = 0; i < n; i++)
	{
		std::cout << g0[i] << " ";
	}
	std::cout << std::endl;
	*/

	// CE and ce0 determine the equality constraints
	// CE*x = ce0
	m = dim + 1;
	CE.resize(n, m);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m -1; j++)
		{
			if(i == j)	
				CE[i][j] = -1;
			else if(i >= dim)
			{
				CE[i][j] = facet->points[i - dim]->coord[j];
			}
			else
				CE[i][j] = 0;
		}
	}
	for(int i = 0; i < n; i++)
	{
		if(i < dim)
			CE[i][m-1] = 0;
		else
			CE[i][m-1] = 1;
	}
	/*
	std::cout << "CE:" << std::endl;;
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m; j++)
		{
			std::cout << CE[i][j] << " ";
		}
		std::cout << std::endl;;
	}
	*/
	ce0.resize(m);
	for (int j = 0; j < m; j++)
	{
		if(j != m - 1)
			ce0[j] = 0;
		else
			ce0[j] = -1;
	}
	/*
	std::cout << "ce0:" << std::endl;;
	for(int i = 0; i < m; i++)
	{
		std::cout << ce0[i] << " ";
	}
	std::cout << std::endl;
	*/

	// CI and ci0 determine the inequality constraints
	// CI*x >= ci0
	p = 2 * dim;
	CI.resize(n, p);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < p; j++)
		{
			if(i < dim)	
				CI[i][j] = 0;
			else if(i == j)
			{
				CI[i][j] = -1;
			}
			else if (i - dim == j)
				CI[i][j] = 1;
			else
				CI[i][j] = 0;
		}
	}

	ci0.resize(p);
	for (int j = 0; j < p; j++)
	{
		if(j < dim)
			ci0[j] = 0;
		else
			ci0[j] = 1;
	}

	x.resize(n);

	//invoke solve_quadprog() in QuadProg++ to solve the problem for us
	// result is stored in x
	dist = solve_quadprog(G, g0, CE, ce0, CI, ci0, x);

	//transform x into our struct
	point_t* result = alloc_point(dim);
	for(int i = 0; i < dim; i++)
	{
		dist += query->coord[i] * query->coord[i];
		result->coord[i] = x[i];
	}

	/*
	for(int i = dim; i < n; i++)
	{
		if(isZero(x[i]))
			printf("%lf ", x[i]);
	}
	printf("\n");
	*/

	//printf("f: %f \n", dist);
	//print_point(result);
	//printf("p to p dist: %f \n", pow(calc_dist(result, query),2));
	return result;
}

point_t* findFavorite(point_set_t* p, point_t* u)
{
	point_t* result = p->points[0];
	double best = dot_prod(result, u);

	for(int i = 1; i < p->numberOfPoints; i++)
	{
		double current =  dot_prod(p->points[i], u);
		if (current > best)
		{
			best = current;
			result = p->points[i];
		}

	}
	return result;
}