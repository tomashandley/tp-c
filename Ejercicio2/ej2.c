#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

double *vec ;
pthread_mutex_t lock;
double sum= 0;
double resx = 0;
double resy = 0;
int total_elementos = 0,cant_elementos_thread = 0, cant_threads = 0;
int posvec = 0;

void ayuda();

void *threadIni(void *vargp){
 	int *myid = (int *)vargp;
 	int cant_elementos_operar = cant_elementos_thread;
 	pthread_mutex_lock(&lock);
	// printf("Thread ID: %d\n", *myid);
	while(cant_elementos_operar > 0 && total_elementos > 0){

        // if(((int)(*vec + posvec) % 2) == 0) {
		if((posvec % 2) == 0) {
            resx += *(vec + posvec);
        } else {
            resy += *(vec + posvec);
        }
		posvec++;
		cant_elementos_operar--;
		total_elementos--;
	}
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}


int main(int argc, char const *argv[]){
	if(argc != 3 || strcmp(argv[1],"-h") || strcmp(argv[1],"-help") || strcmp(argv[1],"-?")){
		ayuda();
		return 1;
	}
	
	vec = (double*) malloc(32);
	char *first=malloc(16);
	char *second = malloc(16);
	double fir = 0,sec = 0;
	int i = 0;

	cant_threads = atoi(argv[2]);

    char const* const fileName = argv[1]; /* should check that argc > 1 */
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[512];

	while (fgets(line, sizeof(line), file) != NULL) {
    //    /* note that fgets don't strip the terminating \n, checking its
    //       presence would allow to handle lines longer that sizeof(line) */

    	int size = sizeof(line);
		second = strchr(line,',') + 1 ;
		strncpy(first,line,5);
		sscanf(second,"%lf",&sec);
		sscanf(first,"%lf",&fir);
		double res = fir + sec;
		*(vec + i) = fir;
		i++;
		*(vec + i) = sec;
		i++;
		total_elementos += 2;
    }

    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    cant_elementos_thread = (int)total_elementos / cant_threads + (total_elementos % cant_threads != 0);
    printf("Cada thread va a operar: %d elementos\n", cant_elementos_thread);
    fclose(file);

    pthread_t tid;

  	if(pthread_mutex_init(&lock,NULL) != 0){
  		  printf("\n mutex init failed\n");
        return 1;
  	}

    for (i = 0; i < cant_threads; i++) {
        pthread_create(&tid, NULL, threadIni, (void *)&i);
    }

    while(total_elementos > 0){
    }

    // printf("total : %lf\n",sum);

    FILE* fout = fopen("salida","w");
    // fprintf(fout, "Cantidad operaciones thread : %d\n",cant_elementos_thread);
    fprintf(fout, "Suma : %lf , %lf\n",resx, resy);
    fclose(fout);
    return 1;
}
void ayuda()
{
	printf("Este proceso recibe dos parametros. El primero es el path del archivo de entrada (.txt).\n");
	printf("Y el segundo la cantidad de hilos en la que se dividira la carga de trabajo.\n");
	printf("Ejemplo: ./ej2 ./archivo_datos 4\n");
	printf("Realiza la suma vectorial de los pares ordenados que se encuentran en el .txt\n");
}