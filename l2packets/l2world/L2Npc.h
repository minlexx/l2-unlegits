#ifndef H_L2NPC
#define H_L2NPC
#include "L2Character.h"

/** \class L2Npc
Every L2Npc is L2Character:\n
Every L2Character is L2Object (has objectID, x, y, z)\n
Every L2Character has LEVEL!\n
Every L2Character has its charName and charTitle\n
Every L2Character has its heading\n
Every L2Character has moving speed in states: running, walking\n
Every L2Character has its move destination: xDst, yDst, zDst\n
Every l2Character can calc its position based on destnation point and time passed since last calc\n
Every L2Character has its base stats: INT WIT MEN CON STR DEX\n
Every L2Character has its stats: pAtk, mAtk, ..., curHp, maxHp, ...\n
Every L2Character has abnormalEffect\n
Every L2Character has its target\n
Every L2Character can be in combat\n
Every L2Character has its collision radius and height\n
\n
Additionally:\n
Every L2Npc has its NPC templateID\n
Every L2Npc can be attackable or not (attackable is mob, other is NPC. chooses default action on click)\n
Every L2Npc has weapon/shield or bow/arrow in left/right hand\n
*/

class L2Npc : public L2Character
{
public:
	/** Default constructor calls setUnused() (zeroes all members) */
	L2Npc();

	/** Copy constructor
	 * \param other source to copy from
	 */
	L2Npc( const L2Npc& other );

	/* Assigns all members of L2Npc class instance to other
	 * \param other source to copy from
	 * \return reference to this object */
	virtual const L2Npc& operator=( const L2Npc& other );

	/** Default destructor calls setUnused() (zeroes all members) */
	virtual ~L2Npc();

public:
	/** Initializes object's state (zeroes all members)
	* \return none
	*/
	virtual void setUnused();

public:
	/** Query database for name and title, using this object's templateID, and set them */
	virtual void setNpcNameTitleByTemplate();

public:
	bool parse_NpcInfo( void *l2_game_packet, L2_VERSION l2_version );

public:
	unsigned int templateID; ///< NPC template ID
	int isAttackable; ///< =1, if npc is auto attackable on double click (mob); 0 if double click begins chat with NPC (NPC)
	unsigned int iid_left_hand; ///< item ID of left hand weapon item
	unsigned int iid_chest; ///< item ID of chest (?)
	unsigned int iid_right_hand; ///< item ID of right hand weapon item
};

#endif
