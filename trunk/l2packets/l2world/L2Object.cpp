#include "stdafx.h"
#include "L2Object.h"

L2Object::~L2Object()
{
	setUnused();
}

L2Object::L2Object( const L2Object& other )
{
	this->operator=( other );
}

const L2Object& L2Object::operator=( const L2Object& other )
{
	if( this == &other ) return (*this);
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->objectID = other.objectID;
	return (*this);
}

bool L2Object::operator==( const L2Object& other )
{
	return this->objectID == other.objectID;
}
