#pragma once

#include <map>
#include <string>

namespace server { namespace config
{

typedef std::map< std::string, std::string > ConfData;

class Config
{
private:
	Config( void );
	~Config( void );

	void load( void );
public:
	static Config& getConfig( )
	{
		static Config conf;
		return conf;
	}
	static void setFileName( const std::string& confFileName )
	{
		fileName = confFileName;
	}

	std::string& modify( const std::string& key );
	void save( std::string& confFileName ) const;
	ConfData getRawData( void ) const;

	std::string getString( const std::string& key, const std::string& def ) const;
	int getInt( const std::string& key, int def ) const;
	float getFloat( const std::string& key, float def ) const;
	unsigned char getUChar( const std::string& key, unsigned char def ) const;
	bool getBool( const std::string& key, bool def ) const;
private:
    static std::string fileName;
	ConfData conf; 
};

} }
