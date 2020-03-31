#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "tower.h"

typedef struct
{
	int powerInventory[4];	/**<How many of each power the player has*/
	Bool perks[5];			/**<If the player has one of the 5 perks*/
}Player;

typedef enum
{
	PN_Pierce = 0,
	PN_Speed = 1,
	PN_Discount = 2,
	PN_Money = 3,
	PN_StingerUps = 4
}PerkNumbers;

void player_addPerk(PerkNumbers perkNum);
void player_consume(TowerTypes powerType);

/**
* @brief Loads a player from a player save file
* @param saveFile the file to load from
* @return Player specified by saveFile
* @note Returns a new, empty player on invalid saveFile
*/
Player* player_load(char* saveFile);

#endif