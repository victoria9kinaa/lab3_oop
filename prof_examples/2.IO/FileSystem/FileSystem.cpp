#include <windows.h>
#include <Shlobj.h>
#include <VersionHelpers.h>
#include <conio.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <functional>
using namespace std;

wstring GetString(function<DWORD WINAPI(DWORD, LPTSTR)> f)
{
	DWORD dwLength = f(0, nullptr);
	vector<wchar_t> vBuff(dwLength);
	f(dwLength, &vBuff[0]);
	return wstring(&vBuff[0], dwLength);
}

wstring GetString(function<UINT(LPTSTR, UINT)> f)
{
	DWORD dwLength = f(nullptr, 0);
	vector<wchar_t> vBuff(dwLength);
	f(&vBuff[0], dwLength);
	return wstring(&vBuff[0], dwLength);
}

wstring GetString(function<BOOL(LPTSTR, LPDWORD)> f)
{
	DWORD dwLength = 0;
	f(nullptr, &dwLength);
	vector<wchar_t> vBuff(dwLength);
	f(&vBuff[0], &dwLength);
	return wstring(&vBuff[0], dwLength);
}

wstring GetFolderPath(int nFolder)
{
	wchar_t buff[MAX_PATH];
	SHGetFolderPath(nullptr, nFolder, nullptr, SHGFP_TYPE_CURRENT, buff);
	return wstring(buff);
}

void ListDir(wstring sDir, bool bSubDir = true)
{
	WIN32_FIND_DATA fData;
	HANDLE hFind = FindFirstFile((sDir + L"\\*.*").c_str(), &fData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			wcout << sDir << L"\\" << fData.cFileName;
			if (!(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				wcout << L"\t" << fData.nFileSizeLow;
			wcout << endl;
			if (!bSubDir || !lstrcmp(fData.cFileName, L".") || !lstrcmp(fData.cFileName, L".."))
				continue;
			if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				ListDir(sDir + L"\\" + fData.cFileName);
			}
		} while (FindNextFile(hFind, &fData));
		FindClose(hFind);
	}
}

void WatchDir(const wchar_t* sDir)
{
	HANDLE hFind = FindFirstChangeNotification(sDir, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE);

	while (true)
	{
		WaitForSingleObject(hFind, INFINITE);
		ListDir(sDir, false);
		FindNextChangeNotification(hFind);
	}

	FindCloseChangeNotification(hFind);
}

int main()
{
	// OSVERSIONINFOEX vi = { sizeof(OSVERSIONINFOEX) };
	// GetVersionEx((LPOSVERSIONINFO)&vi);

	wcout << "IsWindows10OrGreater:\t" << IsWindows10OrGreater() << endl;
	wcout << "IsWindows8OrGreater:\t" << IsWindows8OrGreater() << endl << endl;

	DWORD dwDrives = GetLogicalDrives();
	wcout << bitset<32>(dwDrives) << endl << "      ZYXWVUTSRQPONMLKJIHGFEDCBA" << endl << endl;

	wcout
		<< GetString(GetLogicalDriveStrings) << endl
		<< GetString(GetWindowsDirectory) << endl
		<< GetString(GetSystemDirectory) << endl
		<< GetString(GetCurrentDirectory) << endl
		<< GetString(GetTempPath) << endl
		<< GetString(GetUserName) << endl
		<< GetString(GetComputerName) << endl
		<< GetFolderPath(CSIDL_PROGRAM_FILES) << endl
		<< GetFolderPath(CSIDL_PROGRAM_FILES_COMMON) << endl
		<< GetFolderPath(CSIDL_SYSTEM) << endl
		<< GetFolderPath(CSIDL_WINDOWS) << endl
		<< GetFolderPath(CSIDL_PERSONAL) << endl
		<< GetFolderPath(CSIDL_APPDATA) << endl
		<< GetFolderPath(CSIDL_LOCAL_APPDATA) << endl
		<< GetFolderPath(CSIDL_COMMON_APPDATA) << endl;

	wchar_t buff[MAX_PATH];
	GetTempFileName(GetString(GetTempPath).c_str(), L"NCC", 42, buff);
	wcout << buff << endl;

	_getch();

	ListDir(L"..");
	WatchDir(L".");

	_getch();
}
