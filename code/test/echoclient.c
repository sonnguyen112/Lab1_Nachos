#include "syscall.h"
#define MAX_MESSAGE_LEN 256

int main()
{

    int socketid;
    int connect;
    int send;
    int re;
    char buffer[MAX_MESSAGE_LEN];
    char message[MAX_MESSAGE_LEN];
    int messageLen;

    Print("Input message: ");
    Read(message, MAX_MESSAGE_LEN, 0);

    socketid = SocketTCP();
    if (socketid != -1)
    {
        connect = Connect(socketid, "127.0.0.1", 8080);
        if (connect != -1)
        {
            messageLen = Send(socketid, message, MAX_MESSAGE_LEN);
            re = Receive(socketid, buffer, messageLen);
            Print(buffer);
        }
        else
        {
            Print("Connect Fail");
        }

        SocketClose(socketid);
    }
    else
    {
        Print("Open Socket File Fail");
    }

    Halt();
}