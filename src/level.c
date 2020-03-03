#include "level.h"
#include "gf2d_sprite.h"
#include "simple_json.h"
#include "simple_logger.h"

static Level* LOADED_LEVEL;

void paths_save(void* data, void* context);

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
	level->paths = gfc_list_new();
	level->allyBuckets = gfc_list_new();
	level->optimalBuckets = gfc_list_new();
	return level;
}

Level* level_load(char* levelFile)
{
	SJson* levelJson;
	SJson* background;
	SJson* paths;
	SJson* currPath;
	char* backgroundName;
	Level* level;
	levelJson = sj_load(levelFile);
	if (!levelJson) {
		slog("Level json %s not found!", levelFile);
		return level_new("images/backgrounds/bg_flat.png");
	}
	background = sj_object_get_value(levelJson, "background");
	if (!background) {
		slog("No background key found in level file! Defaulting...");
		backgroundName = "images/backgrounds/bg_flat.png";
	}
	else {
		backgroundName = sj_get_string_value(background);
		if (!backgroundName) {
			slog("No string found after background key in level file! Defaulting...");
			backgroundName = "images/backgrounds/bg_flat.png";
		}
	}
	level = level_new(backgroundName);

	paths = sj_object_get_value(levelJson, "paths");
	if (!paths) {
		slog("No paths key found in level file! Defaulting...");
		
	}
	


	sj_free(levelJson);
	sj_free(background);
	sj_free(paths);
	sj_free(currPath);

	LOADED_LEVEL = level;
	return level;
}

void level_save(char* fileName, Level* level) {
	SJson* file = sj_object_new();
	if (level->background) {
		SJson* spriteFile = sj_new_str(level->background->filepath);
		sj_object_insert(file, "spriteFile", spriteFile);
	}
	if (level->paths) {
		SJson* paths = sj_array_new();
		gfc_list_foreach(level->paths, paths_save, paths);
	}
	//add path saving
	char* saveFileName = malloc(strlen("levels/") + strlen(fileName) + strlen(".json") + 1);
	strcpy(saveFileName, "mobs/");
	strcat(saveFileName, fileName);
	strcat(saveFileName, ".json");
	sj_save(file, saveFileName);
	free(saveFileName);
}

void paths_save(void* data, void* context) {

}
