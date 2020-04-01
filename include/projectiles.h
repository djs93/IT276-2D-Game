#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_entity.h"

/**
* @brief Spawn a slinger bolt
* @param parent the parent of the bolt. sets initial direction from rotation + 180
* @return A pointer to the spawned bolt
*/
Entity* stingerBolt_spawn(Entity* parent);

/**
* @brief Move a stinger bolt in a straight line
* @param self the stinger bolt to move
* @note used for other projectiles as well
*/
void boltMove(Entity* self);

/**
* @brief Think for a stinger bolt. Kills the bolt if  it gets to the edge of the screen or reaches the rest of its distance or uses all of its pierce
* @param self the stinger bolt to think
* @note used for other projectiles as well
*/
void boltThink(Entity* self);

/**
* @brief Defines what happens when a stinger bolt touches another entity. Damages enemies
* @param self the stinger bolt to check collisions for
* @param other the other entity to check collisions against
* @note used for other projectiles as well
*/
void boltTouch(Entity* self, Entity* other);

/**
* @brief Kills a bolt.
* @param self the stinger bolt to kill
* @note used for other projectiles as well
*/
void boltDie(Entity* self);

/**
* @brief Damages all enemies around it. Used when laser explosion upgrade triggers on bolt death.
* @param self the laser bolt to trigger the explosion on and free
*/
void laserExplosionDie(Entity* self);

/**
* @brief Dummy function. Does no movement.
* @param self the entity to NOT move
* @note used for other projectiles as well
*/
void waveMove(Entity* self);

/**
* @brief Expand a wave's radius based on its parent's attack speed and range
* @param self the wave to scale
* @note used for other projectiles as well
*/
void waveThink(Entity* self);
/*
void musicTrap_think(Entity* self);
void musicTrap_move(Entity* self);
Entity* musicTrap_spawn(Entity* parent);
*/

/**
* @brief Think for a homing bolt. Looks for a target and paths towards it, then executes normal bolt think.
* @param self the projectile to think
*/
void homingThink(Entity* self);

/**
* @brief spawn a new slinger pellet
* @param parent the parent to spawn the pellet on, cannot be null
* @return the spawned entity
*/
Entity* slingerPellet_spawn(Entity* parent);
/**
* @brief spawn a new slinger pellet
* @param parent the parent to spawn the pellet on, cannot be null
* @return the spawned entity
*/
Entity* laserlaser_spawn(Entity* parent);
/**
* @brief spawn a new water wave
* @param parent the parent to spawn the wave on, cannot be null
* @return the spawned entity
*/
Entity* waterwave_spawn(Entity* parent);
/**
* @brief spawn a new snow wave
* @param parent the parent to spawn the wave on, cannot be null
* @return the spawned entity
*/
Entity* snowwave_spawn(Entity* parent);
/**
* @brief spawn a new music note
* @param parent the parent to spawn the note on, cannot be null
* @return the spawned entity
*/
Entity* musicnote_spawn(Entity* parent);
/**
* @brief spawn a new explosion wave
* @param parent the parent to spawn the explosion on, cannot be null
* @return the spawned entity
*/
Entity* explosion_spawn(Entity* parent);
/**
* @brief spawn a new snow cannon projectile
* @param parent the parent to spawn the projectile on, cannot be null
* @return the spawned entity
*/
Entity* snowCannon_spawn(Entity* parent);


#endif