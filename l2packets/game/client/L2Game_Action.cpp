#include "stdafx.h"
#include "L2Game_Action.h"

/* L2J:
	private int _objectId;
	@SuppressWarnings("unused")
	private int _originX;
	@SuppressWarnings("unused")
	private int _originY;
	@SuppressWarnings("unused")
	private int _originZ;
	private int _actionId;
	
	@Override
	protected void readImpl()
	{
		_objectId = readD(); // Target object Identifier
		_originX = readD();
		_originY = readD();
		_originZ = readD();
		_actionId = readC(); // Action identifier : 0-Simple click, 1-Shift click
	}
*/

L2Game_Action::L2Game_Action()
{
	this->_initNull();
}

L2Game_Action::L2Game_Action( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_Action::create( unsigned int objectID,
						   unsigned int originX,
						   unsigned int originY,
						   unsigned int originZ,
						   unsigned char bUseShift )
{
	this->writeReset();
	this->writeUChar( 0x1f ); // packet type - Action
	this->writeUInt( objectID );
	this->writeUInt( originX );
	this->writeUInt( originY );
	this->writeUInt( originZ );
	this->writeUChar( bUseShift );
	return true;
}

unsigned int L2Game_Action::read_objectID()
{
	if( !(this->canReadBytes(5)) ) return 0;
	this->readReset();
	this->readUChar(); // pcode
	return this->readUInt();
}

int L2Game_Action::read_originX() { return this->readInt(); }
int L2Game_Action::read_originY() { return this->readInt(); }
int L2Game_Action::read_originZ() { return this->readInt(); }
char L2Game_Action::read_useShift() { return this->readChar(); }
