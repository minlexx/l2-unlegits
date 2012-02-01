#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ServerPackets.h"
#include "GS.h"
//
#include "world/model/base/ClassIdTree.h"
#include "datatables/CharTemplateTable.h"

/*
Server: Len  967 [NewCharacterSuccess]
C7 03
0D              // opcode
0C 00 00 00     // number of templates

//    [ for each template ]

00 00 00 00    // race
00 00 00 00    // class id
46 00 00 00    // 0x46
28 00 00 00    // base STR
0A 00 00 00    // 0x0A
46 00 00 00    // 0x46
1E 00 00 00    // base DEX
0A 00 00 00
46 00 00 00
2B 00 00 00    // base CON
0A 00 00 00
46 00 00 00
15 00 00 00    // base INT
0A 00 00 00
46 00 00 00
0B 00 00 00    // base WIT
0A 00 00 00
46 00 00 00
19 00 00 00    // base MEN
0A 00 00 00
*/

L2GamePacket *ServerPackets::NewCharacterSuccess( GameClient *pl )
{
	pl = NULL; // reference to player is not needed here
	//
	unsigned int numTemplates = 0;
	L2Game_NewCharacterTemplate tmpl[32];
	int ic = 0;
	ClassIdTree *cidtree = ClassIdTree::getInstance();
	for( ic = 0; ic < ClassIdTree::NUM_CLASS_IDS; ic++ )
	{
		// no Inspector, though it's base class
		if( ic == ClassId::CID_INSPECTOR ) continue;
		// get ClassId if class
		const ClassId *cid = cidtree->getClassId( ic );
		if( cid )
		{
			// no parents, base classes
			if( cid->getParentId() == ClassId::CID_NONE )
			{
				// get template base stats for class
				const L2PlayerTemplate *plt = CharTemplateTable::getTemplate( ic );
				if( plt )
				{
					tmpl[numTemplates].classID = ic;
					tmpl[numTemplates].base_CON = plt->baseCON;
					tmpl[numTemplates].base_DEX = plt->baseDEX;
					tmpl[numTemplates].base_INT = plt->baseINT;
					tmpl[numTemplates].base_MEN = plt->baseMEN;
					tmpl[numTemplates].base_STR = plt->baseSTR;
					tmpl[numTemplates].base_WIT = plt->baseWIT;
					tmpl[numTemplates].race = (int)plt->race;
					numTemplates++;
				}
			}
		}
		if( numTemplates >= 32 ) break;
	}
	//
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x0D ); // NewCharacterSuccess
	p->writeUInt( numTemplates );
	for( ic = 0; ic < (int)numTemplates; ic++ )
	{
		p->writeD( tmpl[ic].race );
		p->writeD( tmpl[ic].classID );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_STR );
		p->writeD( 0x0A );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_DEX );
		p->writeD( 0x0A );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_CON );
		p->writeD( 0x0A );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_INT );
		p->writeD( 0x0A );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_WIT );
		p->writeD( 0x0A );
		p->writeD( 0x46 );
		p->writeD( tmpl[ic].base_MEN );
		p->writeD( 0x0A );
	}
	return p;
}
