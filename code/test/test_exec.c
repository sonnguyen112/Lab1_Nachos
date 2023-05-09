#include "syscall.h"

int main() {
    int pid1;
    int pid2;
    int sema;
    sema = CreateSemaphore("sm", 1);
    if (sema == -1){
        Print("Error");
    }
    pid1 = Exec("cat");
    pid2 = Exec("copy");
    Join(pid1);
    Join(pid2);
}