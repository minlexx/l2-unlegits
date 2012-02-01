#ifndef BINTREENODE_CLAN_H_
#define BINTREENODE_CLAN_H_
#include "BinTree.h"

class ClanTreeNode : public BinTreeNode
{
public:
	ClanTreeNode();
	ClanTreeNode( unsigned int clanID,
		const wchar_t *clanName,
		const wchar_t *allyName );
	virtual ~ClanTreeNode();

public:
	void           setClanID( unsigned int clanID ) { m_clanID = clanID; }
	void           setClanName( const wchar_t *clanName );
	void           setAllyName( const wchar_t *allyName );

public:
	unsigned int   clanID() const { return m_clanID; }
	const wchar_t *clanName() const;
	const wchar_t *allyName() const;

protected:
	unsigned int m_clanID;
	wchar_t      m_clanName[128];
	wchar_t      m_allyName[128];

//protected:
	//wchar_t      m_str_null[16];
};

#endif /* BINTREENODE_CLAN_H_ */
