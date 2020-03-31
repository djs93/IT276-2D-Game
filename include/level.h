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
	int lives; /**<How many lives the player has*/
	List* roundTimings; /**<How long after each enemy to spawn the next (in milliseconds) (List of ints)*/
	List* roundEnemies; /**<What to spawn after the last enemy (List of EnemyTypes)*/
	Bool roundOver; /**<If the current round is over or not*/
}Level;

Level* get_loaded_level();

Level* level_new(char* backgroundFile);

void level_save(char* fileName);
Level* level_load(char* levelFile);

Bool pathCollision(Entity* entity);

Path2D* nextPath();

void do_collisions();

/**
* @brief Modifies player cash and sets UI accordingly
* @param amount The amount to add or subtract from the player
*/
void level_addCash(float amount);

/**
* @brief Updates the level, spawns new enemies
*/
void level_update();

/**
* @brief Starts the levels next round if the previous is over
*/
void level_start_round();

#endif