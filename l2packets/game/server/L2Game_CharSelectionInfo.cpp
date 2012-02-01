#include "stdafx.h"
#include "L2Game_CharSelectionInfo.h"

L2Game_CharSelectionInfo::L2Game_CharSelectionInfo()
{
	this->_initNull();
}

L2Game_CharSelectionInfo::L2Game_CharSelectionInfo( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_CharSelectionInfo::read_nChars( unsigned int *ret )
{
	if( !ret ) return false;
	this->readReset();
	if( !this->canReadBytes(10) ) return false;
	this->readUChar(); // pcode
	(*ret) = this->readUInt();
	return true;
}

// ret can be NULL
bool L2Game_CharSelectionInfo::read_server_maxChars( unsigned int *ret )
{
	if( !ret ) return false;
	this->readReset();
	if( !this->canReadBytes(10) ) return false;
	this->readUChar(); // pcode
	this->readUInt(); // nChars
	if( ret ) (*ret) = this->readUInt();
	this->readUChar(); // read 0x00
	return true;
}

bool L2Game_CharSelectionInfo::read_next_charSelectInfoBlock( L2_VERSION l2_version,
	struct L2Game_CharSelectionInfoBlock *c )
{
	if( !c ) return false;
	memset( c, 0, sizeof(L2Game_CharSelectionInfoBlock) );
	// TODO: how to detect that packet may be incorrect?
	// char info must be AT LEAST 280 bytes long (Hellbound)
	if( !this->canReadBytes(280) ) return false;
	const wchar_t *wstr = NULL;
	int i = 0;
	wstr = this->readUnicodeStringPtr();
	if( !wstr ) return false;
	wcsncpy( c->charName, wstr, 31 );
	c->charID = this->readUInt();
	wstr = this->readUnicodeStringPtr();
	if( !wstr ) return false;
	wcsncpy( c->accountName, wstr, 31 );
	c->sessionID = readUInt();
	c->clanID = readUInt();
	readUInt(); // 0x00
	c->sex = readUInt();
	c->race = readUInt();
	c->baseClassID = readUInt();
	c->isActive = readUInt();
	c->x = readInt();
	c->y = readInt();
	c->z = readInt();
	c->HP_cur = readDouble();
	c->MP_cur = readDouble();
	c->SP = readUInt();
	c->Exp = readUInt64();
	c->level = readUInt();
	c->karma = readUInt();
	c->PK_kills = readUInt();
	c->PVP_kills = readUInt();
	for( i=0; i<7; i++ ) readUInt(); // 7 0x00
	c->iid_hair_all = readUInt();
	c->iid_R_ear = readUInt();
	c->iid_L_ear = readUInt();
	c->iid_neck = readUInt();
	c->iid_R_finger = readUInt();
	c->iid_L_finger = readUInt();
	c->iid_head = readUInt();
	c->iid_R_hand = readUInt();
	c->iid_L_hand = readUInt();
	c->iid_gloves = readUInt();
	c->iid_chest = readUInt();
	c->iid_legs = readUInt();
	c->iid_feet = readUInt();
	c->iid_back = readUInt();
	c->iid_LR_hand = readUInt();
	c->iid_hair = readUInt();
	c->iid_hair_2 = readUInt();
	c->iid_R_bracelet = readUInt();
	c->iid_L_bracelet = readUInt();
	for( i=0; i<6; i++ ) readUInt(); // DECO1 .. DECO6
	if( l2_version >= L2_VERSION_T23 )
		c->iid_belt = readUInt(); // Gracia Final T2.3
	c->hairStyle = readUInt();
	c->hairColor = readUInt();
	c->face = readUInt();
	c->HP_max = readDouble();
	c->MP_max = readDouble();
	c->deleteSeconds = readUInt();
	c->classID = readUInt();
	c->lastUsedChar = readUInt();
	c->enchantEffect = readUChar();
	c->augmentID = readUShort();
	c->augmentSmth = readUShort();
	c->transformID = readUInt();
	return true;
}
