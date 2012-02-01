#include "stdafx.h"
#include "HealItemsTable.h"
#include "utils.h"
#include "Logger.h"

HealItem::HealItem()
{
	itemName[0] = 0;
	itemID = reuseDelayMsec = 0;
	priority = 0;
	lastUseTime = 0;
	percentUse = 0;
}

HealItem::HealItem( const HealItem &other )
{
	strcpy( itemName, other.itemName );
	itemID = other.itemID;
	reuseDelayMsec = other.reuseDelayMsec;
	priority = other.priority;
	lastUseTime = other.lastUseTime;
	percentUse = other.percentUse;
}

/*HealItem& HealItem::operator=( const HealItem& other )
{
	strcpy( itemName, other.itemName );
	itemID = other.itemID;
	reuseDelaySecs = other.reuseDelaySecs;
	priority = other.priority;
	lastUseTime = other.lastUseTime;
	return (*this);
}*/

void HealItem::GetItemNameW( wchar_t *out )
{
	if( !out ) return;
	out[0] = 0;
	if( itemName[0] == 0 ) return;
	MultiByteToWideChar( CP_ACP, 0, itemName, -1, out, 127 );
}


HealItemsTable::HealItemsTable()
{
	clear();
}

HealItemsTable::~HealItemsTable()
{
	clear();
}

void HealItemsTable::clear()
{
	int i;
	for( i=0; i<MAXNUMHEALITEMS; i++ )
	{
		hp_healers[i].itemID = mp_healers[i].itemID = cp_healers[i].itemID = 0;
	}
	enableHealHP = true;
	enableHealMP = true;
	enableHealCP = true;
}

bool HealItemsTable::LoadFromFile( const char *fileName )
{
	if( !fileName ) return false;
	clear();
	int last_added = 0;
	FILE *f = fopen( fileName, "rt" );
	if( !f ) return false;
	char line[512] = {0};
	//
	char itemName[128] = {0};
	unsigned int itemID = 0;
	unsigned int reuseDelay = 0;
	int priority = 0;
	int percentUse = 0;
	//
	int cur_read = 0;
	const int READ_CP = 1;
	const int READ_HP = 2;
	const int READ_MP = 3;
	//
	char *token;
	char delim[] = "=;\r\n";
	//
	while( !feof( f ) )
	{
		if( freadline( f, line, sizeof(line)-1 ) == 0 ) continue;
		if( line[0] == '#' ) continue;
		if( strcmp( line, "[CP healers]" ) == 0 )
		{
			cur_read = READ_CP;
			last_added = 0;
			continue;
		}
		if( strcmp( line, "[HP healers]" ) == 0 )
		{
			cur_read = READ_HP;
			last_added = 0;
			continue;
		}
		if( strcmp( line, "[MP healers]" ) == 0 )
		{
			cur_read = READ_MP;
			last_added = 0;
			continue;
		}
		if( !strchr( line, '=' ) ) continue;

		if( strstr( line, "CPHealEnable" ) == line )
		{
			token = strtok( line, "=" );
			if( token ) token = strtok( NULL, "=" );
			if( token ) sscanf( token, "%d", &(this->enableHealCP) );
			continue;
		}
		if( strstr( line, "HPHealEnable" ) == line )
		{
			token = strtok( line, "=" );
			if( token ) token = strtok( NULL, "=" );
			if( token ) sscanf( token, "%d", &(this->enableHealHP) );
			continue;
		}
		if( strstr( line, "MPHealEnable" ) == line )
		{
			token = strtok( line, "=" );
			if( token ) token = strtok( NULL, "=" );
			if( token ) sscanf( token, "%d", &(this->enableHealMP) );
			continue;
		}

		// token itemName
		token = strtok( line, delim );
		if( !token ) continue;
		strcpy( itemName, token );
		// token itemID
		token = strtok( NULL, delim );
		if( !token ) continue;
		sscanf( token, "%u", &itemID );
		// token reuseDelay
		token = strtok( NULL, delim );
		if( !token ) continue;
		sscanf( token, "%u", &reuseDelay );
		// token priority
		token = strtok( NULL, delim );
		if( !token ) continue;
		sscanf( token, "%d", &priority );
		// token percentUse
		token = strtok( NULL, delim );
		if( !token ) continue;
		sscanf( token, "%d", &percentUse );
		// log
		//switch( cur_read )
		//{
		//case READ_HP: log_error( LOG_USERAI, "HealItemTable: READ_HP: %s [ID=%u] (%d sec, %d priotiy)\n", itemName, itemID, reuseDelay, priority ); break;
		//case READ_MP: log_error( LOG_USERAI, "HealItemTable: READ_MP: %s [ID=%u] (%d sec, %d priotiy)\n", itemName, itemID, reuseDelay, priority ); break;
		//case READ_CP: log_error( LOG_USERAI, "HealItemTable: READ_CP: %s [ID=%u] (%d sec, %d priotiy)\n", itemName, itemID, reuseDelay, priority ); break;
		//}
		//
		//add
		switch( cur_read )
		{
		case READ_HP:
			{
				strcpy( hp_healers[last_added].itemName, itemName );
				hp_healers[last_added].itemID = itemID;
				hp_healers[last_added].reuseDelayMsec = reuseDelay;
				hp_healers[last_added].priority = priority;
				hp_healers[last_added].lastUseTime = 0;
				hp_healers[last_added].percentUse = percentUse;
				last_added++;
			} break;
		case READ_MP:
			{
				strcpy( mp_healers[last_added].itemName, itemName );
				mp_healers[last_added].itemID = itemID;
				mp_healers[last_added].reuseDelayMsec = reuseDelay;
				mp_healers[last_added].priority = priority;
				mp_healers[last_added].lastUseTime = 0;
				mp_healers[last_added].percentUse = percentUse;
				last_added++;
			} break;
		case READ_CP:
			{
				strcpy( cp_healers[last_added].itemName, itemName );
				cp_healers[last_added].itemID = itemID;
				cp_healers[last_added].reuseDelayMsec = reuseDelay;
				cp_healers[last_added].priority = priority;
				cp_healers[last_added].lastUseTime = 0;
				cp_healers[last_added].percentUse = percentUse;
				last_added++;
			} break;
		}
	}
	fclose( f );
	sort();
	return true;
}

void HealItemsTable::sort()
{
	int i, j;
	HealItem temp;
	// hp healers
	for( i=0; i<=(MAXNUMHEALITEMS-2); i++ )
	{
		for( j=i+1; j<=(MAXNUMHEALITEMS-1); j++ )
		{
			if( hp_healers[j].priority > hp_healers[i].priority )
			{
				// save temp
				//temp = hp_healers[i];
				strcpy( temp.itemName, hp_healers[i].itemName );
				temp.itemID          = hp_healers[i].itemID;
				temp.priority        = hp_healers[i].priority;
				temp.reuseDelayMsec  = hp_healers[i].reuseDelayMsec;
				temp.percentUse      = hp_healers[i].percentUse;
				// copy j -=> i
				//hp_healers[i] = hp_healers[j];
				strcpy( hp_healers[i].itemName, hp_healers[j].itemName );
				hp_healers[i].itemID          = hp_healers[j].itemID;
				hp_healers[i].priority        = hp_healers[j].priority;
				hp_healers[i].reuseDelayMsec  = hp_healers[j].reuseDelayMsec;
				hp_healers[i].percentUse      = hp_healers[j].percentUse;
				//restore i
				//hp_healers[j] = temp;
				strcpy( hp_healers[j].itemName, temp.itemName );
				hp_healers[j].itemID          = temp.itemID;
				hp_healers[j].priority        = temp.priority;
				hp_healers[j].reuseDelayMsec  = temp.reuseDelayMsec;
				hp_healers[j].percentUse      = temp.percentUse;
			}
		}
	}
	// mp healers
	for( i=0; i<=(MAXNUMHEALITEMS-2); i++ )
	{
		for( j=i+1; j<=(MAXNUMHEALITEMS-1); j++ )
		{
			if( mp_healers[j].priority > mp_healers[i].priority )
			{
				// save temp
				temp = mp_healers[i];
				// copy j -=> i
				mp_healers[i] = mp_healers[j];
				//restore i
				mp_healers[j] = temp;
			}
		}
	}
	// cp healers
	for( i=0; i<=(MAXNUMHEALITEMS-2); i++ )
	{
		for( j=i+1; j<=(MAXNUMHEALITEMS-1); j++ )
		{
			if( cp_healers[j].priority > cp_healers[i].priority )
			{
				// save temp
				temp = cp_healers[i];
				// copy j -=> i
				cp_healers[i] = cp_healers[j];
				//restore i
				cp_healers[j] = temp;
			}
		}
	}
}

bool HealItemsTable::getHPItem( int idx, HealItem& out )
{
	if( (idx<0) || (idx>=MAXNUMHEALITEMS) ) return false;
	out.itemID = 0;
	out.itemName[0] = 0;
	if( hp_healers[idx].itemID == 0 ) return false;
	//out = hp_healers[idx];
	strcpy( out.itemName,  hp_healers[idx].itemName );
	out.itemID           = hp_healers[idx].itemID;
	out.lastUseTime      = hp_healers[idx].lastUseTime;
	out.priority         = hp_healers[idx].priority;
	out.reuseDelayMsec   = hp_healers[idx].reuseDelayMsec;
	out.percentUse       = hp_healers[idx].percentUse;
	return true;
}

bool HealItemsTable::getMPItem( int idx, HealItem& out )
{
	if( (idx<0) || (idx>=MAXNUMHEALITEMS) ) return false;
	out.itemID = 0;
	out.itemName[0] = 0;
	if( mp_healers[idx].itemID == 0 ) return false;
	out = mp_healers[idx];
	return true;
}

bool HealItemsTable::getCPItem( int idx, HealItem& out )
{
	if( (idx<0) || (idx>=MAXNUMHEALITEMS) ) return false;
	out.itemID = 0;
	out.itemName[0] = 0;
	if( cp_healers[idx].itemID == 0 ) return false;
	out = cp_healers[idx];
	return true;
}

// type: 0 - hp; 1 - mp; 2 - cp
bool HealItemsTable::markUsedNow( HEALITEM_TYPE type, int idx, unsigned int tickCount )
{
	if( (idx<0) || (idx>=MAXNUMHEALITEMS) ) return false;
	switch( type )
	{
	case HIT_HP:
		{
			if( hp_healers[idx].itemID == 0 ) return false;
			hp_healers[idx].lastUseTime = tickCount;
		} break;
	case HIT_MP:
		{
			if( mp_healers[idx].itemID == 0 ) return false;
			mp_healers[idx].lastUseTime = tickCount;
		} break;
	case HIT_CP:
		{
			if( cp_healers[idx].itemID == 0 ) return false;
			cp_healers[idx].lastUseTime = tickCount;
		} break;
	default: return false; break;
	}
	return true;
}

bool HealItemsTable::delItemFromTable( HEALITEM_TYPE type, int index )
{
	if( (index<0) || (index>=MAXNUMHEALITEMS) ) return false;
	int i;
	switch( type )
	{
	case HIT_MARKUSED_HP:
		{
			hp_healers[index].itemID = 0;
			hp_healers[index].priority = 0;
			// shift
			i=index;
			while( i<=(MAXNUMHEALITEMS-2) )
			{
				hp_healers[i] = hp_healers[i+1];
				i++;
			}
		} break;
	case HIT_MARKUSED_MP:
		{
			mp_healers[index].itemID = 0;
			mp_healers[index].priority = 0;
			// shift
			i=index;
			while( i<=(MAXNUMHEALITEMS-2) )
			{
				mp_healers[i] = mp_healers[i+1];
				i++;
			}
		} break;
	case HIT_MARKUSED_CP:
		{
			cp_healers[index].itemID = 0;
			cp_healers[index].priority = 0;
			// shift
			i=index;
			while( i<=(MAXNUMHEALITEMS-2) )
			{
				cp_healers[i] = cp_healers[i+1];
				i++;
			}
		} break;
	default: return false; break;
	}
	//sort();
	return true;
}

bool HealItemsTable::SaveToFile( const char *fileName )
{
	if( !fileName ) return false;
	FILE *f = fopen( fileName, "wt" );
	if( !f ) return false;

	// header
	fprintf( f,
"# Format:\n"
"# Item Name=ItemID;reuseDelay(milliSeconds);priority;activatePercent\n"
"# No spaces between '=', ';' !\n"
"# priority is number between 1 and 100, determines which item will be used first\n"
"# priority 0 means 'do not use this item' :)\n"
"# ActivatePercent is stat value in percent from maximum, when this item can be used\n"
"\n" );

	int i;

	// CP healers
	fprintf( f,
		"[CP healers]\n"
		"CPHealEnable=%d\n", this->enableHealCP );
	for( i=0; i<MAXNUMHEALITEMS; i++ )
	{
		if( this->cp_healers[i].itemID == 0 ) continue;
		fprintf( f, "%s=%u;%u;%d;%d\n",
			this->cp_healers[i].itemName,
			this->cp_healers[i].itemID,
			this->cp_healers[i].reuseDelayMsec,
			this->cp_healers[i].priority,
			this->cp_healers[i].percentUse );
	}
	fprintf( f, "\n" );

	// HP healers
	fprintf( f,
		"[HP healers]\n"
		"HPHealEnable=%d\n", this->enableHealHP );
	for( i=0; i<MAXNUMHEALITEMS; i++ )
	{
		if( this->hp_healers[i].itemID == 0 ) continue;
		fprintf( f, "%s=%u;%u;%d;%d\n",
			this->hp_healers[i].itemName,
			this->hp_healers[i].itemID,
			this->hp_healers[i].reuseDelayMsec,
			this->hp_healers[i].priority,
			this->hp_healers[i].percentUse );
	}
	fprintf( f, "\n" );

	// MP healers
	fprintf( f,
		"[MP healers]\n"
		"MPHealEnable=%d\n", this->enableHealMP );
	for( i=0; i<MAXNUMHEALITEMS; i++ )
	{
		if( this->mp_healers[i].itemID == 0 ) continue;
		fprintf( f, "%s=%u;%u;%d;%d\n",
			this->mp_healers[i].itemName,
			this->mp_healers[i].itemID,
			this->mp_healers[i].reuseDelayMsec,
			this->mp_healers[i].priority,
			this->mp_healers[i].percentUse );
	}
	fprintf( f, "\n" );

	fclose( f );
	return true;
}

bool HealItemsTable::addHealItem( HEALITEM_TYPE type, const HealItem *example )
{
	if( !example ) return false;
	int free_idx = -1;
	int i;
	unsigned int itemID;
	for( i=0; i<MAXNUMHEALITEMS; i++ )
	{
		itemID = 0;
		switch( type )
		{
		case HIT_HP: itemID = hp_healers[i].itemID; break;
		case HIT_MP: itemID = mp_healers[i].itemID; break;
		case HIT_CP: itemID = cp_healers[i].itemID; break;
		}
		if( itemID == 0 )
		{
			free_idx = i;
			break;
		}
	}
	if( free_idx == -1 ) return false;
	switch( type )
	{
	case HIT_HP: hp_healers[free_idx] = (*example); break;
	case HIT_MP: mp_healers[free_idx] = (*example); break;
	case HIT_CP: cp_healers[free_idx] = (*example); break;
	}
	sort();
	return true;
}

bool HealItemsTable::setHealItem( HEALITEM_TYPE type, int idx, const HealItem *example )
{
	if( !example ) return false;
	if( (idx<0) || (idx>=MAXNUMHEALITEMS) ) return false;
	switch( type )
	{
	case HIT_HP: hp_healers[idx] = (*example); break;
	case HIT_MP: mp_healers[idx] = (*example); break;
	case HIT_CP: cp_healers[idx] = (*example); break;
	default: return false; break;
	}
	return true;
}
