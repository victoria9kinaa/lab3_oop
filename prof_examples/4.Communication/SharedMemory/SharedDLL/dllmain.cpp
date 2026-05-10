// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include "SharedDll.h"

#pragma warning(disable: 4996)

#pragma data_seg("Shared")
__declspec(SHAREDEXP) HWND ghWnd = NULL;
#pragma data_seg()

__declspec(allocate("Shared")) wchar_t gsSharedString[1024] = {0};

#pragma comment(linker, "/SECTION:Shared,RWS")

extern "C"
{

__declspec(SHAREDEXP) void __stdcall GetSharedString(wchar_t* s)
{
	wcscpy(s, gsSharedString);
}

__declspec(SHAREDEXP) void __stdcall SetHwnd(HWND hWnd)
{
	ghWnd = hWnd;
}

}


BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

