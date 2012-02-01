#pragma once
#include "L2CharTemplate.h"
#include "world/model/base/ClassId.h"
#include "world/model/base/ClassIdTree.h"
#include "world/model/base/Race.h"

class L2PlayerTemplate: public L2CharTemplate
{
public: // class for template item
	class PcTemplateItem
	{		
	public:
		PcTemplateItem();
		PcTemplateItem( int itemId, int amount, bool equipped );
		PcTemplateItem( const PcTemplateItem& other );
		const PcTemplateItem& operator=( const PcTemplateItem& other );
		int getItemId() const;
		int getAmount() const;
		bool isEquipped() const;
	protected:
		int m_itemId;
		int m_amount;
		bool m_equipped;
	};

public:
	L2PlayerTemplate( StatsSet& set );
	void addItem( int itemId, int amount, bool equipped );
	const std::list<L2PlayerTemplate::PcTemplateItem>& getItems() const;

public:
	int iClassId;
	const ClassId *classId;
	Race race;
	
	int spawnX;
	int spawnY;
	int spawnZ;
	
	int classBaseLevel;
	double lvlHpAdd;
	double lvlHpMod;
	double lvlCpAdd;
	double lvlCpMod;
	double lvlMpAdd;
	double lvlMpMod;

	int baseLoad;
	bool canCraft;
	
protected:
	std::list<L2PlayerTemplate::PcTemplateItem> m_items;
};
