#include "stdafx.h"
#include "DistPri.h"

DistancePriorities::DistancePriorities()
{
	removeAll();
}

DistancePriorities::~DistancePriorities()
{
	removeAll();
}

int DistancePriorities::getPriorityForDistance( double dist ) const
{
	if( dist > 10000.0 ) return 0; // too far
	int idist = (int)dist, i=0, last_pri_mod = -1;
	for( i=0; i<MAXN; i++ )
	{
		if( (m_min[i] >= 0) && (m_max[i] >= 0) )
		{
			last_pri_mod = m_pri_mod[i]; // remember last priority modifier
			if( (m_min[i] <= idist) && (idist < m_max[i]) )
				return m_pri_mod[i];
		}
	}
	// if we still here, no range found for dist
	// return latest priority from list
	return last_pri_mod;
}

void DistancePriorities::removeAll()
{
	int i;
	for( i=0; i<MAXN; i++ )
	{
		m_min[i] = m_max[i] = -1;
		m_pri_mod[i] = 0;
	}
}

int DistancePriorities::addRangePri( int min, int max, int pri_mod )
{
	if( min<0 || max<=min ) return -1; // invalid parameters
	int i=0, idx = -1;
	for( i=0; i<MAXN; i++ )
	{
		if( m_min[i] == -1 )
		{
			idx = i;
			break;
		}
	}
	if( idx == -1 ) return -1;
	m_min[idx] = min;
	m_max[idx] = max;
	m_pri_mod[idx] = pri_mod;
	return idx;
}

void DistancePriorities::initDefaultPriorities()
{
	removeAll();
	addRangePri( 0, 400, 50 );     // [0   .. 400)
	addRangePri( 400, 600, 30 );   // [400 .. 600)
	addRangePri( 600, 1000, 10 );  // [600 .. 1000)
}

void DistancePriorities::getRangeInfo( int range_idx, int *min, int *max, int *pri_mod )
{
	if( (range_idx>=0) && (range_idx<MAXN) )
	{
		if( min ) (*min) = m_min[range_idx];
		if( max ) (*max) = m_max[range_idx];
		if( pri_mod ) (*pri_mod) = m_pri_mod[range_idx];
	}
}
