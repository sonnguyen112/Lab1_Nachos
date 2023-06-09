#include "syscall.h"
#define MAX_NAME_LEN 50
#define MAX_CONTENT_LEN 256

int main()
{
    char srcFileName[MAX_NAME_LEN];
    char destFileName[MAX_NAME_LEN];
    int srcFileID;
    int destFIleID;
    char srcFileContent[MAX_CONTENT_LEN];
    int socketid;
    char destFileContent[MAX_CONTENT_LEN];
    int connect;
    Print("Input src file's name: ");
    Read(srcFileName, MAX_NAME_LEN, 0);
    Print("Input dest file's name: ");
    Read(destFileName, MAX_NAME_LEN, 0);
    srcFileID = Open(srcFileName, 1);
    if (srcFileID != -1)
    {
        socketid = SocketTCP();
        if (socketid != -1)
        {
            connect = Connect(socketid, "127.0.0.1", 8080);
            if (connect != -1)
            {
                if (Create(destFileName) != -1)
                {
                    destFIleID = Open(destFileName, 0);
                    if (destFIleID != -1)
                    {
                        int readSize = Read(srcFileContent, MAX_CONTENT_LEN, srcFileID);
                        Send(socketid, srcFileContent, readSize);
                        Receive(socketid, destFileContent, readSize);
                        Write(destFileContent, readSize, destFIleID);
                        Close(destFIleID);
                    }
                    else
                    {
                        Print("Open dest file FAIL\n");
                    }
                }
                else
                {
                    Print("Create dest file FAIL\n");
                }
            }
            else
            {
                Print("Connect FAIL");
            }
            SocketClose(socketid);
        }
        else
        {
            Print("Open SocketTCP FAIL\n");
        }
        Close(srcFileID);
    }
    else
    {
        Print("Open src file FAIL\n");
    }
    Halt();
}