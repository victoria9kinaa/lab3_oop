#include "../Lab3Devyatkina.TransportDll/SysProgDevyatkina.h"
#include "ServerDevyatkina.h"

int main()
{
	std::locale::global(std::locale(""));
	wcout.imbue(std::locale());

	ServerDevyatkina server;
	server.run();

	return 0;
}

