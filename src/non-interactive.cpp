#include "non-interactive.h"

int non_interacitve()
{
	char input[MAX_FILENAME_LENG];
	printf("input filename (e.g., 5d.txt):\n");
	scanf("%s", input);

	point_set_t* P = read_points(input);

	int D = P->points[0]->dim;


	double time, mrr;
	while(true)
	{
		int option = 0;
		while (option > 7 || option < 1 )
		{
			printf("-----------------------------------------------------\n");
			printf("Select Algorihtms:\n \t(1) GraphDP \n\t(2) biSearch \n\t(3) sweepDP \n\t(4) Sphere \n\t(5) Cube \n\t(6) DMM \n\t(7) IncGreedy \nYour option: ");
			scanf("%d", &option);


			if(D != 2 && (option == 1 || option == 2 || option == 3))
			{
				printf("It only suppports 2D data!\n");
				option = 0;
			}
		}

		int k = 0;
		while (k < D)
		{
			printf("Set the integer k (k should be large than D): ");
			scanf("%d", &k);
		}


		if (option != 7)
		{
			point_set_t* S;
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


			if(option == 1)
				S = graphDP(P, k);
			else if(option == 2)
				S = biSearch(P, k);
			else if(option == 3)
				S = sweepDP(P, k);
			else if(option == 4)
				S = sphereWSImpLP(P, k);
			else if(option == 5)
				S = cube(k, P);
			else if(option == 6)
				S = DMM(P, k);

		#ifndef WIN32
			gettimeofday(&end, NULL);
			time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
		#else
			end = std::clock();
			time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
		#endif

			mrr = evaluateLP(P, S, 0);

			printf("Time: %f, Maxmum Regret Ratio: %f \n", time, mrr);
			printf("Tuple selected:\n");
			print_point_set(S);
	
			point_t* u = alloc_point(D);
			bool isValid = false;

			while (!isValid)
			{
				printf("Provide a positve %d-dimensinoal vector: \n", D);
				for(int i = 0; i < D; i++)
					scanf("%lf", &u->coord[i]);

				bool allZero = true;
				bool hasNegative = false;

				for(int i = 0; i < D; i++)
				{
					if(!isZero(u->coord[i]))
						allZero = false;
					if(u->coord[i] < 0)
						hasNegative = true;
				}
				if (!allZero && !hasNegative)
					isValid = true;
			}
		

			point_t* globalBest = findFavorite(P, u);
			point_t* localBest = findFavorite(S, u);

			printf("Your utility vector is : ");
			print_point(u);
			printf("Your global favorite tuple is : ");
			print_point(globalBest);
			printf("Your local favorite tuple is : ");
			print_point(localBest);
			printf("Your regret ratio is : %f\n", 1 - dot_prod(localBest,u)/dot_prod(globalBest,u));

			release_point(u);
			release_point_set(S,false);
		}
		else
		{
			point_set_t* S = alloc_point_set(0);
			int nextK = 1;
			while(nextK)
			{
				int oldSize = S->numberOfPoints;

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

				S = IncGreedy(k, P, S);

			#ifndef WIN32
				gettimeofday(&end, NULL);
				time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
			#else
				end = std::clock();
				time = (end - start) / (double)(CLOCKS_PER_SEC / 1000) / 1000;
			#endif

				mrr = evaluateLP(P, S, 0);
				printf("\nTime: %f, Maxmum Regret Ratio: %f \n", time, mrr);
				
				if(oldSize != S->numberOfPoints)
				{
					printf("New tuples selected:\n");
					for(int i = oldSize; i < S->numberOfPoints; i++)
						print_point(S->points[i]);

					printf("Do you want the next %d tuples? Enter 1 to continue and enter 0 to exit: ", k);
					scanf("%d", &nextK);
				}
				else
				{
					printf("No more tuple selected!\n");
					nextK = 0;
				}
				
			}


			
		}
	}

	release_point_set(P, true);

	return 0;

}