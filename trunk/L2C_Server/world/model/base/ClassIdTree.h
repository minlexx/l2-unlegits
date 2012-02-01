#pragma once
#include "Race.h"
#include "ClassId.h"

class ClassIdTree
{
protected:
	static ClassIdTree *_instance;
	static int _refCount;
public:
	static ClassIdTree *getInstance();
	static void freeInstance();
	enum NumClassIds { NUM_CLASS_IDS = 137 };

protected:
	ClassIdTree();
	~ClassIdTree();
	void addClassId( int id, const wchar_t *name, bool isMage, bool isSummoner, Race race, int parentId );

public:
	const ClassId *getClassId( int class_id ) const;
	const ClassId *getParentClassId( int class_id ) const;

protected:
	// array to hold class tree info
	ClassId *m_class[NUM_CLASS_IDS]; // 136 classes
};
