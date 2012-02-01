#pragma once
#include "L2ItemTemplate.h"

class L2EtcItemTemplate: public L2ItemTemplate
{
public:
	L2EtcItemTemplate( L2ItemSubType subType, StatsSet& set );
	virtual ~L2EtcItemTemplate();

public:
	const char *getHandler() const { return m_handler.c_str(); }

protected:
	std::string m_handler;
	// TODO: skills?
};
