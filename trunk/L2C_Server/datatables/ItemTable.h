#pragma once
#include "l2c_utils.h"
#include "../world/templates/StatsSet.h"
#include "../world/templates/item/L2ItemTemplate.h"
#include "../world/templates/item/L2ArmorTemplate.h"
#include "../world/templates/item/L2EtcItemTemplate.h"
#include "../world/templates/item/L2WeaponTemplate.h"

class ItemTable
{
protected:
	ItemTable();
	~ItemTable();
	static ItemTable *s_instance;
	static int s_refCount;

public:
	static ItemTable *getInstance();
	static void freeInstance();

public:
	void load();
	void reload();

public:
	const L2ItemTemplate *getTemplate( unsigned int itemId );

protected:
	static unsigned int select_count( void *vcon, const wchar_t *table );
	static unsigned int select_max( void *vcon, const wchar_t *table );
	void loadEtcItems( void *vcon );
	void loadArmors( void *vcon );
	void loadWeapons( void *vcon );

protected:
	CriticalSection      m_lock;
	L2ItemTemplate     **m_allTemplates;
	//L2ArmorTemplate     *m_armorTemplates;
	//L2WeaponTemplate    *m_weaponTemplates;
	//L2EtcItemTemplate   *m_etcItemTemplates;
	unsigned int         m_cnt_all;
	unsigned int         m_maxItemId;
	//unsigned int         m_cnt_armors;
	//unsigned int         m_cnt_weapons;
	//unsigned int         m_cnt_etcItems;
	unsigned long long int m_nTotalBytesAllocated;
};
