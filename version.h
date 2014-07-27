#pragma once

#include <string>

namespace server
{
	
static const int major = 0;
static const int minor = 0;
static const int rev   = 1;

// ---------------------------------------------

static std::string getVerStr( void )
{
	std::string version;
	version.append( std::to_string( major ) ).append( "." ) \
	       .append( std::to_string( minor ) ).append( "." ) \
	       .append( std::to_string( rev ) );

	return version;
}

}
