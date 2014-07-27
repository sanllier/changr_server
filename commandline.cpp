#include "commandline.h"
#include "config.h"
#include "configpresets.h"
#include "cmpresets.h"
#include "exception.h"
#include "version.h"

#include <iostream>

namespace server
{

Commandline::Commandline( void )
{
	greeting();
}

Commandline::~Commandline( void )
{  

}

// ---------------------------------------------

int Commandline::process( void )
{
	static const server::config::Config& conf = server::config::Config::getConfig();
	static const std::string greetingSymbol = conf.getString( server::config::CM_GREETING_SYMB, "@ " );
	std::string command;

	std::cout << greetingSymbol;
	std::getline( std::cin, command);

	if ( command == HALT )
		return HALT_CODE;

	if ( command == PRT_CONF )
	{
		config::ConfData rawConf = conf.getRawData();
		std::cout << "\r\n";
		for ( auto& confItem : rawConf )
			std::cout << "  " << confItem.first << " = " << confItem.second << "\r\n";
		std::cout << "\r\n";
		return SPIN_CODE;
	}

	if ( command == SAVE_CONF )
	{
		std::cout << "  Enter file name: ";
		std::getline( std::cin, command );
		try {
			conf.save( command );
			std::cout << "  Saved!\r\n";
		}
		catch ( Exception& exc )
		{
			std::cout << exc.what() << " " << exc.location() << "\r\n";
		}
		return SPIN_CODE;
	}

	return SPIN_CODE;
}

void Commandline::greeting( void )
{
	std::cout << " ---------------------------------------------\r\n" \
		      << "|                    SERVER                   | \r\n" \
			  << "|                version: " << server::getVerStr() << "               |\r\n" \
			  << " ---------------------------------------------\r\n";
}

void Commandline::print( const std::string& line )
{
	static const server::config::Config& conf = server::config::Config::getConfig();
	static const std::string greetingSymbol = conf.getString( server::config::CM_GREETING_SYMB, "@ " );

	std::cout << "\r\n  " << line << "\r\n" << greetingSymbol;
}

}
