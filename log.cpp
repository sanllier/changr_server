#include "log.h"
#include <ctime>

#pragma warning(disable: 4996) // 'localtime': This function or variable may be unsafe. Consider using localtime_s instead.

// ---------------------------------------------

char Log::logFile[ 255 ] = "log.txt";

// ---------------------------------------------

Log::Log( const std::string& fileName )
{
	logStream.open( fileName, std::ofstream::out | std::ofstream::app );
}

Log::~Log( void )
{
	Log::getLog() << "end of log session";
	logStream     << "-----------------------------------------\r\n";
    logStream.close();
}

// ---------------------------------------------

Log& Log::operator<<( const std::string& logData )
{
	std::time_t rawtime;
    std::tm* timeinfo;
	const int bufLen = 80;
    char buffer[ bufLen ];

    std::time( &rawtime );
    timeinfo = std::localtime( &rawtime );
    std::strftime(buffer, bufLen, "%Y-%m-%d %H:%M:%S", timeinfo);
	logStream << buffer << " | " << logData << "\r\n";    

	return *this;
}
