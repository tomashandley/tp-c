#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int PORT;

int configuracionCliente(char *servidor);
void Cliente(int clienteSockfd);
void ayuda(string s);

int main(int argc, char **argvs)
{
    system("clear");
    if(argc != 3){
        ayuda("-h");
        return 0;
    }

	cout<<"Conectando a "<<argvs[1]<<":"<<argvs[2]<<endl;
    PORT = atoi(argvs[2]);
    struct sockaddr_in serv_addr;

    socklen_t addrlen = sizeof (serv_addr);

    int clienteSockfd;
    clienteSockfd = configuracionCliente(argvs[1]);
    
    Cliente(clienteSockfd);
    return 0;
}

int configuracionCliente(char *servidor)
{
    int clienteSockfd;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof (serv_addr);

    clienteSockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clienteSockfd < 0)
    {
        cout<<"Error en Socket"<<endl;
        exit(1);
    }
    bzero((char *) & serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;

    if (strcmp(servidor, "localhost") == 0)
		strcpy(servidor,"127.0.0.1");
    serv_addr.sin_addr.s_addr = inet_addr(servidor);
    serv_addr.sin_port = htons(PORT);

    if(connect(clienteSockfd,(struct sockaddr *)&serv_addr, addrlen) < 0)
    {
        cout<<"Error en Socket"<<endl;
        exit(1);
    }
    return clienteSockfd;
}

void Cliente(int clienteSockfd)
{
    char cad[256];
    string buffer_para_servidor;

    read(clienteSockfd, cad, sizeof (cad));//bienvenido
    cout<<cad<<endl;

    getline(cin,buffer_para_servidor);
    for_each(buffer_para_servidor.begin(),buffer_para_servidor.end(),[](char &c){
        c = ::toupper(c);
    });
    write(clienteSockfd, buffer_para_servidor.c_str(), sizeof (buffer_para_servidor));

    while(buffer_para_servidor != "QUIT"){
        read(clienteSockfd, cad, sizeof (cad));
        int cant = atoi(cad);
        
        if(cant > 0){
            cout<<endl<<cant<<" articulos coinciden con la consulta"<<endl;
            read(clienteSockfd, cad, sizeof (cad));
            cout<<cad<<endl;
            for(int i=0;i<cant;i++){
                read(clienteSockfd, cad, sizeof (cad));
                cout<<cad<<endl;
            }
        }
        else{
            cout<<"No hay articulos que coincidan con la consulta."<<endl;
        }

        read(clienteSockfd, cad, sizeof (cad));//bienvenido
        cout<<endl<<cad<<endl;

        getline(cin,buffer_para_servidor);
        for_each(buffer_para_servidor.begin(),buffer_para_servidor.end(),[](char &c){
            c = ::toupper(c);
        });
        write(clienteSockfd, buffer_para_servidor.c_str(), sizeof (buffer_para_servidor));
    }
    close(clienteSockfd);
}
void ayuda(string s)
{
    if(s == "-h" || s == "-help" || s == "-?")
        cout<<"Debe ingresar la IP y el PUERTO del servidor al que se desea conectar."<<endl
            <<"Ejemplo: ./cliente 127.0.0.1 5000"<<endl<<endl
            <<"Una vez conectado al servidor, este le pedira que ingrese la consulta en el formato CAMPO VALOR."<<endl
            <<"Ejemplo: id 16008"<<endl
            <<"Ejemplo: articulo PALMITO MAROLIO RODAJA 800 gr"<<endl
            <<"Ejemplo: producto cafe"<<endl
            <<"Ejemplo: marca natura"<<endl<<endl;
}