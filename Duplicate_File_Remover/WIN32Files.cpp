#include "WIN32Files.h"

WIN32Files::WIN32Files(void):fileHandle(fileHandleInternal), fileInfo(fileInfoInternal), fileIsValid (isValid), isValid(false), fileHandleInternal(INVALID_HANDLE_VALUE)
{
}


WIN32Files::~WIN32Files(void)
{
	if (isValid == true)
	{
		if (WIN32::FindClose(fileHandleInternal) == 0)
		{
			std::cout << "An error has occurred in closing this file search. The error was as follows: ";

			auto successCheck = WIN32::GetLastError();
			TCHAR errorText [256];
			WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,successCheck,0,errorText,256,NULL);
			std::cout << errorText;
			system("pause");
		}
	}
}

ulong WIN32Files::initialSearch(const std::string & searchTerm)
{
	if (fileHandleInternal != INVALID_HANDLE_VALUE) //To clear up any residue in the class before initiating a new search
	{
		closeFile();
		isValid = false;
	}

	fileHandleInternal = WIN32::FindFirstFile(searchTerm.c_str(), &fileInfoInternal);

	if (fileHandleInternal == INVALID_HANDLE_VALUE)
	{
		isValid = false;
		return WIN32::GetLastError();
	}

	isValid = true;
	return 0;
}

ulong WIN32Files::continueSearch()
{
	char lastFile[260];
	strcpy_s(lastFile, 260, fileInfoInternal.cFileName);

	WIN32::FindNextFile(fileHandleInternal, &fileInfoInternal);

	char thisFile[260];
	strcpy_s(thisFile, 260, fileInfoInternal.cFileName);

	if (fileHandleInternal == 0)
	{
		isValid = false;
		return WIN32::GetLastError();
	}

	isValid = true;
	if (strcmp(lastFile, thisFile) == 0)
		return ERROR_NO_MORE_FILES;
	else
		return 0;
}

std::string WIN32Files::errorMsg(ulong errorCode, bool outputToConsole = true)
{
	TCHAR errorText [256];
	WIN32::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, errorText, 256, NULL);
	if (outputToConsole == true)
	{
		std::cout << errorText;
		return "";
	}
	else
	{
		std::string sendOut=errorText;
		return sendOut;
	}
}

ushort WIN32Files::deleteFile(const std::string &directoryInput, ushort &confirmDelete)
{
	if (confirmDelete!=3) //enables "yes to all" functionality to spare headaches
	{
		std::cout << "Are you sure you want to delete " << fileInfoInternal.cFileName << "? Press 1 if no, 2 if yes, 3 if yes to all.\r\n Answer: "; //output file name to be deleted
		std::cin>>confirmDelete;

		while (confirmDelete < 1 || confirmDelete > 3)
		{
			std::cout << std::endl << "Invalid option! Press 1 for no, 2 for yes, 3 for search all found folders.\r\n Answer: ";
			std::cin >> confirmDelete;
		}
	}

	if (confirmDelete!=1)
	{
		char deleteTerm [256]; //preparing file name/path string for deletion
		strcpy_s (deleteTerm, 256, directoryInput.c_str());
		strcat_s (deleteTerm, 256, fileInfoInternal.cFileName);

		if (std::remove(deleteTerm) != 0)
		{
			std::cout << "Error deleting file." << std::endl;
			return 1;
		}
		//isValid = false;
	}
	else
		std::cout << "File skipped." << std::endl;

	return 0;
}

ushort WIN32Files::addCurrentFileToList(const std::string &directoryInput, ushort &confirmAdd)
{
	auto compareResult1 = strcmp(fileInfoInternal.cFileName, ".");
	auto compareResult2 = strcmp(fileInfoInternal.cFileName, "..");

	if (compareResult1 != 0 && compareResult2 != 0)
	{
		if (confirmAdd != 3)
		{
			std::cout << "The folder " << directoryInput.c_str() << fileInfoInternal.cFileName << " was found inside the search directory.\r\n"
				<< "Would you like to search this folder as well? Press 1 for no, 2 for yes, 3 for search all found folders.\r\n Answer: ";
			std::cin >> confirmAdd;

			while (confirmAdd < 1 || confirmAdd > 3)
			{
				std::cout << std::endl << "Invalid option! Press 1 for no, 2 for yes, 3 for search all found folders.\r\n Answer: ";
				std::cin >> confirmAdd;
			}
		}

		if (confirmAdd != 1)
		{
			std::string NameArrange = directoryInput + fileInfoInternal.cFileName;
			listToSearch.push_back(NameArrange);
		}
	}
	return 0;
}

ulong WIN32Files::closeFile()
{
	if (WIN32::FindClose(fileHandleInternal) == 0)
		return WIN32::GetLastError();

	isValid = false;
	return 0;
}