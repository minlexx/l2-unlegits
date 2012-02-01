#ifndef L2SKILL_H_
#define L2SKILL_H_

/** \class L2Skill
* Information about skill: skill ID, skill level and active/passive type.
*/

class L2Skill
{
public:
	L2Skill(): isPassive(1), id(0), level(0) {}
	~L2Skill() {}
public:
	int isPassive; ///< 1, if is passive; 0 otherwise
	unsigned int id; ///< skill ID
	unsigned int level; ///< skill level
};

#endif
