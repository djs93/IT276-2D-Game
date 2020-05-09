#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_sprite.h"
#include "gfc_list.h"
#include "gf2d_entity.h"

typedef struct Level_S{
	Sprite* background; /**<The background for the level*/
	List* paths; /**<The paths for enemies to follow. Must be at least one*/
	int currPath; /**<Keeps track of what path to spawn the next enemy*/
	List* collisionPaths; /**<The lines for stopping towers from being placed on paths. Not paths, just a lotta lines. Made automatically*/
	List* optimalBuckets; /**<Buckets used for extremely efficient tower enemy checking*/
	List* allyBuckets; /**<Buckets containing allies for support tower checking*/
	float pathDistance; /**<Minimum distance player can place towers away from path*/
	List* needUpdating; /**<Entities needing to have their buckets updated. Made automatically*/
	int playerHealth; /**<How much health the player has*/
	float playerCash; /**<How much cash the player has*/
	int round; /**<What round we're on*/
	float spawnCooldown; /**<How long until spawning the next enemy*/
	int currentEnemy; /**<Current enemy spawned (index)*/
	List* roundTimings; /**<How long after each enemy to spawn the next (in milliseconds) (List of ints)*/
	List* roundEnemies; /**<What to spawn after the last enemy (List of EnemyTypes)*/
	Bool roundOver; /**<If the current round is over or not*/
	int regenPerRound; /**<How much life to regenerate each round*/
	char* fileName; /**<Level's filename*/
	char* nextLevel; /**<File name of next level to load*/
}Level;

/**
* @brief Get the currently loaded level
* @return A pointer to the current level
*/
Level* get_loaded_level();

/**
* @brief Create a new empty level with a specified background
* @param backgroundFile the background to the load with the empty level
* @return A pointer to the new empty level
*/
Level* level_new(char* backgroundFile);

/**
* @brief Save the level's state. This includes, health, cash, round, and towers placed
* @param fileName the name for the level save file
* @note name the save "saves/level.json" to auto-load when the game opens
*/
void level_save(char* fileName);

/**
* @brief Load a base level from a level file. This does NOT include towers, health, or cash
* @param levelFile the file for the level. Outlines path width, paths, and background.
* @return A pointer to the level loaded
*/
Level* level_load(char* levelFile);

/**
* @brief Initialize the loaded level. Adds cash and lives
*/
void level_init();

/**
* @brief Load and initialize a level
* @param levelName the path to the file
*/
void level_load_new(char* levelName);

/**
* @brief Reload the currently loaded level
*/
void level_reload();

/**
* @brief Reload the currently loaded level with one life and 10000 cash
*/
void level_reload_test();

/**
* @brief Load a level from a level save. This does includes towers, health, round, and cash
* @param levelSaveFile the file for the level save. Outlines towers, health, round, and cash as well as the base level file to load.
* @return A pointer to the level loaded
*/
Level* level_load_from_save(char* levelSaveFile);

/**
* @brief Checks if an entity collides with any of the paths
* @param entity the entity to check path collisions
* @return True if the entity collides with any of the loaded level's paths
*/
Bool pathCollision(Entity* entity);

/**
* @brief Return the level's next available spawning path. Round-robin
* @return A pointer to the path
*/
Path2D* nextPath();

/**
* @brief Runs collisions for every entity
*/
void do_collisions();

/**
* @brief Modifies player cash and sets UI accordingly
* @param amount The amount to add or subtract from the player
*/
void level_addCash(float amount);

/**
* @brief Modifies player life and sets UI accordingly
* @param amount The amount of life to add or subtract from the player
*/
void level_addLife(int amount);

/**
* @brief Updates the level, spawns new enemies
*/
void level_update();

/**
* @brief Starts the levels next round if the previous is over
*/
void level_start_round();

/**
* @brief Modifies regen of life per round
* @param amount The amount of life to additionally restore each round
*/
void level_addRegen(int amountPerRound);

/**
* @brief Loads the next level
*/
void level_loadNext();

/**
* @brief calculate the boundaries for a path
* @param data the path to generate lines for
* @param context the Level* level to generate lines for
*/
void calcPathBoundaries(void* data, void* context);

#endif