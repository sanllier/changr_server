#include "config.h"
#include "exception.h"
#include "log.h"

#include <fstream>

namespace server
{

Config::Config( const std::string& confFileName  )
	:fileName( confFileName )
{
	reload();
}

Config::~Config(void)
{

}

// --------------------------------------------- 

inline std::string cutOff( const std::string& str )
{
	static const char openDelimeter  = '(';
	static const char closeDelimeter = ')';

	const size_t openDelPos  = str.find_first_of( openDelimeter );
	const size_t closeDelPos = str.find_last_of( closeDelimeter );

	return str.substr( openDelPos + 1, closeDelPos - openDelPos - 1 );
}

void Config::reload( void )
{
	conf.clear();

	std::ifstream confFile( fileName.c_str(), std::ifstream::in );
	std::string buf;
	while ( !confFile.eof() )
	{
		std::getline( confFile, buf );

		size_t found = buf.find_first_of( "//" );
		if ( found != std::string::npos )
			buf.resize( found );

		found = buf.find_first_of( "=" );
		if ( found == std::string::npos )
			continue;

		std::string key = cutOff( buf.substr( 0, found ) );
		std::string val = cutOff( buf.substr( found + 1, buf.length() - found ) );

		if ( !key.length() )
			continue;

		auto foundVal = conf.find( key );
		if ( foundVal != conf.end() )
		{
			Log::getLog() << std::string( "config value redefinition: " ).append( key ) \
																		  .append( " = " ) \
																		  .append( conf[ key ] ) \
																		  .append( " <- " ) \
																		  .append( val );
			foundVal->second = val;
		}
		else
			conf[ key ] = val;
	}

	Log::getLog() << std::string( "config file was loaded" );
	confFile.close();
}

std::string& Config::modify( const std::string& key )
{
	auto found = conf.find( key );

	if ( found != conf.end() )
	{
		Log::getLog() << std::string( "config value could be redefinitioned: " ).append( key ) \
																	       	    .append( " = " ) \
																		        .append( conf[ key ] ) \
																		        .append( " <- " ) \
																		        .append( "UNK" );
		return found->second;
	}
	else
	{
		conf[ key ] = std::string();
		return conf.find( key )->second;
	}
}

// ---------------------------------------------

std::string Config::getString( const std::string& key, const std::string& def ) const
{
	auto found = conf.find( key );
	return found == conf.end() ? def : found->second;
}

int Config::getInt( const std::string& key, int def ) const
{
	auto found = conf.find( key );
	return found == conf.end() ? def :  std::stoi( found->second );
}

float Config::getFloat( const std::string& key, float def ) const
{
	auto found = conf.find( key );
	return found == conf.end() ? def :  std::stof( found->second );
}

bool Config::getBool( const std::string& key, bool def ) const
{
    auto found = conf.find( key );
	static const std::string trueStr( "true" );

	if ( found != conf.end() && found->second == trueStr )
		return true;
	else
		return false;
}

}
