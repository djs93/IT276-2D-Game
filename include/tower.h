#ifndef __TOWER_H__
#define __TOWER_H__

#include "gf2d_entity.h"

typedef enum
{
	TT_Stinger,
	TT_Slingshot,
	TT_Laser,
	TT_Water,
	TT_Techno,
	TT_Snowglobe,
	TT_Music
}TowerTypes;

Entity* stinger_spawn(Vector2D position);
Entity* slingshot_spawn(Vector2D position);
Entity* laser_spawn(Vector2D position);
Entity* water_spawn(Vector2D position);
Entity* techno_spawn(Vector2D position);
Entity* snowglobe_spawn(Vector2D position);
Entity* music_spawn(Vector2D position);

Entity* placement_spawn(TowerTypes type);

void stinger_think(Entity* self);
void slingshot_think(Entity* self);
void laser_think(Entity* self);
void water_think(Entity* self);
void techno_think(Entity* self);
void snowglobe_think(Entity* self);
void music_think(Entity* self);

void placement_think(Entity* self);
void placement_detach();

#endif