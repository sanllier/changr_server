#pragma once

#include <string>
#include <stddef.h>

namespace server
{

class Exception
{
public:	
	class ExcData
	{
	private:
		void init( const void* excData = 0, size_t excDataSize = 0 );
	public:
		ExcData( const void* excData = 0, size_t excDataSize = 0 );
		ExcData( const ExcData& excData );
		~ExcData( void );

		ExcData& operator=( const ExcData& excData );

		void*  data( void ) const;
		size_t size( void ) const;
	private:
	    void* rawData;
		size_t dataSize;
	};

public:
	Exception( const std::string& what, const std::string& location, const ExcData& data );
	~Exception( void );

	const char* what( void ) const;
	const char* location( void ) const;
	ExcData     data( void ) const;
private:
	std::string excWhat;
	std::string excLocation;
	ExcData     excData;
};

}
