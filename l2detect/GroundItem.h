#ifndef H_GROUNDITEM
#define H_GROUNDITEM

class GroundItem: public L2Object
{
public:
	GroundItem() { setUnused(); }
	~GroundItem() { setUnused(); }
public:
	virtual void setUnused();
public:
	unsigned int           itemID;
	int                    stackable;
	unsigned long long int count;
};

#endif
