#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

static int num_workers;
static int *workers;

// citirea am luat-o din laboratorul 10
// functie care citeste din fisier
void read_workers(int rank) {
    FILE *fp;
    char file_name[40];
    sprintf(file_name, "cluster%d.txt", rank);

    fp = fopen(file_name, "r");
	fscanf(fp, "%d", &num_workers);

	workers = malloc(sizeof(int) * num_workers);

	for (size_t i = 0; i < num_workers; i++)
	 	fscanf(fp, "%d", &workers[i]);
}

// functie ce printeaza o topologie
void print_topology(int rank, int **topology, int nProcesses){
	printf("%d ->",rank );
		for (int i = 0; i< 4; i++){
			if (topology[i][0] != 0){
			printf(" %d:",i );
			}
			for (int j =0; j< nProcesses; j++){
				if (topology[i][j] != 0 && topology[i][j+1] != 0){
					printf("%d,",topology[i][j]);
				}else if (topology[i][j] != 0){
					printf("%d",topology[i][j]);
				}else{
					break;
				}
				
			}
			
		}
		printf("\n");
}

// functie ce calculeaza o topologie cu eroare
// principiul este identic cu cel fara eroare
// nu am mai dat send de la si catre rank1
// rank 1 are in topologie doar propriile date 
int ** find_topology_err (int rank, int num_workers, int* workers, int nProcesses){
	int ** topology = malloc(sizeof(int*) * nProcesses);
	int * vTopology = calloc(sizeof(int), nProcesses);
	MPI_Status status;
	for (size_t i = 0; i < nProcesses; i++) {
		topology[i] = calloc(sizeof(int), nProcesses);
	}
	for (int i =0; i< num_workers; i++){
		topology[rank][i] = workers[i];
	}

	
	if (rank == 0){
		for (int j = 2; j < 4 ; j++){
			MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[j][k] = vTopology[k];
					}else {
						break;
					}
			}
		}

		printf("M(%d,%d)\n",rank, 3);
		MPI_Send(topology[0], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
				
	}
	if (rank == 1 ){
		topology[0][0] = 0;
		topology[2][0] = 0;
		topology[3][0] = 0;

	}

	if (rank == 2){
			
		MPI_Send(topology[2], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);

		MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[0][k] = vTopology[k];
					}
			}
		MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[3][k] = vTopology[k];
					}
			}	
		
		
	}
	if (rank == 3){
		
		MPI_Recv(vTopology, nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
		for (int k = 0; k < nProcesses; k++){
				if (vTopology != 0){
					topology[2][k] = vTopology[k];
				}else {
					break;
				}
		}
		
		
		MPI_Send(topology[2], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 0);
		MPI_Send(topology[3], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 0);

		MPI_Recv(vTopology, nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[0][k] = vTopology[k];
					}else {
						break;
					}
		}
		MPI_Send(topology[0], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);
		MPI_Send(topology[3], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);


	}

	return topology;
}



// functie ce calculeaza topologia
int ** find_topology (int rank, int num_workers, int* workers, int nProcesses){
	int ** topology = malloc(sizeof(int*) * nProcesses);
	int * vTopology = calloc(sizeof(int), nProcesses);
	MPI_Status status;
	// aloc memorie pentru matricia de topologie
	for (size_t i = 0; i < nProcesses; i++) {
		topology[i] = calloc(sizeof(int), nProcesses);
	}

	// scriu datele rank-ului curent in topologie
	for (int i =0; i< num_workers; i++){
		topology[rank][i] = workers[i];
	}

	// pentru rank 0
	
	if (rank == 0){
		// primeste de la rank3 datele despre rank 1,2 si 3
		for (int j = 1; j < 4 ; j++){
			MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[j][k] = vTopology[k];
					}else {
						break;
					}
			}
		}

		// trimite date despre el rank-ului 3
		printf("M(%d,%d)\n",rank, 3);
		MPI_Send(topology[0], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
		
		
		
		
	}

	// pentru rank 1
	if (rank == 1){
		// trimite date despre el rank-ului 2
		MPI_Send(topology[1], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);


		// primeste de la rank 2 date despre 0,2 si 3
		for (int j = 0; j < 4 ; j++){
			if (j != 1){
				MPI_Recv(vTopology, nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
				for (int k = 0; k < nProcesses; k++){
						if (vTopology != 0){
							topology[j][k] = vTopology[k];
						}else{
							break;
						}
				}

			}
			
		}
		
	}

	// rank2
	if (rank == 2){
		// primeste date de la rank1 
			MPI_Recv(vTopology, nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[1][k] = vTopology[k];
					}else {
						break;
					}
			}
		

		// trimite catre rank 3 datte despre 1 si 2
		MPI_Send(topology[1], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);
		MPI_Send(topology[2], nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);

		// primeste date de la rank 3 despre 0 si 3
		MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[0][k] = vTopology[k];
					}
			}
		MPI_Recv(vTopology, nProcesses, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[3][k] = vTopology[k];
					}
			}	
		
		// trimite lui rank 1 date despre 0,2,si 3
		MPI_Send(topology[0], nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 1);
		MPI_Send(topology[2], nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 1);
		MPI_Send(topology[3], nProcesses, MPI_INT, 1, 0, MPI_COMM_WORLD);	
		printf("M(%d,%d)\n",rank, 1);

		
	}
	if (rank == 3){
		
		// primeste de la rank 2 date despre 1 si 2
		for (int j = 1; j < 3 ; j++){
			MPI_Recv(vTopology, nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
			for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[j][k] = vTopology[k];
					}else {
						break;
					}
			}
		}
		
		// trimite la rank 0 date despre 1 2 3
		MPI_Send(topology[1], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 0);
		MPI_Send(topology[2], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 0);
		MPI_Send(topology[3], nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 0);

		// primeste de la rank 0 date despre rank0
		MPI_Recv(vTopology, nProcesses, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		for (int k = 0; k < nProcesses; k++){
					if (vTopology != 0){
						topology[0][k] = vTopology[k];
					}else {
						break;
					}
		}

		// trimite la rank 2 date despre 0 si 3
		MPI_Send(topology[0], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);
		MPI_Send(topology[3], nProcesses, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);


	}


	//returnez topologia obtinuta
	return topology;

}

// functe apelata de clustere care trimit topologia workeri-lor
void send_to_workers(int rank, int **topology, int nProcesses, int num_workers, int *workers){
		// trimite tuturor workeri-lor pe care ii coordoneaza topologia
		
			for (int j = 0 ; j< num_workers; j++ ){
				for (int i = 0; i < 4; i++){
					MPI_Send(topology[i], nProcesses, MPI_INT, workers[j], 0, MPI_COMM_WORLD);
					printf("M(%d,%d)\n",rank, workers[j]);
				
				}

			}

}

// functie apelata de workers
// fiecare worker primeste topologia de la parinte
//returneaza procesul parinte
int receive_workers (int rank, int nProcesses){
	int ** topology = malloc(sizeof(int*) * nProcesses);
	for (size_t i = 0; i < nProcesses; i++) {
		topology[i] = calloc(sizeof(int), nProcesses);
	}


	int * vTopology = calloc(sizeof(int), nProcesses);
	int parent = -1;
	
	MPI_Status status;
	
	// fiecare worker asteapta 4 mesaje, unul pentru fiecare vector din topologie
		for (int i = 0; i < 4 ; i++){
			MPI_Recv(vTopology, nProcesses, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				
				for (int k = 0; k < nProcesses; k++){
						if (vTopology != 0){
							topology[i][k] = vTopology[k];
						}else {
							break;
						}
				}
			
		}
	
	
	// printeaza topologia primita
	print_topology(rank , topology, nProcesses);

	// returneaza procesul parinte (cel de la care a primit topologia)
	parent = status.MPI_SOURCE;
	return parent;	

}
// functie prin care fiecare cluster imparte o parte din vector fiecarui worker
// vectorul modificat este transmis inapoi si ajunge la clusterul 0 care il afiseaza
void my_func ( int rank, int *workers, int num_workers, int c, int vec[], int N,  int err){
	// tasks reprezinta numarul elemente din vector ce trebuie procesate
	// de catre workerii clusterului curent
	int tasks = c / num_workers;
	MPI_Status status;
	int vec_final[N];
	int m = 0;
	int number ;

	// se imparte vectorul la numarul de workerii ai parintelui
	for (int i = 0; i < num_workers; i++){
		int vec_aux[N];
		int L = i * tasks;
		int R;
		int k = 0;

		if (i != num_workers -1){
			R = L + tasks;

		}else {
			R  = c; 

		}
		// vectorul ce trebuie procesat de worker-ul cu indice i
		for (int j = L; j< R; j++ ){
			vec_aux[k] = vec[j];
			k++;
		}

		number = R-L;
		// trimit catre worker N
		// numarul de elemente ce trebuie procesate
		// vectorul cu elementele ce trebuie procesate
		MPI_Send(&N, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
		MPI_Send(&number, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
		MPI_Send(vec_aux, N, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, workers[i]);
		int vec_modif[N];

		// primesc vectorul modificat
		MPI_Recv(vec_modif, N, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status);
		
		// unesc vectorii trimisi de fiecare worker
		for (int p = 0; p< number; p++ ){
			vec_final[m] = vec_modif[p];

			m++;

		}

	

	}


	// cazul in care clusterul 1 este izolat
	if (err == 2) {
		if (rank == 0){
			int v2[N], v3[N];
			int n2,n3;
			int rezultat[N];
			int p =0;
			// ptimesc vectorii calculati de 2 si 3
			MPI_Recv(&n2 ,1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&n3 ,1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v2 ,N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v3 ,N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			// unesc vectorii procesati de 0, 2 si 3
			for (int i = 0; i< m; i++){
				rezultat[p] = vec_final[i];
				p++;
			}
			for (int i = 0; i< n2; i++){
				rezultat[p] = v2[i];
				p++;
			}
			for (int i = 0; i< n3; i++){
				rezultat[p] = v3[i];
				p++;
			}
			
			//afisez vectorul final
			printf("Rezultat:");
			for (int i = 0; i< N; i++ ){
				printf(" %d", rezultat[i]);
			}
			printf("\n");

		}
		
		if (rank == 2){
			// trimit lui 3 vectorul calculat de 2 
			MPI_Send(&m, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
			MPI_Send(vec_final, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 3);

		}
		if (rank == 3){
			int  v2[N];
			int  n2;
			// trimit lui 0 vectorii calculati de 2 si 3
			MPI_Recv(&n2 ,1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v2 ,N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

			MPI_Send(&n2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			MPI_Send(v2, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(vec_final, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 0);

			
		}

		// daca vectorul 1 nu este izolat
	}else {
			
		if (rank == 0){
			int v1[N], v2[N], v3[N];
			int n1,n2,n3;
			int rezultat[N];
			int p =0;
			// primesc vectorii procesati de 1,2 si 3 si dimensiunile acestora de la rank 3
			MPI_Recv(&n1 ,1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&n2 ,1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&n3 ,1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			MPI_Recv(v1 ,N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v2 ,N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v3 ,N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			// unesc toti vectorii in unul singur
			for (int i = 0; i< m; i++){
				rezultat[p] = vec_final[i];
				p++;
			}
			for (int i = 0; i< n1; i++){
				rezultat[p] = v1[i];
				p++;
			}
			for (int i = 0; i< n2; i++){
				rezultat[p] = v2[i];
				p++;
			}
			for (int i = 0; i< n3; i++){
				rezultat[p] = v3[i];
				p++;
			}

			// afisez vectorul
			printf("Rezultat:");
			for (int i = 0; i< N; i++ ){
				printf(" %d", rezultat[i]);
			}
			printf("\n");

		}
		if (rank == 1){
			// trimit lui 2 vectorul procesat de 1
			MPI_Send(&m, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
			MPI_Send(vec_final, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 2);


			
		}
		if (rank == 2){
			int v1[N];
			int n1;
			// primesc de la 1 vectorul procesat
			MPI_Recv(&n1 ,1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v1 ,N, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

			// trimit lui 3 vectorii procesati de 1 si 2
			MPI_Send(&n1, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);

			MPI_Send(v1, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
			MPI_Send(vec_final, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 3);

		}
		if (rank == 3){
			int v1[N], v2[N];
			int n1, n2;
			// primesc de la 2 vectorii procesati de 1 si 2
			MPI_Recv(&n1 ,1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&n2 ,1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

			MPI_Recv(v1 ,N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(v2 ,N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

			// trimit lui 0 vectorii procesati de 1 2 3
			MPI_Send(&n1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&n2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			MPI_Send(v1, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(v2, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(vec_final, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 0);

			
		}


	}



}

// functie apelata doar de CLUSTERUL 0
// imparte vectorul ce trebuie procesat
void split_work(int rank, int **topology, int nProcesses, int num_workers, int *workers, int N, int err){
	int left0, left1, left2, left3, right0, right1, right2, right3;
	int count=0;
	int vector[N];
	MPI_Status status;
	// creez vectorul
	for (int i = 0; i< N; i++){
		vector[i] = N- i -1;
	}

	
	// aflu numarul de workers pe care ii au restul clusterelor
	int num_workers1 =0;
	int num_workers2 =0;
	int num_workers3 =0;

	for (int j =0 ;j < nProcesses; j++){
		if (topology[1][j] != 0){
			num_workers1 ++;
		}
		if (topology[2][j] != 0){
			num_workers2 ++;
		}
		if (topology[3][j] != 0){
			num_workers3 ++;
		}
	}

	// daca clusterul 1 este izolat, nu ii atribui task-uri
	if (err == 2 ){
		// numarul de elemente din vector ce trebuie procesat de fiecare worker
		int tasks_per_worker = N/ (nProcesses - 4 -num_workers1);
		left0 = 0;
		right0 = tasks_per_worker * num_workers;

		left2 = right0;
		right2 = left2 +tasks_per_worker * num_workers2;

		left3 = right2;
		right3 = N;

		int c0 = 0, c2 = 0, c3 = 0;
		int vec0[N], vec2[N], vec3[N];
		// imparte vectorul celor 3 clustere in functie de numarul de workers
		for (int i = 0; i< N; i++){
			if ( i >= left0 && i < right0){
				vec0[c0] = vector[i];
				c0++;
			}
			if ( i >= left2 && i < right2){
				vec2[c2] = vector[i];
				c2++;
			}
			if ( i >= left3 && i < right3){
				vec3[c3] = vector[i];
				c3++;
			}
		}

		// trimit cluster-ului 3 vectorii impartiti , urmand ca acesta sa ii transmita mai departe
		MPI_Send(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&c3, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&c2, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);
		
		
		MPI_Send(vec3, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(vec2, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);

		// functie trimite la fiecare worker bucata de vector ce trebuie procesata
		my_func (rank, workers, num_workers, c0, vec0, N, err);

	// daca clusterul 1 nu este izolat
	}else {
		// numarul de elemente din vector ce trebuie procesat de fiecare worker
		int tasks_per_worker = N/ (nProcesses - 4);
		left0 = 0;
		right0 = tasks_per_worker * num_workers;

		left1 = right0 ;
		right1 = left1 + tasks_per_worker * num_workers1;

		left2 = right1;
		right2 = left2 +tasks_per_worker * num_workers2;

		left3 = right2;
		right3 = N;

		int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
		int vec0[N], vec1[N], vec2[N], vec3[N];
		// imparte vectorul celor 4 clustere in functie de numarul de workers
		for (int i = 0; i< N; i++){
			if ( i >= left0 && i < right0){
				vec0[c0] = vector[i];
				c0++;
			}
			if ( i >= left1 && i < right1){
				vec1[c1] = vector[i];
				c1++;
			}
			if ( i >= left2 && i < right2){
				vec2[c2] = vector[i];
				c2++;
			}
			if ( i >= left3 && i < right3){
				vec3[c3] = vector[i];
				c3++;
			}
		}
		
		// trimit cluster-ului 3 vectorii impartiti , urmand ca acesta sa ii transmita mai departe
		MPI_Send(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&c3, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&c2, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&c1, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);
		
		
		MPI_Send(vec3, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(vec2, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(vec1, N, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 3);

		// functie trimite la fiecare worker bucata de vector ce trebuie procesata
		my_func (rank, workers, num_workers, c0, vec0, N, err);


	}
	
			
}



void tasks_lvl1 (int rank, int *workers, int num_workers, int err ){
	MPI_Status status;
	// daca clusterul 1 este izolat
	if (err == 2){
		// clusterul 3 primeste subvectorii de la clusterul 0
		if (rank == 3){
		int c3, c2, N;
		MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&c3, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&c2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		int  vec2[N], vec3[N];
		MPI_Recv(vec3,N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(vec2, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// trimite lui 2 subvectorul corespunzator lui
		MPI_Send(&N, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&c2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
	
		MPI_Send(vec2, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);

		// functie trimite la fiecare worker bucata de vector ce trebuie procesata
		my_func (rank, workers, num_workers, c3, vec3, N, err);
	

		}

		if (rank == 2){
			int  c2, N;
			// clusterul 2 isi primeste subvectorul ce trebuie procesat de la clusterul 3
			MPI_Recv(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&c2, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			int vec2[N];
			MPI_Recv(vec2, N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			// functie trimite la fiecare worker bucata de vector ce trebuie procesata
			my_func (rank, workers, num_workers, c2, vec2, N, err);


		}

	
	// daca clusterul 1 nu este izolat
	}else{
		if (rank == 3){
		int c3, c2, c1, N;
		// clusterul 3 primeste subvectorii de la clusterul 0
		MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&c3, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&c2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&c1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		int vec1[N], vec2[N], vec3[N];
		MPI_Recv(vec3,N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(vec2, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(vec1, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// trimite lui 2 subvectorul corespunzator lui , si subvectorul pentru 1
		MPI_Send(&N, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&c2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&c1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
	
		MPI_Send(vec2, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(vec1, N, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(%d,%d)\n",rank, 2);

		// functie trimite la fiecare worker bucata de vector ce trebuie procesata
		my_func (rank, workers, num_workers, c3, vec3, N, err);
	

		}

		if (rank == 2){
			int  c2, c1, N;
			MPI_Recv(&N, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&c2, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&c1, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			int vec1[N], vec2[N];
			MPI_Recv(vec2, N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(vec1, N, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);

			// trimite lui 1subvectorul corespunzator lui
			MPI_Send(&N, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(&c1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(vec1, N, MPI_INT, 1, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, 1);

			// functie trimite la fiecare worker bucata de vector ce trebuie procesata
			my_func (rank, workers, num_workers, c2, vec2, N, err);


		}

		if (rank == 1){
			int c1, N;
			// primeste de la 2 subvectorul corespunzator lui
			MPI_Recv(&N, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&c1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

			int vec1[N];
			MPI_Recv(vec1, N, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

			// functie trimite la fiecare worker bucata de vector ce trebuie procesata
			my_func (rank, workers, num_workers, c1, vec1, N, err);

		}
	}
	

}

// functie apelata de workers, prin care isi primesc subvectorul si il modifica
void tasks_lvl2 (int rank , int parent, int err){
	MPI_Status status;
		// daca un worker il are parinte pe 1 si acesta este izolat, nu face nimic
		if (err ==2 && parent ==1){

		}else {
			int N ;
			int number; 
			// worker-ul primeste numarul de elemente ce trenuie procesate
			
			MPI_Recv(&N, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			int vec_aux[N];
			// primeste vectorul cu elemente ce trebuie procesate
			MPI_Recv(vec_aux, N, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			// modifica vectorul
			for (int  i = 0; i < number; i++){
				vec_aux[i] *= 5; 
			}
			// trimite vectorul inapoi parintelui
			MPI_Send(vec_aux, N, MPI_INT, parent, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n",rank, parent);
		}

}



int main(int argc, char * argv[]) {
	int rank, nProcesses;
	int parent;
	int ** topology;
	int N ; 
	int err;
	
	N = atoi(argv[1]);
	err = atoi(argv[2]);
	
	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	
	// procesul este parinte
	if (rank < 4){
		// citesc datele din fisier
		read_workers(rank);
			
		//construiesc topologia si o afisez
		if (err < 2){
			// clusterul 1 nu este izolat
			topology = find_topology( rank ,num_workers, workers, nProcesses);
		
		}else {
			// clusterul 1 este complet izolat
			topology = find_topology_err( rank ,num_workers, workers, nProcesses);
		}
		
		print_topology(rank , topology, nProcesses);
		// trimit catre workers topologia
		send_to_workers(rank, topology, nProcesses, num_workers,workers);
	
		if (rank == 0){
			// impart vectorul celorlalte procese
			split_work(rank, topology, nProcesses, num_workers,workers,N, err);
		} else {
			// primesc subvectorul ce trebuie procesat
			tasks_lvl1(rank, workers, num_workers, err);
		}
		
	// procesul este worker
	} else {
		// primesc topologia si aflu procesul parinte
		parent = receive_workers(rank, nProcesses);
		// primesc subvectorul ce trebuie procesat
		tasks_lvl2(rank, parent, err);
	}

	
	MPI_Finalize();
	return 0;
}




