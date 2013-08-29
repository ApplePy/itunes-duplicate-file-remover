#include <iostream>
#include <string>
#include "windows.h"
#include <vector>
#include "userpref.h"
#include "WIN32Files.h"
#include "file_manipulation.h"

//DECLARATIONS
ulong theEngine(userpref &searchParams, WIN32Files &activeFile, file_manipulation &deleteList, bool directorySearchMode);
void modifySearch(std::vector<std::string> &directoriesToSearch, userpref &searchParams);

typedef unsigned short ushort;
typedef unsigned long ulong;

/* Execution is as follows:
1) Grab the search term that the user wants
2) Search the given directory for folders; ask if they want to apply this search to all subdirectories or not
3) Index all the subdirectories
4) Search all the files in the current directory for deletion
5) Modify the search string to add in the next directory to search from index
6) Repeat steps 2-5 as necessary
7) Output error codes and exit program


Error Codes (Separate from Windows API errors):
0: An intermediate step completed successfully
1: Unexpected error
2: No more files matching search term in current directory (depreciated)
3: No files found matching search term in current directory (depreciated)
4: not used
5: All searches completed, no more subdirectories to check; everything completed successfully
*/

ulong main()
{
	std::cout << "Welcome to the Duplicate File Removal Tool" << std::endl<< std::endl;
	std::string directTemp;
	std::string maskTemp;

	std::cout << "Enter the COMPLETE path to folder to search for duplicates: ";
	std::getline(std::cin,directTemp);

	std::cout << "Enter the search mask (* can be anything of any length, ? is unknown character): ";
	std::getline(std::cin,maskTemp);

	userpref searchParams (directTemp, maskTemp);
	file_manipulation deleteList (searchParams.directoryInput + "DeletedList.txt");
	directTemp.clear();
	maskTemp.clear();
	std::cout << std::endl << std::endl << "Note: This program creates a list of the deleted files at " << deleteList.get_filepath();

	WIN32Files activeFile;


	ulong errorCode = 0;

	while (errorCode != 5) //This loops checks the current folder for directories, then for files to delete, then refers to the directories list to pull a new directory name to search, then restarts
	{
		errorCode = theEngine(searchParams, activeFile, deleteList, true); //Directory check stage
		if (errorCode != ERROR_NO_MORE_FILES) //something unexpected happened
		{
			activeFile.errorMsg(errorCode, true);
			auto closeCode = activeFile.closeFile();
			if (closeCode != 0)
				activeFile.errorMsg(closeCode, true);
			break;
		}

		errorCode = theEngine(searchParams, activeFile, deleteList, false); //file check stage
		switch (errorCode)
		{
		case (1):
			break;
		case (ERROR_FILE_NOT_FOUND): //identical to ERROR_NO_MORE_FILES case; checks to see if there's more folders to search, otherwise exits
			if (activeFile.listToSearch.size() == 0)
				errorCode = 5;
			else
			{
				modifySearch(activeFile.listToSearch, searchParams);
				errorCode = 0;
			}
			break;
		case (ERROR_NO_MORE_FILES): //identical to ERROR_FILE_NOT_FOUND case; checks to see if there's more folders to search, otherwise exits
			if (activeFile.listToSearch.size() == 0)
				errorCode = 5;
			else
			{
				modifySearch(activeFile.listToSearch, searchParams);
				errorCode = 0;
			}
			break;
		default: //An unexpected error occurred
			activeFile.errorMsg(errorCode, true);
			break;
		}
	}

	if (errorCode == 5) //Editing the error code to zero so that Windows doesn't gripe
		errorCode = 0;
	std::cout << std::endl << std::endl << "Program terminating... ";
	system ("pause");
	return errorCode;
}


ulong theEngine(userpref &searchParams, WIN32Files &activeFile, file_manipulation &deleteList, const bool directorySearchMode)
{
	ulong engineErrorCode = 0;

	if (directorySearchMode == true) //constructs new search string to use to find folders
	{
		searchParams.setMaskInput("*");
		searchParams.setSearchTerm();
	}
	else //reset the search parameters back to what the user is looking for
	{
		searchParams.setMaskInput(reset);
		searchParams.setSearchTerm();
	}

	engineErrorCode = activeFile.initialSearch(searchParams.searchTerm); //initiates search
	if (engineErrorCode != 0)
		return engineErrorCode;

	while (engineErrorCode == 0) //continues search, respective of mode
	{
		if (directorySearchMode == true)
		{
			if (activeFile.fileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				activeFile.addCurrentFileToList(searchParams.directoryInput, searchParams.confirmAdd);
		}
		else
		{
			if (activeFile.fileInfo.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				engineErrorCode = activeFile.deleteFile(searchParams.directoryInput,searchParams.confirmDelete);
				if (engineErrorCode != 0 && engineErrorCode !=2)
					break;
				if (engineErrorCode != 2)
					deleteList.write_file(searchParams.directoryInput + activeFile.fileInfo.cFileName + '\n', "append");
			}
		}
		engineErrorCode = activeFile.continueSearch();
		if (engineErrorCode != 0)
			break;
	}

	return engineErrorCode;
}

void modifySearch(std::vector<std::string> &directoriesToSearch, userpref &searchParams)
{
	searchParams.setDirectoryInput(directoriesToSearch[directoriesToSearch.size() - 1]);
	searchParams.setSearchTerm();
	directoriesToSearch.pop_back();
}