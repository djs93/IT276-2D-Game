#include "level.h"
#include "gf2d_sprite.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "Geometry2D.h"
#include "bucket.h"
#include "gf2d_windows.h"
#include "game.h"
#include "gf2d_element_button.h"
#include "gf2d_elements.h"
#include "gf2d_graphics.h"
#include <enemies.h>

static Level* LOADED_LEVEL;

void paths_save(void* data, void* context);
void calcPathBoundaries(void* data, void* context);
void calcBoundaryLines(void* data, void* context);
void calcOptimalBuckets(void* data, void* context); 
List* getRoundEnemies(int round);
List* getRoundTimings(int round);
void endRound();
Bool enemiesExist();
void spawnNext();

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
	level->collisionPaths = gfc_list_new();
	level->allyBuckets = gfc_list_new();
	level->optimalBuckets = gfc_list_new();
	level->needUpdating = gfc_list_new();
	return level;
}

Level* level_load(char* levelFile)
{
	SJson* levelJson;
	SJson* background;
	SJson* paths;
	SJson* currPath;
	SJson* currPoint;
	SJson* nextPoint;
	SJson* tempJson;
	Path2D* currPathPath;
	Path2D currPathPathPath;
	List* lines;
	List* pathsList;
	Line2D currLine;
	Line2D* currLineLine;
	char* backgroundName;
	Level* level;
	int i;
	int j;
	int k;
	float pathDist;

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
		lines = gfc_list_new();
		currLine = line2d(point2d(0, 0), point2d(1200, 720));
		currLineLine = gfc_allocate_array(sizeof(Line2D), 1);
		currLineLine->end = currLine.end;
		currLineLine->start = currLine.start;
		lines = gfc_list_append(lines, currLineLine);
		currPathPath = gfc_allocate_array(sizeof(Path2D), 1);
		currPathPathPath = path2d(lines);
		currPathPath->end = currPathPathPath.end;
		currPathPath->start = currPathPathPath.start;
		currPathPath->lines = currPathPathPath.lines;
		currPathPath->totalLength = currPathPathPath.totalLength;
		pathsList = gfc_list_new();
		pathsList = gfc_list_append(pathsList, currPathPath);
		level->paths = pathsList;
	}
	else {
		pathsList = gfc_list_new();
		for (i = 0; i < sj_array_get_count(paths); i++) {
			currPath = sj_array_get_nth(paths, i);
			lines = gfc_list_new();
			for (j = 0; j < sj_array_get_count(currPath) - 1; j++) {
				currPoint = sj_array_get_nth(currPath, j);
				nextPoint = sj_array_get_nth(currPath, j + 1);
				currLineLine = gfc_allocate_array(sizeof(Line2D), 1);
				sj_get_integer_value(sj_array_get_nth(currPoint, 0), &k);
				currLineLine->start.x = k;
				sj_get_integer_value(sj_array_get_nth(currPoint, 1), &k);
				currLineLine->start.y = k;
				sj_get_integer_value(sj_array_get_nth(nextPoint, 0), &k);
				currLineLine->end.x = k;
				sj_get_integer_value(sj_array_get_nth(nextPoint, 1), &k);
				currLineLine->end.y = k;
				lines = gfc_list_append(lines, currLineLine);
			}
			currPathPath = gfc_allocate_array(sizeof(Path2D), 1);
			currPathPathPath = path2d(lines);
			currPathPath->end = currPathPathPath.end;
			currPathPath->start = currPathPathPath.start;
			currPathPath->lines = currPathPathPath.lines;
			currPathPath->totalLength = currPathPathPath.totalLength;
			pathsList = gfc_list_append(pathsList, currPathPath);
		}
		level->paths = pathsList;
	}
	tempJson = sj_object_get_value(levelJson, "pathWidth");
	if (tempJson) {
		sj_get_float_value(tempJson, &pathDist);
		level->pathDistance = pathDist;
	}
	else {
		level->pathDistance = 15.0f;
	}
	gfc_list_foreach(pathsList, calcPathBoundaries, level);

	gfc_list_foreach(pathsList, calcOptimalBuckets, level);

	level->currPath = 0;

	//sj_free(levelJson);
	//sj_free(background);
	//sj_free(paths);
	//sj_free(currPath);

	level->roundOver = true;
	level->round = -1;
	LOADED_LEVEL = level;
	level->playerHealth = 0;
	level_addCash(100.0f);
	level_addLife(100);
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

void calcPathBoundaries(void* data, void* context) {
	gfc_list_foreach(((Path2D*)data)->lines, calcBoundaryLines, context);
}

void calcBoundaryLines(void* data, void* context) {
	float dx, dy;
	Vector2D normal1;
	Vector2D normal2;
	Line2D* border1;
	Line2D* border2;
	Level* level;
	level = context;

	border1 = gfc_allocate_array(sizeof(Line2D), 1);
	border2 = gfc_allocate_array(sizeof(Line2D), 1);
	//get normal of line
	Line2D* line = data;
	dx = line->end.x - line->start.x;
	dy = line->end.y - line->start.y;
	normal1 = vector2d(-dy, dx);
	vector2d_normalize(&normal1);
	normal2 = vector2d(dy, -dx);
	vector2d_normalize(&normal2);

	//make two lines
	//move them along normal
	border1->end.x = line->end.x + normal1.x*level->pathDistance;
	border1->end.y = line->end.y + normal1.y * level->pathDistance;
	border1->start.x = line->start.x + normal1.x * level->pathDistance;
	border1->start.y = line->start.y + normal1.y * level->pathDistance;

	border2->end.x = line->end.x + normal2.x * level->pathDistance;
	border2->end.y = line->end.y + normal2.y * level->pathDistance;
	border2->start.x = line->start.x + normal2.x * level->pathDistance;
	border2->start.y = line->start.y + normal2.y * level->pathDistance;
	
	//add them to collision line list
	level->collisionPaths = gfc_list_append(level->collisionPaths, border1);
	level->collisionPaths = gfc_list_append(level->collisionPaths, border2);
}

void calcOptimalBuckets(void* data, void* context) {
	gfc_list_foreach(((Path2D*)data)->lines, calcOptimalLineBuckets, context);
}

Bool pathCollision(Entity* entity) {
	int i;
	Line2D* currLine;
	List* colpaths = get_loaded_level()->collisionPaths;
	for (i = 0; i < colpaths->count; i++)
	{
		currLine = colpaths->elements[i].data;
		if (LineCircle(*currLine, entity->boundingBox)) {
			return true;
		}
	}
	return false;
}

Path2D* nextPath()
{
	Path2D* path = gfc_list_get_nth(LOADED_LEVEL->paths, LOADED_LEVEL->currPath);
	LOADED_LEVEL->currPath = (LOADED_LEVEL->currPath + 1) % gfc_list_get_count(LOADED_LEVEL->paths);
	return path;
}

void do_collisions() {
	int i,j,k;
	List* optis = LOADED_LEVEL->optimalBuckets;
	Bucket* bucket;
	Entity* currEnt;
	Entity* otherEnt;
	for (i = 0; i < optis->count; i++) {
		bucket = gfc_list_get_nth(optis, i);
		if (bucket->entities->count < 2) {
			continue;
		}
		for (j = 0; j < bucket->entities->count; j++) {
			currEnt = gfc_list_get_nth(bucket->entities, j);
			if (currEnt->_inuse != 1) {
				continue;
			}
			if (!currEnt->touch) {
				continue;
			}
			for (k = 0; k < bucket->entities->count; k++) {
				if (k == j) {
					continue;
				}
				otherEnt = gfc_list_get_nth(bucket->entities, k);
				if (CircleCircle(currEnt->boundingBox, otherEnt->boundingBox)) {
					currEnt->touch(currEnt, otherEnt);
				}
			}
		}
	}
}

void level_addCash(float amount)
{
	Element* list;
	Element* currLabel;
	TextLine str;
	LOADED_LEVEL->playerCash += amount;
	slog("Cash: $%f", LOADED_LEVEL->playerCash);
	currLabel = gf2d_window_get_element_by_id(getCashWindow(), 0);
	sprintf(str, "$%i", (int)LOADED_LEVEL->playerCash);
	gf2d_element_label_set_text(currLabel, str);
}

void level_addLife(int amount)
{
	Element* list;
	Element* currLabel;
	TextLine str;
	LOADED_LEVEL->playerHealth += amount;
	if (LOADED_LEVEL->playerHealth < 0) {
		LOADED_LEVEL->playerHealth = 0;
	}
	slog("Life: %i", LOADED_LEVEL->playerHealth);
	currLabel = gf2d_window_get_element_by_id(getLifeWindow(), 0);
	sprintf(str, "Life: %i", LOADED_LEVEL->playerHealth);
	gf2d_element_label_set_text(currLabel, str);
}

void level_update()
{
	if (LOADED_LEVEL->roundOver == true) { return; }
	//if no more enemies and no more to spawn, 
	if (LOADED_LEVEL->playerHealth <= 0) {
		//show game over screen
		slog("U ded");
		LOADED_LEVEL->roundOver = true;
	}
	if (LOADED_LEVEL->currentEnemy >= LOADED_LEVEL->roundEnemies->count) {
		//check no more in play
		if (enemiesExist()) {
			return;
		}
		else {
			endRound();
		}
		//else:
	}
	else {
		if (LOADED_LEVEL->spawnCooldown < 0.0000001f) {
			spawnNext();
			LOADED_LEVEL->currentEnemy += 1;
			if (LOADED_LEVEL->currentEnemy >= LOADED_LEVEL->roundEnemies->count) { return; }
			LOADED_LEVEL->spawnCooldown = (int)gfc_list_get_nth(LOADED_LEVEL->roundTimings, LOADED_LEVEL->currentEnemy)/1000.0f;
		}
		else {
			LOADED_LEVEL->spawnCooldown -= gf2d_graphics_get_milli_delta();
		}
	}
}

void endRound() {
	//do regen at round end
	//show go button
	LOADED_LEVEL->roundOver = true;
	//if it's the last round, bring up the rewards screen
}

void level_start_round() {
	if (LOADED_LEVEL->roundOver) {
		LOADED_LEVEL->roundOver = false;
		LOADED_LEVEL->round += 1;
		LOADED_LEVEL->roundEnemies = getRoundEnemies(LOADED_LEVEL->round);
		LOADED_LEVEL->roundTimings = getRoundTimings(LOADED_LEVEL->round);
		LOADED_LEVEL->currentEnemy = 0;
		LOADED_LEVEL->spawnCooldown = 0.0f;
	}
}

Bool enemiesExist() {
	List* optiBuckets;
	Bucket* bucket;
	Entity* currEntity;
	int i, j;
	Bool result = false;
	optiBuckets = LOADED_LEVEL->optimalBuckets;
	for (i = 0; i < optiBuckets->count; i++) {
		bucket = gfc_list_get_nth(optiBuckets, i);
		for (j = 0; j < bucket->entities->count; j++) {
			currEntity = gfc_list_get_nth(bucket->entities, j);
			if (currEntity->_inuse != 1 || currEntity->type != Type_Enemy) {
				continue;
			}
			return true;
		}
	}
	return false;
}

void spawnNext() {
	EnemyTypes type = (EnemyTypes)gfc_list_get_nth(LOADED_LEVEL->roundEnemies, LOADED_LEVEL->currentEnemy);
	switch (type)
	{
	case ET_RED:
		redSpawn(NULL);
		break;
	case ET_BLUE:
		blueSpawn(NULL);
		break;
	case ET_GREEN:
		greenSpawn(NULL);
		break;
	case ET_YELLOW:
		yellowSpawn(NULL);
		break;
	case ET_SUPER:
		superSpawn(NULL);
		break;
	default:
		break;
	}
}

List* getRoundEnemies(int round) {
	List* res;
	if (round < 0 || round > 19) {
		slog("Round must be between 0 and 19 inclusive!");
		return NULL;
	}
	res = gfc_list_new();
	switch (round)
	{
	case 0:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		break;
	case 1:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		break;
	case 2:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		break;
	case 3:
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE); 
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		break;
	case 4:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 5:
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		break;
	case 6:
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 7:
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		break;
	case 8:
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		break;
	case 9:
		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 10:
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		break;
	case 11:
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 12:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_RED);

		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_BLUE);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_SUPER);
		break;
	case 13:
		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);
		break;
	case 14:
		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 15:
		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		break;
	case 16:
		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		res = gfc_list_append(res, ET_YELLOW);
		break;
	case 17:
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);

		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_GREEN);
		break;
	case 18:
		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_SUPER);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);

		res = gfc_list_append(res, ET_RED);
		res = gfc_list_append(res, ET_BLUE);
		res = gfc_list_append(res, ET_GREEN);
		res = gfc_list_append(res, ET_YELLOW);
		break;
	case 19:
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		res = gfc_list_append(res, ET_SUPER);
		break;
	default:
		break;
	}
}

List* getRoundTimings(int round) {
	List* res;
	if (round < 0 || round > 19) {
		slog("Round must be between 0 and 19 inclusive!");
		return NULL;
	}
	res = gfc_list_new();
	switch (round)
	{
	case 0:
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		break;
	case 1:
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 900);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		break;
	case 2:
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		break;
	case 3:
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 4:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 700);
		break;
	case 5:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);
		break;
	case 6:
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 700);
		break;
	case 7:
		res = gfc_list_append(res, 3000);
		res = gfc_list_append(res, 700);
		break;
	case 8:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 9:
		res = gfc_list_append(res, 1500);

		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);

		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		res = gfc_list_append(res, 450);
		break;
	case 10:
		res = gfc_list_append(res, 200);
		res = gfc_list_append(res, 200);
		res = gfc_list_append(res, 200);
		break;
	case 11:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 12:

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 500);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 500);

		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);
		res = gfc_list_append(res, 500);

		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 600);
		res = gfc_list_append(res, 800);

		res = gfc_list_append(res, 500);
		break;
	case 13:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);
		break;
	case 14:
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 15:
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 16:
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 17:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	case 18:
		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 700);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);

		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 600);
		break;
	case 19:
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		res = gfc_list_append(res, 300);
		break;
	default:
		break;
	}
}
