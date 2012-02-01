#ifndef H_CREATECHAR_DLG
#define H_CREATECHAR_DLG

struct CreateCharDialogResult
{
	int     createTemplateIndex;
	wchar_t createCharName[256];
	int     createHairStyle;
	int     createHairColor;
	int     createFace;
	int     createGender;
};

void CreateCharDialog( HWND hWndParent,
					  L2Game_NewCharacterTemplate *templates,
					  int templatesCount,
					  CreateCharDialogResult *pResult );

#endif
