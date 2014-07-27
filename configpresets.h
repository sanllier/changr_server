#pragma once

#include <string>

namespace server { namespace config
{

static const std::string DEF_CONF_FILE = "config.txt";

static const std::string DB_LOGIN( "db-login" );
static const std::string DB_PASS( "db-pass" );
static const std::string DB_NAME( "db-name" );
static const std::string DB_PORT( "db-port" );

static const std::string WINSOCK_LO_VER( "winsock-lo-ver" );
static const std::string WINSOCK_HI_VER( "winsock-hi-ver" );

static const std::string SERVER_PORT( "server-port" );
static const std::string MAX_SERVERS( "max-servers" );
static const std::string LISTEN_QUEUE_SIZE( "listen-queue-size" );

static const std::string CM_GREETING_SYMB( "cm-greeting-symb" );

} }
