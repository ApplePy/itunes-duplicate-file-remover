#include <iostream>
#include <string>
#include "windows.h"
#include <vector>

//DECLARATIONS
int theEngine(const std::string searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const std::string &directoryInput, const bool directorySearchMode = false);
int initialSearch(const std::string & searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const bool directorySearchMode = false);
int fileDeletion (const std::string & directoryInput, WIN32_FIND_DATA & fileStruct, unsigned short & confirmDelete);
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


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Execution is as follows:
	1) Grab the search term that the user wants
	2) Search the given directory for folders; ask if they want to apply this search to all subdirectories or not
	3) Index all the subdirectories
	4) Search all the files in the current directory for deletion
	5) Modify the search string to add in the next directory to search from index
	6) Repeat steps 2-5 as necessary
	7) Output error codes and exit program


	Error Codes:
	0: An intermediate step completed successfully
	1: Unexpected error
	2: No more files matching search term in current directory
	3: No files found matching search term in current directory
	4: not used
	5: All searches completed, no more subdirectories to check; everything completed successfully
	*/

	std::vector<std::string> directoriesToSearch;
	int errorCode = 0;
	WIN32_FIND_DATA fileStruct; //contains the file/directory currently under observation
	HANDLE fileHandle;

	while (errorCode != 1 && errorCode != 5)
	{
		theEngine(searchTerm, fileStruct, fileHandle, directoryInput, true); //Directory check

		errorCode = theEngine(searchTerm,fileStruct,fileHandle,directoryInput);
		switch (errorCode)
		{
		case (1):
			break;
		default:
			bool directoryCheckResult = checkIfMoreDirectoriesAreLeftToSearch(); //NOT FINISHED
			if (directoryCheckResult == false)
				errorCode = 5;
			else
				modifySearchTermToSearchNextDirectory(); //NOT FINISHED
			break;
		}
	}

	return end_execution(errorCode);
}


int addDirectoryToList(const std::string & directoryInput, WIN32_FIND_DATA &fileStruct, HANDLE & fileHandle) //NOT FINISHED; NO DECLARATION ABOVE EITHER
{
	/*Notes to Self:
	This directory will add the full path of the folder to the vector as a string to be run as a fresh search
	when it's time comes. However, a flag does not need to be made to signal the entry into a subdirectory;
	everything is recursive and logic will signal itself. If that made any sense.
	
	Also, don't forget to exclude the dot folders from addition to the list.

	Also, don't forget to make the function that retrieves the stored paths and sets them up for searching.
	*/
	std::cout << "The folder " << fileStruct.cFileName << " was found inside the search directory.\n"
		<< "Would you like to search this folder as well? 1 for yes, 2 for no, 3 for search all found folders.";
}


int theEngine(std::string searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const std::string &directoryInput, const bool directorySearchMode = false)
{
	unsigned short confirmDelete = 0;
	int engineErrorCode = 0;

	if (directorySearchMode == true) //constructs new search string to use to find folders
		searchTerm = directoryInput + "*";

	engineErrorCode = initialSearch(searchTerm, fileStruct, fileHandle, directorySearchMode);
	if (engineErrorCode != 0)
		return engineErrorCode;

	while (engineErrorCode==0) //iterates through the folder searching for search term.
	{
		if (directorySearchMode == true)
		{
			engineErrorCode = addDirectoryToList();//not implemented yet, sorry --DON'T FOREGET TO EXCLUDE THE DOT FOLDERS!
			if (engineErrorCode != 0)
				break;
		}
		else
		{
			engineErrorCode = fileDeletion(directoryInput, fileStruct, confirmDelete);
			if (engineErrorCode!=0)
				break;
		}
		engineErrorCode = theNextFile(fileStruct,fileHandle);
	}

	return engineErrorCode;
}

int initialSearch(const std::string & searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const bool directorySearchMode = false)
{
	if (directorySearchMode == true)
		fileHandle = WIN32::FindFirstFileEx(searchTerm.c_str(), FindExInfoStandard, &fileStruct, FindExSearchLimitToDirectories, NULL, NULL); //intial directory search
	else
		fileHandle = WIN32::FindFirstFileEx(searchTerm.c_str(), FindExInfoStandard, &fileStruct, FindExSearchNameMatch, NULL, NULL); //intial file search

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		auto successCheck = WIN32::GetLastError();
		TCHAR errorText [256];
		WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
		std::cout << errorText;

		if (successCheck==ERROR_FILE_NOT_FOUND) //nothing matched the search mask
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

int fileDeletion (const std::string & directoryInput, WIN32_FIND_DATA & fileStruct, unsigned short & confirmDelete)
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

	return 0;
}

int theNextFile (WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle)
{
	auto nextFile = WIN32::FindNextFile(fileHandle, &fileStruct); //gets next file to examine
	if (nextFile==0)
	{
		auto successCheck = WIN32::GetLastError();
		TCHAR errorText [256];
		WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
		std::cout << errorText;

		if (successCheck==ERROR_NO_MORE_FILES)
		{
			WIN32::FindClose(fileHandle);
			return 2;
		}
		else
		{
			WIN32::FindClose(fileHandle);
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