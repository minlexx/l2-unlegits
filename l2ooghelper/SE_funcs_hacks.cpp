#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

// l2c_sendPacketHex( string packetHex )
int l2c_sendPacketHex( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	const char *hex = lua_tolstring( L, 1, NULL );
	if( !hex ) return 0;
	cl->game_sendPacketHex( hex );
	return 0;
}
