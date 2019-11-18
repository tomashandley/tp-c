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

using namespace std;

#define PORT 5000

int configuracionCliente(char *servidor);
void Cliente(int clienteSockfd);

int main(int argc, char **argvs)
{
    system("clear");

	printf("conectando a %s\n",argvs[1]);

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
        printf("Error en Socket\n");
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
        printf("Error en Socket\n");
        exit(1);
    }
    return clienteSockfd;
}

void Cliente(int clienteSockfd)
{
    char buffer_para_servidor[256],cad[256];
    string buffer;

    read(clienteSockfd, cad, sizeof (cad));//bienvenido
    printf("%s\n",cad);

    //scanf("%s", buffer_para_servidor);
    getline(cin,buffer);
    cout<<buffer<<endl;
    write(clienteSockfd, buffer.c_str(), sizeof (buffer));

    while(strcmp(buffer_para_servidor,"QUIT") != 0){

        read(clienteSockfd, cad, sizeof (cad));//bienvenido
        printf("%s\n",cad);

        getline(cin,buffer);
        cout<<buffer<<endl;
        write(clienteSockfd, buffer.c_str(), sizeof (buffer));
    }
    close(clienteSockfd);
}