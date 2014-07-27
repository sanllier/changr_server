#pragma once

#include <string>

#define CHECK_MAGIC(a) ( a.magic[0] == 'A' && a.magic[1] == 'G' && a.magic[2] == 'S' )

namespace server
{

struct Greeting 
{
	char magic[3];	// AGS
	unsigned char cmd;
	char id[20];

	Greeting()
	{
		memset( this, 0, sizeof( Greeting ) );
	}
};

}
