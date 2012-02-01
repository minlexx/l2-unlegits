#ifndef H_BOT_CONFIG
#define H_BOT_CONFIG

#include "BotConfigIDList.h"

typedef enum eCFG_VALTYPE { CFG_TYPE_NONE, CFG_TYPE_INT, CFG_TYPE_FLT, CFG_TYPE_STR } CFG_VALTYPE;

class BotConfigValue
{
public:
	BotConfigValue() { type = CFG_TYPE_NONE; clear(); }
	~BotConfigValue() { clear(); }
	BotConfigValue( const BotConfigValue& other );
	BotConfigValue& operator=( const BotConfigValue& other );
public:
	void        clear();
	void        clearVal();
public:
	void        setStr( const char *stringValue );
	void        setInt( int iVal );
	void        setFlt( double dVal );
public:
	const char *getStr() const;
	int         getInt() const { return i; }
	double      getFlt() const { return d; }
	CFG_VALTYPE getType() const { return type; }
public:
	const char  *name() const { return (const char *)m_name; }
	void         setName( const char *valName );
protected:
	char         m_name[64];
	CFG_VALTYPE  type;
	char        *str;
	double       d;
	int          i;
};

class BotConfig
{
public:
	BotConfig();
	~BotConfig();
	BotConfig( const BotConfig& other );
	BotConfig& operator=( const BotConfig& other );
public:
	void            removeAllValues();
public:
	static const int MAX_VALUES = 512;
public:
	bool            readConfig( const char *fileName, bool clearExistingConfig = true );
	bool            saveConfig( const char *fileName ) const;
	// unicode
	bool            readConfig( const wchar_t *fileName, bool clearExistingConfig = true );
	bool            saveConfig( const wchar_t *fileName ) const;
public:
	CFG_VALTYPE     getValType( const char *valName ) const;
	int             getValInt( const char *valName ) const;
	double          getValFlt( const char *valName ) const;
	const char     *getValStr( const char *valName ) const;
	BotConfigValue  getVal( const char *valName ) const;
	const BotConfigValue& getValByIdx( int idx ) const;
	bool            valExists( const char *valName ) const;
public:
	void            setValue( const char *valName, BotConfigValue &val );
	void            setValInt( const char *valName, int i );
	void            setValFlt( const char *valName, double d );
	void            setValStr( const char *valName, const char *str );
protected:
	int             findValueIndex( const char *valName ) const;
	int             findFreeIndex() const;
	bool            addValue( BotConfigValue& val );
protected:
	BotConfigValue  value[MAX_VALUES];
};

#endif
