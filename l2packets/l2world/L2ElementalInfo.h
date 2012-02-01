#ifndef H_L2_ELEMENTAL_INFO
#define H_L2_ELEMENTAL_INFO

class L2ElementalInfo
{
public:
	L2ElementalInfo();
	L2ElementalInfo( const L2ElementalInfo& other );
	const L2ElementalInfo& operator=( const L2ElementalInfo& other );
public:
	void clear();
public:
	int attackElementType;
	int attackElementValue;
	int defAttrFire;
	int defAttrWater;
	int defAttrWind;
	int defAttrEarth;
	int defAttrHoly;
	int defAttrUnholy;
};

#endif
