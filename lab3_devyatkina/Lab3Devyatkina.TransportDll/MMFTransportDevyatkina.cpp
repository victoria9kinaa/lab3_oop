#include "MMFTransportDevyatkina.h"
#include "MessageDevyatkina.h"
#include <string>

std::mutex MMFTransportDevyatkina::s_mmfMutex;

MMFTransportDevyatkina::MMFTransportDevyatkina(const wchar_t* fileName, const wchar_t* mapName)
	:m_fileName(fileName),
	m_mapName(mapName)
{
}

MMFTransportDevyatkina::~MMFTransportDevyatkina()
{
}

void MMFTransportDevyatkina::send(MessageDevyatkina& m) const
{
	std::lock_guard<std::mutex> lock(s_mmfMutex);
	
	HANDLE hFile = CreateFile(m_fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	const int nBytes = int(sizeof(HeaderDevyatkina) + m.header.size);
	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, nBytes, m_mapName);
	if (!hMap)
	{
		CloseHandle(hFile);
		return;
	}

	BYTE* buff = (BYTE*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, nBytes);
	if (!buff)
	{
		CloseHandle(hMap);
		CloseHandle(hFile);
		return;
	}

	memcpy(buff, &m.header, sizeof(HeaderDevyatkina));
	if (m.header.size > 0 && !m.data.empty())
		memcpy(buff + sizeof(HeaderDevyatkina), m.data.data(), m.header.size);

	UnmapViewOfFile(buff);
	CloseHandle(hMap);
	CloseHandle(hFile);
}

void MMFTransportDevyatkina::receive(MessageDevyatkina& m) const
{
	std::lock_guard<std::mutex> lock(s_mmfMutex);
	
	HANDLE hFile = CreateFile(m_fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(HeaderDevyatkina), m_mapName);
	if (!hMap)
	{
		CloseHandle(hFile);
		return;
	}

	void* buff = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HeaderDevyatkina));
	if (!buff)
	{
		CloseHandle(hMap);
		CloseHandle(hFile);
		return;
	}
	m.header = *((HeaderDevyatkina*)buff);

	UnmapViewOfFile(buff);
	CloseHandle(hMap);

	const int nBytes = int(sizeof(HeaderDevyatkina) + m.header.size);
	hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, nBytes, m_mapName);
	if (!hMap)
	{
		CloseHandle(hFile);
		return;
	}

	buff = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, nBytes);
	if (!buff)
	{
		CloseHandle(hMap);
		CloseHandle(hFile);
		return;
	}

	m.data.clear();
	if (m.header.size > 0)
	{
		const wchar_t* p = (const wchar_t*)((BYTE*)buff + sizeof(HeaderDevyatkina));
		m.data.assign(p, p + (m.header.size / sizeof(wchar_t)));
	}

	UnmapViewOfFile(buff);
	CloseHandle(hMap);
	CloseHandle(hFile);
}

