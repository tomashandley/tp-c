#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <semaphore.h>
#include <iostream>
#include <sstream>

using namespace std;

sem_t *semUsoMemoriaMutex;
sem_t *semEsperandoRtaMutex;
sem_t *semHayRespuesta;
sem_t *semHayConsulta;

sem_t* abrirSemaforo(const char* nombre);

int main(int argc, char* argv[]) 
{ 
    abrirSemaforo("/usomemoria");
    abrirSemaforo("/esperandorespuesta");
    abrirSemaforo("/hayrespuesta");
    abrirSemaforo("/hayconsulta");

    cout<<argc<<endl;
    int i;
    for(i=0;i<argc;i++)
        cout<<argv[i]<<endl;
    stringstream consultaStream;
    consultaStream<<argv[1]<<" "<<argv[2];
    cout<<consultaStream.str()<<endl;
    // return 0;

    /* the size (in bytes) of shared memory object */
    const int SIZE = 4096000; 
  
    /* name of the shared memory object */
    const char* name = "OS"; 
  
    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory object */
    void* ptr; 
  
    /* open the shared memory object */
    shm_fd = shm_open(name, O_RDWR, 0666); 
  
    ftruncate(shm_fd, SIZE);
    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0); 
  
    sem_wait(semEsperandoRtaMutex);
    sem_wait(semUsoMemoriaMutex);
    // escribo consulta
    sprintf((char *)ptr, "%s", consultaStream.str().c_str());

    sem_post(semUsoMemoriaMutex);
    sem_post(semHayConsulta);
    // sem_wait(semHayRespuesta);
    // sem_wait(semUsoMemoriaMutex);
    // // leo respuesta
    // sem_post(semUsoMemoriaMutex);
    // sem_post(semEsperandoRtaMutex);
    /* read from the shared memory object */
    // printf("%s", (char*)ptr); 
  
    /* remove the shared memory object */
    shm_unlink(name); 
    return 0; 
}

sem_t* abrirSemaforo(const char* nombre)
{
    return sem_open(nombre,0);
}