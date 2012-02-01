#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**writeC(0x18);
writeD(_objectId);
writeD(_attributes.size());

for (Attribute temp: _attributes)
{
    writeD(temp.id);
    writeD(temp.value);
}**/
void L2Client::ph_StatusUpdate( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	//
	const unsigned int LEVEL = 0x01;
    const unsigned int EXP = 0x02;
    const unsigned int STR = 0x03;
    const unsigned int DEX = 0x04;
    const unsigned int CON = 0x05;
    const unsigned int INT = 0x06;
    const unsigned int WIT = 0x07;
    const unsigned int MEN = 0x08;

    const unsigned int CUR_HP = 0x09;
    const unsigned int MAX_HP = 0x0a;
    const unsigned int CUR_MP = 0x0b;
    const unsigned int MAX_MP = 0x0c;

    const unsigned int SP = 0x0d;
    const unsigned int CUR_LOAD = 0x0e;
    const unsigned int MAX_LOAD = 0x0f;

    const unsigned int P_ATK = 0x11;
    const unsigned int ATK_SPD = 0x12;
    const unsigned int P_DEF = 0x13;
    const unsigned int EVASION = 0x14;
    const unsigned int ACCURACY = 0x15;
    const unsigned int CRITICAL = 0x16;
    const unsigned int M_ATK = 0x17;
    const unsigned int CAST_SPD = 0x18;
    const unsigned int M_DEF = 0x19;
    const unsigned int PVP_FLAG = 0x1a;
    const unsigned int KARMA = 0x1b;

    const unsigned int CUR_CP = 0x21;
    const unsigned int MAX_CP = 0x22;
	//
	unsigned int objectID = p->readUInt();
	unsigned int nAttributesUpdated = p->readUInt();
	unsigned int i;
	//
	if( objectID == pcls->usr.objectID ) // update user stats
	{
		for( i=0; i<nAttributesUpdated; i++ )
		{
			unsigned int attrib = p->readUInt();
			int attribVal = p->readInt();
			switch( attrib )
			{
			case CUR_HP: pcls->usr.curHp = (double)attribVal; break;
			case MAX_HP: pcls->usr.maxHp = (double)attribVal; break;
			case CUR_MP: pcls->usr.curMp = (double)attribVal; break;
			case MAX_MP: pcls->usr.maxMp = (double)attribVal; break;
			case CUR_CP: pcls->usr.curCp = (double)attribVal; break;
			case MAX_CP: pcls->usr.maxCp = (double)attribVal; break;
			case CUR_LOAD: pcls->usr.curLoad = attribVal; break;
			case MAX_LOAD: pcls->usr.maxLoad = attribVal; break;
			case LEVEL: pcls->usr.level = attribVal; break;
			case EXP: pcls->usr.experience  = attribVal; break;
			case SP: pcls->usr.skill_points = attribVal; break;
			case STR: pcls->usr.s_STR = attribVal; break;
			case DEX: pcls->usr.s_DEX = attribVal; break;
			case CON: pcls->usr.s_CON = attribVal; break;
			case INT: pcls->usr.s_INT = attribVal; break;
			case WIT: pcls->usr.s_WIT = attribVal; break;
			case MEN: pcls->usr.s_MEN = attribVal; break;
			//
			case P_ATK: pcls->usr.pAtk = attribVal; break;
			case ATK_SPD: pcls->usr.pAtkSpd = attribVal; break;
			case P_DEF: pcls->usr.pDef = attribVal; break;
			case EVASION: pcls->usr.evasion = attribVal; break;
			case ACCURACY: pcls->usr.accuracy = attribVal; break;
			case CRITICAL: pcls->usr.critical = attribVal; break;
			case M_ATK: pcls->usr.mAtk = attribVal; break;
			case CAST_SPD: pcls->usr.mAtkSpd = attribVal; break;
			case M_DEF: pcls->usr.mDef = attribVal; break;
			case PVP_FLAG: pcls->usr.pvpFlag = attribVal; break;
			case KARMA: pcls->usr.karma = attribVal; break;
			}
		}
		pcls->postUpdateUI( UPDATE_USER );
	}
	else if( objectID == pcls->usr.targetObjectID ) // update target's stats
	{
		// for target only support update curHp/maxHp
		for( i=0; i<nAttributesUpdated; i++ )
		{
			unsigned int attrib = p->readUInt();
			int attribVal = p->readInt();
			switch( attrib )
			{
			case CUR_HP: pcls->usr.targetCurHp = (double)attribVal; break;
			case MAX_HP: pcls->usr.targetMaxHp = (double)attribVal; break;
			}
		}
		pcls->postUpdateUI( UPDATE_USER_TARGET );
	}
}
