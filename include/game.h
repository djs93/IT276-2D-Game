#ifndef __GAME_H__
#define __GAME_H__

#define ENTITY_MAX 60
#define UI_MAX 15

#include "gf2d_windows.h"

typedef enum
{
	GS_MainMenu = 0,
	GS_InGameMenu = 1,
	GS_InGame = 2,
	GS_InContentEditor = 3
}GameState;

GameState state;
void mainMenuToLevelOne();
void mainMenuToContentEditor();
int loadLevel(char* levelFile);
void toMainMenu();

Window* getCashWindow();
Window* getUpgradeWindow();

#endif