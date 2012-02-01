#pragma once

class DistancePriorities
{
public:
	static const int MAXN = 10; // maximum of 10 ranges
public:
	DistancePriorities();
	~DistancePriorities();

public:
	void initDefaultPriorities();
	void removeAll();
	int  getPriorityForDistance( double dist ) const;
	int  addRangePri( int min, int max, int pri_mod );
	void getRangeInfo( int range_idx, int *min, int *max, int *pri_mod );

protected:
	int  m_min[MAXN];
	int  m_max[MAXN];
	int  m_pri_mod[MAXN];
};
