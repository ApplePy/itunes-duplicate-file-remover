#pragma once
#include "windows.h"
#include <iostream>
#include <string>
#include <vector>

typedef unsigned short ushort;
typedef unsigned long ulong;

class WIN32Files
{
public:
	WIN32Files(void);
	~WIN32Files(void);
	ulong initialSearch(const std::string &searchTerm);
	ulong continueSearch();
	ushort deleteFile(const std::string &directoryInput, ushort &confirmDelete);
	ushort addCurrentFileToList(const std::string &fileInput, ushort &confirmAdd);
	std::string errorMsg(ulong errorCode, bool outputToConsole);
	ulong closeFile();
	
	const HANDLE &fileHandle; //public-facing uneditable versions of class variables
	const WIN32_FIND_DATA &fileInfo; //public-facing uneditable versions of class variables
	const bool &fileIsValid; //public-facing uneditable versions of class variables
	std::vector<std::string> listToSearch;

protected:
	HANDLE fileHandleInternal;
	WIN32_FIND_DATA fileInfoInternal;

private:
	bool isValid;
};

