#include "syscall.h"
#define MAX_NAME_LEN 50
#define MAX_CONTENT_LEN 256

int main(){
    char fileName1[MAX_NAME_LEN];
    char fileName2[MAX_NAME_LEN];
    Print("Input the first file's name:");
    Read(fileName1, MAX_NAME_LEN, 0);
    Print("Input the second file's name:");
    Read(fileName2, MAX_NAME_LEN, 0);
}