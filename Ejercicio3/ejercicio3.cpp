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

#include <limits.h>
#include <stdlib.h>

using namespace std;

struct articulo{
    int id;
    string articulo;
    string producto;
    string marca;
};
list<articulo> articulos{};
string fifoConsulta;
string fifoRespuesta;
int fdConsulta;
int fdRespuesta;

static void createDaemonProcess();
void ayuda();
void procesar(int fdConsulta, int fdRespuesta);
void cargarArchivo(string path);
void handler(int);

int main(int argc, char* argv[])
{
    if(argc != 4 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-?") == 0){
        ayuda();
        return 0;
    }
    createDaemonProcess();

    signal(SIGTERM, handler);

    char path[100];

    realpath(argv[1],path);
    cargarArchivo(path);

    realpath(argv[2],path);
    fifoConsulta = path;
    unlink(fifoConsulta.c_str());
    mkfifo(fifoConsulta.c_str(),0666);
    fdConsulta = open(fifoConsulta.c_str(),O_RDWR);
    
    realpath(argv[3],path);
    fifoRespuesta = path;
    unlink(fifoRespuesta.c_str());
    mkfifo(fifoRespuesta.c_str(),0666);
    fdRespuesta = open(fifoRespuesta.c_str(),O_RDWR);
    
    procesar(fdConsulta, fdRespuesta);
}


static void createDaemonProcess()
{
    pid_t pid, sid;
    // printf("parent pid: %d\n", getpid());
    pid = fork(); // fork a new child process
    if(pid != 0)
        cout<<"Pid proceso demonio: "<<pid<<endl;
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
    cout<<"Este proceso recibe por parametro el path del archivo de articulos, el path del fifo del que lee las consultas y el path del fifo donde escribira las respuestas"<<endl
        <<"Ejemplo: ./ejercicio3 ./articulos.txt ./fifoConsulta ./fifoRespuesta"<<endl
        <<"Crea el demonio y devuelve su pid."<<endl
        <<"Para finalizarlo ejecute: kill -15 pid"<<endl<<endl;
}
void procesar(int fdConsulta, int fdRespuesta)
{
    char str[100];
    while(1){
        // leo consulta   
        int n = read(fdConsulta,str,100);
        string consulta = string(str).substr(0,n-1);
        for_each(consulta.begin(),consulta.end(),[](char &c){
            c = ::toupper(c);
        });
        articulo pedido = {};
        string campo,valor;
    	stringstream ss(consulta),respuestaStream;
        getline(ss,campo,'=');
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
        
        // proceso consulta
        int cant = 0;
        list<articulo>::iterator it;
        respuestaStream << endl << "Los articulos con " << campo << " igual a " << valor << " son:" << endl
                            << "ID\t\tARTICULO\t\tPRODUCTO\tMARCA"<<endl;
        for (it = articulos.begin(); it != articulos.end(); it++)
        {
            if(pedido.id == it->id ||
                pedido.articulo == it->articulo ||
                pedido.producto == it->producto ||
                pedido.marca == it->marca){
                    cant++;
                    respuestaStream << it->id << " \t " << it->articulo << " \t " << it->producto << "\t" << it->marca <<endl;
                }
        }

        string respuesta;
        if(cant > 0)
            respuesta = respuestaStream.str().c_str();
        else{
            respuesta = "No hay ningun articulo que coincida con la busqueda: ";
            respuesta.append(consulta);
            respuesta.append("\n");
        }
        write(fdRespuesta, respuesta.c_str(), respuesta.length());
    }
}
void cargarArchivo(string path)
{
    ifstream archivoArticulos;
    archivoArticulos.open(path);
    if (archivoArticulos.fail()) {
        cerr << "Error al abrir el archivo" << endl;
        archivoArticulos.close();
        exit(1);
    }
    int i = 0;
    while (!archivoArticulos.eof())
    {
        articulo art;
        string id;
        getline(archivoArticulos,id,';');
        art.id = atoi(id.c_str());
        getline(archivoArticulos,art.articulo,';');
        for_each(art.articulo.begin(),art.articulo.end(),[](char &c){
            c = ::toupper(c);
        });
        getline(archivoArticulos,art.producto,';');
        for_each(art.producto.begin(),art.producto.end(),[](char &c){
            c = ::toupper(c);
        });
        getline(archivoArticulos,art.marca,'\r');
        for_each(art.marca.begin(),art.marca.end(),[](char &c){
            c = ::toupper(c);
        });
        if(i > 0 && art.id > 0 && art.articulo != "" && art.producto != "" && art.marca != "")
            articulos.push_back(art);
        i++;
    }
    archivoArticulos.close();
    // cout << articulos.size() << endl;
}
void handler(int)
{
    close(fdConsulta);
    unlink(fifoConsulta.c_str());
    close(fdRespuesta);
    unlink(fifoRespuesta.c_str());
}