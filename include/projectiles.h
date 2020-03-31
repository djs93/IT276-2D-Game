#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_entity.h"

Entity* stingerBolt_spawn(Entity* parent);
void boltMove(Entity* self);
void boltThink(Entity* self);
void boltTouch(Entity* self, Entity* other);
void boltDie(Entity* self);

void laserExplosionDie(Entity* self);

void waveMove(Entity* self);
void waveThink(Entity* self);
/*
void musicTrap_think(Entity* self);
void musicTrap_move(Entity* self);
Entity* musicTrap_spawn(Entity* parent);
*/
void homingThink(Entity* self);
Entity* slingerPellet_spawn(Entity* parent);
Entity* laserlaser_spawn(Entity* parent);
Entity* waterwave_spawn(Entity* parent);
Entity* snowwave_spawn(Entity* parent);
Entity* musicnote_spawn(Entity* parent);
Entity* explosion_spawn(Entity* parent);


#endif