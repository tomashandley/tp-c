#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>

int stay() {
    while(1 == 1) {

    }
}

int contar()
{
    int cont;
    while(1)
    {
        sleep(2);
        cont = 0;
        for(int i=0; i<1000000 ; i++)
            cont++;
    }

    return 0;
}

int main() /// PADRE
{
    pid_t hijo1;
    pid_t hijo2;

    hijo1 = fork(); /// Primer hijo

    if(hijo1 == 0) { /// Descendencia Primer Hijo
        pid_t nieto1 = fork(); /// Primer nieto
        if(nieto1 == 0) { /// Descendencia Primer Nieto
            pid_t bisnieto1 = fork(); /// Primer bisnieto
            if(bisnieto1 != 0) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 1 - Normal\n",bisnieto1,getpid());
                pid_t bisnieto2 = fork();
                if(bisnieto2 != 0) {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 2 - Normal\n",bisnieto2,getpid());
                } else {
                    /// Esto se ejectua dentro del proceso del Bisnieto 2
                    stay();
                }
            } else {
                /// Esto se ejectua dentro del proceso del Bisnieto 1
                stay();
            }
            /// Esto se ejectua dentro del proceso del Nieto 1
            stay();
        } else {
            printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 1 - Normal\n",nieto1,getpid());
            pid_t nieto2 = fork(); /// Segundo nieto
            if(nieto2 == 0) { /// Descendencia Segundo Nieto
                pid_t bisnieto3 = fork(); /// Tercer bisnieto
                if(bisnieto3 == 0) {
                    contar();
                } else {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 3 - Demonio\n",bisnieto3,getpid());
                    pid_t bisnieto4 = fork();
                    if(bisnieto4 == 0) {
                        contar();
                    } else {
                        printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 4 - Demonio\n",bisnieto4,getpid());
                    }
                    return 0;
                }
                return 0;
            } else {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 2 - Zombie\n",nieto2,getpid());
            }
            stay();
        }
    } else {
        printf ("PID:%d PPID:%d Parentezco-Tipo: Hijo 1 - Normal\n",hijo1,getpid());

        hijo2 = fork(); /// Segundo Hijo
        if(hijo2 == 0) { /// Descendencia Segundo Hijo
            pid_t nieto3 = fork(); /// Tercer Nieto
            if(nieto3 == 0) { /// Descendencia Tercer Nieto
                pid_t bisnieto5 = fork(); /// Quinto bisnieto
                if(bisnieto5 == 0) {
                    contar();
//                    stay();
                } else {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 5 - Demonio\n",bisnieto5,getpid());
                }
                return 0;
            } else {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 3 - Zombie\n",nieto3,getpid());
                stay();
            }
        } else {
            printf ("PID:%d PPID:%d Parentezco-Tipo: Hijo 2 - Normal\n",hijo2,getpid());
            stay();
        }
    }
    stay();
    return 0;
}
