#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "tower.h"

typedef struct
{
	int powerInventory[4];	/**<How many of each power the player has*/
	Bool perks[5];			/**<If the player has one of the 5 perks*/
	int perkMoney;			/**<Currency used to buy perks*/
}Player;

typedef enum
{
	PN_Pierce = 0,
	PN_Speed = 1,
	PN_Discount = 2,
	PN_Money = 3,
	PN_StingerUps = 4
}PerkNumbers;

/**
* @brief add's a perk to the player's inventory
* @param perkNum perk to add to inventory
*/
void player_addPerk(PerkNumbers perkNum);

/**
* @brief subtract a power from the player's inventory
* @param powerType power to add to inventory
*/
void player_consume(TowerTypes powerType);

/**
* @brief Loads a player from a player save file
* @param saveFile the file to load from
* @return Player specified by saveFile
* @note Returns a new, empty player on invalid saveFile
*/
Player* player_load(char* saveFile);

/**
* @brief Saves the current player
* @param saveFile the file to save to
*/
void player_save(char* saveFile);

/**
* @brief Gets the cost of a perk
* @param perkNumber the perk to get the price of
* @return The cost of the perk, -1 if non-valid perk number
*/
int get_perk_cost(PerkNumbers perkNumber);

#endif