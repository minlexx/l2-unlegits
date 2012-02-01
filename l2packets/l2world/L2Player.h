#ifndef H_L2PLAYER
#define H_L2PLAYER
#include "L2Character.h"
#include "L2PlayerPaperDoll.h"
#include "L2ElementalInfo.h"

/** \class L2Player
Every L2Npc is L2Character:\n
Every L2Character is L2Object (has objectID, x, y, z)\n
Every L2Character has LEVEL!\n
Every L2Character has its charName and charTitle\n
Every L2Character has its heading\n
Every L2Character has its hairStyle, hairColor and face\n
Every L2Character has moving speed in states: running, walking\n
Every L2Character has its move destination: xDst, yDst, zDst\n
Every L2Character can calc its position based on destnation point and time passed since last calc\n
Every L2Character has its base stats: INT WIT MEN CON STR DEX\n
Every L2Character has its stats: pAtk, mAtk, ..., curHp, maxHp, ...\n
Every L2Character has abnormalEffect\n
Every L2Character has its target\n
Every L2Character can be in combat\n
Every L2Character has its collision radius and height\n
\n
Additionally:\n
Every L2Player has classID, baseClassID\n
Every L2Player has its appearance (Race, Sex, Face, Hair style, Hair color)\n
Every L2Player has enchantEffect\n
Every L2Player has its paperdoll: items equipped on it. Every paperdoll item can have augment ID\n
Every L2Player can fish: has isFishing, fishX, fishY, fishZ\n
Every L2Player can create sell shop: sell, buy, package sale, craft shop (has shop type and shop message)\n
Every L2Player can ride animals (mount NPC)\n
Every L2Player can give and receive recommendations\n
Every L2Player can be noble and hero\n
\n
Additionally, some values come from RelationChanged packet, not in CharInfo\n
Every L2Player has relation to current user\n
Every L2Player can be autoAttackable\n
\n


*/

class L2Player : public L2Character
{
public:
	/** Default constructor, just calls setUnused() (zeroes all members) */
	L2Player();

	/** Copy constructor.
	 * \param other source to copy from
	 */
	L2Player( const L2Player& other );

	/** Operator=
	 * \param other source to copy from
	 * \return reference to this object
	 */
	virtual const L2Player& operator=( const L2Player& other );

	/** Just calls setUnused() (zeroes all members) */
	virtual ~L2Player();
public:
	/** Initializes object state (zeroes all members) */
	virtual void setUnused();
public:
	/** Converts player's race ID to displayable string
	* \param out pointer to unicode string buffer to receive string
	* \return out
	*/
	virtual void getRaceStr( wchar_t *out ) const;

	/** Converts player's sex number to displayable string
	* \param out pointer to unicode string buffer to receive string
	* \return out
	*/
	virtual void getSexStr( wchar_t *out ) const;

	/** Converts player's class ID to displayable string.\n
	* This is current selected subclass (not base class).
	* \param out pointer to unicode string buffer to receive string
	* \return out
	*/
	virtual void getClassStr( wchar_t *out ) const;

	/** Converts player's base class ID to displayable string.\n
	* This is always player's base class.
	* \param out pointer to unicode string buffer to receive string
	* \return out
	*/
	virtual void getBaseClassStr( wchar_t *out ) const;

public:
	// relation to current user constants
	static const int RELATION_NONE         = 0; ///< default value, no mean
	static const int RELATION_PVP_FLAG     = 0x00002; ///< pvp ???
	static const int RELATION_HAS_KARMA    = 0x00004; ///< karma ???
	static const int RELATION_LEADER       = 0x00080; ///< leader
	static const int RELATION_INSIEGE      = 0x00200; ///< true if in siege
	static const int RELATION_ATTACKER     = 0x00400; ///< true when attacker
	static const int RELATION_ALLY         = 0x00800; ///< blue siege icon, cannot have if red
	static const int RELATION_ENEMY        = 0x01000; ///< true when red icon, doesn't matter with blue
	static const int RELATION_MUTUAL_WAR   = 0x08000; ///< double fist (war)
	static const int RELATION_1SIDED_WAR   = 0x10000; ///< single fist (war)
	// private store type constants
	static const int PRIVATE_STORE_NONE        = 0; ///< player has no private store
	static const int PRIVATE_STORE_SELL        = 1; ///< player is selling
	static const int PRIVATE_STORE_BUY         = 3; ///< player is buying
	static const int PRIVATE_STORE_CRAFT       = 5; ///< player is sitting in recipe shop mode
	static const int PRIVATE_STORE_PACKAGESALE = 8; ///< player is in package sale mode

public:
	bool parse_CharInfo( void *l2_game_packet, L2_VERSION l2_version );
	bool parse_UserInfo( void *l2_game_packet, L2_VERSION l2_version );

public:
	unsigned int classID; ///< current class ID, which may be subclass and may differ from base class ID
	unsigned int baseClassID; ///< player's base class ID
	int race; ///< race number
	int sex; ///< 1 - female, 0 - male
	int hairStyle;
	int hairColor;
	int face;
	unsigned long long experience;   // only from UserInfo
	unsigned int skill_points; // only from UserInfo
	int curLoad; // only from UserInfo
	int maxLoad; // only from UserInfo
	int pkKills; // only from UserInfo
	int pvpKills; // only from UserInfo
	unsigned int enchantEffect; ///< player's weapon enchant value
	// from RelationChanged
	unsigned int relation; ///< relation (siege flags and clan war flags,fists)
	int autoAttackable; ///< 1, if player is auto attackable (2-sided war, for example)
	unsigned int paperdoll_oid[32]; ///< array of paperdoll objectIDs (only from UserInfo)
	unsigned int paperdoll_iid[32]; ///< array of paperdoll itemIDs
	unsigned int paperdoll_augid[32]; ///< array of paperdoll augmentationIDs
	int isFakeDeath;
	int isFishing;
	int fishX;
	int fishY;
	int fishZ;
	int privateStoreType; ///< player's private store type. See PRIVATE_STORE_* constants (1 - sellshop, ...)
	wchar_t privateStoreMsgBuy[64]; ///< buy shop message
	wchar_t privateStoreMsgSell[64]; ///< sell shop message
	wchar_t privateStoreMsgRecipe[64]; ///< recipe shop message
	unsigned int mountType; ///< 1-on Strider, 2-on Wyvern, 3-on Great Wolf, 0-no mount
	unsigned int mountNpcId; ///< mount NPC template ID
	int recomLeft; ///< number of recommendations this player can give to others
	int recomHave; ///< number of received recommendations
	int isNoble; ///< 1 if is noblesse
	int isHero; ///< 1 if is hero
	unsigned int nameColor;
	unsigned int titleColor;
	unsigned int pledgeClass; ///< pledge class (TODO)
	unsigned int pledgeType;  ///< pledge type (TODO)
	unsigned int cursedWeaponId;
	int clanRepScore; ///< clan reputation score or 0, if no clan
	unsigned int transformId;
	unsigned int agathionId;
	unsigned int vehicleObjectId; // O_o Gracia Final
	int hasDwarvenCraft;          // only for UserInfo
	int numCubics;
	unsigned int cubicId[4];
	unsigned int clanPrivs;       // only for UserInfo
	int inventoryLimit;           // only for UserInfo
	L2ElementalInfo elements;     // only for UserInfo
	int fame;                     // only for UserInfo
	int vitalityLevel;            // only for UserInfo
};

#endif
