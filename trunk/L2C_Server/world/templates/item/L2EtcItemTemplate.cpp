#include "pch.h"
#include "L2EtcItemTemplate.h"
#include "l2c_utils.h"

L2EtcItemTemplate::L2EtcItemTemplate( L2ItemSubType subType, StatsSet& set ):
	L2ItemTemplate( TYPE_ETCITEM, subType, set )
{
	set.getString( "handler", m_handler );
}

L2EtcItemTemplate::~L2EtcItemTemplate()
{
}
