#ifndef __ENEMIES_H__
#define __ENEMIES_H__

#include "gf2d_entity.h"

typedef enum {
	ET_RED,
	ET_BLUE,
	ET_GREEN,
	ET_YELLOW,
	ET_SUPER
}EnemyTypes;

/**
* @brief spawn a new red flower
* @param parent the parent to spawnt he flower on, null is at the start of the level's next path
* @return the spawned entity
*/
Entity* redSpawn(Entity* parent);
/**
* @brief spawn a new blue flower
* @param parent the parent to spawnt he flower on, null is at the start of the level's next path
* @return the spawned entity
*/
Entity* blueSpawn(Entity* parent);
/**
* @brief spawn a new green flower
* @param parent the parent to spawnt he flower on, null is at the start of the level's next path
* @return the spawned entity
*/
Entity* greenSpawn(Entity* parent);
/**
* @brief spawn a new yellow flower
* @param parent the parent to spawnt he flower on, null is at the start of the level's next path
* @return the spawned entity
*/
Entity* yellowSpawn(Entity* parent);
/**
* @brief spawn a new super flower
* @param parent the parent to spawnt he flower on, null is at the start of the level's next path
* @return the spawned entity
*/
Entity* superSpawn(Entity* parent);

/**
* @brief red flower's think. Kills flower on <=0 health. Restores max speed after slow over.
* @param self the flower to do thinking
*/
void redThink(Entity* self);
/**
* @brief blue flower's think. Kills flower on <=0 health. Restores max speed after slow over.
* @param self the flower to do thinking
*/
void blueThink(Entity* self);
/**
* @brief green flower's think. Kills flower on <=0 health. Restores max speed after slow over.
* @param self the flower to do thinking
*/
void greenThink(Entity* self);
/**
* @brief yellow flower's think. Kills flower on <=0 health. Restores max speed after slow over.
* @param self the flower to do thinking
*/
void yellowThink(Entity* self);
/**
* @brief super flower's think. Kills flower on <=0 health. Restores max speed after slow over.
* @param self the flower to do thinking
*/
void superThink(Entity* self);

/**
* @brief All flowers' move function. Handles redirection on path.
* @param self the flower to do moving
*/
void flowerMove(Entity* self);

/**
* @brief kill a red flower
* @param self the flower to kill
* @return NULL
*/
Entity* redDie(Entity* self);

/**
* @brief kill a blue flower
* @param self the flower to kill
* @return the red flower spawned on death
*/
Entity* blueDie(Entity* self);

/**
* @brief kill a green flower
* @param self the flower to kill
* @return the blue flower spawned on death
*/
Entity* greenDie(Entity* self);

/**
* @brief kill a yellow flower
* @param self the flower to kill
* @return the yellow flower spawned on death
*/
Entity* yellowDie(Entity* self);

/**
* @brief kill a super flower
* @param self the flower to kill
* @return the yellow flower spawned on death
*/
Entity* superDie(Entity* self);

#endif