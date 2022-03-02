//#include "stdAfx.h"

#include "lp.h"

#include <vector>

//#define DEBUG_LP

// Takes an array of points s (of size N) and  a point pt and returns
// the direction in which pt has the worst regret ratio (in array v)
// as well as this regret ratio itself. 

/* We solve the following LP with col variables v[0], v[1], ..., v[D], x 
   and row variables q_1 ... q_{K-1}, r1, r_2
   
   Max x
   s.t. - (pt.a[0]-s[0].a[0])v[0] - (pt.a[1]-s[0].a[1])v[1] - ... - (pt.a[D-1]-s[0].a[D-1])v[D-1] + x = q_1
        - (pt.a[0]-s[1].a[0])v[0] - (pt.a[1]-s[1].a[1])v[1] - ... - (pt.a[D-1]-s[1].a[D-1])v[D-1] + x = q_2
        ...
        - (pt.a[0]-s[K-1].a[0])v[0] - (pt.a[1]-s[K-1].a[1])v[1] - ... - (pt.a[D-1]-s[K-1].a[D]-1)v[D-1] + x= q_K
           pt.a[0]v[0] + pt.a[1]v[1]  ....  + pt.a[D-1]v[D-1] = r1
          -pt.a[0]v[0] - pt.a[1]v[1]  ....  - pt.a[D-1]v[D-1] = r2
   variables have the following bounds
       0 <= v[j] < infty
       0 <= x < infty
       -infty < q_i  <=0
       -infty < r1 <= 1
       -infty < r2 <= -1
*/


// Use LP to check whehter a point pt is a conical combination of the vectors in ExRays
bool insideCone(std::vector<point_t*> ExRays, point_t* pt)
{
	int M = ExRays.size();
	int D = pt->dim;

	int* ia = new int[1 + D * M];  //TODO: delete
	int* ja = new int[1 + D * M];  //TODO: delete
	double* ar = new double[1 + D * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "inside_cone");
	glp_set_obj_dir(lp, GLP_MAX);


	glp_add_rows(lp, D);  // add D rows: q_1...q_D
							  // Add rows q_1 ... q_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_FX, pt->coord[i - 1], pt->coord[i-1]); // qi = pt->coord[i-1]
	}
	

	glp_add_cols(lp, M);    // add D columns: v[1] ... v[D]
								// Add col v[1] ... v[D]
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);

		glp_set_col_name(lp, i, buf);
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
		glp_set_obj_coef(lp, i, 0.0);  // objective: 0
	}

	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= D; i++) {
		for (j = 1; j <= M; j++) {

			ia[counter] = i; ja[counter] = j;
			ar[counter++] = ExRays[j-1]->coord[i-1]; 
		}
	}



	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);



										  // running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 

	glp_simplex(lp, &parm);

	bool isOutside = glp_get_status(lp) == GLP_NOFEAS;

	//printf("solution is optimal: %d\n", GLP_OPT);
	//printf("no feasible solution: %d\n", GLP_NOFEAS);


	//printf("return: %d\n", result);
	//for (i = 0; i < M; i++)
	//{
	//	double v = glp_get_col_prim(lp, i + 1);
	//	printf("w%d = %lf\n", i + 1, v);
	//}

	glp_delete_prob(lp); // clean up
	delete[]ia;
	delete[]ja;
	delete[]ar;

	return !isOutside;
}


// Use LP to find a feasible point of the half sapce intersection (used later in Qhull for half space intersection)
point_t* find_feasible(std::vector<hyperplane_t*> hyperplane)
{
	int M = hyperplane.size();
	int D = hyperplane[0]->normal->dim;

	// D + 2variables: D for dim, 2 for additional var for feasible
	int* ia = new int[1 + (D + 2) * M];  //TODO: delete
	int* ja = new int[1 + (D + 2) * M];  //TODO: delete
	double* ar = new double[1 + (D + 2) * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "find_feasible");
	glp_set_obj_dir(lp, GLP_MAX);


	glp_add_rows(lp, M);  // add D rows: q_1...q_D
							  // Add rows q_1 ... q_D
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_UP, 0, 0); // qi = 0
	}
	

	glp_add_cols(lp, D + 2);    // add D columns: v[1] ... v[D]
								// Add col v[1] ... v[D]
	for (i = 1; i <= D + 2; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);

		glp_set_col_name(lp, i, buf);

		if(i <= D)
			glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0); // -infty <= v[i] < infty
		else if (i == D + 1)
			glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
		else
			glp_set_col_bnds(lp, i, GLP_UP, 0.0, D+1);

		if(i == D + 2)
			glp_set_obj_coef(lp, i, 1);  // objective: 0
		else
			glp_set_obj_coef(lp, i, 0.0);  // objective: 0
	}


	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= M; i++) {
		for (j = 1; j <= D + 2; j++) {

			ia[counter] = i; ja[counter] = j;
			
			if(j <= D)
			{
				ar[counter++] = hyperplane[i-1]->normal->coord[j-1];
				//printf("%lf ", hyperplane[i-1]->normal->coord[j-1]);
			}
			else if (j == D+1)
			{
				ar[counter++] = hyperplane[i-1]->offset;
				//printf("%lf ", hyperplane[i-1]->offset);
			}
			else if (j == D+2)
			{
				ar[counter++] = 1;
				//printf("1.00000\n");
			}
		}
	}

	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);

										  // running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 

	glp_simplex(lp, &parm);


	point_t* feasible_pt = alloc_point(D);
	double w1, w2;
	w1 = glp_get_col_prim(lp, D+1);
	w2 = glp_get_col_prim(lp, D+1);

	if(w1 < 0 || w2 < 0 || isZero(w1) || isZero(w2))
	{
		printf("LP feasible error.\n");
		return NULL;
	}
	for (i = 0; i < D; i++)
	{
		double v = glp_get_col_prim(lp, i + 1);
		//printf("w%d = %lf\n", i + 1, v);
		feasible_pt->coord[i] = v / w1;
	}

	//printf("solution status: %d\n",glp_get_status(lp));
	//printf("solution is unbounded: %d\n", GLP_UNBND);
	//printf("solution is optimal: %d\n", GLP_OPT);
	//printf("no feasible solution: %d\n", GLP_NOFEAS);
	//printf("return: %lf\n", glp_get_obj_val(lp));
	//for (i = 0; i < D + 2; i++)
	//{
	//	double v = glp_get_col_prim(lp, i + 1);
	//	printf("w%d = %lf\n", i + 1, v);
	//}

	glp_delete_prob(lp); // clean up
	delete[]ia;
	delete[]ja;
	delete[]ar;

	return feasible_pt;
}

// solve the LP in frame computation
void solveLP(std::vector<point_t*> B, point_t* b, double& theta, point_t* & pi)
{
	int M = B.size()+1;
	int D = b->dim;

	point_t* mean = alloc_point(D);
	for(int i = 0; i < D; i++)
		mean->coord[i] = 0;
	for(int i = 0; i < B.size(); i++)
	{
		for(int j = 0; j < D; j++)
			mean->coord[j] += B[i]->coord[j];
	}
	for(int i = 0; i < D; i++)
		mean->coord[i] /= B.size();

	int* ia = new int[1 + D * M];  //TODO: delete
	int* ja = new int[1 + D * M];  //TODO: delete
	double* ar = new double[1 + D * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "sloveLP");
	glp_set_obj_dir(lp, GLP_MIN);


	glp_add_rows(lp, D);  // add D rows: q_1...q_D
							  // Add rows q_1 ... q_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_FX, b->coord[i - 1], b->coord[i-1]); // qi = pt->coord[i-1]
	}
	

	glp_add_cols(lp, M);    // add D columns: v[1] ... v[D]
								// Add col v[1] ... v[D]
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);

		glp_set_col_name(lp, i, buf);
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty

		if(i == 1)
			glp_set_obj_coef(lp, i, 1); 
		else
			glp_set_obj_coef(lp, i, 0.0); 
	}

	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= D; i++) {
		for (j = 1; j <= M; j++) {

			if(j == 1)
			{
				ia[counter] = i; ja[counter] = j;
				ar[counter++] = -mean->coord[i-1]; 
			}
			else
			{
				ia[counter] = i; ja[counter] = j;
				ar[counter++] = B[j-2]->coord[i-1]; 
			}
		}
	}



	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);



										  // running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 
	
	glp_simplex(lp, &parm);


	//printf("solution is optimal: %d\n", GLP_OPT);
	//printf("no feasible solution: %d\n", GLP_NOFEAS);
	bool feasible = glp_get_status(lp) == GLP_NOFEAS;

	theta = glp_get_obj_val(lp);
	for(int i = 0; i < D; i++)
	{
		pi->coord[i] = glp_get_row_dual(lp, i + 1);
	}
	//printf("return primal: %lf\n", glp_get_obj_val(lp));
	//for (i = 0; i < M; i++)
	//{
	//	double v = glp_get_col_prim(lp, i + 1);
	//	printf("p%d = %lf\n", i + 1, v);
	//}

	//printf("return dual: \n");
	//double dot_v = 0;
	//for (i = 0; i < D; i++)
	//{
	//	double v = glp_get_row_dual(lp, i + 1);
	//	dot_v += v * b->coord[i];
	//	printf("d%d = %lf\n", i + 1, v);
	//}
	//printf("dual objective: %lf\n", dot_v);

	glp_delete_prob(lp); // clean up
	delete[]ia;
	delete[]ja;
	delete[]ar;

	//printf("LP-verify:\n");
	//if(theta > 0 && insideCone(B, b) || isZero(theta) && !insideCone(B, b))
	//	printf("In-out error.\n");
	//if(!isZero(dot_prod(pi, b) - theta))
	//	printf("Objective value error.\n");
	//for(int i = 0; i < B.size(); i++)
	//{
	//	double v = dot_prod(B[i], pi);
	//	if(v >0 && !isZero(v))
	//		printf("Hyperplane error.\n");
	//}
	//double v = dot_prod(mean,pi);
	//if(v > -1 && !isZero(v+1))
	//	printf("Mean vector error.\n");

	release_point(mean);
}


double worstDirection(point_set_t *s, point_t* pt, double* &v)
{
	int K = s->numberOfPoints;
	int D = pt->dim;

    int* ia = new int[1+(K+5)*(D+5)];  //TODO: delete
	int* ja = new int[1+(K+5)*(D+5)];  //TODO: delete
    double* ar = new double[1+(K+5)*(D+5)];   //TODO: delete
    int i, j;   
    double epsilon=0.0000000000001; 


    glp_prob *lp;
    lp=glp_create_prob(); 
    glp_set_prob_name(lp, "max_regret_ratio");
    glp_set_obj_dir(lp, GLP_MAX);
    
 
    glp_add_rows(lp, K+2);  // add K+2 rows: q_1...q_k and r_1 and r_2
    // Add rows q_1 ... q_K 
    for (i=1; i<=K; i++) {
        char buf[10]; 
        sprintf(buf, "q%d", i);
        glp_set_row_name(lp, i, buf); 
        glp_set_row_bnds(lp, i, GLP_UP, 0.0, 0.0); // -infity < qi<=0
    }
       // Add rows r_1 and r_2
    glp_set_row_name(lp, K+1, "r1");  
    glp_set_row_bnds(lp, K+1, GLP_UP, 0.0, 1.0+epsilon); // r1 <= 1
    glp_set_row_name(lp, K+2, "r2");  
    glp_set_row_bnds(lp, K+2, GLP_UP, 0.0, -1.0+epsilon); // r2 <=-1
    
    
    glp_add_cols(lp, D+1);    // add D+1 columns: v[1] ... v[D] and x
    // Add col v[1] ... v[D]
    for (i=1; i<=D; i++) {
        char buf[10]; 
        sprintf(buf, "v%d", i);
        
        glp_set_col_name(lp, i, buf); 
        glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
        glp_set_obj_coef(lp, i, 0.0);  // objective: maximize x ONLY
    }
    
    // Add col x
    glp_set_col_name(lp, D+1, "x");
    glp_set_col_bnds(lp, D+1, GLP_FR, 0.0, 0.0); // -infty <= x <= infty
    glp_set_obj_coef(lp, D+1, 1.0);  // objective: maximize x

    /*
            v[1]           v[2]   ...        v[D]    x
      q1 -(p-p_i)[0] -(p-p_i)[1] ... -(p-p_i)[D-1]   1
      q2  ......  same as q1  .................
      ...
      qK  ......  same as q1 .................
      r1   p[0]           p[1]    ...        p[D-1]  0
      r2  -p[0]         -p[1]    ...       -p[D-1]   0
    */
    
    int counter=1; 
    // set value on row q1 ... qk
    for (i=1; i<=K; i++) {
        
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, D+1, 1.0); //DEBUG
        #endif
        
        ia[counter]=i; ja[counter]=D+1; ar[counter++]=1.0; // a["qi", "x"] =1
        for (j=1; j<=D; j++) {
            #ifdef DEBUG_LP
            fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, j, -(pt.a[j-1]-s[i-1].a[j-1])); //DEBUG
            #endif
            ia[counter]=i; ja[counter]=j; 
			ar[counter++] = -(pt->coord[j-1]-s->points[i-1]->coord[j-1]); // a["qi", "v[j]"] = -(pt-s[i-1])[j-1]
       }    
    }

    // set value on row r1 and r2
    #ifdef DEBUG_LP
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, D+1, 0); //DEBUG
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, D+1, 0); //DEBUG
    #endif
    ia[counter]=K+1; ja[counter]=D+1; ar[counter++]=0.0; // a["r1", "x"]=0 
    ia[counter]=K+2; ja[counter]=D+1; ar[counter++]=0.0; // a["r2", "x"]=0

    for (i=1; i<=D; i++){
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, i, pt.a[i-1]); //DEBUG
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, i, -pt.a[i-1]); //DEBUG
        #endif
		ia[counter]=K+1; ja[counter]=i; ar[counter++]=pt->coord[i-1];   // e.g. a["r1", "v[1]"]=pt[0];   
		ia[counter]=K+2; ja[counter]=i; ar[counter++]=-pt->coord[i-1];   // e.g. a["r2", "v[1]"]=-pt[0];   
    }
    
    

    // loading data  
    glp_load_matrix(lp, counter-1, ia, ja, ar);    
    
    
    // Use this to print out the LP if you're debugging
    #ifdef DEBUG_LP
    glp_write_lp(lp, NULL, "testlp.lp");  // DEBUG
    #endif
    
    
    // running simplex
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=GLP_MSG_OFF; // turn off all message by glp_simplex 
    glp_simplex(lp, &parm);    
    
    // write solution 
    #ifdef DEBUG_LP
    glp_print_sol(lp, "testlp.sol");  // DEBUG
    #endif
    
    // get values
    double regret_ratio=glp_get_obj_val(lp); 
    for (i=0; i<D; i++)
        v[i]=glp_get_col_prim(lp, i+1); // v[0] is at column 1, v[1] at col 2 ...
    
    
    glp_delete_prob(lp); // clean up
    delete []ia;
	delete []ja;
	delete []ar;
	
    return regret_ratio; 
}    

double worstDirection(int index, point_set_t *s, point_t* pt, double* &v)
{
	int K = index;
	int D = pt->dim;

    int* ia = new int[1+(K+5)*(D+5)];  //TODO: delete
	int* ja = new int[1+(K+5)*(D+5)];  //TODO: delete
    double* ar = new double[1+(K+5)*(D+5)];   //TODO: delete
    int i, j;   
    double epsilon=0.0000000000001; 


    glp_prob *lp;
    lp=glp_create_prob(); 
    glp_set_prob_name(lp, "max_regret_ratio");
    glp_set_obj_dir(lp, GLP_MAX);
    
 
    glp_add_rows(lp, K+2);  // add K+2 rows: q_1...q_k and r_1 and r_2
    // Add rows q_1 ... q_K 
    for (i=1; i<=K; i++) {
        char buf[10]; 
        sprintf(buf, "q%d", i);
        glp_set_row_name(lp, i, buf); 
        glp_set_row_bnds(lp, i, GLP_UP, 0.0, 0.0); // -infity < qi<=0
    }
       // Add rows r_1 and r_2
    glp_set_row_name(lp, K+1, "r1");  
    glp_set_row_bnds(lp, K+1, GLP_UP, 0.0, 1.0+epsilon); // r1 <= 1
    glp_set_row_name(lp, K+2, "r2");  
    glp_set_row_bnds(lp, K+2, GLP_UP, 0.0, -1.0+epsilon); // r2 <=-1
    
    
    glp_add_cols(lp, D+1);    // add D+1 columns: v[1] ... v[D] and x
    // Add col v[1] ... v[D]
    for (i=1; i<=D; i++) {
        char buf[10]; 
        sprintf(buf, "v%d", i);
        
        glp_set_col_name(lp, i, buf); 
        glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
        glp_set_obj_coef(lp, i, 0.0);  // objective: maximize x ONLY
    }
    
    // Add col x
    glp_set_col_name(lp, D+1, "x");
    glp_set_col_bnds(lp, D+1, GLP_FR, 0.0, 0.0); // -infty <= x <= infty
    glp_set_obj_coef(lp, D+1, 1.0);  // objective: maximize x

    /*
            v[1]           v[2]   ...        v[D]    x
      q1 -(p-p_i)[0] -(p-p_i)[1] ... -(p-p_i)[D-1]   1
      q2  ......  same as q1  .................
      ...
      qK  ......  same as q1 .................
      r1   p[0]           p[1]    ...        p[D-1]  0
      r2  -p[0]         -p[1]    ...       -p[D-1]   0
    */
    
    int counter=1; 
    // set value on row q1 ... qk
    for (i=1; i<=K; i++) {
        
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, D+1, 1.0); //DEBUG
        #endif
        
        ia[counter]=i; ja[counter]=D+1; ar[counter++]=1.0; // a["qi", "x"] =1
        for (j=1; j<=D; j++) {
            #ifdef DEBUG_LP
            fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, j, -(pt.a[j-1]-s[i-1].a[j-1])); //DEBUG
            #endif
            ia[counter]=i; ja[counter]=j; 
			ar[counter++] = -(pt->coord[j-1]-s->points[i-1]->coord[j-1]); // a["qi", "v[j]"] = -(pt-s[i-1])[j-1]
       }    
    }

    // set value on row r1 and r2
    #ifdef DEBUG_LP
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, D+1, 0); //DEBUG
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, D+1, 0); //DEBUG
    #endif
    ia[counter]=K+1; ja[counter]=D+1; ar[counter++]=0.0; // a["r1", "x"]=0 
    ia[counter]=K+2; ja[counter]=D+1; ar[counter++]=0.0; // a["r2", "x"]=0

    for (i=1; i<=D; i++){
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, i, pt.a[i-1]); //DEBUG
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, i, -pt.a[i-1]); //DEBUG
        #endif
		ia[counter]=K+1; ja[counter]=i; ar[counter++]=pt->coord[i-1];   // e.g. a["r1", "v[1]"]=pt[0];   
		ia[counter]=K+2; ja[counter]=i; ar[counter++]=-pt->coord[i-1];   // e.g. a["r2", "v[1]"]=-pt[0];   
    }
    
    

    // loading data  
    glp_load_matrix(lp, counter-1, ia, ja, ar);    
    
    
    // Use this to print out the LP if you're debugging
    #ifdef DEBUG_LP
    glp_write_lp(lp, NULL, "testlp.lp");  // DEBUG
    #endif
    
    
    // running simplex
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=GLP_MSG_OFF; // turn off all message by glp_simplex 
    glp_simplex(lp, &parm);    
    
    // write solution 
    #ifdef DEBUG_LP
    glp_print_sol(lp, "testlp.sol");  // DEBUG
    #endif
    
    // get values
    double regret_ratio=glp_get_obj_val(lp); 
    for (i=0; i<D; i++)
        v[i]=glp_get_col_prim(lp, i+1); // v[0] is at column 1, v[1] at col 2 ...
    
    
    glp_delete_prob(lp); // clean up
    delete []ia;
	delete []ja;
	delete []ar;
	
    return regret_ratio; 
}    

double worstDirection(int index, point_set_t *s, point_set_t *point_set, bool* active, point_t* pt, double* &v)
{
	int K = index;
	int D = pt->dim;

	int* ia = new int[1+(K + (point_set->numberOfPoints - K - 1) +5)*(D+5)];  //TODO: delete
	int* ja = new int[1+(K + (point_set->numberOfPoints - K - 1) +5)*(D+5)];  //TODO: delete
    double* ar = new double[1+(K + (point_set->numberOfPoints - K - 1) +5)*(D+5)];   //TODO: delete
    int i, j;   
    double epsilon=0.0000000000001; 


    glp_prob *lp;
    lp=glp_create_prob(); 
    glp_set_prob_name(lp, "max_regret_ratio");
    glp_set_obj_dir(lp, GLP_MAX);
    
 
    glp_add_rows(lp, K+ (point_set->numberOfPoints - K - 1) +2);  // add K+2 rows: q_1...q_k and r_1 and r_2, and (point_set->numberOfPoints - K - 1) for 2RMS
    // Add rows q_1 ... q_K 
    for (i=1; i<=K; i++) {
        char buf[10]; 
        sprintf(buf, "q%d", i);
        glp_set_row_name(lp, i, buf); 
        glp_set_row_bnds(lp, i, GLP_UP, 0.0, 0.0); // -infity < qi<=0
    }
       // Add rows r_1 and r_2
    glp_set_row_name(lp, K+1, "r1");  
    glp_set_row_bnds(lp, K+1, GLP_UP, 0.0, 1.0+epsilon); // r1 <= 1
    glp_set_row_name(lp, K+2, "r2");  
    glp_set_row_bnds(lp, K+2, GLP_UP, 0.0, -1.0+epsilon); // r2 <=-1
    
	for (i = 1; i <= (point_set->numberOfPoints - K - 1); i++){
		char buf[10]; 
        sprintf(buf, "e%d", i);
        glp_set_row_name(lp,  K+2+i, buf); 
        glp_set_row_bnds(lp,  K+2+i, GLP_UP, 0.0, 0.0); // -infity < ei<=0
	}
    
    glp_add_cols(lp, D+2);    // add D+1 columns: v[1] ... v[D] and x
    // Add col v[1] ... v[D]
    for (i=1; i<=D; i++) {
        char buf[10]; 
        sprintf(buf, "v%d", i);
        
        glp_set_col_name(lp, i, buf); 
        glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
        glp_set_obj_coef(lp, i, 0.0);  // objective: maximize x ONLY
    }
    
    // Add col x
    glp_set_col_name(lp, D+1, "x");
    glp_set_col_bnds(lp, D+1, GLP_FR, 0.0, 0.0); // -infty <= x <= infty
    glp_set_obj_coef(lp, D+1, 1.0);  // objective: maximize x

	// Add col y
    glp_set_col_name(lp, D+2, "y");
    glp_set_col_bnds(lp, D+2, GLP_LO, 0.0, 0.0); // 0 <= y <= infty
    glp_set_obj_coef(lp, D+2, 0.0);  // objective: maximize x

    /*
            v[1]           v[2]   ...        v[D]    x y
      q1 -(p-p_i)[0] -(p-p_i)[1] ... -(p-p_i)[D-1]   1 0
      q2  ......  same as q1  .................
      ...
      qK  ......  same as q1 .................
      r1   p[0]           p[1]    ...        p[D-1]  0 0
      r2  -p[0]         -p[1]    ...       -p[D-1]   0 0
	  e1 (p'_i-p)[0] +(p'_i-p)[1] ...+(p'_i-p)[D-1]  0 -1
	  e2 ........
    */
    
    int counter=1; 
    // set value on row q1 ... qk
    for (i=1; i<=K; i++) {
        
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, D+1, 1.0); //DEBUG
        #endif
        
        ia[counter]=i; ja[counter]=D+1; ar[counter++]=1.0; // a["qi", "x"] =1
		ia[counter]=i; ja[counter]=D+2; ar[counter++]=0.0; // a["qi", "y"] =0
        for (j=1; j<=D; j++) {
            #ifdef DEBUG_LP
            fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, j, -(pt.a[j-1]-s[i-1].a[j-1])); //DEBUG
            #endif
            ia[counter]=i; ja[counter]=j; 
			ar[counter++] = -(pt->coord[j-1]-s->points[i-1]->coord[j-1]); // a["qi", "v[j]"] = -(pt-s[i-1])[j-1]
       }    
    }

    // set value on row r1 and r2
    #ifdef DEBUG_LP
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, D+1, 0); //DEBUG
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, D+1, 0); //DEBUG
    #endif
    ia[counter]=K+1; ja[counter]=D+1; ar[counter++]=0.0; // a["r1", "x"]=0 
    ia[counter]=K+2; ja[counter]=D+1; ar[counter++]=0.0; // a["r2", "x"]=0
	ia[counter]=K+1; ja[counter]=D+2; ar[counter++]=0.0; // a["r1", "y"]=0 
    ia[counter]=K+2; ja[counter]=D+2; ar[counter++]=0.0; // a["r2", "y"]=0

    for (i=1; i<=D; i++){
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, i, pt.a[i-1]); //DEBUG
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, i, -pt.a[i-1]); //DEBUG
        #endif
		ia[counter]=K+1; ja[counter]=i; ar[counter++]=pt->coord[i-1];   // e.g. a["r1", "v[1]"]=pt[0];   
		ia[counter]=K+2; ja[counter]=i; ar[counter++]=-pt->coord[i-1];   // e.g. a["r2", "v[1]"]=-pt[0];   
    }
    
    // set value on row e1
	int col_count = K+3;
	for (i=0; i<point_set->numberOfPoints; i++) {
        
		if(!active[i] || pt == point_set->points[i])
			continue;

        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, D+1, 1.0); //DEBUG
        #endif
        
        ia[counter]=col_count; ja[counter]=D+1; ar[counter++]=0.0; // a["ei", "x"] =0
		ia[counter]=col_count; ja[counter]=D+2; ar[counter++]=-1.0; // a["ei", "y"] =-1
        for (j=1; j<=D; j++) {
            #ifdef DEBUG_LP
            fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, j, -(pt.a[j-1]-s[i-1].a[j-1])); //DEBUG
            #endif
            ia[counter]=col_count; ja[counter]=j; 
			ar[counter++] = (point_set->points[i]->coord[j-1] -  pt->coord[j-1]); // a["ei", "v[j]"] = (point_set[i]-pt)[j-1]
       }   
	   col_count++;
    }

    // loading data  
    glp_load_matrix(lp, counter-1, ia, ja, ar);    
    
    
    // Use this to print out the LP if you're debugging
    #ifdef DEBUG_LP
    glp_write_lp(lp, NULL, "testlp.lp");  // DEBUG
    #endif
    
    
    // running simplex
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=GLP_MSG_OFF; // turn off all message by glp_simplex 
    glp_simplex(lp, &parm);    
    
    // write solution 
    #ifdef DEBUG_LP
    glp_print_sol(lp, "testlp.sol");  // DEBUG
    #endif
    
    // get values
    double regret_ratio=glp_get_obj_val(lp); 
    for (i=0; i<D; i++)
        v[i]=glp_get_col_prim(lp, i+1); // v[0] is at column 1, v[1] at col 2 ...
    
    
    glp_delete_prob(lp); // clean up
    delete []ia;
	delete []ja;
	delete []ar;
	
    return regret_ratio; 
}    

double worstDirection(int index, point_set_t *s, point_t* pt, float* &v)
{
	int K = index;
	int D = pt->dim;

    int* ia = new int[1+(K+5)*(D+5)];  //TODO: delete
	int* ja = new int[1+(K+5)*(D+5)];  //TODO: delete
    double* ar = new double[1+(K+5)*(D+5)];   //TODO: delete
    int i, j;   
    double epsilon=0.0000000000001; 


    glp_prob *lp;
    lp=glp_create_prob(); 
    glp_set_prob_name(lp, "max_regret_ratio");
    glp_set_obj_dir(lp, GLP_MAX);
    
 
    glp_add_rows(lp, K+2);  // add K+2 rows: q_1...q_k and r_1 and r_2
    // Add rows q_1 ... q_K 
    for (i=1; i<=K; i++) {
        char buf[10]; 
        sprintf(buf, "q%d", i);
        glp_set_row_name(lp, i, buf); 
        glp_set_row_bnds(lp, i, GLP_UP, 0.0, 0.0); // -infity < qi<=0
    }
       // Add rows r_1 and r_2
    glp_set_row_name(lp, K+1, "r1");  
    glp_set_row_bnds(lp, K+1, GLP_UP, 0.0, 1.0+epsilon); // r1 <= 1
    glp_set_row_name(lp, K+2, "r2");  
    glp_set_row_bnds(lp, K+2, GLP_UP, 0.0, -1.0+epsilon); // r2 <=-1
    
    
    glp_add_cols(lp, D+1);    // add D+1 columns: v[1] ... v[D] and x
    // Add col v[1] ... v[D]
    for (i=1; i<=D; i++) {
        char buf[10]; 
        sprintf(buf, "v%d", i);
        
        glp_set_col_name(lp, i, buf); 
        glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
        glp_set_obj_coef(lp, i, 0.0);  // objective: maximize x ONLY
    }
    
    // Add col x
    glp_set_col_name(lp, D+1, "x");
    glp_set_col_bnds(lp, D+1, GLP_FR, 0.0, 0.0); // -infty <= x <= infty
    glp_set_obj_coef(lp, D+1, 1.0);  // objective: maximize x

    /*
            v[1]           v[2]   ...        v[D]    x
      q1 -(p-p_i)[0] -(p-p_i)[1] ... -(p-p_i)[D-1]   1
      q2  ......  same as q1  .................
      ...
      qK  ......  same as q1 .................
      r1   p[0]           p[1]    ...        p[D-1]  0
      r2  -p[0]         -p[1]    ...       -p[D-1]   0
    */
    
    int counter=1; 
    // set value on row q1 ... qk
    for (i=1; i<=K; i++) {
        
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, D+1, 1.0); //DEBUG
        #endif
        
        ia[counter]=i; ja[counter]=D+1; ar[counter++]=1.0; // a["qi", "x"] =1
        for (j=1; j<=D; j++) {
            #ifdef DEBUG_LP
            fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, i, j, -(pt.a[j-1]-s[i-1].a[j-1])); //DEBUG
            #endif
            ia[counter]=i; ja[counter]=j; 
			ar[counter++] = -(pt->coord[j-1]-s->points[i-1]->coord[j-1]); // a["qi", "v[j]"] = -(pt-s[i-1])[j-1]
       }    
    }

    // set value on row r1 and r2
    #ifdef DEBUG_LP
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, D+1, 0); //DEBUG
    fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, D+1, 0); //DEBUG
    #endif
    ia[counter]=K+1; ja[counter]=D+1; ar[counter++]=0.0; // a["r1", "x"]=0 
    ia[counter]=K+2; ja[counter]=D+1; ar[counter++]=0.0; // a["r2", "x"]=0

    for (i=1; i<=D; i++){
        #ifdef DEBUG_LP
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+1, i, pt.a[i-1]); //DEBUG
        fprintf(stderr, "%d: a[%d, %d]=%lf\n", counter, K+2, i, -pt.a[i-1]); //DEBUG
        #endif
		ia[counter]=K+1; ja[counter]=i; ar[counter++]=pt->coord[i-1];   // e.g. a["r1", "v[1]"]=pt[0];   
		ia[counter]=K+2; ja[counter]=i; ar[counter++]=-pt->coord[i-1];   // e.g. a["r2", "v[1]"]=-pt[0];   
    }
    
    

    // loading data  
    glp_load_matrix(lp, counter-1, ia, ja, ar);    
    
    
    // Use this to print out the LP if you're debugging
    #ifdef DEBUG_LP
    glp_write_lp(lp, NULL, "testlp.lp");  // DEBUG
    #endif
    
    
    // running simplex
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=GLP_MSG_OFF; // turn off all message by glp_simplex 
    glp_simplex(lp, &parm);    
    
    // write solution 
    #ifdef DEBUG_LP
    glp_print_sol(lp, "testlp.sol");  // DEBUG
    #endif
    
    // get values
    double regret_ratio=glp_get_obj_val(lp); 
    for (i=0; i<D; i++)
        v[i]=glp_get_col_prim(lp, i+1); // v[0] is at column 1, v[1] at col 2 ...
    
    
    glp_delete_prob(lp); // clean up
    delete []ia;
	delete []ja;
	delete []ar;
	
    return regret_ratio; 
}  

// determinant code from http://www.c.happycodings.com/Beginners_Lab_Assignments/code62.html
double determinant(int n, double** a)
{
    int i, j, k;
    double mult;
    double det = 1.0;

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            mult = a[j][i]/a[i][i];
            for(k = 0; k < n; k++)
            {
                if (i==j) break;
                a[j][k] = a[j][k] - a[i][k] * mult;
            }
        }
    }

    for(i = 0; i < n; i++)
    {
        det = det * a[i][i];
    }

    return det;
}