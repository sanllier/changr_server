#pragma once

namespace server
{

static const int MAX_SERVERS = 32;

class Server
{
public:
	Server(void);
	~Server(void);
private:
	static int servers;
};

}
