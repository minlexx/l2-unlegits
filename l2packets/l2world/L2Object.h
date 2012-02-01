#ifndef H_L2OBJECT
#define H_L2OBJECT

#include "../L2_versions.h"

/** \class L2Object
Every object has its objectID and can also have its coordinates in L2World
*/

class L2Object
{
public:
	/** L2Object default constructor
	 */
	L2Object(): x(0), y(0), z(0), objectID(0) {}

	/** L2Object copy constructor
	 * \param other copy source
	 */
	L2Object( const L2Object& other );

	/** Assigns this->x,y,z,objectID to other's x,y,z,objectID
	 * \param other copy source
	 * \return reference to this object
	 */
	virtual const L2Object& operator=( const L2Object& other );

	/** Compares objectIDs
	 * \param other object to compare with
	 * \return true, if objects are equal (equal objectIDs)
	 */
	virtual bool operator==( const L2Object& other );

	/** Destructor calls setUnused()
	* \see setUnused
	*/
	virtual ~L2Object();
public:
	/** Sets object coordinates in world
	* \param sx, sy, sz - new coordinates
	* \return none
	*/
	virtual void setXYZ( int sx, int sy, int sz ) { x = sx; y = sy; z = sz; }
public:
	/** Test objectID
	* \return 1, if objectID == 0; 0 if objectID is nonzero.
	*/
	virtual int  isUnused() const { return (objectID == 0); }
	/** Initializes object's state, zeroes all members (sets coords to 0, objectID to 0)
	* \return none
	*/
	virtual void setUnused() { objectID = 0; x = 0; y = 0; z = 0; }
public:
	/** object's X coordinate */
	int x;
	/** object's Y coordinate */
	int y;
	/** object's Z coordinate */
	int z;
	/** objectID is unique in game world */
	unsigned int objectID;
};

#endif
