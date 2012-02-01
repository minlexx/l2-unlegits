#ifndef H_L2CHARACTER
#define H_L2CHARACTER
#include "L2Object.h"

/** \class L2Character

L2Character represents common properties for all moving objects in game:\n
treasure chests, doors, chairs, mobs, npcs, players...\n
\n
Every character is L2Object:\n
Every L2Object has its objectID and can also have its coordinates in L2World)\n
\n
Additionally:\n
Every L2Character has LEVEL!\n
Every L2Character has its charName and charTitle\n
Every L2Character has its heading\n
Every L2Character has moving speed in states: running, walking\n
Every L2Character has its move destination: xDst, yDst, zDst\n
Every L2Character can calc its position based on destnation point and time passed since last calc\n
Every L2Character has its base stats: INT WIT MEN CON STR DEX\n
Every L2Character has its stats: pAtk, mAtk, ..., curHp, maxHp, ...\n
Every L2Character has abnormalEffect\n
Every L2Character has its target\n
Every L2Character can be in combat\n
Every L2Character has its collision radius and height\n
Every L2Character has its pvpFlag, karma\n
Every L2Character has its clan, ally IDs and clan/ally crest IDs\n
*/

class L2Character : public L2Object
{
public:
	/** Default constructor, just calls setUnused() (zeroes all members) */
	L2Character();

	/** Copy constructor
	 * \param other source to copy from */
	L2Character( const L2Character& other );

	/** Operator =
	 * \param other source to copy from
	 * \return reference to this object
	 */
	virtual const L2Character& operator=( const L2Character& other );

	/** Default destructor, just calls setUnused() (zeroes all members) */
	virtual ~L2Character();

public:
	/** Initializes object's state (zeroes all members)
	* \return none
	*/
	virtual  void setUnused();

public:
	/** MoveToLocation packet parser.
	* MoveToLocation packet is sent when some object starts moving to some point.
	* Updates x,y,z, xDst,yDst,zDst.
	* Updates lastMoveTickTime (sets to current time).
	* \param l2_game_packet pointer to L2GamePacket object to parse
	* \return none
	* \see lastMoveTickTime
	*/
	virtual  bool parse_MoveToLocation( void *l2_game_packet );

	/** Calculates current character position based on x, y, z, xDst, yDst, zDst,
	* runSpeed / walkSpeed, isRunning, current time and last time when character coords were
	* known exactly (from some packet or after previous calculation).
	* Updates x,y,z and lastMoveTickTime (sets to current time).
	* \return none
	* \see lastMoveTickTime
	*/
	virtual  void processMoveTick();

	/** Starts 'moving' character: sets destination coords and current coords.
	* Updates x,y,z,xDst,yDst,zDst,lastMoveTickTime.
	* \param mxd,myd,mzd destination point coords
	* \param mx,my,mz current char coords
	* \return none
	* \see lastMoveTickTime
	*/
	virtual  void startMoveTo( int mxd, int myd, int mzd, int mx, int my, int mz );

	/** sets xDst,yDst,zDst to 0x7FFFFFFF, indicating that character is not moving.
	* \return none
	* \see xDst,yDst,zDst
	*/
	virtual  void stopMove() { xDst = 0x7FFFFFFF; yDst = 0x7FFFFFFF; zDst = 0x7FFFFFFF; }

	/** moving test
	* \return 1 if char is moving, 0 if not
	* \see xDst,yDst,zDst
	*/
	virtual  int  isMoving() { return ((xDst != 0x7FFFFFFF) && (yDst != 0x7FFFFFFF) && (zDst!=0x7FFFFFFF)); }

public:
	/** Sets char name.
	* \param name new char name to set.
	*/
	virtual  void setName( const wchar_t *name );
	virtual  void setName( const char *name );

	/** Sets char title.
	* \param title new char title to set.
	*/
	virtual  void setTitle( const wchar_t *title );
	virtual  void setTitle( const char *title );

	virtual  const wchar_t *getName(); //< not really used
	virtual  const wchar_t *getTitle(); //< not really used

public:
	int level; ///< character level
	wchar_t charName[128]; ///< character name
	wchar_t charTitle[128]; ///< character title (displayed above name in client ^^)
	int xDst; ///< x-destination move coord (=0x7FFFFFFF if char is not moving)
	int yDst; ///< y-destination move coord (=0x7FFFFFFF if char is not moving)
	int zDst; ///< z-destination move coord (=0x7FFFFFFF if char is not moving)
	unsigned int heading; ///< heading... not really used
	int runSpeed; ///< run speed
	int walkSpeed; ///< walk speed
	int isRunning; ///< 1, if char is runing, 0 if walking
	int isSitting; ///< 1, if char is sitting, 0 if standing
	int isAlikeDead; ///< 1, if characted is dead or looks like dead (fake death)
	int isFlying; ///< 1, if character is flying (?) T2.3
	unsigned long long int lastMoveTickTime; ///< timer used to store last time when character coords were known exactly
	int s_STR; ///< STR base stat
	int s_DEX; ///< DEX base stat
	int s_CON; ///< CON base stat
	int s_INT; ///< INT base stat
	int s_WIT; ///< WIT base stat
	int s_MEN; ///< MEN base stat
	int pAtk;
	int pDef;
	int mAtk;
	int mDef;
	int pAtkSpd;
	int mAtkSpd;
	int accuracy;
	int evasion;
	int critical;
	double curHp; ///< current HP value
	double maxHp; ///< maximum HP value
	double curMp; ///< current MP value
	double maxMp; ///< maximum MP value
	double curCp; ///< current CP value
	double maxCp; ///< maximum CP value
	unsigned int abnormalEffect; ///< flags for visible buffs/defuffs: poison, medusa, bleed, ...
	unsigned int targetObjectID; ///< objectID of object which is targeted by this character
	int isInCombat; ///< 1, if char is in combat state, 0 if not
	double collisionRadius;
	double collisionHeight;
	int pvpFlag;
	int karma;
	unsigned int clanID;
	unsigned int clanCrestID;
	unsigned int clanCrestLargeID;
	unsigned int allyID;
	unsigned int allyCrestID;
};

#endif
