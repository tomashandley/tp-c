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

using namespace std;

#define PORT 5000
#define MAX 10

int CONECTADOS = 0;

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
int configuracionServidor();
void* Servidor(void* arg);

int main()
{
    cout<<"servidor"<<endl;

    cargarArchivo();

    int sockfd = configuracionServidor();

	printf("Esperando clientes\n");
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
            printf("Error en Socket\n");
            exit(1);
        }

        /*Inicializa thread*/
        //printf("Esperando clientes...\n");
        if (pthread_create(&thread, NULL, Servidor, &clienteSockfd) != 0)
        {
            printf("No se pudo crear el thread\n");
            exit(1);
        }
        printf("\nNUEVO CLIENTE CONECTADO\n");
        CONECTADOS++;
        printf("%d clientes conectados\n",CONECTADOS);
        pthread_detach(thread);

    }
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
int configuracionServidor()
{
     int sockfd;
    struct sockaddr_in serverAddr;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      printf("Error en el Socket\n");
      exit(1);
    }

    memset(&serverAddr, 0, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) & serverAddr, sizeof (serverAddr)) < 0)
    {
      printf("Error en Socket\n");
      exit(1);
     }

    if (listen(sockfd, MAX) < 0)
    {
      printf("Error en Socket\n");
      exit(1);
    }

    return sockfd;
}
void* Servidor(void* arg)
{
    int sockEntrada = *(int *) arg;
    char buffer_cliente[256],cad[256];
    strcpy(cad,"Bienvenida/o ingrese la consulta en el formato CAMPO VALOR o QUIT para salir:");
    write(sockEntrada,cad,sizeof(cad));

    //leo consulta campo valor
    read(sockEntrada, buffer_cliente, sizeof (buffer_cliente));
    cout<<"consulta: "<<buffer_cliente<<endl<<endl;
    
    while(strcmp(buffer_cliente,"QUIT") != 0){
        articulo pedido = {};
        string campo,valor,buffer(buffer_cliente);
    	stringstream ss(buffer);
        getline(ss,campo,' ');
        std::for_each(campo.begin(),campo.end(),[](char &c){
            c = ::tolower(c);
        });
        getline(ss,valor);

        if(campo == "id"){
            pedido.id = atoi(valor.c_str());
        }
        else if(campo == "articulo"){
            pedido.articulo = valor;
        }
        else if(campo == "producto"){
            pedido.producto = valor;
        }
        else if(campo == "marca"){
            pedido.marca = valor;
        }
        cout<<"id: "<<pedido.id<<endl
            <<"articulo: "<<pedido.articulo<<endl
            <<"marca: "<<pedido.marca<<endl
            <<"producto: "<<pedido.producto<<endl<<endl;

        strcpy(cad,"Ingrese la consulta en el formato CAMPO VALOR o QUIT para salir:");
        write(sockEntrada,cad,sizeof(cad));

        //leo opcion elegida
        read(sockEntrada, buffer_cliente, sizeof (buffer_cliente));
        cout<<"consulta: "<<buffer_cliente<<endl;
    }
    
    CONECTADOS--;
    printf("\nSE HA DESCONECTADO UN CLIENTE, HAY %d CONECTADOS\n",CONECTADOS);

    close(sockEntrada);

    pthread_exit((void*) 0);
}