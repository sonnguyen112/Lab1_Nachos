/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

bool SysCreateFile(char *fileName)
{
  if (strlen(fileName) == 0)
  {
    DEBUG(dbgSys, "\nFile name can't be empty");
    return false;
  }
  else if (fileName == NULL)
  {
    DEBUG(dbgSys, "\nNot enough memory in system");
    return false;
  }
  else
  {
    DEBUG(dbgSys, "\nFile's name read successfully");
    if (!kernel->fileSystem->Create(fileName))
    {
      DEBUG(dbgSys, "\nError creating file");
      return false;
    }
  }
  return true;
}

int SysOpen(char *fileName, int type)
{
  if (type != 0 && type != 1)
    return -1;

  int id = kernel->fileSystem->OpenFileId(fileName, type);
  if (id == -1)
    return -1;
  DEBUG(dbgSys, "\nOpened file");
  return id;
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

int SysRead(char* buffer, int charCount, int fileId) {
    return kernel->fileSystem->Read(buffer, charCount, fileId);
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 1) {
        return kernel->synchConsoleOut->PutString(buffer, charCount);
    }
    return kernel->fileSystem->Write(buffer, charCount, fileId);
}

int SysSeek(int seekPos, int fileId) {
    if (fileId <= 1) {
        DEBUG(dbgSys, "\nCan't seek in console");
        return -1;
    }
    return kernel->fileSystem->Seek(seekPos, fileId);
}

int SysRemove(char* fileName){
  int fileID = SysOpen(fileName, 1);
  if (fileID != -1){
    if (kernel->fileSystem->Remove(fileName)) return 0;
    return -1;
  }
  return -1;
}

int SysSocketTCP()
{
  return kernel->fileSystem->SocketTCP();
}

int SysConnect(int socketid, char *ip, int port)
{
  return kernel->fileSystem->SocketConnect(socketid, ip, port);
}

int SysSend(int socketid, char *buffer, int len){
    return kernel->fileSystem->SocketSend(socketid, buffer, len);
}

int SysReceive(int socketid, char *buffer, int len){
    return kernel->fileSystem->SocketReceive(socketid, buffer, len);
}

int SysSocketClose(int socketid){
  return kernel->fileSystem->SocketClose(socketid);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
