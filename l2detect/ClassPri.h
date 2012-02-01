#pragma once

#ifndef L2MaxClasses  // if L2Data.h from L2Packets.h is not included...
#define L2MaxClasses 137 // >= Kamael
#endif

class ClassPriorities
{
public:
	ClassPriorities();
	~ClassPriorities();

public:
	void initDefaultPriorityList();
	void setPriForClass( int classId, int pri );
	int  getPriForClass( int classId ) const;

protected:
	int m_pri[L2MaxClasses];
};
