#include "level.h"
#include "gf2d_sprite.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "Geometry2D.h"
#include "bucket.h"

static Level* LOADED_LEVEL;

void paths_save(void* data, void* context);
void calcPathBoundaries(void* data, void* context);
void calcBoundaryLines(void* data, void* context);
void calcOptimalBuckets(void* data, void* context);

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
