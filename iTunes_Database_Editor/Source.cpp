#include <iostream>
#include <string>
#include <memory>
#include "iTunesCOMInterface.h"
#include "file_manipulation.h"

BSTR ConvertMBSToBSTR(const std::string& str);
std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
std::string ConvertBSTRToMBS(BSTR bstr);

enum searchMode {name, file};

int main ()
{
	std::cout << "Welcome to the iTunes Database Duplicate Removal Tool" << std::endl<< std::endl;

	std::cout << "Will you be searching via \"name\" or \"file\" today? ";
	std::string reply;
	std::getline(std::cin,reply);

	searchMode searchType;
	std::unique_ptr<std::string> maskInput (new std::string);
	std::unique_ptr<file_manipulation> fileInput;
	if (reply == "name")
	{
		do
		{
			std::cout << "Enter search mask for duplicate song names here (default is * 1): ";
			std::getline(std::cin, *maskInput);
			if (*maskInput == "")
				*maskInput = "* 1";
		} while (false);
		searchType = name;
	}
	else if (reply == "file")
	{
		do
		{
			std::string deletedList = "";
			std::cin.clear();
			std::cout << "Enter path to file containing the paths of the deleted files here: ";
			std::getline(std::cin, deletedList);
			std::unique_ptr<file_manipulation> deletedListFile (new file_manipulation (deletedList));
			deletedListFile.swap(fileInput);
			deletedListFile.reset();
		} while(fileInput->if_exists() == false);
		searchType = file;
	}
	else
		return 1;
	reply.clear();

	CoInitialize(NULL);

	HRESULT  hRes;
	IiTunes* iITunes;

	// note - CLSID_iTunesApp and IID_IiTunes are defined in iTunesCOMInterface_i.c
	hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);

	//Grabs the iTunes library in a way that makes it easy to grab an individual track
	IITLibraryPlaylist* mainLibrary;
	iITunes->get_LibraryPlaylist(&mainLibrary);
	IITTrackCollection* libraryTracks;
	mainLibrary->get_Tracks(&libraryTracks);
	long libraryTracksLength;
	libraryTracks->get_Count(&libraryTracksLength);
	
	int lines = 0;
	if (searchType == file)
	{
		fileInput->read_file("all");
		lines = fileInput->count_input_lines();
		fileInput->parse_input();
	}

	IITTrack* track;
	ITTrackKind trackType;
	for (long iterator=1; iterator <= libraryTracksLength; ++iterator)
	{
		hRes = libraryTracks->get_Item(iterator, &track);
		if (hRes != S_OK)
			break;
		hRes = track->get_Kind(&trackType);
		if (hRes != S_OK)
			break;
		if (trackType == ITTrackKindFile)
		{
			BSTR trackLocation = SysAllocString(L"");
			BSTR trackName = SysAllocString(L"");
			IITFileOrCDTrack *fileTrack = NULL;
			track->QueryInterface(IID_IITFileOrCDTrack, (void**)&fileTrack); //how to convert a IITTrack into a IITFileOrCDTrack
			if (fileTrack != NULL)
			{
				fileTrack->get_Location(&trackLocation);
				if (hRes != S_OK)
					break;
				fileTrack->get_Name(&trackName);
				if (hRes != S_OK)
					break;
				BSTR temp;
				fileTrack->get_Artist(&temp);

				if (searchType == file)
				{
					std::string CStringVersionLocation = ConvertBSTRToMBS(trackLocation);
					std::string CStringVersionName = ConvertBSTRToMBS(trackName);
					for (long fileIterator = 0; fileIterator < lines; ++fileIterator)
					{
						if (fileInput->parsedInput[fileIterator] == CStringVersionLocation)
						{
							//edit database track name
							unsigned int pos=0;
							pos = CStringVersionName.rfind(" 1", std::string::npos);
							std::string newName = CStringVersionName.substr(0, pos);
							fileTrack->put_Name(ConvertMBSToBSTR(newName));

							//edit database file location
							unsigned int extensionPos=0;
							extensionPos = CStringVersionLocation.rfind(".", std::string::npos);
							pos = CStringVersionLocation.rfind(" 1.", std::string::npos);
							std::string newLocation = CStringVersionLocation.substr(0, pos);
							newLocation = newLocation + CStringVersionLocation.substr(extensionPos, std::string::npos);
							fileTrack->put_Location(ConvertMBSToBSTR(newLocation));
							break;
						}
						else if (CStringVersionLocation == "")
						{
							std::cout << CStringVersionName << " has no location. Check this out." << std::endl;
							//system("pause");
							break;
						}
					}
				}
				else if (searchType == name)
				{
					std::cout << "Not implemented yet. Sorry!";
					CoUninitialize();
					return 0;
				}
				else
				{
					CoUninitialize();
					return 1;
				}

			}
			SysFreeString(trackLocation);
			SysFreeString(trackName);
		}
		else
			continue;		
	}

	CoUninitialize();
	system("pause");
	return 0;
}

std::string ConvertBSTRToMBS(BSTR bstr)
{
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

	std::string dblstr(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
		pstr, wslen /* not necessary NULL-terminated */,
		&dblstr[0], len,
		NULL, NULL /* no default char */);

	return dblstr;
}

BSTR ConvertMBSToBSTR(const std::string& str)
{
	int wslen = ::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
		str.data(), str.length(),
		NULL, 0);

	BSTR wsdata = ::SysAllocStringLen(NULL, wslen);
	::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
		str.data(), str.length(),
		wsdata, wslen);
	return wsdata;
}