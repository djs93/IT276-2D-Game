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

Entity* redSpawn();
Entity* blueSpawn();
Entity* greenSpawn();
Entity* yellowSpawn();
Entity* superSpawn();

void redThink(Entity* self);
void blueThink(Entity* self);
void greenThink(Entity* self);
void yellowThink(Entity* self);
void superThink(Entity* self);

void redMove(Entity* self);
void blueMove(Entity* self);
void greenMove(Entity* self);
void yellowMove(Entity* self);
void superMove(Entity* self);

Entity* redDie(Entity* self);
Entity* blueDie(Entity* self);
Entity* greenDie(Entity* self);
Entity* yellowDie(Entity* self);
Entity* superDie(Entity* self);

#endif