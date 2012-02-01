#include "stdafx.h"
#include "GroundItem.h"

void GroundItem::setUnused()
{
	L2Object::setUnused();
	itemID = 0;
	stackable = 0;
	count = 0;
}
