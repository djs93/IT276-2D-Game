#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_sprite.h"
#include "gfc_list.h"
#include "gf2d_entity.h"

typedef struct Level_S{
	Sprite* background; /**<The background for the level*/
	List* paths; /**<The paths for enemies to follow. Must be at least one*/
	List* collisionPaths; /**<The lines for stopping towers from being placed on paths. Not paths, just a lotta lines. Made automatically*/
	List* optimalBuckets; /**<Buckets used for extremely efficient tower enemy checking*/
	List* allyBuckets; /**<Buckets containing allies for support tower checking*/
	float pathDistance; /**<Minimum distance player can place towers away from path*/
	List* needUpdating; /**<Entities needing to have their buckets updated. Made automatically*/
}Level;

Level* get_loaded_level();

Level* level_new(char* backgroundFile);

void level_save(char* fileName);
Level* level_load(char* levelFile);

Bool pathCollision(Entity* entity);

#endif