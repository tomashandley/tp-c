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
#include <algorithm>
#include <csignal>

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
int SIZE = 4096000;
string NOMBRE = "mem";

void cargarArchivo();
sem_t* crearSemaforo(const char* nombre, int valor);
void handler(int);
static void createDaemonProcess();
void ayuda();

int main(int argc, char* argv[]) 
{ 
    if(argc > 1 || argv[1] == "-h" || argv[1] == "-help" || argv[1] == "-?"){
        ayuda();
        return 0;
    }
    createDaemonProcess();

    signal(SIGINT, handler);

    semUsoMemoriaMutex = crearSemaforo("/usomemoria",1);
    semEsperandoRtaMutex = crearSemaforo("/esperandorespuesta",1);
    semHayRespuesta = crearSemaforo("/hayrespuesta",0);
    semHayConsulta = crearSemaforo("/hayconsulta",0);

    cargarArchivo();

    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory obect */
    void* ptr; 
  
    /* create the shared memory object */
    shm_fd = shm_open(NOMBRE.c_str(), O_CREAT | O_RDWR, 0666); 
  
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE); 
  
    /* memory map the shared memory object */
    ptr = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0); 
    
    while(1){

	    sem_wait(semHayConsulta);
	    sem_wait(semUsoMemoriaMutex);
        // leo consulta   
        string consulta = string((char*)ptr);
        for_each(consulta.begin(),consulta.end(),[](char &c){
            c = ::toupper(c);
        });
        articulo pedido = {};
        string campo,valor,respuesta;
    	stringstream ss(consulta),respuestaStream;
        getline(ss,campo,' ');
        getline(ss,valor);
        if(campo == "ID"){
            pedido.id = atoi(valor.c_str());
        }
        else if(campo == "ARTICULO"){
            pedido.articulo = valor;
        }
        else if(campo == "PRODUCTO"){
            pedido.producto = valor;
        }
        else if(campo == "MARCA"){
            pedido.marca = valor;
        } 

        sem_post(semUsoMemoriaMutex);
        // proceso consulta
        list<articulo>::iterator it;
        for (it = articulos.begin(); it != articulos.end(); it++)
        {
            if(pedido.id == it->id ||
                pedido.articulo == it->articulo ||
                pedido.producto == it->producto ||
                pedido.marca == it->marca){
                    respuestaStream << it->id << " \t " << it->articulo << " \t " << it->producto << "\t" << it->marca <<endl;
                }
        }
        sem_wait(semUsoMemoriaMutex);
        // escribo respuesta
        sprintf((char *)ptr, "%s", respuestaStream.str().c_str());
        sem_post(semUsoMemoriaMutex);
        sem_post(semHayRespuesta);
    }
    
    /*
    stringstream respuestaStream;
    // respuestaStream << a.id << " \t " << a.marca << " \t " << a.producto << "\t" << a.articulo <<endl;
    list<articulo>::iterator it;
    for (it = articulos.begin(); it != articulos.end(); it++)
    {
        respuestaStream << it->id << " \t " << it->marca << " \t " << it->producto << "\t" << it->articulo <<endl;
    }
    // sprintf((char *)ptr, "%s", message_0); 
    sprintf((char *)ptr, "%s", respuestaStream.str().c_str());
    */
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
    //cout << articulos.size() << endl;
}
sem_t* crearSemaforo(const char* nombre, int valor)
{
    return sem_open(nombre, O_CREAT, S_IRUSR | S_IWUSR, valor);
}
void handler(int sig)
{
	sem_close(semEsperandoRtaMutex);
	sem_unlink("/esperandorespuesta");
    sem_close(semHayConsulta);
    sem_unlink("/hayconsulta");
    sem_close(semHayRespuesta);
    sem_unlink("/hayrespuesta");
    sem_close(semUsoMemoriaMutex);
    sem_unlink("/usomemoria");
	shm_unlink(NOMBRE.c_str());
}
static void createDaemonProcess()
{
    pid_t pid, sid;
    printf("parent pid: %d\n", getpid());
    pid = fork(); // fork a new child process
    if(pid != 0)
        printf("fork pid proceso demonio: %d\n", pid);
    if (pid < 0)
    {
        printf("fork failed!\n");
        exit(1);
    }
    if (pid > 0) // its the parent process
    {
        exit(0); //terminate the parent process succesfully
    }
    

    umask(0);       //unmasking the file mode
    sid = setsid(); //set new session
    if (sid < 0)
    {
        exit(1);
    }
    //close(STDIN_FILENO);
    //close(STDERR_FILENO);
}
void ayuda()
{
    cout<<"Este proceso crea el demonio y devuelve su pid."<<endl
        <<"Para finalizarlo ejecute: kill -2 pid"<<endl<<endl;
}