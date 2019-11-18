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

#define PORT 5000

int configuracionCliente(char *servidor);
void Cliente(int clienteSockfd);

int main(int argc, char **argvs)
{
    system("clear");

	cout<<"conectando a "<<argvs[1]<<endl;

    struct sockaddr_in serv_addr;

    socklen_t addrlen = sizeof (serv_addr);

    int clienteSockfd;
    clienteSockfd = configuracionCliente(argvs[1]);
    /*llama funcion del cliente pasando descriptor*/	
    Cliente(clienteSockfd);
    exit(0);
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