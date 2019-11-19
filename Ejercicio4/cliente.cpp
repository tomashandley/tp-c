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
#include <string.h>

using namespace std;

struct articulo{
    int id;
    string articulo;
    string producto;
    string marca;
};

int main()
{
    cout<<"cliente"<<endl;
    int fd = shm_open("miMemoria",O_RDWR,0600);
    list<articulo> *lista;
    lista = (list<articulo> *)mmap(NULL,sizeof(articulo)*3,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    close(fd);
    cout<<(*lista).size()<<endl;
    cout<<(*lista).back().producto<<endl;
    cout<<(*lista).size()<<endl;
    cout<<(*lista).size()<<endl;
    cout<<(*lista).size()<<endl;
    // articulo a = *numeros.back();
    // cout<<a.producto<<endl;
    list<articulo>::iterator it;
    for (it = (*lista).begin(); it != (*lista).end(); it++)
    {
        cout<<"asd"<<endl;
    }
    munmap(lista,sizeof(int)*6);
    shm_unlink("miMemoria");
}