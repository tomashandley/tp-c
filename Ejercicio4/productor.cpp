#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

using namespace std;

struct articulo{
    int id;
    string articulo;
    string producto;
    string marca;
};
list<articulo> articulos{};

sem_t *semUsoMemoriaMutex;
sem_t *semEsperandoRtaMutex;
sem_t *semHayRespuesta;
sem_t *semHayConsulta;

void cargarArchivo();
sem_t* crearSemaforo(const char* nombre, int valor);
int main() 
{ 
    semUsoMemoriaMutex = crearSemaforo("/usomemoria",1);
    semEsperandoRtaMutex = crearSemaforo("/esperandorespuesta",1);
    semHayRespuesta = crearSemaforo("/hayrespuesta",0);
    semHayConsulta = crearSemaforo("/hayconsulta",0);

    /* the size (in bytes) of shared memory object */
    const int SIZE = 4096000; 
  
    /* name of the shared memory object */
    const char* name = "OS"; 
  
    /* strings written to shared memory */
    const char* message_0 = "Hello"; 
    const char* message_1 = "World!"; 
  
    cargarArchivo();

    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory obect */
    void* ptr; 
  
    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
  
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE); 
  
    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0); 
      
    while(1){
        sem_wait(semHayConsulta);
        sem_wait(semUsoMemoriaMutex);
        // leo consulta
        printf("%s", (char*)ptr);        
        // sem_post(semUsoMemoriaMutex);
        // proceso consulta
        // sem_wait(semUsoMemoriaMutex);
        // escribo respuesta
        // sem_post(semUsoMemoriaMutex);
        // sem_post(semHayRespuesta);
    }
    stringstream respuestaStream;
    // respuestaStream << a.id << " \t " << a.marca << " \t " << a.producto << "\t" << a.articulo <<endl;
    list<articulo>::iterator it;
    for (it = articulos.begin(); it != articulos.end(); it++)
    {
        respuestaStream << it->id << " \t " << it->marca << " \t " << it->producto << "\t" << it->articulo <<endl;
    }
    /* write to the shared memory object */
    // sprintf((char *)ptr, "%s", message_0); 
    sprintf((char *)ptr, "%s", respuestaStream.str().c_str());

    return 0;
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
    	getline(archivoArticulos,art.marca,'\r');
        articulos.push_back(art);
    }
    archivoArticulos.close();
    cout << articulos.size() << endl;
}
sem_t* crearSemaforo(const char* nombre, int valor)
{
    return sem_open(nombre, O_CREAT, S_IRUSR | S_IWUSR, valor);
}