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
int SIZE = 4096000;
string NOMBRE_MEMORIA = "mem";

sem_t* abrirSemaforo(const char* nombre);
void ayuda();

int main(int argc, char* argv[]) 
{ 
    semUsoMemoriaMutex = abrirSemaforo("/usomemoria");
    semEsperandoRtaMutex = abrirSemaforo("/esperandorespuesta");
    semHayRespuesta = abrirSemaforo("/hayrespuesta");
    semHayConsulta = abrirSemaforo("/hayconsulta");

    if(argc != 3){
        ayuda();
        return 0;
    }
    
    stringstream consultaStream;
    consultaStream<<argv[1]<<" "<<argv[2];
  
    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory object */
    void* ptr; 
  
    /* open the shared memory object */
    shm_fd = shm_open(NOMBRE_MEMORIA.c_str(), O_RDWR, 0666); 
  
    ftruncate(shm_fd, SIZE);
    /* memory map the shared memory object */
    ptr = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0); 
    sem_wait(semEsperandoRtaMutex);
    sem_wait(semUsoMemoriaMutex);
    // escribo consulta
    sprintf((char *)ptr, "%s", consultaStream.str().c_str());
    sem_post(semUsoMemoriaMutex);
    sem_post(semHayConsulta);
    sem_wait(semHayRespuesta);
    sem_wait(semUsoMemoriaMutex);
    // leo respuesta
    printf("%s", (char*)ptr);
    sem_post(semUsoMemoriaMutex);
    sem_post(semEsperandoRtaMutex);

    return 0; 
}

sem_t* abrirSemaforo(const char* nombre)
{
    return sem_open(nombre,0);
}
void ayuda()
{
    cout<<"Debe la consulta en el formato CAMPO VALOR."<<endl
        <<"Ejemplo: ./cliente id 16008"<<endl
        <<"Ejemplo: ./cliente articulo \"PALMITO MAROLIO RODAJA 800 gr\""<<endl
        <<"Ejemplo: ./cliente producto cafe"<<endl
        <<"Ejemplo: ./cliente marca natura"<<endl<<endl;
}