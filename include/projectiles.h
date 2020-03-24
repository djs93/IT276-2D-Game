#ifndef __PROJECTILES_H__
#define __PROJECTILES_H__

#include "gf2d_entity.h"

Entity* stingerBolt_spawn(Entity* parent);
void boltMove(Entity* self);
void boltThink(Entity* self);
void boltTouch(Entity* self, Entity* other);
void boltDie(Entity* self);

void waveMove(Entity* self);
void waveThink(Entity* self);

Entity* slingerPellet_spawn(Entity* parent);
Entity* laserlaser_spawn(Entity* parent);
Entity* waterwave_spawn(Entity* parent);

#endif