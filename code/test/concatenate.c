#include "syscall.h"
#define MAX_NAME_LEN 50
#define MAX_CONTENT_LEN 256

int main(){
    char fileName1[MAX_NAME_LEN];
    char fileName2[MAX_NAME_LEN];
    int fileId1;
    int fileId2;
    char fileContent1[MAX_CONTENT_LEN];
    char fileContent2[MAX_CONTENT_LEN];
    Print("Input the first file's name:");
    Read(fileName1, MAX_NAME_LEN, 0);
    Print("Input the second file's name:");
    Read(fileName2, MAX_NAME_LEN, 0);
    fileId1 = Open(fileName1, 1);
    if (fileId1 != -1){
        fileId2 = Open(fileName2, 1);
        if (fileId2 != -1){
            Read(fileContent1, MAX_CONTENT_LEN, fileId1);
            Read(fileContent2, MAX_CONTENT_LEN, fileId2);
            Print(fileContent1);
            Print(fileContent2);
            Print("\n");
            Close(fileId2);
        }
        else{
            Print("Open file 2 FAIL");
        }
        Close(fileId1);
    }   
    else{
        Print("Open file 1 FAIL");
    }
    Halt();
}