#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>       
#include <limits.h>
#include <stdlib.h>
#include <cctype>
using namespace std;

#define MAX_THREADS 50

struct vector{
    int id;
    double x;
    double y;
};
list<vector> vectores;

double resultados[MAX_THREADS];

struct vectoresPorHilo{
    int hilo;
    list<vector> vectores;
};
list<vectoresPorHilo> hilos;

pthread_t tabla_thr[MAX_THREADS];

void ayuda();

void* imprime_mensaje(void *ptr) {
    int i;
    int id;

    // list<vector>::iterator it = vectores.begin();
    list<vectoresPorHilo>::iterator it = hilos.begin();

    id = *(int *) ptr;
    // cout<<id<<" "<<hilos.size()<<endl;
    if(id < hilos.size()){
        advance(it,id);
        vectoresPorHilo vph = *it;
        cout<<"El hilo "<<vph.hilo<<" realizara la suma de "<<vph.vectores.size()<<" vectores."<<endl;
        list<vector>::iterator itVectores = vph.vectores.begin();
        while(itVectores != vph.vectores.end())
        {
            double res = (itVectores->x)+(itVectores->y);
            // cout<<(*itVectores).x<<" "<<(*itVectores).y<<endl;
            // resultados.push_back(res);
            resultados[itVectores->id] = res;
            itVectores++;
        }
    }
    else
        cout<<"El hilo "<<id<<" realizara la suma de "<<0<<" vectores."<<endl;

    return 0;
}


int main(int argc, char *argv[])
{
    if(argc != 3 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-?") == 0){
        ayuda();
        return 0;
    }

    int cant_threads = atoi(argv[2]);

    pthread_t hilo0, hilo1,hilo2;
    int id0=0, id1=1, id2=2;

    pthread_t threads[cant_threads];
    int cant=0, cantVectores = 0;
    ifstream archivo;
    char path[100];
    realpath(argv[1],path);
    archivo.open(path);
    if (archivo.fail()) {
        cerr << "Error al abrir el archivo" << endl;
        archivo.close();
        exit(1);
    }
    int hiloActual = 0;
    while (!archivo.eof())
    {
        // cout<<"------------------------------"<<endl;
        vector v;
        string x, y;
        getline(archivo,x,',');
        v.x = stod(x);
        getline(archivo,y,'\n');
        v.y = stod(y);
        v.id = cantVectores;
        bool esta = false;
        // cout<<"hiloActual "<<hiloActual<<" lista "<<hilos.empty()<<endl;
        if(hilos.empty()){
            vectoresPorHilo vph;
            vph.hilo = hiloActual;
            vph.vectores.push_back(v);
            hilos.push_back(vph);
        }
        else{
            list<vectoresPorHilo>::iterator itHilos = hilos.begin();
            // for (itHilos = hilos.begin(); itHilos != hilos.end(); itHilos++)
            while(itHilos != hilos.end() && !esta)
            {
                // cout<<"itHilos->hilo == hiloActual "<<itHilos->hilo<<" "<<hiloActual<<" "<<(itHilos->hilo == hiloActual)<<endl;
                if(itHilos->hilo == hiloActual){
                    itHilos->vectores.push_back(v);
                    esta = true;
                }
                itHilos++;
            }
            if(!esta){
                // cout<<"no esta"<<endl;
                vectoresPorHilo vph;
                vph.hilo = hiloActual;
                vph.vectores.push_back(v);
                hilos.push_back(vph);
            }
        }

        hiloActual++;
        if(hiloActual >= cant_threads)
            hiloActual = 0;
        // if (pthread_create(&tabla_thr[cant],NULL,&imprime_mensaje,(void*)&cant)!= 0){
		// 	perror("pthread_create");
		// 	exit(1);
		// }
        // pthread_join(tabla_thr[cant],NULL);
        // cant++;
    // cout<<"hilos.size()"<<hilos.size()<<endl;
    //     cout<<"------------------------------"<<endl;
        cantVectores++;
    }
    hilos.begin();
    // cout<<"cant vectores "<<vectores.size()<<endl;
    // list<vectoresPorHilo>::iterator itHilos;
    // for (itHilos = hilos.begin(); itHilos != hilos.end(); itHilos++)
    // {
    //     cout<<"hilo "<<itHilos->hilo<<endl;
    //     cout<<"cant vectores que procesa "<<itHilos->vectores.size()<<endl;        
    // }

    while(cant < cant_threads){
        if (pthread_create(&tabla_thr[cant],NULL,&imprime_mensaje,(void*)&cant)!= 0){
            perror("pthread_create");
            exit(1);
        }
        pthread_join(tabla_thr[cant],NULL);
        cant++;
    }
    ofstream salida("salida");
    for(int i=0;i<cantVectores;i++)
    {
        salida<<resultados[i]<<endl;
    }
    return 0;
}
void ayuda()
{
    cout<<"Este proceso recibe dos parametros. El primero es el path del archivo de entrada (.txt)."<<endl;
	cout<<"Y el segundo la cantidad de hilos en la que se dividira la carga de trabajo."<<endl;
	cout<<"Ejemplo: ./ej2 ./archivo_datos 4"<<endl;
	cout<<"Realiza la suma vectorial de los pares ordenados que se encuentran en el .txt"<<endl;
    cout<<"E imprime el resultado en un archivo con el nombre salida en el path del proceso."<<endl;
}