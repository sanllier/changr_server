#pragma once

#include <map>
#include <string>

namespace server
{

class Config
{
public:
	Config( const std::string& confFileName );
	~Config( void );

	void reload( void );

	std::string getString( const std::string& key, const std::string& def ) const;
	int getInt( const std::string& key, int def ) const;
	float getFloat( const std::string& key, float def ) const;
	bool getBool( const std::string& key, bool def ) const;
private:
	std::string fileName;
	std::map< std::string, std::string > conf; 
};

}
