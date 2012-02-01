#pragma once
#include "world/templates/chars/L2PlayerTemplate.h"

class CharTemplateTable
{
protected:
	static CharTemplateTable *s_instance;
	static int s_refCount;
	CharTemplateTable();
	~CharTemplateTable();
public:
	static CharTemplateTable *getInstance();
	static void freeInstance();

public:
	void load();
	static const wchar_t *getClassNameById( int classId );
	static const L2PlayerTemplate *getTemplate( int classId );

protected:
	void addItemToAllTemplates( int itemId, int amount, bool equipped );

protected:
	std::map<int, L2PlayerTemplate *> m_templates;
};
