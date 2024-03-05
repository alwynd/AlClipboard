#pragma once

#ifndef ALCLIPBOARD_H
#define ALCLIPBOARD_H

#include <windowComponent_64.h>
#pragma comment(lib, "WindowComponent_64.lib")

#include <process.h>

const char *DEFAULT_DIRECTORY_NAME = "ClipBoardCache";

void doProcess(void *pKey);

enum AL_COPY_OR_PASTE
{
	ALCOPY_NONE=0,
	ALCOPY_COPY = 1,
	ALCOPY_PASTE = 2,
};

#endif

/// This class is responsible for reading and writing to and from the clipboard and storage.
class AlClipboard
{
public:
	AlClipboard();										//Constructor.
	~AlClipboard();										//Destructor.

	bool writeToDisk(char *pKey, char *pBuffer);		//Writes clipboard content to disk, using the key provided.
	bool readFromDisk(char *pKey, char *pBuffer, int pMaxSize);			//Writes clipboard content to disk, using the key provided.

	bool writeToClipboard(char *pKey);								//Copies the buffer to the clipboard as text.
	bool readFromClipboard(char *pKey);				//Copies the clipboard, to the buffer as text.

	void zeroMem();					//Zero this memory.
};