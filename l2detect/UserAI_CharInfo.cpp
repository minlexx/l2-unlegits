#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "ConfigIni.h"

extern class CConfig g_cfg;

void UserAI::UAI_Parse_CharInfo( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	int i;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	L2Player *cha = new L2Player();

	// parse
	p->getPacketType();    /*writeC(0x31);*/
	cha->x = p->readInt(); /*writeD(_x);*/
	cha->y = p->readInt(); /*writeD(_y);*/
	cha->z = p->readInt(); /*writeD(_z);*/
	p->readUInt();         /*writeD(0x00);*/
	cha->objectID = p->readUInt(); /*writeD(_activeChar.getObjectId());*/

	// invis GM left inivs mode?
	if( cls->invisGM.isInvisGM( cha->objectID ) )
	{
		cls->invisGM.deletePossibleInvisGM( cha->objectID );
		log_error_np( LOG_WARNING, "[INVISGM] possible invis GM left invis mode [%u]\n", cha->objectID );
	}

	cha->setName( p->readUnicodeStringPtr() ); // writeS(_activeChar.getAppearance().getVisibleName());
	cha->race        = p->readUInt(); // writeD(_activeChar.getRace().ordinal());
	cha->sex         = p->readInt();  // writeD(_activeChar.getAppearance().getSex()? 1 : 0);
	cha->baseClassID = p->readUInt(); // writeD(_activeChar.getBaseClass());

	/*writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_UNDER));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HEAD));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_RHAND));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LHAND));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_GLOVES));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_CHEST));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LEGS));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_FEET));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_BACK));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LRHAND));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR2));
	// T1 new d's
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_RBRACELET));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LBRACELET));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO1));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO2));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO3));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO4));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO5));
	writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_DECO6));*/
	// end of t1 new d's
	for( i=0; i<20; i++ ) p->readInt();
	if( g_cfg.L2_version >= L2_VERSION_T23 ) p->readD(); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_BELT));

	// c6 new h's
	/*writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_UNDER));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_HEAD));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_RHAND));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_LHAND));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_GLOVES));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_CHEST));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_LEGS));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_FEET));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_BACK));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_LRHAND));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_HAIR));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_HAIR2));
	// T1 new h's
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_RBRACELET));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_LBRACELET));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO1));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO2));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO3));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO4));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO5));
	writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_DECO6));*/
	// end of t1 new h's
	for( i=0; i<20; i++ ) p->readInt();
	if( g_cfg.L2_version >= L2_VERSION_T23 ) p->readD(); // writeD(_inv.getPaperdollAugmentationId(Inventory.PAPERDOLL_BELT));

	// Gracia Final 2.3
	if( g_cfg.L2_client_version >= (int)L2_VERSION_T23 )
	{
		p->readD();
		p->readD();
	}

	cha->pvpFlag = p->readUInt(); /*writeD(_activeChar.getPvpFlag());*/
	cha->karma = p->readInt();    /*writeD(_activeChar.getKarma());*/

	cha->mAtkSpd = p->readInt(); /*writeD(_mAtkSpd);*/
	cha->pAtkSpd = p->readInt(); /*writeD(_pAtkSpd);*/

	cha->pvpFlag = p->readUInt(); /*writeD(_activeChar.getPvpFlag());*/
	cha->karma = p->readInt();    /*writeD(_activeChar.getKarma());*/

	cha->runSpeed = p->readInt();  /*writeD(_runSpd);*/
	cha->walkSpeed = p->readInt(); /*writeD(_walkSpd);*/
	p->readInt();                  /*writeD(_swimRunSpd);*/ 
	p->readInt();                  /*writeD(_swimWalkSpd);*/ 
	p->readInt();                  /*writeD(_flRunSpd);*/    
	p->readInt();                  /*writeD(_flWalkSpd);*/   
	p->readInt();                  /*writeD(_flyRunSpd);*/   
	p->readInt();                  /*writeD(_flyWalkSpd);*/  
	double moveMult = p->readDouble();  /*writeF(_activeChar.getMovementSpeedMultiplier());*/ 
	double atkMult  = p->readDouble();  /*writeF(_activeChar.getAttackSpeedMultiplier());*/   

	// apply multipliers
	if( moveMult > 0 )
	{
		cha->runSpeed = (int)( ((double)(cha->runSpeed)) * moveMult );
		cha->walkSpeed = (int)( ((double)(cha->walkSpeed)) * moveMult );
	}
	if( atkMult > 0 ) cha->pAtkSpd = (int)( ((double)(cha->pAtkSpd)) * atkMult );

	cha->collisionRadius = p->readDouble(); // collisionRadius
	cha->collisionHeight = p->readDouble(); // collisionHeight

	cha->hairStyle = p->readInt(); /*writeD(_activeChar.getAppearance().getHairStyle());*/ 
	cha->hairColor = p->readInt(); /*writeD(_activeChar.getAppearance().getHairColor());*/ 
	cha->face      = p->readInt(); /*writeD(_activeChar.getAppearance().getFace());*/      

	cha->setTitle( p->readUnicodeStringPtr() ); //writeS(_activeChar.getAppearance().getVisibleTitle());

	cha->clanID      = p->readUInt(); /*writeD(_activeChar.getClanId());*/      
	cha->clanCrestID = p->readInt(); /*writeD(_activeChar.getClanCrestId());*/ 
	cha->allyID      = p->readInt(); /*writeD(_activeChar.getAllyId());*/      
	cha->allyCrestID = p->readInt(); /*writeD(_activeChar.getAllyCrestId());*/ 

	// In UserInfo leader rights and siege flags, but here found nothing??
	// Therefore RelationChanged packet with that info is required
	p->readInt(); // relation, which is always 0

	cha->isSitting   = (int)(!p->readUChar()); /*writeC(_activeChar.isSitting() ? 0 : 1);*/ // standing = 1  sitting = 0
	cha->isRunning   = (int)p->readUChar(); /*writeC(_activeChar.isRunning() ? 1 : 0);*/ // running = 1   walking = 0
	cha->isInCombat  = (int)p->readUChar(); /*writeC(_activeChar.isInCombat() ? 1 : 0);*/  
	cha->isAlikeDead = (int)p->readUChar(); /*writeC(_activeChar.isAlikeDead() ? 1 : 0);*/ 

	//writeC(_activeChar.getAppearance().getInvisible() ? 1 : 0);	// invisible = 1  visible =0
	p->readUChar(); // is Invisible

	p->readUChar(); /*writeC(_activeChar.getMountType());*/ // 1-on Strider, 2-on Wyvern, 3-on Great Wolf, 0-no mount
	p->readUChar(); /*writeC(_activeChar.getPrivateStoreType());*/ //  1 - sellshop

	int nCubics = 0;
	nCubics = (int)p->readUShort(); // writeH(_activeChar.getCubics().size());
	for( i=0; i<nCubics; i++ ) p->readUShort(); // for (int id : _activeChar.getCubics().keySet()) writeH(id);

	p->readUChar(); // writeC(0x00); // find party members
	cha->abnormalEffect = p->readUInt(); // writeD(_activeChar.getAbnormalEffect());

	p->readUChar();  // RecomLeft // isFlying ? 2 : 0 // WTF
	p->readUShort(); /*writeH(_activeChar.getRecomHave());*/ //Blue value for name (0 = white, 255 = pure blue)
	p->readUInt();   /*writeD(_activeChar.getMountNpcId() + 1000000);*/

	cha->classID = p->readUInt(); // writeD(_activeChar.getClassId().getId());
	p->readUInt(); // writeD(0x00);
	cha->enchantEffect = (unsigned int)p->readUChar(); // writeC(_activeChar.getEnchantEffect());

	p->readUChar(); // writeC(0x00); //team circle around feet 1= Blue, 2 = red

	cha->clanCrestLargeID = p->readUInt();// writeD(_activeChar.getClanCrestLargeId());
	cha->isNoble = p->readUChar(); // writeC(_activeChar.isNoble() ? 1 : 0); // Symbol on char menu ctrl+I
	cha->isHero  = p->readUChar(); // writeC(_activeChar.isHero()// hero aura

	cha->isFishing = p->readUChar(); /*writeC(_activeChar.isFishing() ? 1 : 0);*/ //0x01: Fishing Mode (Cant be undone by setting back to 0)
	cha->fishX = p->readInt(); /*writeD(_activeChar.getFishx());*/ 
	cha->fishY = p->readInt(); /*writeD(_activeChar.getFishy());*/ 
	cha->fishZ = p->readInt(); /*writeD(_activeChar.getFishz());*/ 

	p->readInt(); /*writeD(_activeChar.getAppearance().getNameColor());*/
	cha->heading = p->readUInt();/*writeD(_heading);*/ 

	/*writeD(_activeChar.getPledgeClass());
	writeD(_activeChar.getPledgeType());

	writeD(_activeChar.getAppearance().getTitleColor());

	if (_activeChar.isCursedWeaponEquipped())
		writeD(CursedWeaponsManager.getInstance().getLevel(_activeChar.getCursedWeaponEquippedId()));
	else
		writeD(0x00);

	if (_activeChar.getClanId() > 0)
		writeD(_activeChar.getClan().getReputationScore());
	else
		writeD(0x00); 

	// T1
	writeD(_activeChar.getTransformationId());
	writeD(_activeChar.getAgathionId());

	// T2
	writeD(0x01);
	
	// T2.3
    writeD(_activeChar.getSpecialEffect());
    writeD(0x00);
    writeD(0x00);
    writeD(0x00);*/

	// set last time when chars coordinates were known exactly
	cha->lastMoveTickTime = GetTickCount();
	CharArray_AddCharInfo( cha );

	// do we see our offparty BD/SWS?
	if( cls->off_BD_SWS.inv_bd_enable )
	{
		if( cls->offBD_inrange == false )
		{
			if( wcscmp( cha->charName, cls->off_BD_SWS.nameBD ) == 0 )
			{
				cls->offBD_inrange = true;
				log_error( LOG_USERAI, "BD [%S] now in range\n", cha->charName );
			}
		}
	}
	if( cls->off_BD_SWS.inv_sws_enable )
	{
		if( cls->offSWS_inrange == false )
		{
			if( wcscmp( cha->charName, cls->off_BD_SWS.nameSWS ) == 0 )
			{
				cls->offSWS_inrange = true;
				log_error( LOG_USERAI, "SWS [%S] now in range\n", cha->charName );
			}
		}
	}

	delete cha;
}
