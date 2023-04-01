#include "syscall.h"
#define MAX_NAME_LEN 50

int main(){
    char* fileName;
    Print("Input file's name: ");
    Read(fileName, MAX_NAME_LEN, 0);
    if (Create(fileName) != -1){
        Print("Create file OK\n");
    }
    else{
        Print("Create file FAIL\n");
    }
    Halt();
}