#include "stdafx.h"
#include "GroundItem.h"

/*void GroundItem::db_query()
{
	if( itemName[0] == 0 )
	{
		L2Data_DB_GetItemNamePicByID( itemID, itemName, NULL );
		if( itemName[0] == 0 ) sprintf( itemName, "item_%u", itemID );
	}
}*/

void GroundItem::setUnused()
{
	L2Object::setUnused();
	itemID = 0;
	stackable = 0;
	count = 0;
}
