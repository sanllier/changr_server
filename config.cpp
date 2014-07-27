#include "config.h"
#include "configpresets.h"
#include "exception.h"
#include "log.h"

#include <fstream>

namespace server { namespace config
{

std::string Config::fileName = std::string( DEF_CONF_FILE );

// ---------------------------------------------

Config::Config( void )
{
	load();
}

Config::~Config( void )
{

}

// --------------------------------------------- 

inline bool isFileExist( const std::string& fileName )
{
	std::ifstream file( fileName.c_str(), std::ifstream::in );
	bool isExist = file.good();

	file.close();
	return isExist;
}

inline std::string cutOff( const std::string& str )
{
	static const char openDelimeter  = '(';
	static const char closeDelimeter = ')';

	const size_t openDelPos  = str.find_first_of( openDelimeter );
	const size_t closeDelPos = str.find_last_of( closeDelimeter );

	return str.substr( openDelPos + 1, closeDelPos - openDelPos - 1 );
}

void Config::load( void )
{
	conf.clear();

	if ( !isFileExist( fileName ) )
		throw Exception( std::string( "Config file is not exist: " ).append( fileName ), 
			             std::string( "void Config::load( void )" ), 
						 Exception::ExcData( fileName.c_str(), fileName.length() + 1 ) );

	std::ifstream confFile( fileName.c_str(), std::ifstream::in );

	if ( !confFile.good() )
	{
		confFile.close();
		throw Exception( std::string( "Error while opening config file: " ).append( fileName ), 
			             std::string( "void Config::load( void )" ), 
						 Exception::ExcData( fileName.c_str(), fileName.length() + 1 ) );
	}

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

void Config::save( std::string& confFileName ) const
{
	if ( isFileExist( confFileName ) )
	{
		const int dotPos = confFileName.find_first_of( "." );
		std::string oldFileName = confFileName; 
		oldFileName.insert( dotPos, "_old" );

		std::ifstream src( confFileName, std::ios::binary | std::ios::in );
        std::ofstream dst( oldFileName, std::ios::binary | std::ios::out );
	
		if ( !src.good() || !dst.good() )
		{
			src.close();
			dst.close();
			throw Exception( std::string( "Error while copying config file." ),
				             std::string( "void Config::save( std::string& fileName ) const" ),
							 Exception::ExcData() );
		}
		dst << src.rdbuf();
		src.close();
		dst.close();
	}

	std::ofstream confFile( confFileName, std::ios::out);

	if ( !confFile.good() )
	{
		confFile.close();
		throw Exception( std::string( "Error while saving config in file: " ).append( confFileName ),
			             std::string( "void Config::save( std::string& fileName ) const" ),
						 Exception::ExcData() );
	}
	
	for ( auto& item : conf )
		confFile << "(" << item.first << ") = (" << item.second << ")\r\n";

	confFile.close();
}

ConfData Config:: getRawData( void ) const
{
	return conf;
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
	return found == conf.end() ? def : std::stof( found->second );
}

unsigned char Config::getUChar( const std::string& key, unsigned char def ) const
{
	auto found = conf.find( key );
	int temp = found == conf.end() ? def : std::stoi( found->second );

	if ( temp < 0 )
		return 0;
	if ( temp > 255 )
		return 255;
	return temp;
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

} }
