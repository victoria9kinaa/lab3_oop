#pragma once

#include "ITransportDevyatkina.h"
#include "TransportApiDevyatkina.h"
#include <windows.h>
#include <mutex>

class LAB3DEVYATKINA_TRANSPORTDLL_API MMFTransportDevyatkina : public ITransportDevyatkina
{
public:
	MMFTransportDevyatkina(const wchar_t* fileName = L"filemap.dat", const wchar_t* mapName = L"DevyatkinaMap");
	~MMFTransportDevyatkina();

	virtual void send(MessageDevyatkina& m) const override;
	virtual void receive(MessageDevyatkina& m) const override;

private:
	const wchar_t* m_fileName;
	const wchar_t* m_mapName;
	static std::mutex s_mmfMutex;
};

