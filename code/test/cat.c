#include "syscall.h"
#define MAX_NAME_LEN 50
#define MAX_CONTENT_LEN 256

int main(){
    char fileName[MAX_NAME_LEN];
    char fileContent[MAX_CONTENT_LEN];
    int fileID;
    Print("Input file's name: ");
    Read(fileName, MAX_NAME_LEN, 0);
    fileID = Open(fileName, 0);
    if (fileID != -1){
        int readSize = Read(fileContent, MAX_CONTENT_LEN, fileID);
        if (readSize != -1){
            Print(fileContent);
            Print("\n");
        }
        else{
            Print("Read file FAIL\n");
        }
    }
    else{
        Print("Open file FAIL\n");
    }
    Halt();
}