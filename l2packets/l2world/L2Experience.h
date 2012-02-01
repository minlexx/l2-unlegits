#ifndef H_L2_EXPERIENCE_
#define H_L2_EXPERIENCE_

/** \class L2Experience
* Some functions and constants to work with player exp values.
*/

class L2Experience
{
public:
	/**
	* Converts exp value to current exp percent.
	* \param exp current experience value
	* \return precent value in current level
	*/
	static double getExpPercent( long long exp );
	
public:
	/**
	 * This is the first UNREACHABLE level.<BR>
	 *   ex: If you want a max at 85 & 100.00%, you have to put 86.<BR><BR>
	 */
	static const int MAX_LEVEL = 86;
	static const int MIN_NEWBIE_LEVEL = 6;
	static const int MAX_NEWBIE_LEVEL = 39;
};

#endif
