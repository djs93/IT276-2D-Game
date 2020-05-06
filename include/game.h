#ifndef __GAME_H__
#define __GAME_H__

#define ENTITY_MAX 60
#define UI_MAX 15

#include "gf2d_windows.h"
#include "player.h"

typedef enum
{
	GS_MainMenu = 0,
	GS_InGameMenu = 1,
	GS_InGame = 2,
	GS_InContentEditor = 3
}GameState;

GameState state;

/**
* @brief hide the next round "go" button
*/
void hideGoButton();

/**
* @brief show the next round "go" button
*/
void showGoButton();

/**
* @brief get the cash window
* @return a pointer to the cash window
*/
Window* getCashWindow();

/**
* @brief get the life window
* @return a pointer to the life window
*/
Window* getLifeWindow();

/**
* @brief get the upgrade window
* @return a pointer to the upgrade window
*/
Window* getUpgradeWindow();

/**
* @brief set if a window preventing entity deselection has occurred
* @param state the state to set the window pressed state to
*/
void setWindowPressed(Bool state);

/**
* @brief get if a window preventing entity deselection has been clicked
* @return true if a window preventing entity deselection has been clicked, false else
*/
Bool getWindowPressed();

/**
* @brief get the current player
* @return a pointer to the current player
*/
Player* getPlayer();

/**
* @brief show the end of level reward window with specified rewards
* @param power the power obtained
* @param perk the perk obtained
*/
void showRewardWindow(TowerTypes power, PerkNumbers perk);
/**
* @brief hide the reward window
*/
void hideRewardWindow();
/**
* @brief show the game over window
*/
void showGameOver();
/**
* @brief hide the game over window
*/
void hideGameOver();
/**
* @brief set the round ui label to the specified round PLUS ONE
* @param round the round index to display
* @note the displayed value will be round + 1
*/
void setRoundUI(int round);

/**
* @brief reload the prices on the tower selection ui
*/
void reloadPrices();

/**
* @brief get the main menu window
* @return a pointer to the main menu window
*/
Window* getMainMenuWindow();

/**
* @brief toggle the in-game gui on and off
*/
void toggleInGameGUI();
#endif