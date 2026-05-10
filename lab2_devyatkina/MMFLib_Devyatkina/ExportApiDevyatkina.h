#pragma once

#ifndef EXPORT_API_DEVYATKINA_H
#define EXPORT_API_DEVYATKINA_H

#ifdef MMFLIB_DEVYATKINA_EXPORTS
#define MMF_API_DEVYATKINA __declspec(dllexport)
#else
#define MMF_API_DEVYATKINA __declspec(dllimport)
#endif

extern "C"
{
    MMF_API_DEVYATKINA int SendCommandDevyatkina(int commandType, int targetId, int param, const wchar_t* text,
        int* ackStatus, wchar_t* ackText, int ackTextChars, int timeoutMs);
    MMF_API_DEVYATKINA int WaitCommandDevyatkina(int timeoutMs);
    MMF_API_DEVYATKINA int ReceiveCommandDevyatkina(int* commandType, int* targetId, int* param,
        wchar_t* textBuffer, int textChars);
    MMF_API_DEVYATKINA int SendAckDevyatkina(int status, const wchar_t* text);
}

#endif
