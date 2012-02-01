#include "stdafx.h"
#include "ClassPri.h"

ClassPriorities::ClassPriorities()
{
	int i;
	for( i=0; i<L2MaxClasses; i++ )
		m_pri[i] = 0;
}

ClassPriorities::~ClassPriorities()
{
}

void ClassPriorities::initDefaultPriorityList()
{
	int i;
	for( i=0; i<L2MaxClasses; i++ ) setPriForClass( i, 10 );
	setPriForClass( 97, 500 );
	setPriForClass( 105, 400 );
	setPriForClass( 16, 200 );
	setPriForClass( 30, 100 );
}

void ClassPriorities::setPriForClass( int classId, int pri )
{
	if( classId<0 || (classId>=L2MaxClasses) || (pri<0) ) return;
	if( classId>=58 && classId<=87 ) return; // unused intreval 1
	if( classId>=119 && classId<=122 ) return; // unused intreval 1
	m_pri[classId] = pri;
}

int ClassPriorities::getPriForClass( int classId ) const
{
	if( classId<0 || (classId>=L2MaxClasses) ) return -1;
	if( classId>=58 && classId<=87 ) return -1; // unused intreval 1
	if( classId>=119 && classId<=122 ) return -1; // unused intreval 1
	return m_pri[classId];
}
