#ifndef H_MAPSHOWPARAMS
#define H_MAPSHOWPARAMS

class st_MapShowParams
{
public:
	void init();
public:
	// map checks
	int  zrange;
	int  mobs;
	int  npcs;
	int  pcs;
	int  items;
	int  vectors;
	int  fightrange;
	int  realmap;
	int  sellshop;
	int  craftshop;
	int  buyshop;
};

#endif
