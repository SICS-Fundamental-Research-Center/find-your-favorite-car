//#include "stdafx.h"

#include "construct.h"




/*
Vvi build_inpu_asym(int k, int dim) {


	// the distance between concecutive parallel hyperplanes
   double dist_bet = 1.0 / (t + 1);
   
   Vvi vvi;
   Vi vi;
   for(int i = 0; i < t + 1; i++)
   {
	   //make the points at the center of each hypercube
		vi.push_back(i * dist_bet + dist_bet / 2);
		//cout << i * dist_bet + dist_bet / 2 << endl;;
	}
	
   // the query points are put on the front facet of hypercubes
   // and therefore, there exists one dimension for each point with value 1.
   // we need another (dim - 1) number of points to define a point in dim-dimensional space
   // and each of this dimension are formed by the same set of value
   for(int i = 0; i < dim - 1; i++) 
      vvi.push_back(vi);
   return vvi;
}
*/




/*
 *	The general way to construct I in arbitrary dimension.
 */
point_set_t* construct_I(int dim, int k)
{
	// we assume k >= dim
	if(k < dim)
	{
		printf("dimension cannot be larger than k.\n");
		exit(0);
	}
	
	if(k < dim * dim)
	{
		point_t* p = alloc_point(dim);
		for(int i = 0; i < dim; i++)
			p->coord[i] = (sqrt(dim * 1.0) + 1) / sqrt(dim * 1.0);
		
		point_set_t* I = alloc_point_set(1);
		I->points[0] = p;
		return I;
	}

	// t is the number of hyperplanes in each family that are used to divide the facet of hypercube (t >= 0)
	// we assume that k = d * d * (t + 1)^(d - 1), if not, round it down
	// therefore, real_k <= k is the maximum int satisfies our requirement
	int t = pow(k / dim * 1.0 / dim, 1.0 / (dim - 1)) - 1;
	int real_k = dim * pow(t + 1,  dim - 1.0);

	point_set_t* I = alloc_point_set(real_k);
	int index = 0;

	// build the input
	Vvi input = build_input(t, dim);
	//std::cout << input << "\n\n";

	// obtain the output which is the catersian product of (dim-1) set of doubles
	Vvi output;
	Vi outputTemp;
	cart_product(output, outputTemp, input.begin(), input.end());
	//std::cout << output << "\n\n";
	//std::cout << output.at(0).at(0) << "\n";

	// with the (dim - 1) coordinates in hands, we let the remaining one coordinate to be 1
	// there are dim number of choices
	point_t* p = alloc_point(dim);
	for(int i = 0; i < dim; i++)
	{
		for(int j = 0; j < output.size(); j++)
		{
			
			Vi temp_p = output.at(j);
			for(int l = 0; l < dim; l++)
			{
				if(l < i)
					p->coord[l] = temp_p.at(l);
				else if(l == i)
					p->coord[l] = 1; // the i-th dimension is set to be 1
				else
					p->coord[l] = temp_p.at(l - 1);
			}
			//print_point(p);
			// put the resulting point in I, remenber to scale the point so that it lies on the sphere of radius (1 + sqrt(dim))

			//satisfy the proof
			//I->points[index++] = scale( (1 + sqrt(dim * 1.0))/calc_len(p), p);

			//avoid the nearest point lie on the boundary and waste the last point in facet
			I->points[index++] = scale( (sqrt(dim * 1.0))/calc_len(p), p);
		}
	}
	release_point(p);

	//printf("\n");
	//print_point_set(I);

	return I;
}
