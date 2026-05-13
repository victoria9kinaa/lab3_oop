#pragma once

#pragma once

#include <windows.h>

#include "ProtocolDevyatkina.h"

#ifdef LAB4DEVYATKINA_TRANSPORTDLL_EXPORTS
#define LAB4DEVYATKINA_TRANSPORTDLL_API __declspec(dllexport)
#else
#define LAB4DEVYATKINA_TRANSPORTDLL_API __declspec(dllimport)
#endif

struct MsgStructDevyatkina
{
	int to;
	MessageTypesDevyatkina msgType;
	wchar_t* data;
};

extern "C"
{
	LAB4DEVYATKINA_TRANSPORTDLL_API int Devyatkina_Connect();
	LAB4DEVYATKINA_TRANSPORTDLL_API void Devyatkina_Disconnect();

	LAB4DEVYATKINA_TRANSPORTDLL_API int Devyatkina_SendMessage(MsgStructDevyatkina msg);
	LAB4DEVYATKINA_TRANSPORTDLL_API BOOL Devyatkina_ReceiveMessage(MsgStructDevyatkina* msg);
}

