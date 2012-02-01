#include "stdafx.h"
#include "Logger.h"
#include "ClanList.h"
#include "BinTreeNode_Clan.h"

BinTree *bt_clans = NULL;
HWND clanList_updateHWND;
UINT clanList_updateMSG;
CRITICAL_SECTION clanList_cs;
UINT clanList_count;

void ClanList_Init()
{
	clanList_count = 0;
	clanList_updateHWND = NULL;
	clanList_updateMSG = 0;
	InitializeCriticalSection( &clanList_cs );
	EnterCriticalSection( &clanList_cs );
	if( bt_clans == NULL )
	{
		log_error( LOG_DEBUG, "ClanList_Init(): create bintree\n" );
		bt_clans = new BinTree();
	}
	else
	{
		log_error( LOG_DEBUG, "ClanList_Init(): clear bintree\n" );
		bt_clans->deleteAll();
	}
	LeaveCriticalSection( &clanList_cs );
}

void ClanList_Lock() { EnterCriticalSection( &clanList_cs ); }
void ClanList_Unlock() { LeaveCriticalSection( &clanList_cs ); }

void ClanList_SetUpdateCommand( HWND hWnd, UINT uMsg )
{
	clanList_updateHWND = hWnd;
	clanList_updateMSG = uMsg;
}

void ClanList_SendUpdate()
{
	if( !clanList_updateHWND ) return;
	if( clanList_updateMSG < WM_USER ) return;
	PostMessage( clanList_updateHWND, clanList_updateMSG, 0, 0 );
}

void ClanList_Free()
{
	if( !bt_clans ) log_error( LOG_DEBUG, "ClanList_Free() - already NULL\n" );
	else
	{
		ClanList_Lock();
		delete bt_clans;
		bt_clans = NULL;
		ClanList_Unlock();
		ClanList_SendUpdate();
		log_error( LOG_DEBUG, "ClanList_Free() - freed\n" );
	}
	clanList_count = 0;
}

void ClanList_Add( unsigned int clanID, const wchar_t *clanName, const wchar_t *allyName )
{
	if( !clanName || !allyName ) return;
	log_error( LOG_DEBUG, "ClanList_Add: [%S] (id %u) [%S]... ", clanName, clanID, allyName );

	ClanList_Lock();
	if( !bt_clans )
	{
		log_error( LOG_DEBUG, " (BinTree created)... \n" );
		bt_clans = new BinTree();
	}
	BinTreeNode_Clan *node = new BinTreeNode_Clan( clanID, clanName, allyName );

	// first try to find existing char
	BinTreeNode *existing = bt_clans->findNode( clanID );
	if( existing )
	{
		// update info on existing
		BinTreeNode_Clan *cl = (BinTreeNode_Clan *)existing;
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
	ClanList_Unlock();
	ClanList_SendUpdate();
}

void ClanList_Delete( unsigned int clanID )
{
	if( !bt_clans ) return;
	log_error( LOG_DEBUG, "ClanList_Delete( %u )... ", clanID );
	//if( bt_clans->delNodeDebug( oid, stdout ) ) log_error_np( LOG_DEBUG, "OK\n" );
	ClanList_Lock();
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
	ClanList_Unlock();
	ClanList_SendUpdate();
}

void ClanList_DeleteAll()
{
	if( !bt_clans ) return;
	ClanList_Lock();
	bt_clans->deleteAll();
	clanList_count = 0;
	ClanList_Unlock();
	ClanList_SendUpdate();
}

void ClanList_displayNode( FILE *f, BinTreeNode *node )
{
	if( !f || !node ) return;
	BinTreeNode_Clan *btn = (BinTreeNode_Clan *)node;
	fprintf( f, " %S Ally %S [%u]\n", btn->clanName(), btn->allyName(), btn->clanID() );
	if( node->left ) ClanList_displayNode( f, node->left );
	if( node->right ) ClanList_displayNode( f, node->right );
}

void ClanList_Display( FILE *f )
{
	if( !f || !bt_clans ) return;
	ClanList_Lock();
	fprintf( f, "===== Clans list =====\n" );
	BinTreeNode *root = bt_clans->getRoot();
	if( !root ) fprintf( f, "Clans list empty!\n" );
	else ClanList_displayNode( f, root );
	fprintf( f, "======================\n" );
	//bt_clans->printTree( f );
	ClanList_Unlock();
}

unsigned int ClanList_GetCount() { return clanList_count; }

void ClanList_GetClansArray_ProcessNode( BinTreeNode_Clan **clans_array, int *pi, BinTreeNode *cur )
{
	if( !cur || !clans_array || !pi ) return;
	BinTreeNode_Clan *cl = (BinTreeNode_Clan *)cur;
	clans_array[(*pi)] = new BinTreeNode_Clan( (*cl) );
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
	BinTreeNode_Clan **clans_array = (BinTreeNode_Clan **)malloc( (sizeof(void *) * clanList_count ) );
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
}

void ClanList_GetClanNameByID( unsigned int clanID, TCHAR *out )
{
	if( !out || !bt_clans ) return;
	out[0] = 0;
	ClanList_Lock();
	BinTreeNode_Clan *cl = (BinTreeNode_Clan *)bt_clans->findNode( clanID );
	if( !cl )
	{
		ClanList_Unlock();
		return;
	}
#if defined(UNICODE) || defined(_UNICODE)
	_tcscpy( out, cl->clanName() );
#else
	WideCharToMultiByte( CP_ACP, 0, cl->clanName(), -1, out, 127, NULL, NULL );
#endif
	ClanList_Unlock();
}
