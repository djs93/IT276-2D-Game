#include "level.h"
#include "gf2d_sprite.h"
#include "simple_json.h"

static Level* LOADED_LEVEL;

Level* get_loaded_level() {
	return LOADED_LEVEL;
}

Level* level_new(char* backgroundFile)
{
	Level* level;
	level = (Level*)gfc_allocate_array(sizeof(Level), 1);
	if (backgroundFile) {
		level->background = gf2d_sprite_load_image(backgroundFile);
	}
	return level;
}

Level* level_load(char* levelFile)
{
	return NULL;
}

void level_save(char* fileName, Level* level) {
	SJson* file = sj_object_new();
	if (level->background) {
		SJson* spriteFile = sj_new_str(level->background->filepath);
		sj_object_insert(file, "spriteFile", spriteFile);
	}
	//add path saving
	char* fileName = malloc(strlen("levels/") + strlen(fileName) + strlen(".json") + 1);
	strcpy(fileName, "mobs/");
	strcat(fileName, fileName);
	strcat(fileName, ".json");
	sj_save(file, fileName);
	free(fileName);
}
