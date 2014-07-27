#pragma once

#include <string>

namespace server
{

static const int SPIN_CODE = 0x0;
static const int HALT_CODE = 0xf;

// ---------------------------------------------

class Commandline
{
private:
	Commandline( void );
	~Commandline( void );
public:
	static Commandline& getCM( void )
	{
		static Commandline cm;
		return cm;
	}
	
	int process( void );
	void greeting( void );
	void print( const std::string& line );
};

}
