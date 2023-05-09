#include "syscall.h"
#define MAX_NAME_LEN 50
#define MAX_CONTENT_LEN 256

int main(){
    char fileName[MAX_NAME_LEN];
    char fileContent[MAX_CONTENT_LEN];
    int fileID;
    int readSize;
    Down("sm");
    Print("Input file's name for cat: ");
    Read(fileName, MAX_NAME_LEN, 0);
    fileID = Open(fileName, 0);
    Up("sm");
    if (fileID != -1){
        readSize = Read(fileContent, MAX_CONTENT_LEN, fileID);
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
    // Halt();
}