#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    pid_t child_pid = fork();

    //child process
    if (child_pid == 0){
        printf("### Child ###\nCurrent PID: %d and Child PID: %d\n",
               getpid(), child_pid);
    }else{
        sleep(1);
        printf("### Parent ###\nCurrent PID: %d and Child PID: %d\n",
               getpid(), child_pid);
        
    }
    return 0;
}
