#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

struct articulo{
    int id;
    string articulo;
    string producto;
    string marca;
};
list<articulo> articulos{};

struct art{
    list<articulo> articulos{};
};

void cargarArchivo();
//semaforo usoMemoriaMutex, esperandoRespuestaMutex, hayRespuestaMutex
void* crearMemoria(const char* nombre, size_t size);
int main()
{
    cout<<"servidor"<<endl;

    cargarArchivo();
    cout<<articulos.back().id<<" "<<articulos.back().articulo<<" "<<articulos.back().producto<<" "<<articulos.back().marca<<endl;

    int fd = shm_open("miMemoria",O_CREAT|O_RDWR,0600);
    //ar = (list<articulo> *)crearMemoria("/mem",sizeof(list<articulo>));
    ftruncate(fd,sizeof(articulo)*(articulos.size()));
    // ftruncate(fd,sizeof(int)*6);

//    list<int> numeros{1,2,3,4,5,6};
//    cout<<numeros.size()<<endl;
//    list<int> *lista;
//    lista = (list<int> *)mmap(NULL,sizeof(int)*6,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
//    memcpy(lista,&numeros,(sizeof(int)*6)+1);
    list<articulo> *lista;
    lista = (list<articulo> *)mmap(NULL,sizeof(articulo)*articulos.size(),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    memcpy(lista,&articulos,(sizeof(articulo)*articulos.size()));
    
    munmap(lista,(sizeof(articulo)*articulos.size()));
    close(fd);
}

void cargarArchivo()
{
    ifstream archivoArticulos;
    archivoArticulos.open("articulos.txt");
    if (archivoArticulos.fail()) {
        cerr << "Error al abrir el archivo" << endl;
        archivoArticulos.close();
        exit(1);
    }
    
    while (!archivoArticulos.eof())
    {
    	articulo art;
        string id;
    	getline(archivoArticulos,id,';');
    	art.id = atoi(id.c_str());
    	getline(archivoArticulos,art.articulo,';');
    	getline(archivoArticulos,art.producto,';');
    	getline(archivoArticulos,art.marca);
        articulos.push_back(art);
    }
    archivoArticulos.close();
    cout << articulos.size() << endl;
}
void* crearMemoria(const char* nombre, size_t size)
{
    int fd = shm_open(nombre, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, size);
    void *dir = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    return dir;
}