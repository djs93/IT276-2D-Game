#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <gf2d_sprite.h>
#include <gfc_list.h>

typedef struct Level_S{
	Sprite* background; /**<The background for the level*/
	List* paths; /**<The paths for enemies to follow. Must be at least one*/
	List* buckets; /**<Buckets used to store entities for faster distance and collision checks*/
	List* optimalBuckets; /**<Buckets used for extremely efficient tower enemy checking*/
}Level;

Level* get_loaded_level();

Level* level_new(char* backgroundFile);

void level_save(char* fileName);
Level* level_load(char* levelFile);

#endif