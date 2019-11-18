#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>

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
            }
            pid_t bisnieto2 = fork(); /// Segundo bisnieto
            if((bisnieto2 != 0) && (bisnieto1 != 0)) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 2 - Normal\n",bisnieto2,getpid());
            }
        }
        if((nieto1 == 0) && (nieto2 == 0)) { /// Descendencia Segundo Nieto
            pid_t bisnieto3 = fork(); /// Tercer bisnieto
            if(bisnieto3 != 0) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 3 - Normal\n",bisnieto3,getpid());
            }
            pid_t bisnieto4 = fork(); /// Cuarto bisnieto
            if(bisnieto4 != 0 && (bisnieto3 != 0)) {
                printf ("PID:%d PPID:%d Parentezco-Tipo: Bisnieto 4 - Normal\n",bisnieto4,getpid());
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
