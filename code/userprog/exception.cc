// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
#define MaxFileLength 32
void moveProgramCounter()
{
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

char *User2System(int addr, int convert_length = -1)
{
	int length = 0;
	bool stop = false;
	char *str;

	do
	{
		int oneChar;
		kernel->machine->ReadMem(addr + length, 1, &oneChar);
		length++;
		// if convert_length == -1, we use '\0' to terminate the process
		// otherwise, we use convert_length to terminate the process
		stop = ((oneChar == '\0' && convert_length == -1) ||
				length == convert_length);
	} while (!stop);

	str = new char[length];
	for (int i = 0; i < length; i++)
	{
		int oneChar;
		kernel->machine->ReadMem(addr + i, 1,
								 &oneChar); // copy characters to kernel space
		str[i] = (unsigned char)oneChar;
	}
	return str;
}

void System2User(char* str, int addr, int convert_length = -1) {
    int length = (convert_length == -1 ? strlen(str) : convert_length);
    for (int i = 0; i < length; i++) {
        kernel->machine->WriteMem(addr + i, 1,
                                  str[i]);  // copy characters to user space
    }
    kernel->machine->WriteMem(addr + length, 1, '\0');
}

void CreateHandle()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *filename = User2System(virtAddr, MaxFileLength + 1);
	if (SysCreateFile(filename))
	{
		kernel->machine->WriteRegister(2, 0);
	}
	else
	{
		kernel->machine->WriteRegister(2, -1);
	}
	delete filename;
	return moveProgramCounter();
}

void OpenHandle(){
	int virtAddr = kernel->machine->ReadRegister(4);
    char* fileName = User2System(virtAddr);
    int type = kernel->machine->ReadRegister(5);
	kernel->machine->WriteRegister(2, SysOpen(fileName, type));
	delete fileName;
	return moveProgramCounter();
}

void CloseHandle(){
	int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysClose(id));
	return moveProgramCounter();
}

void ReadHandle(){
	int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

	if (fileId == 0){
		int i = 0;
		while(true){
			char c = kernel->synchConsoleIn->GetChar();
			kernel->machine->WriteMem(virtAddr + i, 1, c);
			if (c == '\n'){
				break;
			}
			i++;
		}
		kernel->machine->WriteMem(virtAddr + i, 1, '\n');
		return moveProgramCounter();
	}

    DEBUG(dbgFile,
          "Read " << charCount << " chars from file " << fileId << "\n");

	int readSize = SysRead(buffer, charCount, fileId);
	// printf("Read Size: %d", readSize);
    kernel->machine->WriteRegister(2, readSize);
    System2User(buffer, virtAddr, charCount);

    delete[] buffer;
	return moveProgramCounter();
}

void WriteHandle(){
	int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    DEBUG(dbgFile,
          "Write " << charCount << " chars to file " << fileId << "\n");

    kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));
    System2User(buffer, virtAddr, charCount);

    delete[] buffer;
	return moveProgramCounter();
}

void SeekHandle(){
	int seekPos = kernel->machine->ReadRegister(4);
    int fileId = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));

	return moveProgramCounter();
}

void RemoveHandle(){
	int virtAdd = kernel->machine->ReadRegister(4);
	char* fileName = User2System(virtAdd);
	kernel->machine->WriteRegister(2, SysRemove(fileName));
	return moveProgramCounter();
}

void PrintHandle(){
	int virtAdd = kernel->machine->ReadRegister(4);
	char* buffer = User2System(virtAdd);
	SysWrite(buffer, strlen(buffer), 1);
	return moveProgramCounter();
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			return moveProgramCounter();

			ASSERTNOTREACHED();

			break;

		case SC_Create:
			return CreateHandle();
		case SC_Open:
			return OpenHandle();
		case SC_Close:
			return CloseHandle();
		case SC_Read:
			return ReadHandle();
		case SC_Write:
			return WriteHandle();
		case SC_Seek:
			return SeekHandle();
		case SC_Remove:
			return RemoveHandle();
		case SC_Print:
			return PrintHandle();

		default:
			cerr
				<< "Unexpected system call "
				<< type
				<< "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
