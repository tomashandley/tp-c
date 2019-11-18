#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>

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
        pid_t nieto2 = fork(); /// Segundo nieto
        if((nieto1 == 0) && (nieto2 != 0)) { /// Descendencia Primer Nieto
            pid_t bisnieto1 = fork(); /// Primer bisnieto
            if(bisnieto1 != 0) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 1 - Normal\n",bisnieto1,getpid());
            } else { ///Estoy en el bisnieto 1
                /* Creo el primer zombie */
                pid_t zombie1 = fork();
                if(zombie1 != 0) {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Tataranieto 1 - Zombie\n",zombie1,getpid());
                    sleep(50);
                }
            }
            pid_t bisnieto2 = fork(); /// Segundo bisnieto
            if((bisnieto2 != 0) && (bisnieto1 != 0)) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 2 - Normal\n",bisnieto2,getpid());
            } else { /// Estoy en el segundo bisnieto
                if((bisnieto2 == 0) && (bisnieto1 == 0)) { /// Creo el segundo zombie
                    pid_t zombie2 = fork();
                    if(zombie2 != 0) {
                        printf ("PID:%d PPID:%d Parentezco-Tipo: Tataranieto 2 - Zombie\n",zombie2,getpid());
                        sleep(50);
                    }
                }
            }
        }
        if((nieto1 == 0) && (nieto2 == 0)) { /// Descendencia Segundo Nieto
            pid_t bisnieto3 = fork(); /// Tercer bisnieto
            if(bisnieto3 != 0) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 3 - Normal\n",bisnieto3,getpid());
            } else {
                pid_t demonio1 = fork();
                if(demonio1 == 0) {
                    contar();
                } else {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Tataranieto 3 - Demonio\n",demonio1,getpid());
                    return 0;
                }
            }
            pid_t bisnieto4 = fork(); /// Cuarto bisnieto
            if(bisnieto4 != 0 && (bisnieto3 != 0)) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 4 - Normal\n",bisnieto4,getpid());
            } else {
                pid_t demonio2 = fork();
                if(demonio2 == 0) {
                    contar();
                } else {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Tataranieto 4 - Demonio\n",demonio2,getpid());
                    return 0;
                }
            }
        }
        if((nieto1 != 0) && (nieto2 != 0)) {
            printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 1 - Normal\n",nieto1,getpid());
            printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 2 - Normal\n",nieto2,getpid());
        }
    } else {
        printf ("PID:%d PPID:%d Parentezco-Tipo: Hijo 1 - Normal\n",hijo1,getpid());
        hijo2 = fork(); /// Segundo Hijo
        if(hijo2 == 0) { /// Descendencia Segundo Hijo
            pid_t nieto3 = fork(); /// Tercer Nieto
            if(nieto3 == 0) { /// Descendencia Tercer Nieto
                pid_t bisnieto5 = fork(); /// Quinto bisnieto
                if(bisnieto5 != 0) {
                    printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 5 - Normal\n",bisnieto5,getpid());
                } else {
                    pid_t demonio3 = fork();
                    if(demonio3 == 0) {
                        contar();
                    } else {
                        printf ("PID:%d PPID:%d Parentezco-Tipo: Tataranieto 5 - Demonio\n",demonio3,getpid());
                        return 0;
                    }
                }
            } else {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Nieto 3 - Normal\n",nieto3,getpid());
            }
        } else {
            printf ("PID:%d PPID:%d Parentezco-Tipo: Hijo 2 - Normal\n",hijo2,getpid());
            printf("\nPresione una tecla para terminar\n");
            gets();
        }
    }

    return 0;
}
