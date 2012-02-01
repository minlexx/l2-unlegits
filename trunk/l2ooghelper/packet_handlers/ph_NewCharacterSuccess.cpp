#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
// dlg
#include "CreateCharDlg.h"

void L2Client::ph_NewCharacterSuccess( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_NewCharacterSuccess *ncs = new L2Game_NewCharacterSuccess(
		p->getBytesPtr(), p->getPacketSize() );
	//
	int nTemplates = ncs->read_templatesCount();
	int i;
	const int supported_maxTemplates = 48;
	//
	log_error( LOG_DEBUG, "NewCharacterSuccess: %d templates\n", nTemplates );
	// check
	if( nTemplates > supported_maxTemplates )
	{
		log_error( LOG_WARNING, "NewCharacterSuccess: truncated templates count from %d to %d\n",
			nTemplates, supported_maxTemplates );
		nTemplates = supported_maxTemplates;
	}
	L2Game_NewCharacterTemplate char_templates[supported_maxTemplates];
	memset( &char_templates, 0, sizeof(char_templates) );
	for( i=0; i<nTemplates; i++ )
	{
		ncs->read_nextCharacterTemplate( &(char_templates[i]) );
		//
		log_error( LOG_DEBUG, "tmpl[%d]: race %d(%s), class %d(%s), %d,%d,%d,%d,%d,%d\n",
			i,
			char_templates[i].race,    L2Data_getRace( char_templates[i].race ),
			char_templates[i].classID, L2Data_getClass (char_templates[i].classID ),
			char_templates[i].base_STR, char_templates[i].base_DEX, char_templates[i].base_CON,
			char_templates[i].base_INT, char_templates[i].base_WIT, char_templates[i].base_MEN );
	}
	//
	delete ncs;
	ncs = NULL;
	//
	if( nTemplates > 0 )
	{
		CreateCharDialogResult res;
		CreateCharDialog( pcls->hWnd, char_templates, nTemplates, &res );
		if( res.createTemplateIndex < 0 )
		{
			// creation cancelled
			log_error( LOG_DEBUG, "Char creation cancelled...\n" );
			pcls->send_RequestGotoLobby();
			return;
		}
		//
		log_error( LOG_DEBUG, "Creating char [%S] tmpl index %d\n",
			res.createCharName, res.createTemplateIndex );
		// send CharacterCreate
		pcls->send_CharacterCreate( res.createCharName,
			&(char_templates[res.createTemplateIndex]),
			res.createHairStyle, res.createHairColor, res.createFace, res.createGender );
		// request char selection info
		pcls->send_RequestGotoLobby();
	}
}
