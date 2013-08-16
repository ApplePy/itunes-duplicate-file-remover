#include <iostream>
#include <string>
#include "windows.h"

//DECLARATIONS
int theEngine(std::string searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const std::string &directoryInput);
int initialSearch(const std::string & searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle);
int fileDeletion (const std::string & directoryInput, WIN32_FIND_DATA & fileStruct, const HANDLE & fileHandle, unsigned short & confirmDelete);
int theNextFile (WIN32_FIND_DATA & fileStruct, const HANDLE & fileHandle);
int end_execution(int code);

int main()
{
	std::string directoryInput; //Directory to search in
	std::string maskInput; //Search mask
	std::string searchTerm; //Concatenation of directoryInput and maskInput to feed to the Windows search API

	std::cout << "Enter the search path: ";
	std::getline(std::cin,directoryInput);

	std::cout << "Enter the search mask: ";
	std::getline(std::cin,maskInput);

	if (directoryInput.back()!='\\') //if the user forgot an ending backslash to the directory input, add it in for them
		directoryInput=directoryInput+"\\";

	searchTerm=directoryInput+maskInput;


	WIN32_FIND_DATA fileStruct; //contains the file/directory currently under observation
	HANDLE fileHandle;

	int errorCode = theEngine(searchTerm,fileStruct,fileHandle,directoryInput);

	return end_execution(errorCode);
}

int theEngine(std::string searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const std::string &directoryInput)
{
	unsigned short confirmDelete = 0;
	int errorCode = 1;

	errorCode = initialSearch(searchTerm, fileStruct, fileHandle);
	if (errorCode != 0)
		return errorCode;

	while (errorCode==0) //iterates through the folder(s) searching for search term.
	{
		errorCode = fileDeletion(directoryInput, fileStruct, fileHandle, confirmDelete);
	}

	return errorCode;
}

int initialSearch(const std::string & searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle)
{
	fileHandle = WIN32::FindFirstFile(searchTerm.c_str(), &fileStruct); //intial file search
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		if (WIN32::GetLastError()==ERROR_FILE_NOT_FOUND) //nothing matched the search mask
		{
			return 3;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int fileDeletion (const std::string & directoryInput, WIN32_FIND_DATA & fileStruct, const HANDLE & fileHandle, unsigned short & confirmDelete)
{
	if (confirmDelete!=3) //enables "yes to all" functionality to spare headaches
	{
		std::cout << "Are you sure you want to delete " << fileStruct.cFileName << "? Press 1 if yes, 0 if no, 3 if yes to all." << std::endl; //output file name to be deleted
		std::cin>>confirmDelete;
	}

	if (confirmDelete!=0)
	{
		char deleteTerm [256]; //preparing file name/path string for deletion
		strcpy_s (deleteTerm, 256, directoryInput.c_str());
		strcat_s (deleteTerm, 256, fileStruct.cFileName);

		if (std::remove(deleteTerm)!=0)
		{
			return 1;
		}
	}
	else
		std::cout << "File skipped." << std::endl;

	return theNextFile (fileStruct, fileHandle);
}

int theNextFile (WIN32_FIND_DATA & fileStruct, const HANDLE & fileHandle)
{
	auto nextFile = WIN32::FindNextFile(fileHandle, &fileStruct); //gets next file to examine
	if (nextFile==0)
	{
		if (WIN32::GetLastError()==ERROR_NO_MORE_FILES)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int end_execution(int code) //Handles the translation between error code and corresponding text
{
	switch(code)
	{
	case 1:
		std::cout << "ERROR!";
		break;
	case 2:
		std::cout << "This program is finished. No more files here to delete." << std::endl;
		code = 0;
		break;
	case 3:
		std::cout << "No matching files found." << std::endl;
		code = 0;
		break;
	}

	std::cout << std::endl <<std::endl<< std::endl << "Program terminating... ";
	system ("pause");
	return code;
}