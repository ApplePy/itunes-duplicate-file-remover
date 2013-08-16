#include <iostream>
#include <string>
#include "windows.h"

int end_execution(int code)
{
	switch(code)
	{
	case 0:
		std::cout << std::endl <<std::endl<< std::endl << "Program terminating... ";
		break;
	case 1:
		std::cout << "ERROR!";
		break;
	}
	system ("pause");
	return code;
}


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

	bool exitCheck=false;
	HANDLE fileHandle = WIN32::FindFirstFile(searchTerm.c_str(), &fileStruct); //intial file search
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		if (WIN32::GetLastError()==ERROR_FILE_NOT_FOUND) //nothing matched the search mask
		{
			std::cout << "No matching files found." << std::endl;
			return end_execution(0);
		}
		else
		{
			return end_execution(1);
		}
	}

	unsigned short confirmDelete=0;
	do //iterates through the folder(s) searching for search term.
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
				return end_execution(1);
			}
		}
		else
			std::cout << "File skipped." << std::endl;

		auto nextFile = WIN32::FindNextFile(fileHandle, &fileStruct); //gets next file to examine
		if (nextFile==0)
		{
			if (WIN32::GetLastError()==ERROR_NO_MORE_FILES)
			{
				std::cout << "This program is finished. No more files here to delete." << std::endl;
				exitCheck=true;
			}
			else
			{
				return end_execution(0);
			}
		}
	} while (exitCheck==false);

	return end_execution(0);
}