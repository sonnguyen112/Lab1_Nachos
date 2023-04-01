// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
#define FILE_MAX 10
#define CONSOLE_IN 0
#define CONSOLE_OUT 1
#define MODE_READWRITE 0
#define MODE_READ 1
#define MODE_WRITE 2

class FileTable
{
private:
	OpenFile **openFile;
	int *fileOpenMode;

public:
	FileTable()
	{
		openFile = new OpenFile *[FILE_MAX];
		fileOpenMode = new int[FILE_MAX];
		fileOpenMode[CONSOLE_IN] = MODE_READ;
		fileOpenMode[CONSOLE_OUT] = MODE_WRITE;
	}

	int Insert(char *fileName, int openMode)
	{
		int freeIndex = -1;
		int fileDescriptor = -1;
		for (int i = 2; i < FILE_MAX; i++)
		{
			if (openFile[i] == NULL)
			{
				freeIndex = i;
				break;
			}
		}

		if (freeIndex == -1)
		{
			return -1;
		}

		if (openMode == MODE_READWRITE)
			fileDescriptor = OpenForReadWrite(fileName, FALSE);
		if (openMode == MODE_READ)
			fileDescriptor = OpenForRead(fileName, FALSE);

		if (fileDescriptor == -1)
			return -1;
		openFile[freeIndex] = new OpenFile(fileDescriptor);
		fileOpenMode[freeIndex] = openMode;

		return freeIndex;
	}

	int Remove(int index)
	{
		if (index < 2 || index >= FILE_MAX)
			return -1;
		if (openFile[index])
		{
			delete openFile[index];
			openFile[index] = NULL;
			return 0;
		}
		return -1;
	}

	int Read(char *buffer, int charCount, int index)
	{
		if (index >= FILE_MAX)
			return -1;
		if (openFile[index] == NULL)
			return -1;
		int result = openFile[index]->Read(buffer, charCount);
		return result;
	}

	int Write(char *buffer, int charCount, int index)
	{
		if (index >= FILE_MAX)
			return -1;
		if (openFile[index] == NULL || fileOpenMode[index] == MODE_READ)
			return -1;
		return openFile[index]->Write(buffer, charCount);
	}

	int Seek(int pos, int index)
	{
		if (index <= 1 || index >= FILE_MAX)
			return -1;
		if (openFile[index] == NULL)
			return -1;
		// use seek(-1) to move to the end of file
		if (pos == -1)
			pos = openFile[index]->Length();
		if (pos < 0 || pos > openFile[index]->Length())
			return -1;
		return openFile[index]->Seek(pos);
	}

	~FileTable()
	{
		for (int i = 0; i < FILE_MAX; i++)
		{
			if (openFile[i])
				delete openFile[i];
		}
		delete[] openFile;
		delete[] fileOpenMode;
	}
};

class FileSystem
{
public:
	FileTable *fileTable;
	FileSystem() {
		fileTable = new FileTable; 
	}
	~FileSystem(){
		delete fileTable;
	}

	bool Create(char *name)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor);
	}

	int OpenFileId(char *name, int openMode) {
        return fileTable->Insert(name, openMode);
    }

	int Close(int id) { return fileTable->Remove(id); }

	int Read(char *buffer, int charCount, int id) {
        return fileTable->Read(buffer, charCount, id);
    }

	int Write(char *buffer, int charCount, int id) {
        return fileTable->Write(buffer, charCount, id);
    }

	int Seek(int position, int id) {
        return fileTable->Seek(position, id);
    }


	bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem
{
public:
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)

	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
