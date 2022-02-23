#include "preferLearn.h"

// get the index of the "current best" point
// P: the input car set
// C_idx: the indexes of the current candidate favorite car in P
// ext_vec: the set of extreme vecotr
int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec)
{
	int dim = P->points[0]->dim;

	// the set of extreme points of the candidate utility range R
	vector<point_t*> ext_pts;
	ext_pts = get_extreme_pts(ext_vec);

	// use the "mean" utility vector in R (other strategies could also be used)
	point_t* mean = alloc_point(dim);
	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] = 0;
	}
	for(int i = 0; i < ext_pts.size(); i++)
	{
		for(int j = 0; j < dim; j++)
			mean->coord[j] += ext_pts[i]->coord[j];
	}
	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] /= ext_pts.size();
	}

	// look for the maximum utility point w.r.t. the "mean" utility vector
	int best_pt_idx;
	double max = 0;
	for(int i = 0; i < C_idx.size(); i++)
	{
		point_t* pt = P->points[C_idx[i]];

		double v = dot_prod(pt, mean);
		if(v > max)
		{
			max = v;
			best_pt_idx =  C_idx[i];
		}
	}

	for(int i = 0; i < ext_pts.size(); i++)
		release_point(ext_pts[i]);
	return best_pt_idx;
}

// generate s cars for selection in a round
// P: the input car set
// C_idx: the indexes of the current candidate favorite car in P
// s: the number of cars for user selection
// current_best_idx: the current best car
// last_best: the best car in previous interaction
// frame: the frame for obtaining the set of neibouring vertices of the current best vertiex (used only if cmp_option = SIMPLEX)
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, int current_best_idx, int& last_best, vector<int>& frame, int cmp_option)
{
	// the set of s cars for selection
	vector<int> S;

	if(cmp_option == RANDOM) // RANDOM car selection mode
	{
		// randoming select at most s non-overlaping cars in the candidate set 
		while(S.size() < s && S.size() < C_idx.size())
		{
			int idx = rand() % C_idx.size();

			bool isNew = true;
			for(int i = 0; i < S.size(); i++)
			{
				if(S[i] == idx)
				{
					isNew = false;
					break;
				}
			}
			if(isNew)
				S.push_back(idx);
		}
	}
	else if(cmp_option == SIMPLEX) // SIMPLEX car selection mode
	{
		if(last_best != current_best_idx || frame.size() == 0) // the new frame is not computed before (avoid duplicate frame computation)
		{
			// create one ray for each car in P for computing the frame
			vector<point_t*> rays;
			int best_i = -1; 
			for(int i = 0; i < P->numberOfPoints; i++)
			{
				if(i == current_best_idx)
				{
					best_i = i;
					continue; 
				}

				point_t* best = P->points[current_best_idx];
				point_t* newRay = sub(P->points[i], best);
				rays.push_back(newRay);
			}

			// frame compuatation
			frameConeFastLP(rays, frame);
		
			// update the indexes lying after current_best_idx
			for(int i = 0; i < frame.size(); i++)
			{
				if(frame[i] >= current_best_idx)
					frame[i]++;

				//S[i] = C_idx[S[i]];
			}

			for(int i = 0; i < rays.size(); i++)
				release_point(rays[i]);
		}

		//printf("current_best: %d, frame:", P->points[current_best_idx]->id);
		//for(int i = 0; i < frame.size(); i++)
		//	printf("%d ", P->points[frame[i]]->id);
		//printf("\n");

		//S.push_back(best_i);
	
		for(int j = 0; j < C_idx.size(); j++)
		{
			if(C_idx[j] == current_best_idx) // it is possible that current_best_idx is no longer in the candidate set, then no need to compare again
			{
				S.push_back(j);
				break;
			}
		}

		// select at most s non-overlaping cars in the candidate set based on "neighboring vertices" obtained via frame compuation
		for(int i = 0; i < frame.size() && S.size() < s; i++)
		{
			for(int j = 0; j < C_idx.size() && S.size() < s; j++)
			{
				if(C_idx[j] == current_best_idx)
					continue;

				if(C_idx[j] == frame[i])
					S.push_back(j);
			}
		}

		// if less than s car are selected, fill in the remaing one 
		if (S.size() < s && C_idx.size() > s)
		{
			for (int j = 0; j < C_idx.size(); j++)
			{
				bool noIn = true;
				for (int i = 0; i < S.size(); i++)
				{
					if (j == S[i])
						noIn = false;
				}
				if (noIn)
					S.push_back(j);

				if (S.size() == s)
					break;
			}
		}
	}
	else // for testing only. Do not use this!
	{
		vector<point_t*> rays;

		int best_i = -1;
		for(int i = 0; i < C_idx.size(); i++)
		{
			if(C_idx[i] == current_best_idx)
			{
				best_i = i;
				continue;
			}

			point_t* best = P->points[current_best_idx];

			point_t* newRay = sub(P->points[C_idx[i]], best);

			rays.push_back(newRay);
		}

		partialConeFastLP(rays, S, s - 1);
		if(S.size() > s - 1)
			S.resize(s - 1);
		for(int i = 0; i < S.size(); i++)
		{
			if(S[i] >= best_i)
				S[i]++;

			//S[i] = C_idx[S[i]];
		}
		S.push_back(best_i);


		for(int i = 0; i < rays.size(); i++)
			release_point(rays[i]);
	}
	return S;
}

// get the better car among two (whose indexes p_idx and q_idx in P) from the user
int show_to_user(point_set_t* P, int p_idx, int q_idx)
{

	int option = 0;

	// ask the user for the better car among two given options
	while (option != 1 && option != 2)
	{
		printf("Please choose the car you favor more:\n");
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 1", P->points[p_idx]->coord[0], P->points[p_idx]->coord[1], P->points[p_idx]->coord[2], P->points[p_idx]->coord[3]);
		printf("--------------------------------------------------------\n");
		printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 2", P->points[q_idx]->coord[0], P->points[q_idx]->coord[1], P->points[q_idx]->coord[2], P->points[q_idx]->coord[3]);
		printf("--------------------------------------------------------\n");
		printf("Your choice:");
		scanf("%d", &option);
	}

	return option;
}

// generate the options for user selection and update the extreme vecotrs based on the user feedback
// wPrt: record user's feedback
// P_car: the set of candidate cars with seqential ids
// skyline_proc_P: the skyline set of normalized cars
// C_idx: the indexes of the current candidate favorite car in skyline_proc_P
// ext_vec: the set of extreme vecotr
// current_best_idx: the current best car
// last_best: the best car in previous interaction
// frame: the frame for obtaining the set of neibouring vertices of the current best vertiex (used only if cmp_option = SIMPLEX)
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
void update_ext_vec(FILE *wPtr, point_set_t* P_car, point_set_t* skyline_proc_P, vector<int>& C_idx, int s, vector<point_t*>& ext_vec, int& current_best_idx, int& last_best, vector<int>& frame, int cmp_option)
{
	// generate s cars for selection in a round
	vector<int> S = generate_S(skyline_proc_P, C_idx, s, current_best_idx, last_best, frame, cmp_option);
	
	if (S.size() != 2) // we fix s to be 2 in the demo
	{
		printf("invalid option numbers\n");
		exit(0);
	}
	
	//printf("comp: %d %d\n", skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id);
	
	// get the better car among two from the user
	int max_i = show_to_user(P_car, skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id) - 1;
	fprintf(wPtr, "%d\t%d\t%d\n", skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id, skyline_proc_P->points[C_idx[S[max_i]]]->id);

	// update the best option
	last_best = current_best_idx;
	current_best_idx = C_idx[S[max_i]];

	// for each non-favorite car, create a new extreme vecotr
	for (int i = 0; i < S.size(); i++)
	{
		if (max_i == i)
			continue;

		point_t* tmp = sub(skyline_proc_P->points[C_idx[S[i]]], skyline_proc_P->points[C_idx[S[max_i]]]);
		C_idx[S[i]] = -1;

		point_t* new_ext_vec = scale(1 / calc_len(tmp), tmp);

		release_point(tmp);
		ext_vec.push_back(new_ext_vec);
	}

	// directly remove the non-favorite car from the candidate set
	vector<int> newC_idx;
	for (int i = 0; i < C_idx.size(); i++)
	{
		if (C_idx[i] >= 0)
			newC_idx.push_back(C_idx[i]);
	}
	C_idx = newC_idx;
}

// get the cars we prune in this round (compre "before" with "after")
vector<int> get_pruned_C_idx(vector<int> before, vector<int> after)
{
	vector<int> result;

	for(int i = 0; i < before.size(); i++)
	{
		vector<int>::iterator s = find(after.begin(), after.end(), before[i]);

		if ( s == after.end())
			result.push_back(before[i]);
	}
	sort(result.begin(), result.end());

	return result;
}

// the main interactive algorithm
// wPrt: record user's feedback
// P_car: the set of candidate cars with seqential ids
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
point_t* max_utility(FILE *wPtr, point_set_t* P_car, int cmp_option)
{
	int s = 2; // we show 2 options each time
	int prune_option = RTREE; // use the RTREE-based skyline algorithm. SQL can be used as well (but can be slower)

	// pre-process the car database
	point_set_t* proc_P = process_car(P_car);
	// compute the skyline set
	point_set_t* skyline_proc_P = skyline_point(proc_P);

	int dim = skyline_proc_P->points[0]->dim;

	// the indexes of the candidate set
	// initially, it is all the skyline cars
	vector<int> C_idx;
	for (int i = 0; i < skyline_proc_P->numberOfPoints; i++)
		C_idx.push_back(i);

	// the initial exteme vector sets V = {−ei | i ∈ [1, d], ei [i] = 1 and ei [j] = 0 if i , j}.
	vector<point_t*> ext_vec;
	for (int i = 0; i < dim; i++)
	{
		point_t* e = alloc_point(dim);
		for (int j = 0; j < dim; j++)
		{
			if (i == j)
				e->coord[j] = -1;
			else
				e->coord[j] = 0;
		}
		ext_vec.push_back(e);
	}

	int current_best_idx = -1;
	int last_best = -1;
	vector<int> frame;

	// get the index of the "current best" point
	current_best_idx = get_current_best_pt(skyline_proc_P, C_idx, ext_vec);

	// if not skyline
	//sql_pruning(P, C_idx, ext_vec);

	// Qcount - the number of querstions asked
	// Csize - the size of the current candidate set
	double rr = 1;
	int Qcount = 0;
	printf("Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
	fprintf(wPtr, "Qcount: %d, Csize: %d\n", Qcount, C_idx.size());

	// print out the current candidate set
	printf("Current Candidate Set:\n");
	printf("---------------------------------------------\n");
	printf("|%10s|%10s|%10s|%10s|\n", "Price(USD)", "Year", "PowerPS", "Used KM");
	printf("---------------------------------------------\n");
	for (int i = 0; i < C_idx.size(); i++)
	{
		point_t* car = P_car->points[skyline_proc_P->points[C_idx[i]]->id];
		printf("|%10.0lf|%10.0lf|%10.0lf|%10.0lf|\n",  car->coord[0], car->coord[1], car->coord[2], car->coord[3]);
		printf("---------------------------------------------\n");
	}
	printf("\n");

	// interactively reduce the candidate set and shrink the candidate utility range
	while (C_idx.size()> 1)
	{
		Qcount++;
		sort(C_idx.begin(), C_idx.end()); // prevent selecting two different points after different skyline algorithms

		// record the cars before pruning (to be used for seeing which cars are pruned in one interaction)
		vector<int> before(C_idx.size()); 
		copy(C_idx.begin(), C_idx.end(), before.begin());

		// generate the options for user selection and update the extreme vecotrs based on the user feedback
		update_ext_vec(wPtr, P_car, skyline_proc_P, C_idx, s, ext_vec, current_best_idx, last_best, frame, cmp_option);

		if (C_idx.size() == 1) // || global_best_idx == current_best_idx
			break;

		// use the branch-and-bound skyline (BBS) algorithm for maintaining the candidate set 
		rtree_pruning(skyline_proc_P, C_idx, ext_vec, rr, NO_BOUND, HYPER_PLANE);
		sort(C_idx.begin(), C_idx.end()); 

		// print the cars we prune in this round
		printf("This round we prune:\n");
		printf("---------------------------------------------\n");
		printf("|%10s|%10s|%10s|%10s|\n", "Price(USD)", "Year", "PowerPS", "Used KM");
		printf("---------------------------------------------\n");
		vector<int> pruned_C_idx = get_pruned_C_idx(before, C_idx);
		for (int i = 0; i < pruned_C_idx.size(); i++)
		{
			point_t* car = P_car->points[skyline_proc_P->points[pruned_C_idx[i]]->id];
			printf("|%10.0lf|%10.0lf|%10.0lf|%10.0lf|\n",  car->coord[0], car->coord[1], car->coord[2], car->coord[3]);
			printf("---------------------------------------------\n");
		}
		printf("\n");

		// print the current candidate set
		printf("Current Candidate Set:\n");
		printf("---------------------------------------------\n");
		printf("|%10s|%10s|%10s|%10s|\n", "Price(USD)", "Year", "PowerPS", "Used KM");
		printf("---------------------------------------------\n");
		for (int i = 0; i < C_idx.size(); i++)
		{
			point_t* car = P_car->points[skyline_proc_P->points[C_idx[i]]->id];
			printf("|%10.0lf|%10.0lf|%10.0lf|%10.0lf|\n",  car->coord[0], car->coord[1], car->coord[2], car->coord[3]);
			printf("---------------------------------------------\n");
		}

		printf("The number of questions asked: %d\nThe size of the current candidate set: %d\n", Qcount, C_idx.size());
		fprintf(wPtr, "Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
		printf("\n\n");
	}
	printf("\n----------------Summary Result---------------\n");
	printf("The total number of questions asked: %d\n", Qcount);
	fprintf(wPtr, "Qcount: %d, Csize: %d\n", Qcount, C_idx.size());
	printf("---------------------------------------------\n");

	// get the final result (un-normalized)
	point_t* result = P_car->points[skyline_proc_P->points[get_current_best_pt(skyline_proc_P, C_idx, ext_vec)]->id];
	for (int i = 0; i < ext_vec.size(); i++)
		release_point(ext_vec[i]);
	
	return result;
}

// check if a target value in [lb, ub]
bool inRange(int lb, int ub, int target)
{
	if (target > ub || target < lb)
		return false;
	else
		return true;
}

// select cars in P according to the acceptable range for each attribute
// the choices are recorded in wPtr
point_set_t* selectRange(FILE *wPtr, point_set_t* P)
{
	bool finish = false;
	int MAX_NUM_CAR = 1000; // to illustrate, fix the candidate set size to be 1000


	vector<point_t*> candidate;
	while (!finish)
	{
		candidate.clear();
		int lb[4], ub[4];

		// four attributes: price, year, power and used KM
		printf("Specify two integers indicating your affordable price (in USD)range (e.g., 1000 50000): ");
		scanf("%d %d", &lb[0], &ub[0]);
		printf("Specify two integers indicating your affordable year range (e.g., 2001 2017): ");
		scanf("%d %d", &lb[1], &ub[1]);
		printf("Specify two integers indicating your affordable power (in PS) range (e.g., 50 400): ");
		scanf("%d %d", &lb[2], &ub[2]);
		printf("Specify two integers indicating your affordable used KM range (e.g., 10000 150000): ");
		scanf("%d %d", &lb[3], &ub[3]);

		for (int i = 0; i < 4; i++)
			fprintf(wPtr, "%d\t%d\n", lb[i], ub[i]);


		for (int i = 0; i < P->numberOfPoints; i++) // look for valid cars satisfying the initial constraints
		{
			bool isValid = true;
			for (int j = 0; j < 4; j++)
			{
				if (!inRange(lb[j], ub[j], P->points[i]->coord[j]))
					isValid = false;
			}
			if (isValid)
				candidate.push_back(P->points[i]);

			if (candidate.size() == MAX_NUM_CAR) 
				break;
		}

		if (candidate.size() == MAX_NUM_CAR)
			finish = true;
		else
			printf("Too few tuples. Try larger ranges again!\n");
	}

	// creat a new car set with *seqential* indexes (to allow efficient indexing)
	point_set_t* new_P = alloc_point_set(MAX_NUM_CAR);
	for (int i = 0; i < MAX_NUM_CAR; i++)
	{
		new_P->points[i] = copy(candidate[i]);
		new_P->points[i]->id = i;
	}
	release_point_set(P, true);

	return new_P;
}

// the main method for the demo system
void demo()
{
	// the welcome message
	printf("------------Welcome to FindYourFavorite-----------------\n");
	printf("This is a demonstration system for finding your favorite car in a used car database.\n");
	printf("Enter your acceptable range for each attribute.\n");

	printf("You will be presented two cars each time and you need to choose the one you favor more. \n");

	// read cars
	point_set_t* P = read_points("car.txt");

	// record user's feedback in "output/name.txt"
	char outputfile[MAX_FILENAME_LENG];
	char name[MAX_FILENAME_LENG];
	printf("\nPlease input your name: ");
	scanf("%s", name);
	FILE *wPtr;
	sprintf(outputfile, "output/%s.txt", name); 
	wPtr = (FILE *)fopen(outputfile, "w");

	// select cars according to the acceptable range for each attribute
	P = selectRange(wPtr, P);

	// select the car selection mode
	int option = 0;
	while (option != 1 && option != 2)
	{
		printf("Please choose the display method: Simplex(1) or Random(2): ");
		scanf("%d", &option);
	}

	point_t* p; // store the resulting favorite car
	if(option == 1) // SIMPLEX selection mode
	{
		fprintf(wPtr, "SIMPLEX\n");
		p = max_utility(wPtr, P, SIMPLEX);
	}
	else // RANDOM selection mode
	{
		fprintf(wPtr, "\nRandom\n");
		p = max_utility(wPtr, P, RANDOM);	
	}

	// show result
	printf("---------------------------------------------\n");
	printf("Your favorite Car:\n");
	printf("---------------------------------------------\n");
	printf("|%10s|%10s|%10s|%10s|\n", "Price(USD)", "Year", "PowerPS", "Used KM");
	printf("---------------------------------------------\n");
	printf("|%10.0lf|%10.0lf|%10.0lf|%10.0lf|\n",  p->coord[0], p->coord[1], p->coord[2], p->coord[3]);
	printf("---------------------------------------------\n");
	
	fprintf(wPtr, "\n");
	fclose(wPtr);
}
