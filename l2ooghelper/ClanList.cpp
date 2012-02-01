#include "stdafx.h"
#include "Logger.h"
#include "ClanList.h"
#include "ClanTreeNode.h"

void ClanList::Init()
{
	clanList_count = 0;
	clanList_updateHWND = NULL;
	clanList_updateMSG = 0;
	InitializeCriticalSection( &clanList_cs );
	EnterCriticalSection( &clanList_cs );
	if( bt_clans == NULL )
	{
		//log_error( LOG_DEBUG, "ClanList_Init(): create bintree\n" );
		bt_clans = new BinTree();
	}
	else
	{
		//log_error( LOG_DEBUG, "ClanList_Init(): clear bintree\n" );
		bt_clans->deleteAll();
	}
	LeaveCriticalSection( &clanList_cs );
}

void ClanList::Lock() { EnterCriticalSection( &clanList_cs ); }
void ClanList::Unlock() { LeaveCriticalSection( &clanList_cs ); }

void ClanList::SetUpdateCommand( HWND hWnd, UINT uMsg )
{
	clanList_updateHWND = hWnd;
	clanList_updateMSG = uMsg;
}

void ClanList::PostUpdate()
{
	if( !clanList_updateHWND ) return;
	if( clanList_updateMSG < WM_USER ) return;
	PostMessage( clanList_updateHWND, clanList_updateMSG, 0, 0 );
}

void ClanList::Free()
{
	if( !bt_clans ) log_error( LOG_DEBUG, "ClanList_Free() - already NULL\n" );
	else
	{
		Lock();
		delete bt_clans;
		bt_clans = NULL;
		Unlock();
		PostUpdate();
		log_error( LOG_DEBUG, "ClanList_Free() - freed\n" );
	}
	clanList_count = 0;
}

void ClanList::Add( unsigned int clanID, const wchar_t *clanName, const wchar_t *allyName )
{
	if( !clanName || !allyName ) return;
	log_error( LOG_DEBUG, "ClanList_Add: [%S] (id %u) [%S]... ", clanName, clanID, allyName );

	Lock();
	if( !bt_clans )
	{
		log_error( LOG_DEBUG, " (BinTree created)... \n" );
		bt_clans = new BinTree();
	}
	ClanTreeNode *node = new ClanTreeNode( clanID, clanName, allyName );

	// first try to find existing char
	BinTreeNode *existing = bt_clans->findNode( clanID );
	if( existing )
	{
		// update info on existing
		ClanTreeNode *cl = (ClanTreeNode *)existing;
		cl->setClanName( clanName );
		cl->setAllyName( allyName );
		cl->setClanID( clanID );
		//printf( "Updated info for [%S]\n", clanName );
	}
	else // add new
	{
		//if( bt_clans->addNodeDebug( node, stdout ) ) log_error_np( LOG_DEBUG, "OK\n" );
		if( bt_clans->addNode( node ) )
		{
			clanList_count++;
			log_error_np( LOG_DEBUG, "OK\n" );
		}
		else
		{
			log_error_np( LOG_DEBUG, "FAILED\n" );
			log_error( LOG_ERROR, "Error adding clan [%S] to clan list!\n", clanName );
			delete node;
		}
	}
	Unlock();
	PostUpdate();
}

void ClanList::Delete( unsigned int clanID )
{
	if( !bt_clans ) return;
	log_error( LOG_DEBUG, "ClanList_Delete( %u )... ", clanID );
	//if( bt_clans->delNodeDebug( oid, stdout ) ) log_error_np( LOG_DEBUG, "OK\n" );
	Lock();
	if( bt_clans->delNode( clanID ) )
	{
		clanList_count--;
		log_error_np( LOG_DEBUG, "OK\n" );
	}
	else
	{
		log_error_np( LOG_DEBUG, "FAILED\n" );
		//log_error( LOG_ERROR, "Error deleting object %u from chars list!\n", oid );
	}
	Unlock();
	PostUpdate();
}

void ClanList::DeleteAll()
{
	if( !bt_clans ) return;
	Lock();
	bt_clans->deleteAll();
	clanList_count = 0;
	Unlock();
	PostUpdate();
}

void ClanList_displayNode( FILE *f, BinTreeNode *node )
{
	if( !f || !node ) return;
	ClanTreeNode *btn = (ClanTreeNode *)node;
	fprintf( f, " %S Ally %S [%u]\n", btn->clanName(), btn->allyName(), btn->clanID() );
	if( node->left ) ClanList_displayNode( f, node->left );
	if( node->right ) ClanList_displayNode( f, node->right );
}

void ClanList::Display( FILE *f )
{
	if( !f || !bt_clans ) return;
	Lock();
	fprintf( f, "===== Clans list =====\n" );
	BinTreeNode *root = bt_clans->getRoot();
	if( !root ) fprintf( f, "Clans list empty!\n" );
	else ClanList_displayNode( f, root );
	fprintf( f, "======================\n" );
	//bt_clans->printTree( f );
	Unlock();
}

void ClanList_displayNodeConsole( BinTreeNode *node )
{
	if( !node ) return;
	ClanTreeNode *btn = (ClanTreeNode *)node;
	log_error( LOG_USERAI, "Clan: [%S] Ally [%S] [%u]\n", btn->clanName(), btn->allyName(), btn->clanID() );
	if( node->left ) ClanList_displayNodeConsole( node->left );
	if( node->right ) ClanList_displayNodeConsole( node->right );
}

void ClanList::DisplayConsole()
{
	if( !bt_clans ) return;
	Lock();
	log_error( LOG_USERAI, "===== Clans list %d =====\n", clanList_count );
	BinTreeNode *root = bt_clans->getRoot();
	if( !root ) log_error( LOG_USERAI, "Clans list empty!\n" );
	else ClanList_displayNodeConsole( root );
	log_error( LOG_USERAI, "======================\n" );
	//bt_clans->printTree( f );
	Unlock();
}

//unsigned int ClanList_GetCount() { return clanList_count; }

/*void ClanList_GetClansArray_ProcessNode( ClanTreeNode **clans_array, int *pi, BinTreeNode *cur )
{
	if( !cur || !clans_array || !pi ) return;
	ClanTreeNode *cl = (ClanTreeNode *)cur;
	clans_array[(*pi)] = new ClanTreeNode( (*cl) );
	clans_array[(*pi)]->left = clans_array[(*pi)]->right = NULL;
	(*pi) = (*pi) + 1;
	if( cur->left )  ClanList_GetClansArray_ProcessNode( clans_array, pi, cur->left );
	if( cur->right ) ClanList_GetClansArray_ProcessNode( clans_array, pi, cur->right );
}

void *ClanList_GetClansArray()
{
	if( !bt_clans ) return NULL;
	if( clanList_count <= 0 )
	{
		clanList_count = 0;
		return NULL;
	}
	ClanTreeNode **clans_array = (ClanTreeNode **)malloc( (sizeof(void *) * clanList_count ) );
	if( !clans_array ) return NULL;

	int i;
	for( i=0; i<(int)clanList_count; i++ ) clans_array[i] = NULL;
	BinTreeNode *cur;

	ClanList_Lock();

	cur = bt_clans->getRoot();
	if( !cur )
	{
		ClanList_Unlock();
		free( clans_array );
		return NULL;
	}

	i = 0;
	ClanList_GetClansArray_ProcessNode( clans_array, &i, cur );

	ClanList_Unlock();

	return clans_array;
}

void ClanList_FreeClansArray( void *p )
{
	if( !p ) return;
	free( p );
}*/

bool ClanList::GetClanNameByID( unsigned int clanID, TCHAR *out )
{
	if( !out || !bt_clans ) return false;
	out[0] = 0;
	Lock();
	ClanTreeNode *cl = (ClanTreeNode *)bt_clans->findNode( clanID );
	if( !cl )
	{
		Unlock();
		return false;
	}
#if defined(UNICODE) || defined(_UNICODE)
	lstrcpy( out, cl->clanName() );
#else
	WideCharToMultiByte( CP_ACP, 0, cl->clanName(), -1, out, 127, NULL, NULL );
#endif
	Unlock();
	return true;
}
