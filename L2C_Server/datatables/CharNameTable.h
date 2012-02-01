#pragma once
#include "l2c_utils.h"

class CharNameTable
{
protected:
	CharNameTable();
	~CharNameTable();
	static CharNameTable *s_instance;
	static int s_refCount;
public:
	static CharNameTable *getInstance();
	static void freeInstance();

public:
	bool doesCharNameExist( const wchar_t *charName );
	int  getAccountCharCount( const wchar_t *accountName );

	void LockDB();
	void UnlockDB();

protected:
	CriticalSection m_lock;
};
