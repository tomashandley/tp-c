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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <algorithm>
#include <csignal>

using namespace std;

#define MAX 10

struct articulo{
    int id;
    string articulo;
    string producto;
    string marca;
};

list<articulo> articulos{};
int CONECTADOS = 0;
int PORT;
int sockEntrada;

void cargarArchivo(string path);
int configuracionServidor();
void* Servidor(void* arg);
void ayuda();
static void createDaemonProcess();
void handler(int);

int main(int argc, char **argvs)
{
    if(argc != 3){
        ayuda();
        return 0;
    }
    createDaemonProcess();

    signal(SIGTERM, handler);
    PORT = atoi(argvs[1]);
    char path[100];
    realpath(argvs[2],path);

    cargarArchivo(path);

    int sockfd = configuracionServidor();

	//printf("Esperando clientes\n");
	while (1)
	{
        int clienteSockfd;
        struct sockaddr_in clienteAddr;
    /*tamaño de la estrutura*/	
        unsigned int clntLen;
        clntLen = sizeof (clienteAddr);
    /*declara un thread*/
        pthread_t thread;

        if ((clienteSockfd = accept(sockfd, (struct sockaddr *) & clienteAddr, &clntLen)) < 0)
        {
      //      printf("Error en Socket\n");
            exit(1);
        }

        /*Inicializa thread*/
        if (pthread_create(&thread, NULL, Servidor, &clienteSockfd) != 0)
        {
        //    printf("No se pudo crear el thread\n");
            exit(1);
        }
        //printf("\nNUEVO CLIENTE CONECTADO\n");
        CONECTADOS++;
        //printf("%d clientes conectados\n",CONECTADOS);
        pthread_detach(thread);

    }
    return 0;
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
int configuracionServidor()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
    //  printf("Error en el Socket\n");
      exit(1);
    }

    memset(&serverAddr, 0, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) & serverAddr, sizeof (serverAddr)) < 0)
    {
    //  printf("Error en Socket\n");
      exit(1);
    }

    if (listen(sockfd, MAX) < 0)
    {
    //  printf("Error en Socket\n");
      exit(1);
    }

    return sockfd;
}
void* Servidor(void* arg)
{
    sockEntrada = *(int *) arg;
    char buffer_cliente[256],cad[256];
    string consulta;

    strcpy(cad,"Bienvenida/o ingrese la consulta en el formato CAMPO=VALOR o QUIT para salir:");
    write(sockEntrada,cad,sizeof(cad));

    //leo consulta en formato campo=valor
    read(sockEntrada, buffer_cliente, sizeof (buffer_cliente));
    consulta = buffer_cliente;
    for_each(consulta.begin(),consulta.end(),[](char &c){
        c = ::toupper(c);
    });
    // cout<<"cliente: "<<sockEntrada<<" consulta: "<<consulta<<endl<<endl;
    
    while(consulta != "QUIT"){
        articulo pedido = {};
        string campo,valor,respuesta;
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

        list<articulo>::iterator it;
        int cant = 0;
        for (it = articulos.begin(); it != articulos.end(); it++)
        {
            if(pedido.id == it->id ||
                pedido.articulo == it->articulo ||
                pedido.producto == it->producto ||
                pedido.marca == it->marca){
                    cant++;
            }
        }
        
        //cout<<endl<<cant<<" articulos coinciden con la consulta"<<endl;
        sprintf (cad, "%d",cant);
	    write(sockEntrada,cad,sizeof(cad));

        if(cant > 0){
            respuestaStream << "Los articulos con " << campo << " igual a " << valor << " son:" << endl
                            << "ID\t\tARTICULO\t\tPRODUCTO\tMARCA";
            strcpy(cad,respuestaStream.str().c_str());
            write(sockEntrada,cad,sizeof(cad));

            for (it = articulos.begin(); it != articulos.end(); it++)
            {
                if(pedido.id == it->id ||
                    pedido.articulo == it->articulo ||
                    pedido.producto == it->producto ||
                    pedido.marca == it->marca){
                        sprintf (cad, "%d \t %s \t %s \t %s",it->id,
                                it->articulo.c_str(),it->producto.c_str(),
                                it->marca.c_str());
                            write(sockEntrada,cad,sizeof(cad));
                }
            }
        }

        strcpy(cad,"Ingrese la consulta en el formato CAMPO=VALOR o QUIT para salir:");
        write(sockEntrada,cad,sizeof(cad));

        //leo consulta en formato campo=valor
        read(sockEntrada, buffer_cliente, sizeof (buffer_cliente));
        consulta = buffer_cliente;
        for_each(consulta.begin(),consulta.end(),[](char &c){
            c = ::toupper(c);
        });
        // cout<<"cliente: "<<sockEntrada<<" consulta: "<<consulta<<endl<<endl;
    }
    
    CONECTADOS--;
    //cout<<endl<<"SE HA DESCONECTADO UN CLIENTE, HAY "<<CONECTADOS<<" CONECTADOS"<<endl;

    // close(sockEntrada);

    pthread_exit((void*) 0);
}
void ayuda()
{
    cout<<"Debe ingresar el PUERTO que escuchara el servidor y el PATH del archivo de articulos."<<endl
        <<"Ejemplo: ./servidor 5000 ./articulos.txt"<<endl;
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
void handler(int)
{
    close(sockEntrada);

    pthread_exit((void*) 0);
}