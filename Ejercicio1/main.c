#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>

int main() /// PADRE
{
    pid_t pid1;
    pid_t pid2;

    pid1 = fork(); /// Primer hijo

    if(pid1 == 0) { /// Descendencia Primer Hijo
        pid_t pid3 = fork(); /// Primer nieto
        pid_t pid4 = fork(); /// Segundo nieto
        if((pid3 == 0) && (pid4 != 0)) { /// Descendencia Primer Nieto
            pid_t pid5 = fork(); /// Primer bisnieto
            pid_t pid6 = fork(); /// Segundo bisnieto
        }
        if((pid3 == 0) && (pid4 == 0)) { /// Descendencia Segundo Nieto
            pid_t pid7 = fork(); /// Tercer bisnieto
            pid_t pid8 = fork(); /// Cuarto bisnieto
        }
    } else {
        pid2 = fork(); /// Segundo Hijo
        if(pid2 == 0) { /// Descendencia Segundo Hijo
            pid_t pid9 = fork(); /// Tercer Nieto
            if(pid9 == 0) { /// Descendencia Tercer Nieto
                pid_t pid10 = fork(); /// Quinto bisnieto
            }
        }
    }

//    pid2 = fork(); /// Segundo hijo. Primer nieto

//    if((pid1 == 0) && (pid2 != 0)) { /// Descendencia primer hijo
//        pid_t pid3 = fork(); /// Segundo nieto
//    }




    return 0;
}
