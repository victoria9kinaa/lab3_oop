#pragma once

#ifdef LAB3DEVYATKINA_TRANSPORTDLL_EXPORTS
#define LAB3DEVYATKINA_TRANSPORTDLL_API __declspec(dllexport)
#else
#define LAB3DEVYATKINA_TRANSPORTDLL_API __declspec(dllimport)
#endif

extern "C"
{
	LAB3DEVYATKINA_TRANSPORTDLL_API int Devyatkina_Connect();
	LAB3DEVYATKINA_TRANSPORTDLL_API void Devyatkina_Disconnect();

	LAB3DEVYATKINA_TRANSPORTDLL_API int Devyatkina_RequestThreadCount();
	LAB3DEVYATKINA_TRANSPORTDLL_API int Devyatkina_StartThreads(int nThreads);
	LAB3DEVYATKINA_TRANSPORTDLL_API int Devyatkina_StopThread();
	LAB3DEVYATKINA_TRANSPORTDLL_API int Devyatkina_SendText(int to, const wchar_t* text);
}

