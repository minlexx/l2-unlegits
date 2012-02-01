#include "stdafx.h"
#include "L2ElementalInfo.h"

L2ElementalInfo::L2ElementalInfo()
{
	clear();
}

L2ElementalInfo::L2ElementalInfo( const L2ElementalInfo& other )
{
	clear();
	this->operator=( other );
}

const L2ElementalInfo& L2ElementalInfo::operator=( const L2ElementalInfo& other )
{
	if( this == &other ) return (*this);
	attackElementType  = other.attackElementType;
	attackElementValue = other.attackElementValue;
	defAttrFire        = other.defAttrFire;
	defAttrWater       = other.defAttrWater;
	defAttrWind        = other.defAttrWind;
	defAttrEarth       = other.defAttrEarth;
	defAttrHoly        = other.defAttrHoly;
	defAttrUnholy      = other.defAttrUnholy;
	return (*this);
}

void L2ElementalInfo::clear()
{
	attackElementType  = 0;
	attackElementValue = 0;
	defAttrFire        = 0;
	defAttrWater       = 0;
	defAttrWind        = 0;
	defAttrEarth       = 0;
	defAttrHoly        = 0;
	defAttrUnholy      = 0;
}
