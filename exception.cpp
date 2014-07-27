#include "exception.h"
#include "log.h"

#include <string>

namespace server
{

void Exception::ExcData::init( const void* excData, size_t excDataSize )
{
	if ( excData && excDataSize )
	{
		memcpy( rawData, excData, excDataSize ); 
		dataSize = excDataSize;
	}
	else
	{
	    rawData = 0;
		dataSize = 0;
    }
}

Exception::ExcData::ExcData( const void* excData, size_t excDataSize )
{
	init( excData, excDataSize );
}

Exception::ExcData::ExcData( const ExcData& excData )
{
	init( excData.data(), excData.size() );
}

Exception::ExcData& Exception::ExcData::operator=( const ExcData& excData )
{
	if ( rawData )
	{
		delete rawData;
		rawData = 0;
	}
	init( excData.data(), excData.size() );

	return *this;
}

Exception::ExcData::~ExcData( void )
{
	if ( rawData )
	{
		delete rawData;
		rawData = 0;
	}
}

// ---------------------------------------------

void* Exception::ExcData::data( void ) const
{
	return rawData;
}

size_t Exception::ExcData::size( void ) const
{
	return dataSize;
}

// ---------------------------------------------
// ---------------------------------------------
// ---------------------------------------------

Exception::Exception( const std::string& what, const std::string& location, const ExcData& data )
	 :excWhat( what )
	 ,excLocation( location )
	 ,excData( data )	
{
	Log::getLog() << std::string( "Exception was created. What: " ).append( what ) \
		                                                           .append( " Where: " ) \
																   .append( location );
}

Exception::~Exception(void)
{

}

// ---------------------------------------------

const char* Exception::what( void ) const
{
	return excWhat.c_str();
}

const char* Exception::location( void ) const
{
	return excLocation.c_str();
}

Exception::ExcData Exception::data( void ) const
{
	return excData;
}

}
