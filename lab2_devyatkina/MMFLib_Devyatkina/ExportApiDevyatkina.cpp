#include "ExportApiDevyatkina.h"
#include "MMFTransportDevyatkina.h"

#include <memory>

namespace
{
    constexpr wchar_t COMMAND_EVENT_GLOBAL[] = L"Global\\Devyatkina_MMF_CommandEvent";
    constexpr wchar_t COMMAND_EVENT_LOCAL[] = L"Local\\Devyatkina_MMF_CommandEvent";
    constexpr wchar_t ACK_EVENT_GLOBAL[] = L"Global\\Devyatkina_MMF_AckEvent";
    constexpr wchar_t ACK_EVENT_LOCAL[] = L"Local\\Devyatkina_MMF_AckEvent";

    std::unique_ptr<MMFTransportDevyatkina> g_transport;
    HANDLE g_commandEvent = nullptr;
    HANDLE g_ackEvent = nullptr;

    MMFTransportDevyatkina& transport()
    {
        if (!g_transport)
        {
            g_transport = std::make_unique<MMFTransportDevyatkina>();
        }
        return *g_transport;
    }

    HANDLE createEventPair(const wchar_t* globalName, const wchar_t* localName)
    {
        HANDLE h = CreateEventW(nullptr, FALSE, FALSE, globalName);
        if (!h)
        {
            h = CreateEventW(nullptr, FALSE, FALSE, localName);
        }
        return h;
    }

    bool ensureEvents()
    {
        if (!g_commandEvent)
        {
            g_commandEvent = createEventPair(COMMAND_EVENT_GLOBAL, COMMAND_EVENT_LOCAL);
        }
        if (!g_ackEvent)
        {
            g_ackEvent = createEventPair(ACK_EVENT_GLOBAL, ACK_EVENT_LOCAL);
        }
        return g_commandEvent && g_ackEvent;
    }

    void copyText(const std::wstring& src, wchar_t* dst, int dstChars)
    {
        if (!dst || dstChars <= 0) return;
        const int toCopy = static_cast<int>(src.size() < static_cast<size_t>(dstChars - 1)
            ? src.size()
            : static_cast<size_t>(dstChars - 1));
        if (toCopy > 0)
        {
            wmemcpy_s(dst, static_cast<size_t>(dstChars), src.c_str(), static_cast<size_t>(toCopy));
        }
        dst[toCopy] = L'\0';
    }
}

extern "C" int SendCommandDevyatkina(int commandType, int targetId, int param, const wchar_t* text,
    int* ackStatus, wchar_t* ackText, int ackTextChars, int timeoutMs)
{
    if (!ensureEvents()) return 0;

    MessageDevyatkina message;
    message.header.type = commandType;
    message.header.targetId = targetId;
    message.header.param = param;
    message.header.status = 0;
    message.data = text ? text : L"";

    if (!transport().send(message)) return 0;
    SetEvent(g_commandEvent);

    if (WaitForSingleObject(g_ackEvent, timeoutMs > 0 ? static_cast<DWORD>(timeoutMs) : INFINITE) != WAIT_OBJECT_0)
    {
        return 0;
    }

    MessageDevyatkina ack;
    if (!transport().receive(ack)) return 0;
    if (ackStatus) *ackStatus = ack.header.status;
    copyText(ack.data, ackText, ackTextChars);
    return 1;
}

extern "C" int WaitCommandDevyatkina(int timeoutMs)
{
    if (!ensureEvents()) return 0;
    const DWORD timeout = timeoutMs < 0 ? INFINITE : static_cast<DWORD>(timeoutMs);
    return WaitForSingleObject(g_commandEvent, timeout) == WAIT_OBJECT_0 ? 1 : 0;
}

extern "C" int ReceiveCommandDevyatkina(int* commandType, int* targetId, int* param, wchar_t* textBuffer, int textChars)
{
    if (!commandType || !targetId || !param || !textBuffer || textChars <= 1) return 0;
    MessageDevyatkina message;
    if (!transport().receive(message)) return 0;

    *commandType = message.header.type;
    *targetId = message.header.targetId;
    *param = message.header.param;
    copyText(message.data, textBuffer, textChars);
    return 1;
}

extern "C" int SendAckDevyatkina(int status, const wchar_t* text)
{
    if (!ensureEvents()) return 0;

    MessageDevyatkina ack;
    ack.header.type = static_cast<int>(MessageTypeDevyatkina::Ack);
    ack.header.status = status;
    ack.header.targetId = 0;
    ack.header.param = 0;
    ack.data = text ? text : L"";

    if (!transport().send(ack)) return 0;
    SetEvent(g_ackEvent);
    return 1;
}
