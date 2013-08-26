#include <iostream>
#include <string>
#include "windows.h"
#include <vector>
#include "userpref.h"
#include "WIN32Files.h"

//DECLARATIONS
ulong theEngine(userpref &searchParams, WIN32Files &activeFile, const bool directorySearchMode);
//short initialSearch(userpref &searchParams, WIN32Files &activeFile, std::vector<std::string> &directoriesToSearch, const bool directorySearchMode);
//short fileDeletion (userpref &searchParams, WIN32Files &activeFile, std::vector<std::string> &directoriesToSearch, const bool directorySearchMode);
//short addDirectoryToList(userpref &searchParams, WIN32Files &activeFile, std::vector<std::string> &directoriesToSearch, const bool directorySearchMode, std::vector<std::string> &directoriesToSearch);
//short theNextFile (WIN32Files &activeFile, const bool directorySearchMode);
void modifySearch(std::vector<std::string> &directoriesToSearch, userpref &searchParams);
//ulong end_execution(short code);


typedef unsigned short ushort;
typedef unsigned long ulong;

ulong main()
{
	std::string directTemp;
	std::string maskTemp;
	//std::string searchTerm; //Concatenation of directoryInput and maskInput to feed to the Windows search API

	//Uncomment for non-testing
	std::cout << "Enter the search path: ";
	std::getline(std::cin,directTemp);

	std::cout << "Enter the search mask: ";
	std::getline(std::cin,maskTemp);

	userpref searchParams (directTemp, maskTemp);
	directTemp.clear();
	maskTemp.clear();

	WIN32Files activeFile;

	//if (directoryInput.back()!='\\') //if the user forgot an ending backslash to the directory input, add it in for them
	//	directoryInput.append("\\");
	//searchTerm=directoryInput+maskInput;


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


	NOTES TO SELF:
	-------> Inputs are not validated yet!
	*/

	//std::vector<std::string> directoriesToSearch;
	ulong errorCode = 0;
	//WIN32_FIND_DATA fileStruct; //contains the file/directory currently under observation
	//HANDLE fileHandle;
	//unsigned short confirmDelete = 0;
	//unsigned short confirmAdd = 0;

	while (errorCode != 5) //This loops checks the current folder for directories, then for files to delete, then refers to the directories list to pull a new directory name to search, then restarts
	{
		errorCode = theEngine(searchParams, activeFile, true); //Directory check stage
		if (errorCode != ERROR_NO_MORE_FILES)
		{
			activeFile.errorMsg(errorCode, true);
			auto closeCode = activeFile.closeFile();
			if (closeCode != 0)
				activeFile.errorMsg(closeCode, true);
			break;
		}

		errorCode = theEngine(searchParams, activeFile, false); //file check stage
		switch (errorCode)
		{
		case (1):
			break;
		case (ERROR_FILE_NOT_FOUND): //identical to ERROR_NO_MORE_FILES case
			if (activeFile.listToSearch.size() == 0)
				errorCode = 5;
			else
			{
				modifySearch(activeFile.listToSearch, searchParams);
				errorCode = 0;
			}
			break;
		case (ERROR_NO_MORE_FILES): //identical to ERROR_FILE_NOT_FOUND case
			if (activeFile.listToSearch.size() == 0)
				errorCode = 5;
			else
			{
				modifySearch(activeFile.listToSearch, searchParams);
				errorCode = 0;
			}
			break;
		default:
			activeFile.errorMsg(errorCode, true);
			break;
		}
	}

	if (errorCode == 5)
		errorCode = 0;
	std::cout << std::endl <<std::endl<< std::endl << "Program terminating... ";
	system ("pause");
	return errorCode;
}


ulong theEngine(userpref &searchParams, WIN32Files &activeFile, const bool directorySearchMode)
{
	ulong engineErrorCode = 0;

	//if (activeFile.fileHandle != INVALID_HANDLE_VALUE) //Clears any residue in the class
	//{
	//	auto engineErrorCode = activeFile.closeFile();
	//	if (engineErrorCode != 0)
	//		return engineErrorCode;
	//}

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
				if (engineErrorCode != 0)
					return engineErrorCode;
			}
		}
		engineErrorCode = activeFile.continueSearch();
		if (engineErrorCode != 0)
			return engineErrorCode;
	}


	//engineErrorCode = initialSearch(searchTerm, fileStruct, fileHandle, directorySearchMode);
	//if (engineErrorCode != 0)
	//	return engineErrorCode;

	//while (engineErrorCode==0) //iterates through the folder searching for search term.
	//{
	//	if (directorySearchMode == true)
	//	{
	//		engineErrorCode = addDirectoryToList(directoryInput, fileStruct, directoriesToSearch, confirmAdd);
	//		if (engineErrorCode != 0)
	//			break;
	//	}
	//	else
	//	{
	//		engineErrorCode = fileDeletion(directoryInput, fileStruct, confirmDelete);
	//		if (engineErrorCode!=0)
	//			break;
	//	}
	//	engineErrorCode = theNextFile(fileStruct,fileHandle, directorySearchMode);
	//}

	return engineErrorCode;
}

//short initialSearch(const std::string & searchTerm, WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const bool directorySearchMode)
//{
//	//if (directorySearchMode == true)
//	//	fileHandle = WIN32::FindFirstFileEx(searchTerm.c_str(), FindExInfoStandard, &fileStruct, FindExSearchLimitToDirectories, NULL, NULL); //BROKEN.
//	//else
//	fileHandle = WIN32::FindFirstFileEx(searchTerm.c_str(), FindExInfoStandard, &fileStruct, FindExSearchNameMatch, NULL, NULL); //intial file search
//
//	if (fileHandle == INVALID_HANDLE_VALUE)
//	{
//		auto successCheck = WIN32::GetLastError();
//		TCHAR errorText [256];
//		WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
//		std::cout << errorText;
//
//		if (successCheck==ERROR_FILE_NOT_FOUND) //nothing matched the search mask
//		{
//			return 3;
//		}
//		else
//		{
//			return 1;
//		}
//	}
//	return 0;
//}

//short fileDeletion (const std::string & directoryInput, WIN32_FIND_DATA & fileStruct, unsigned short & confirmDelete)
//{
//	if (confirmDelete!=3) //enables "yes to all" functionality to spare headaches
//	{
//		std::cout << "Are you sure you want to delete " << fileStruct.cFileName << "? Press 1 if yes, 0 if no, 3 if yes to all." << std::endl; //output file name to be deleted
//		std::cin>>confirmDelete;
//	}
//
//	if (confirmDelete!=0)
//	{
//		char deleteTerm [256]; //preparing file name/path string for deletion
//		strcpy_s (deleteTerm, 256, directoryInput.c_str());
//		strcat_s (deleteTerm, 256, fileStruct.cFileName);
//
//		if (std::remove(deleteTerm)!=0)
//		{
//			return 1;
//		}
//	}
//	else
//		std::cout << "File skipped." << std::endl;
//
//	return 0;
//}

//short theNextFile (WIN32_FIND_DATA & fileStruct, HANDLE & fileHandle, const bool directorySearchMode)
//{
//	int nextFile = 0;
//	if (directorySearchMode==true)
//	{
//		do
//		{
//			nextFile = WIN32::FindNextFile(fileHandle, &fileStruct); //gets next file to examine
//
//			if (nextFile==0)
//			{
//				auto successCheck = WIN32::GetLastError();
//				TCHAR errorText [256];
//				WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
//				std::cout << errorText;
//
//				if (successCheck==ERROR_NO_MORE_FILES)
//				{
//					WIN32::FindClose(fileHandle);
//					return 2;
//				}
//				else
//				{
//					WIN32::FindClose(fileHandle);
//					return 1;
//				}
//			}
//		} while (fileStruct.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY);
//	}
//	else
//	{
//		do
//		{
//			nextFile = WIN32::FindNextFile(fileHandle, &fileStruct); //gets next file to examine
//
//			if (nextFile==0)
//			{
//				auto successCheck = WIN32::GetLastError();
//				TCHAR errorText [256];
//				WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
//				std::cout << errorText;
//
//				if (successCheck==ERROR_NO_MORE_FILES)
//				{
//					WIN32::FindClose(fileHandle);
//					return 2;
//				}
//				else
//				{
//					WIN32::FindClose(fileHandle);
//					return 1;
//				}
//			}
//		} while (fileStruct.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY);
//	}
//
//	/*if (nextFile==0)
//	{
//	auto successCheck = WIN32::GetLastError();
//	TCHAR errorText [256];
//	WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
//	std::cout << errorText;
//
//	if (successCheck==ERROR_NO_MORE_FILES)
//	{
//	WIN32::FindClose(fileHandle);
//	return 2;
//	}
//	else
//	{
//	WIN32::FindClose(fileHandle);
//	return 1;
//	}
//	}*/
//	return 0;
//}

//short addDirectoryToList(const std::string & directoryInput, WIN32_FIND_DATA &fileStruct, std::vector<std::string> &directoriesToSearch, unsigned short &confirmAdd)
//{
//	/*Notes to Self:
//	This function will add the full path of the new folder to the vector as a string to be run as a fresh search
//	when it's time comes. However, a flag does not need to be made to signal the entry into a subdirectory;
//	everything is recursive and logic will signal itself. If that made any sense.
//
//	Also, don't forget to exclude the dot folders from addition to the list.
//	*/
//
//	auto compareResult1 = strcmp (fileStruct.cFileName, ".");
//	auto compareResult2 = strcmp (fileStruct.cFileName, "..");
//
//	if (compareResult1 != 0 && compareResult2 != 0)
//	{
//		if (confirmAdd != 3)
//		{
//			std::cout << "The folder " << directoryInput.c_str() << fileStruct.cFileName << " was found inside the search directory.\n"
//				<< "Would you like to search this folder as well? 1 for yes, 2 for no, 3 for search all found folders. Answer: ";
//			std::cin >> confirmAdd;
//		}
//		if (confirmAdd != 2)
//		{
//			std::string NameArrange = directoryInput + fileStruct.cFileName;
//			directoriesToSearch.push_back(NameArrange);
//		}
//	}
//	return 0;
//}

void modifySearch(std::vector<std::string> &directoriesToSearch, userpref &searchParams)
{
	searchParams.setDirectoryInput(directoriesToSearch[directoriesToSearch.size() - 1]);
	searchParams.setSearchTerm();
	directoriesToSearch.pop_back();
}

//ulong end_execution(short code) //Handles the translation between error code and corresponding text
//{
//	//switch(code)
//	//{
//	//case 1:
//	//	std::cout << "ERROR!";
//	//	break;
//	//case 2:
//	//	std::cout << "This program is finished. No more files here to delete." << std::endl;
//	//	code = 0;
//	//	break;
//	//case 3:
//	//	std::cout << "No matching files found." << std::endl;
//	//	code = 0;
//	//	break;
//	//}
//	if (code == 5)
//		code =0;
//	std::cout << std::endl <<std::endl<< std::endl << "Program terminating... ";
//	system ("pause");
//	return code;
//}