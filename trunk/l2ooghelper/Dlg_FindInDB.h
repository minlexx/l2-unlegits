#ifndef H_DLG_FINDINDB
#define H_DLG_FINDINDB

class Dlg_FindInDB
{
public:
	typedef enum eMODE
	{
		//MODE_NONE = 0,
		MODE_ITEM = 1,
		MODE_NPC,
		MODE_SKILL
	} MODE;
public:
	Dlg_FindInDB( MODE mode );
	~Dlg_FindInDB();

public:
	bool runDialog( HWND hWndParent );
	Dlg_FindInDB::MODE getMode() const { return this->m_mode; }

public: // results
	unsigned int itemID;
	char         itemName[256];
	wchar_t      itemNameW[256];

protected:
	MODE         m_mode;
};

#endif
