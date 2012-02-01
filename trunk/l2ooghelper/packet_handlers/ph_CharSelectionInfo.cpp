#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
#include "ChooseCharDlg.h"

void L2Client::ph_CharSelectionInfo( class L2Client *pcls, L2GamePacket *p )
{
	// ok, we authorized ok
	pcls->setState( STATE_AUTHED_GAME );
	pcls->addChatToTab( CHAT_SYS, L"Received Char list" );

	// vars
	int                            i = 0;
	unsigned int                   nCharsInCharSelection = 0;
	unsigned int                   nServerMaxChars = 0;
	const int                      supported_MaxChars = 32;
	L2Game_CharSelectionInfoBlock  csb_chars[ supported_MaxChars ];
	int                            iSelectedChar = -1;

	L2Game_CharSelectionInfo *p_game_charsel = new L2Game_CharSelectionInfo();
	p_game_charsel->setBytes( p->getBytesPtr(), p->getPacketSize() );
	// parse CharSelectionInfo
	p_game_charsel->read_nChars( &nCharsInCharSelection );
	p_game_charsel->read_server_maxChars( &nServerMaxChars );
	log_error( LOG_DEBUGDUMP, "Char sel: %d chars (max %d)\n", nCharsInCharSelection, nServerMaxChars );
	memset( csb_chars, 0, sizeof(csb_chars) ); // clear buffer to save chars list to
	for( i=0; i<(int)nCharsInCharSelection; i++ )
	{
		p_game_charsel->read_next_charSelectInfoBlock( pcls->account.getL2Version(), &(csb_chars[i]) );
		log_error( LOG_DEBUGDUMP, "%d: %S lv %d %s\n", i,
			csb_chars[i].charName,
			csb_chars[i].level,
			L2Data_getClass( csb_chars[i].classID ) );
	}
	delete p_game_charsel;
	p_game_charsel = NULL;

	// let user choose char in manual mode
	// or select char automatically if set so
	if( pcls->account.charSelectManual )
	{
		iSelectedChar = -1;
		iSelectedChar = ChooseChar( pcls->hWnd, csb_chars, nCharsInCharSelection );
	}
	else // auto select char
	{
		iSelectedChar = -1;
		for( i=0; i<(int)nCharsInCharSelection; i++ )
		{
			if( _wcsicmp( pcls->account.charSelectName, csb_chars[i].charName ) == 0 ) iSelectedChar = i;
		}
		// auto select failed?
		if( iSelectedChar == -1 )
		{
			log_error( LOG_DEBUG, "Auto select char: char [%S] not found!\n", pcls->account.charSelectName );
			pcls->addChatToTabFormat( CHAT_SYS, L"Auto select char: char [%s] not found!", pcls->account.charSelectName );
			log_error( LOG_DEBUG, "Running manual select...\n" );
			iSelectedChar = ChooseChar( pcls->hWnd, csb_chars, nCharsInCharSelection );
		}
		else
		{
			log_error( LOG_DEBUG, "Auto select char: %d [%S]\n", iSelectedChar, csb_chars[iSelectedChar].charName );
		}
	}

	// return value of ChooseChar dialog of -1 means cancel/error
	if( iSelectedChar == -1 )
	{
		log_error( LOG_OK, "Char selection cancelled\n" );
		pcls->addChatToTab( CHAT_SYS, L"Cancel." );
		pcls->disconnectClient(true);
		return;
	}

	// return value of 0..999 means select char number :)
	if( (iSelectedChar >= 0) && (iSelectedChar <= 999) )
	{
		// reply CharSelect
		log_error( LOG_DEBUG, "Manual choose char: %d [%S]\n", iSelectedChar,
			csb_chars[iSelectedChar].charName );
		pcls->send_CharacterSelect( iSelectedChar );
		return;
	}

	// return value of 1000..1999 means request to delete char
	if( (iSelectedChar >= 1000) && (iSelectedChar <= 1999) )
	{
		iSelectedChar -= 1000;
		log_error( LOG_DEBUG, "Request to delete char [%d] [%S]\n", iSelectedChar,
			csb_chars[iSelectedChar].charName );
		pcls->send_CharacterDelete( iSelectedChar );
		return;
	}

	// return value of 2000..21999 means request to cancel delete char
	if( (iSelectedChar >= 2000) && (iSelectedChar <= 2999) )
	{
		iSelectedChar -= 2000;
		log_error( LOG_DEBUG, "Request to cancel delete char [%d] [%S]\n", iSelectedChar,
			csb_chars[iSelectedChar].charName );
		pcls->send_CharacterRestore( iSelectedChar );
		return;
	}

	// return value of 10000 means request to create char
	if( iSelectedChar == 10000 )
	{
		log_error( LOG_DEBUG, "Request to create char...\n" );
		pcls->send_NewCharacter();
		return;
	}

	log_error( LOG_ERROR, "ph_CharSelectionInfo: ERROR: ChooseChar strange return "
		"iSelectedChar = %d\n", iSelectedChar );
}
