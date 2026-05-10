#define WIN32_LEAN_AND_MEAN
#include "MMFTransportDevyatkina.h"

namespace
{
    constexpr wchar_t MAP_NAME_GLOBAL[] = L"Global\\Devyatkina_MMF_MapData";
    constexpr wchar_t MAP_NAME_LOCAL[] = L"Local\\Devyatkina_MMF_MapData";
    constexpr wchar_t MUTEX_NAME_GLOBAL[] = L"Global\\Devyatkina_MMF_Mutex";
    constexpr wchar_t MUTEX_NAME_LOCAL[] = L"Local\\Devyatkina_MMF_Mutex";
    constexpr wchar_t FILE_NAME[] = L"Devyatkina_MMF.dat";
    constexpr DWORD WAIT_MS = 3000;
}

MMFTransportDevyatkina::MMFTransportDevyatkina()
    : m_mutex(nullptr)
{
    initialize();
}

MMFTransportDevyatkina::~MMFTransportDevyatkina()
{
    if (m_mutex) CloseHandle(m_mutex);
}

bool MMFTransportDevyatkina::initialize()
{
    if (m_mutex != nullptr) return true;

    m_mutex = CreateMutexW(nullptr, FALSE, MUTEX_NAME_GLOBAL);
    if (!m_mutex)
    {
        m_mutex = CreateMutexW(nullptr, FALSE, MUTEX_NAME_LOCAL);
    }
    if (!m_mutex) return false;

    HANDLE probe = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(HeaderDevyatkina), MAP_NAME_GLOBAL);
    if (probe)
    {
        m_mapName = MAP_NAME_GLOBAL;
        CloseHandle(probe);
    }
    else
    {
        m_mapName = MAP_NAME_LOCAL;
    }
    m_fileName = FILE_NAME;

    return true;
}

HANDLE MMFTransportDevyatkina::openSharedFile() const
{
    return CreateFileW(m_fileName.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

bool MMFTransportDevyatkina::send(const MessageDevyatkina& message)
{
    if (!initialize()) return false;
    if (WaitForSingleObject(m_mutex, WAIT_MS) != WAIT_OBJECT_0) return false;

    bool ok = false;
    HANDLE hFile = openSharedFile();
    if (hFile != INVALID_HANDLE_VALUE)
    {
        HeaderDevyatkina header = message.header;
        header.size = static_cast<int>(message.data.size() * sizeof(wchar_t));
        const DWORD totalSize = static_cast<DWORD>(sizeof(HeaderDevyatkina) + header.size);

        HANDLE hMap = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE, 0, totalSize, m_mapName.c_str());
        if (hMap)
        {
            BYTE* view = static_cast<BYTE*>(MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, totalSize));
            if (view)
            {
                memcpy(view, &header, sizeof(HeaderDevyatkina));
                if (header.size > 0)
                {
                    memcpy(view + sizeof(HeaderDevyatkina), message.data.data(), static_cast<size_t>(header.size));
                }
                UnmapViewOfFile(view);
                ok = true;
            }
            CloseHandle(hMap);
        }
        CloseHandle(hFile);
    }
    ReleaseMutex(m_mutex);
    return ok;
}

bool MMFTransportDevyatkina::receive(MessageDevyatkina& message)
{
    if (!initialize()) return false;
    if (WaitForSingleObject(m_mutex, WAIT_MS) != WAIT_OBJECT_0) return false;

    bool ok = false;
    HANDLE hFile = openSharedFile();
    if (hFile != INVALID_HANDLE_VALUE)
    {
        HANDLE hMapHeader = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE, 0, sizeof(HeaderDevyatkina), m_mapName.c_str());
        if (hMapHeader)
        {
            HeaderDevyatkina header {};
            BYTE* headerView = static_cast<BYTE*>(MapViewOfFile(hMapHeader, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HeaderDevyatkina)));
            if (headerView)
            {
                memcpy(&header, headerView, sizeof(HeaderDevyatkina));
                UnmapViewOfFile(headerView);
            }
            CloseHandle(hMapHeader);

            if (header.size >= 0)
            {
                const DWORD totalSize = static_cast<DWORD>(sizeof(HeaderDevyatkina) + header.size);
                HANDLE hMapData = CreateFileMappingW(hFile, nullptr, PAGE_READWRITE, 0, totalSize, m_mapName.c_str());
                if (hMapData)
                {
                    BYTE* dataView = static_cast<BYTE*>(MapViewOfFile(hMapData, FILE_MAP_ALL_ACCESS, 0, 0, totalSize));
                    if (dataView)
                    {
                        memcpy(&header, dataView, sizeof(HeaderDevyatkina));
                        message.header = header;
                        message.data.clear();
                        if (header.size > 0)
                        {
                            const wchar_t* payload = reinterpret_cast<const wchar_t*>(dataView + sizeof(HeaderDevyatkina));
                            message.data.assign(payload, payload + (header.size / static_cast<int>(sizeof(wchar_t))));
                        }
                        UnmapViewOfFile(dataView);
                        ok = true;
                    }
                    CloseHandle(hMapData);
                }
            }
        }
        CloseHandle(hFile);
    }
    ReleaseMutex(m_mutex);
    return ok;
}
