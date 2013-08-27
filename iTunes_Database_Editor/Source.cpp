#include <iostream>
#include <string>
#include "iTunesCOMInterface.h"

BSTR ConvertMBSToBSTR(const std::string& str);
std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
std::string ConvertBSTRToMBS(BSTR bstr);

int main ()
{
	CoInitialize(NULL);

	HRESULT  hRes;
	IiTunes* iITunes;

	// note - CLSID_iTunesApp and IID_IiTunes are defined in iTunesCOMInterface_i.c
	hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);

	IITLibraryPlaylist* mainLibrary;
	iITunes->get_LibraryPlaylist(&mainLibrary);
	IITTrackCollection* libraryTracks;
	mainLibrary->get_Tracks(&libraryTracks);
	long libraryTracksLength;
	libraryTracks->get_Count(&libraryTracksLength);
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
			IITFileOrCDTrack* trackCast = dynamic_cast<IITFileOrCDTrack*> (track); //something is wrong here
			BSTR trackTitle = SysAllocString(L"");
			hRes = track->get_Artist(&trackTitle); //does not work when switched to IITFileOrCDTrack-only functions
			if (hRes != S_OK)
				break;
			std::cout << ConvertBSTRToMBS(trackTitle);
			::SysFreeString(trackTitle);
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