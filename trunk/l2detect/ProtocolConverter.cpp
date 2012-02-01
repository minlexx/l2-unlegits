#include "stdafx.h"
#include "Logger.h"
#include "ConfigIni.h"

#define PCODE_SELLLIST                  0x06
#define PCODE_BUYLIST                   0x07
#define PCODE_ITEMLIST                  0x11
#define PCODE_TRADESTART                0x14
#define PCODE_TRADEOWNADD               0x1A
#define PCODE_TRADEOTHERADD             0x1B
#define PCODE_INVENTORYUPDATE           0x21
#define PCODE_WAREHOUSEDEPOSITLIST      0x41
#define PCODE_WAREHOUSEWITHDRAWLIST     0x42
#define PCODE_PRIVATESTORELISTSELL      0xA1
#define PCODE_PRIVATESTORELISTBUY       0xBE
#define PCODE_RELATIONCHANGED           0xCE

#define PCODE2_EXBUYSELLLITSTPACKET     0x00B7

extern class CConfig g_cfg;

int pconv_was_Init = 0;
int pconv_enabled  = 0;
int pconv_epilogue = 0;
int pconv_table[256];

void ProtoConv_Init()
{
	memset( pconv_table, 0, sizeof(pconv_table) );
	//
	pconv_table[ PCODE_SELLLIST ]              = 1;
	pconv_table[ PCODE_BUYLIST ]               = 1;
	pconv_table[ PCODE_ITEMLIST ]              = 1;
	pconv_table[ PCODE_TRADESTART ]            = 1;
	pconv_table[ PCODE_TRADEOWNADD ]           = 1;
	pconv_table[ PCODE_TRADEOTHERADD ]         = 1;
	pconv_table[ PCODE_INVENTORYUPDATE ]       = 1;
	pconv_table[ PCODE_WAREHOUSEDEPOSITLIST ]  = 1;
	pconv_table[ PCODE_WAREHOUSEWITHDRAWLIST ] = 1;
	pconv_table[ PCODE_PRIVATESTORELISTSELL ]  = 1;
	pconv_table[ PCODE_PRIVATESTORELISTBUY ]   = 1;
	pconv_table[ PCODE_RELATIONCHANGED ]       = 1;
	//
	pconv_was_Init = 1;
	// disable protocol converter for Hellbound <=> Gracia
	// enable conversion only T1/Hellbound => Gracia
	//if( (g_cfg.L2_version <= L2_VERSION_T15) && (g_cfg.L2_client_version >= L2_VERSION_T2) )
	//	pconv_enabled = 1;
	//if( (g_cfg.L2_version >= L2_VERSION_T2) && (g_cfg.L2_client_version <= L2_VERSION_T15) )
	//	pconv_enabled = 1;
	// Gracia Epilogue Server:148 -> Client:146 protocol conversion
	if( (g_cfg.GraciaEpilogueProtocol_148_hacks) &&
		(g_cfg.OverrideGameProtocolVersion == 148) &&
		(g_cfg.L2_client_version == L2_VERSION_T24) &&
		(g_cfg.L2_version == L2_VERSION_T24) )
	{
		pconv_enabled = 1;
		pconv_epilogue = 1;
	}
	if( pconv_enabled )
	{
		log_error( LOG_USERAI, "Enabled protocol converter\n" );
		if( pconv_epilogue ) log_error( LOG_USERAI, "Protocol converter: Gracia Epilogue mode!\n" );
	}
}

int ProtoConv_IsEnabled()
{
	if( !pconv_was_Init ) ProtoConv_Init();
	return pconv_enabled;
}

int ProtoConv_ConvExistsForPacket( unsigned char pcode )
{
	if( !pconv_was_Init ) ProtoConv_Init();
	return pconv_table[ pcode ];
}

bool ProtoConv_ConvertItemList( unsigned char *bytes, unsigned int len,
							   unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // ItemList old
	L2GamePacket iln; // ItemList new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();
	iln.setPacketType( ptype ); // ptype must be == PCODE_ITEMLIST
	//
	int i = 0;
	unsigned short temp_short;
	unsigned int   temp_int;
	unsigned int   objectID;
	unsigned int   itemID;
	unsigned int   count;  // count of one stackable ITEM slot
	unsigned long long int count64;  // count of one stackable ITEM slot (for protocol >= Final)
	unsigned short icount; // count of all used items SLOTS in inventory
	//
	//writeH(_showWindow ? 0x01 : 0x00);
	temp_short = ilo.readUShort();
	iln.writeUShort( temp_short );
	//writeH(count);
	icount = ilo.readUShort();
	iln.writeUShort( icount );
	//
	for( i=0; i<(int)icount; i++ )
	{
		//writeH(temp.getItem().getType1()); // item type1		
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//writeD(temp.getObjectId());
		objectID = ilo.readUInt();
		iln.writeUInt( objectID );
		//writeD(temp.getItemId());
		itemID = ilo.readUInt();
		iln.writeUInt( itemID );
		//writeD(temp.getLocationSlot()); // T1
		temp_int = ilo.readUInt();
		iln.writeUInt( temp_int );
		//writeD(temp.getCount());
		if( !pconv_epilogue )
		{
			count = ilo.readUInt();
			iln.writeUInt( count );
		}
		else
		{
			count64 = ilo.readUInt64();
			iln.writeUInt64( count64 );
		}
		//writeH(temp.getItem().getType2()); // item type2
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//writeH(temp.getCustomType1()); // item type3
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//writeH(temp.isEquipped() ? 0x01 : 0x00);
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//writeD(temp.getItem().getBodyPart());
		temp_int = ilo.readUInt();
		iln.writeUInt( temp_int );
		//writeH(temp.getEnchantLevel()); // enchant level
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//race tickets
		//writeH(temp.getCustomType2()); // item type3
		temp_short = ilo.readUShort();
		iln.writeUShort( temp_short );
		//	writeD(temp.getAugmentation().getAugmentationId());
		temp_int = ilo.readUInt();
		iln.writeUInt( temp_int );
		//writeD(temp.getMana());
		temp_int = ilo.readUInt();
		iln.writeUInt( temp_int );

		// in gracia final elemental attributes are not Ds, they are Hs
		if( g_cfg.L2_version <= L2_VERSION_T22 )
		{
			// T1
			//writeD(temp.getAttackElementType());
			temp_int = ilo.readUInt();
			iln.writeUInt( temp_int );
			//writeD(temp.getAttackElementPower());
			temp_int = ilo.readUInt();
			iln.writeUInt( temp_int );
			int ia = 0;
			for( ia=0; ia<6; ia++)
			{
				// element def attr(i)
				temp_int = ilo.readUInt(); // 1
				iln.writeUInt( temp_int );
			}
		}
		else if( g_cfg.L2_version >= L2_VERSION_T23 )
		{
			//writeH(temp.getAttackElementType());
			temp_short = ilo.readUShort();
			iln.writeUShort( temp_short );
			//writeH(temp.getAttackElementPower());
			temp_short = ilo.readUShort();
			iln.writeUShort( temp_short );
			int ia = 0;
			for( ia=0; ia<6; ia++)
			{
				// element def attr(i)
				temp_short = ilo.readUShort(); // 1
				iln.writeUShort( temp_short );
			}
		}

		// isTimeLimitedItem()  depends on version!!!!
		// read last 4 bytes for each item in inv only if server is >= T2
		temp_int = 0x00000000;
		if( g_cfg.L2_version >= L2_VERSION_T2 )
		{
			//writeD(0x00);
			temp_int = ilo.readUInt();
		}
		// write timeLimitedItem for each item in inv only if client is >= T2
		if( g_cfg.L2_client_version >= L2_VERSION_T2 )
		{
			iln.writeUInt( temp_int );
		}

		// Gracia Epilogue has 3 Hs more
		if( pconv_epilogue )
		{
			// do NOT read 3 enchant effects from original ItemList
			// but don't write them to resulting ItemList
			iln.writeH( 0 ); // Enchant effect 1
			iln.writeH( 0 ); // Enchant effect 2
			iln.writeH( 0 ); // Enchant effect 3
		}
	}

	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_ItemList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertInventoryUpdate( unsigned char *bytes, unsigned int len,
									  unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // InventoryUpdate old
	L2GamePacket iln; // InventoryUpdate new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();
	iln.setPacketType( ptype ); // ptype must be == PCODE_INVENTORYUPDATE
	//
	int count = ilo.readH();  iln.writeH( (short)count );
	// P.S. count обычно = 1 на ява сервах. Но на всякий...
	int i, ie;
	//unsigned short h;
	//unsigned int d;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // update type
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeD( ilo.readD() ); // item location slot
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeH( ilo.readH() ); // item is equipped
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // item custom type 2 (pet name related..?)
		iln.writeD( ilo.readD() ); // item aug id
		iln.writeD( ilo.readD() ); // item mana
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T2
		iln.writeD( ilo.readD() ); // item time
		// T24
		// write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_InventoryUpdate: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertBuyList( unsigned char *bytes, unsigned int len,
							  unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // BuyList old
	L2GamePacket iln; // BuyList new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();
	iln.setPacketType( ptype ); // ptype must be == PCODE_BUYLIST
	//
	iln.writeQ( ilo.readQ() ); // money
	iln.writeD( ilo.readD() ); // buylist ID
	int count = ilo.readH();  iln.writeH( (short)count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // 0x00 ...
		iln.writeH( ilo.readH() ); // 0x00
		iln.writeQ( ilo.readQ() ); // price
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24
		// write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_BuyList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertSellList( unsigned char *bytes, unsigned int len,
							  unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // SellList old
	L2GamePacket iln; // SellList new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();
	iln.setPacketType( ptype ); // ptype must be == PCODE_SELLLIST
	//
	iln.writeQ( ilo.readQ() ); // money
	iln.writeD( ilo.readD() ); // seller npcID
	int count = ilo.readH();  iln.writeH( (short)count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // 0x00 ...
		iln.writeH( ilo.readH() ); // item custom type 2
		iln.writeQ( ilo.readQ() ); // reference price / 2
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24
		// write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_SellList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertRelationChanged( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();
	iln.setPacketType( ptype ); // ptype must be == PCODE_RELATIONCHANGED
	iln.writeD( 0x01 ); // T24 unknown - write to new, but dont read from old
	iln.writeD( ilo.readD() ); // objectID
	iln.writeD( ilo.readD() ); // relation
	iln.writeD( ilo.readD() ); // auto attackable
	iln.writeD( ilo.readD() ); // karma
	iln.writeD( ilo.readD() ); // pvpFlag
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_RelationChanged: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertExBuySellList( unsigned char *bytes, unsigned int len, unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	int count, i, ia;
	ilo.setBytes( bytes, len );
	//
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode FE
	iln.writeH( ilo.readH() ); // ext.opcode 00B7
	iln.writeQ( ilo.readQ() ); // money
	// first comes BuyList
	iln.writeD( ilo.readD() ); // buylist ID
	count = ilo.readH();   iln.writeH( (short)count );
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // type1
		iln.writeD( ilo.readD() ); // oid
		iln.writeD( ilo.readD() ); // iid
		iln.writeQ( ilo.readQ() ); // count
		iln.writeH( ilo.readH() ); // type2
		iln.writeH( ilo.readH() ); // 00
		iln.writeD( ilo.readD() ); // bodypart
		iln.writeH( ilo.readH() ); // enchant level
		iln.writeH( ilo.readH() ); // 0x00
		iln.writeH( ilo.readH() ); // 0x00 // cust.type3?
		iln.writeQ( ilo.readQ() ); // price
		// elementals
		for( ia=0; ia<8; ia++ ) iln.writeH( ilo.readH() ); // elements
		// T24 enchant effects (dont read from old)
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	// next comes SellList
	count = ilo.readH();   iln.writeH( (short)count );
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // type1
		iln.writeD( ilo.readD() ); // oid
		iln.writeD( ilo.readD() ); // iid
		iln.writeQ( ilo.readQ() ); // count
		iln.writeH( ilo.readH() ); // type2
		iln.writeH( ilo.readH() ); // 00
		iln.writeD( ilo.readD() ); // bodypart
		iln.writeH( ilo.readH() ); // enchant level
		iln.writeH( ilo.readH() ); // 0x00
		iln.writeH( ilo.readH() ); // 0x00 // cust.type3?
		iln.writeQ( ilo.readQ() ); // price
		// elementals
		for( ia=0; ia<8; ia++ ) iln.writeH( ilo.readH() ); // elements
		// T24 enchant effects (dont read from old)
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	// next comes refund list
	count = ilo.readH();   iln.writeH( (short)count );
	for( i=0; i<count; i++ )
	{
		iln.writeD( ilo.readD() ); // refund list item idx
		iln.writeD( ilo.readD() ); // iid
		iln.writeQ( ilo.readQ() ); // count
		iln.writeH( ilo.readH() ); // type2
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // enchant level
		iln.writeH( ilo.readH() ); // 00
		iln.writeQ( ilo.readQ() ); // price
		// elementals
		for( ia=0; ia<8; ia++ ) iln.writeH( ilo.readH() ); // elements
		// T24 enchant effects (dont read from old)
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	try
	{
		iln.writeC( ilo.readC() ); // _done
	}
	catch( L2P_ReadException& e )
	{
		UNREFERENCED_PARAMETER( e );
		iln.writeC( 0x00 ); // :(
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_ExBuySellList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertWarehouseDepositList( unsigned char *bytes, unsigned int len, unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	int count, i, ia;
	ilo.setBytes( bytes, len );
	//
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode FE
	iln.writeH( ilo.readH() ); // WH type
	iln.writeQ( ilo.readQ() ); // adena
	count = ilo.readH();  iln.writeH( (short)count ); // items count
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // type1
		iln.writeD( ilo.readD() ); // oid
		iln.writeD( ilo.readD() ); // iid
		iln.writeQ( ilo.readQ() ); // cnt
		iln.writeH( ilo.readH() ); // type2
		iln.writeH( ilo.readH() ); // cust type1
		iln.writeD( ilo.readD() ); // b.part
		iln.writeH( ilo.readH() ); // ench
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // cust type2
		iln.writeD( ilo.readD() ); // oid
		iln.writeQ( ilo.readQ() ); // aug.related
		for( ia=0; ia<8; ia++ ) iln.writeH( ilo.readH() ); // elementals
		iln.writeD( ilo.readD() ); // mana
		iln.writeD( ilo.readD() ); // time
		// T24 enchant effects
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}
	log_error( LOG_ERROR, "ProtoConv_WarehouseDepositList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertWarehouseWithdrawList( unsigned char *bytes, unsigned int len, unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	int count, i, ia;
	ilo.setBytes( bytes, len );
	//
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode FE
	iln.writeH( ilo.readH() ); // WH type
	iln.writeQ( ilo.readQ() ); // adena
	count = ilo.readH();  iln.writeH( (short)count ); // items count
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // type1
		iln.writeD( ilo.readD() ); // oid
		iln.writeD( ilo.readD() ); // iid
		iln.writeQ( ilo.readQ() ); // cnt
		iln.writeH( ilo.readH() ); // type2
		iln.writeH( ilo.readH() ); // cust type1
		iln.writeD( ilo.readD() ); // b.part
		iln.writeH( ilo.readH() ); // ench
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // cust type2
		iln.writeD( ilo.readD() ); // oid
		iln.writeQ( ilo.readQ() ); // aug.related
		for( ia=0; ia<8; ia++ ) iln.writeH( ilo.readH() ); // elementals
		iln.writeD( ilo.readD() ); // mana
		iln.writeD( ilo.readD() ); // time
		// T24 enchant effects
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}
	log_error( LOG_ERROR, "ProtoConv_WarehouseWithdrawalList: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertTradeStart( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode
	iln.writeD( ilo.readD() ); // partner objectId
	int count = ilo.readH();  iln.writeH( (short)count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // item custom type 2 (pet name related..?)
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24 // write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_TradeStart: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertTradeOwnAdd( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode
	int count = ilo.readH();  iln.writeH( (short)count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // item custom type 2 (pet name related..?)
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24 // write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_TradeOwnAdd: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertTradeOtherAdd( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode
	int count = ilo.readH();  iln.writeH( (short)count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeH( ilo.readH() ); // item type 1
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // item type 2
		iln.writeH( ilo.readH() ); // item custom type 1 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeH( ilo.readH() ); // item enchant level
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // item custom type 2 (pet name related..?)
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24 // write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_TradeOtherAdd: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertPrivateStoreListSell( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode
	iln.writeD( ilo.readD() ); // seller obejct ID
	iln.writeD( ilo.readD() ); // isPackageSale
	iln.writeQ( ilo.readQ() ); // player adena
	int count = ilo.readD();  iln.writeD( count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeD( ilo.readD() ); // item type 2
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // ench
		iln.writeH( ilo.readH() ); // item custom type 2 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeQ( ilo.readQ() ); // price
		iln.writeQ( ilo.readQ() ); // ref.price
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24 // write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_PrivateStoreListSell: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}

bool ProtoConv_ConvertPrivateStoreListBuy( unsigned char *bytes, unsigned int len,
								unsigned char **newBytes, unsigned int *newLen )
{
	L2GamePacket ilo; // old
	L2GamePacket iln; // new
	ilo.setBytes( bytes, len );
	unsigned char ptype = ilo.getPacketType();  iln.setPacketType( ptype ); // opcode
	iln.writeD( ilo.readD() ); // seller object ID
	iln.writeQ( ilo.readQ() ); // player adena
	int count = ilo.readD();  iln.writeD( count ); // items count
	int i, ie;
	for( i=0; i<count; i++ )
	{
		iln.writeD( ilo.readD() ); // item type 2
		iln.writeD( ilo.readD() ); // item object id
		iln.writeD( ilo.readD() ); // item item id
		iln.writeQ( ilo.readQ() ); // item count
		iln.writeH( ilo.readH() ); // 00
		iln.writeH( ilo.readH() ); // ench
		iln.writeH( ilo.readH() ); // item custom type 2 (always 0x00)
		iln.writeD( ilo.readD() ); // item bodypart
		iln.writeQ( ilo.readQ() ); // price
		iln.writeQ( ilo.readQ() ); // ref.price
		// T1 elementals
		for( ie=0; ie<8; ie++ ) iln.writeH( ilo.readH() ); // element. attrs
		// T24 // write enchant effects to new packet, but don't read them from source
		iln.writeH(0x00);  iln.writeH(0x00);  iln.writeH(0x00);
	}
	//
	(*newLen) = iln.getPacketSize();
	(*newBytes) = (unsigned char *)malloc( (*newLen) );
	if( (*newBytes) )
	{
		memcpy( (*newBytes), iln.getBytesPtr(), (*newLen) );
		return true;
	}

	log_error( LOG_ERROR, "ProtoConv_PrivateStoreListBuy: some error after processing, "
		"len = %u, newLen = 0x%p, (*newLen) = %u, (*newBytes) = 0x%p\n",
		len, newLen, (*newLen), (*newBytes) );
	(*newLen) = len;
	(*newBytes) = bytes;
	return false;
}



bool ProtoConv_ConvertPacket( unsigned char *bytes, unsigned int len, 
							 unsigned char **newBytes, unsigned int *newLen )
{
	if( !pconv_enabled ) return false;
	if( !pconv_was_Init ) ProtoConv_Init();
	if( !bytes || (len<3) || !newBytes || !newLen ) return false; // assert
	bool bRet = false;
	unsigned char pcode = bytes[2];
	unsigned short pcode2 = 0;
	if( len >= 5 ) pcode2 = bytes[3] | (((unsigned short)bytes[4]) << 8);

	try { // C++ exception handler

	if( pcode == 0xFE ) // extended server packet
	{
		switch( pcode2 )
		{
		case PCODE2_EXBUYSELLLITSTPACKET: bRet = ProtoConv_ConvertExBuySellList( bytes, len, newBytes, newLen ); break;
		default: log_error( LOG_ERROR, "ProtoConvert: convert not found for packet FE:%02X\n", (unsigned int)pcode ); break;
		}
		return bRet;
	}
	if( pconv_table[ pcode ] == 0 )
	{
		log_error( LOG_ERROR, "ProtoConvert: convert not found for packet 0x%02X\n", (unsigned int)pcode );
		return false;
	}
	switch( pcode )
	{
	case PCODE_SELLLIST:        bRet = ProtoConv_ConvertSellList( bytes, len, newBytes, newLen ); break;
	case PCODE_BUYLIST:         bRet = ProtoConv_ConvertBuyList( bytes, len, newBytes, newLen ); break;
	case PCODE_ITEMLIST:        bRet = ProtoConv_ConvertItemList( bytes, len, newBytes, newLen ); break;
	case PCODE_TRADESTART:      bRet = ProtoConv_ConvertTradeStart( bytes, len, newBytes, newLen ); break;
	case PCODE_TRADEOWNADD:     bRet = ProtoConv_ConvertTradeOwnAdd( bytes, len, newBytes, newLen ); break;
	case PCODE_TRADEOTHERADD:   bRet = ProtoConv_ConvertTradeOtherAdd( bytes, len, newBytes, newLen ); break;
	case PCODE_INVENTORYUPDATE: bRet = ProtoConv_ConvertInventoryUpdate( bytes, len, newBytes, newLen ); break;
	case PCODE_WAREHOUSEDEPOSITLIST: bRet = ProtoConv_ConvertWarehouseDepositList( bytes, len, newBytes, newLen ); break;
	case PCODE_WAREHOUSEWITHDRAWLIST: bRet = ProtoConv_ConvertWarehouseWithdrawList( bytes, len, newBytes, newLen ); break;
	case PCODE_PRIVATESTORELISTSELL: bRet = ProtoConv_ConvertPrivateStoreListSell( bytes, len, newBytes, newLen ); break;
	case PCODE_PRIVATESTORELISTBUY: bRet = ProtoConv_ConvertPrivateStoreListBuy( bytes, len, newBytes, newLen ); break;
	case PCODE_RELATIONCHANGED: bRet = ProtoConv_ConvertRelationChanged( bytes, len, newBytes, newLen ); break;
	}

	} // try
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "ProtoConv_ConvertPacket: pcode = 0x%02X, pcode2 = 0x%02X\n"
			"L2P_Exception: %s\n", (unsigned int)pcode, (unsigned int)pcode2, e.what() );
		// cleanup (не допустим утечек памяти!)
		if( (*newBytes) != bytes ) free( (*newBytes) );
		(*newBytes) = bytes;
		(*newLen) = len;
		bRet = false;
	}

	return bRet;
}
