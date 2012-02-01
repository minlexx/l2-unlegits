#ifndef H_USER_STORAGE_MAX_COUNT
#define H_USER_STORAGE_MAX_COUNT

class UserStorageMaxCount
{
public:
	UserStorageMaxCount() {}
	~UserStorageMaxCount() {}
public:
	void parse_ExStorageMaxCount( void *l2_game_packet );
public:
	unsigned int inventory;
	unsigned int warehouse;
	unsigned int freight;
	unsigned int privateSell;
	unsigned int privateBuy;
	unsigned int receipeD;
	unsigned int recipe;
};

#endif
