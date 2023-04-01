#include "syscall.h"
#define MAX_NAME_LEN 50

int main(){
    char fileName[MAX_NAME_LEN];
    Print("Input file's name: ");
    Read(fileName, MAX_NAME_LEN, 0);
    if (Remove(fileName) != -1){
        Print("File is deleted\n");
    }
    else{
        Print("Delete FAIL\n");
    }
    Halt();
}