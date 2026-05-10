#pragma once

#ifdef SHAREDDLL_EXPORTS
#define SHAREDEXP dllexport
#else
#define SHAREDEXP dllimport
#endif

extern __declspec(SHAREDEXP) HWND ghWnd;
extern __declspec(SHAREDEXP) wchar_t gsSharedString[1024];

