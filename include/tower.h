#ifndef __TOWER_H__
#define __TOWER_H__

#include "gf2d_entity.h"
#include "gf2d_windows.h"
#include "gf2d_element_label.h"
#include "gf2d_element_list.h"

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


List* inRange(Entity* self);
void techno_damage(Entity* self, Entity* target);
void setSeekBuckets(Entity* self);

Entity* stinger_spawn(Vector2D position);
Entity* slingshot_spawn(Vector2D position);
Entity* laser_spawn(Vector2D position);
Entity* water_spawn(Vector2D position);
Entity* techno_spawn(Vector2D position);
Entity* snowglobe_spawn(Vector2D position);
Entity* music_spawn(Vector2D position);

void stinger_buy();
void slingshot_buy();
void laser_buy();
void water_buy();
void techno_buy();
void snowglobe_buy();
void music_buy();

Entity* placement_spawn(TowerTypes type);

void stinger_think(Entity* self);
void slingshot_think(Entity* self);
void laser_think(Entity* self);
void water_think(Entity* self);
void techno_think(Entity* self);
void snowglobe_think(Entity* self);
void music_think(Entity* self);

void placement_think(Entity* self);
void placement_detach(Entity* ent);

void setPrices(Window* window);
int getPrice(TowerTypes type);

char* getUpgradeOneDesc(Entity* tower);
char* getUpgradeTwoDesc(Entity* tower);
int getUpgradeOnePrice(Entity* tower);
int getUpgradeTwoPrice(Entity* tower);

void upgradeOne_buy();
void upgradeTwo_buy();

#endif