#include "stdafx.h"
#include "L2Game_SSQInfo.h"

L2Game_SSQInfo::L2Game_SSQInfo()
{
	_initNull();
}

L2Game_SSQInfo::L2Game_SSQInfo( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

/**
protected final void writeImpl()
    {
        writeC(0x73);

        if (_state == 2) // Dawn Sky
        {
            writeH(258);
        }
        else if (_state == 1) // Dusk Sky
        {
            writeH(257);
        }
        else
        {
            writeH(256);
        }
    }
*/

unsigned short int L2Game_SSQInfo::read_SSQ_SkySatus()
{
	if( this->canReadBytes( 3 ) )
	{
		this->getPacketType();
		return this->readUShort();
	}
	else return SSQ_NORMAL_SKY;
}
