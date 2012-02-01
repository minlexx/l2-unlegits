#ifndef H_UserAI_OffpartyBDSWS
#define H_UserAI_OffpartyBDSWS

class UserAI_OffpartyBDSWS
{
public:
	UserAI_OffpartyBDSWS();
	~UserAI_OffpartyBDSWS();
public:
	bool readFromFile( const char *fileName );
	bool saveToFile( const char *fileName );
public:
	void setNameBD_a( const char *anameBD );
	void setNameSWS_a( const char *anameSWS );
public:
	int allEnabled;
	int inv_bd_enable;
	int inv_sws_enable;
	wchar_t nameBD[64];
	wchar_t nameSWS[64];
	int inv_bd_secs;
	int inv_sws_secs;
	int dismiss_bd_secs;
	int dismiss_sws_secs;
};

#endif
