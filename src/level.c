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

void level_save(char* fileName)
{
	SJson* file;
	SJson* tempJson;
	SJson* tempJsonValue;
	SJson* tempJsonValue2;
	SJson* tempJsonValue3;
	Level* level;
	Entity* currEntity;
	Bucket* currBucket;
	List* alreadyAdded;
	int i, j;
	Player* player = getPlayer();
	file = sj_object_new();
	level = LOADED_LEVEL;
	alreadyAdded = gfc_list_new();

	//level file
	//player health
	//player cash
	//round
	//towers

	tempJsonValue = sj_new_str(level->fileName);
	sj_object_insert(file, "fileName", tempJsonValue);

	tempJsonValue = sj_new_int(level->playerHealth);
	sj_object_insert(file, "health", tempJsonValue);

	tempJsonValue = sj_new_float(level->playerCash);
	sj_object_insert(file, "cash", tempJsonValue);

	if (LOADED_LEVEL->roundOver) {
		tempJsonValue = sj_new_int(level->round);
	}
	else {
		tempJsonValue = sj_new_int(level->round - 1);
	}
	sj_object_insert(file, "round", tempJsonValue);

	tempJson = sj_array_new();
	for (i = 0; i < LOADED_LEVEL->allyBuckets->count; i++) {
		currBucket = gfc_list_get_nth(LOADED_LEVEL->allyBuckets, i);
		for (j = 0; j < currBucket->entities->count; j++) {
			currEntity = gfc_list_get_nth(currBucket->entities, j);
			if (currEntity->_inuse != 1 || currEntity->type != Type_Tower || gfc_list_in_list(alreadyAdded, currEntity) >= 0) {
				continue;
			}
			alreadyAdded = gfc_list_append(alreadyAdded, currEntity);
			tempJsonValue = sj_object_new();
			//upgradeID
			tempJsonValue2 = sj_new_int(currEntity->upgradeID);
			sj_object_insert(tempJsonValue, "upgradeID", tempJsonValue2);
			//position
			tempJsonValue2 = sj_array_new();
			tempJsonValue3 = sj_new_float(currEntity->position.x);
			sj_array_append(tempJsonValue2, tempJsonValue3);
			tempJsonValue3 = sj_new_float(currEntity->position.y);
			sj_array_append(tempJsonValue2, tempJsonValue3);
			sj_object_insert(tempJsonValue, "position", tempJsonValue2);
			//type
			tempJsonValue2 = sj_new_int((TowerTypes)currEntity->data);
			sj_object_insert(tempJsonValue, "towerType", tempJsonValue2);
			sj_array_append(tempJson, tempJsonValue);
		}
	}
	if (sj_array_get_count(tempJson) > 0) {
		sj_object_insert(file, "towers", tempJson);
	}
	

	sj_save(file, fileName);
}

Level* level_load_from_save(char* levelSaveFile) {
	SJson* file;
	SJson* tempJson;
	SJson* tempJson2;
	SJson* tempJson3;
	SJson* tempJson4;
	Level* level;
	int tempInt;
	int tempInt2;
	float tempFloat;
	int upgradeID;
	Entity* currTower;
	Vector2D pos;
	file = sj_load(levelSaveFile);
	if (!file) {
		level = level_load("levels/test.json");
		level_addCash(100.0f);
		if (getPlayer()->perks[PN_Money]) {
			level_addCash(150.0f);
		}
		level_addLife(100);
		return level;
	}
	tempJson = sj_object_get_value(file, "fileName");
	if (tempJson) {
		level = level_load(sj_get_string_value(tempJson));
	}
	else {
		level = level_load("levels/test.json");
	}

	tempJson = sj_object_get_value(file, "health");
	if (tempJson) {
		sj_get_integer_value(tempJson, &tempInt);
	}
	else {
		tempInt = 100;
	}
	level_addLife(tempInt);

	tempJson = sj_object_get_value(file, "cash");
	if (tempJson) {
		sj_get_float_value(tempJson, &tempFloat);
	}
	else {
		tempFloat = 100.0f;
	}
	level_addCash(tempFloat);

	tempJson = sj_object_get_value(file, "round");
	if (tempJson) {
		sj_get_integer_value(tempJson, &tempInt);
	}
	else {
		tempInt = -1;
	}
	level->round = tempInt;

	tempJson = sj_object_get_value(file, "towers");
	if (!tempJson) { return level; }
	for (tempInt = 0; tempInt < sj_array_get_count(tempJson); tempInt++) {
		tempJson2 = sj_array_get_nth(tempJson, tempInt);

		tempJson3 = sj_object_get_value(tempJson2, "position");
		tempJson4 = sj_array_get_nth(tempJson3, 0);
		sj_get_float_value(tempJson4, &pos.x);
		tempJson4 = sj_array_get_nth(tempJson3, 1);
		sj_get_float_value(tempJson4, &pos.y);

		tempJson3 = sj_object_get_value(tempJson2, "upgradeID");
		sj_get_integer_value(tempJson3, &upgradeID);

		tempJson3 = sj_object_get_value(tempJson2, "towerType");
		sj_get_integer_value(tempJson3, &tempInt2);
		switch ((TowerTypes)tempInt2)
		{
		case TT_Stinger:
			currTower = stinger_spawn(pos);
			if (upgradeID == 1) {
				applyStingerUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applyStingerUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applyStingerUpgrade(currTower, 0);
				applyStingerUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applyStingerUpgrade(currTower, 0);
				applyStingerUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applyStingerUpgrade(currTower, 1);
				applyStingerUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applyStingerUpgrade(currTower, 1);
				applyStingerUpgrade(currTower, 1);
			}
			break;
		case TT_Slingshot:
			currTower = slingshot_spawn(pos);
			if (upgradeID == 1) {
				applySlingshotUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applySlingshotUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applySlingshotUpgrade(currTower, 0);
				applySlingshotUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applySlingshotUpgrade(currTower, 0);
				applySlingshotUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applySlingshotUpgrade(currTower, 1);
				applySlingshotUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applySlingshotUpgrade(currTower, 1);
				applySlingshotUpgrade(currTower, 1);
			}
			break;
		case TT_Laser:
			currTower = laser_spawn(pos);
			if (upgradeID == 1) {
				applyLaserUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applyLaserUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applyLaserUpgrade(currTower, 0);
				applyLaserUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applyLaserUpgrade(currTower, 0);
				applyLaserUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applyLaserUpgrade(currTower, 1);
				applyLaserUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applyLaserUpgrade(currTower, 1);
				applyLaserUpgrade(currTower, 1);
			}
			break;
		case TT_Water:
			currTower = water_spawn(pos);
			if (upgradeID == 1) {
				applyWaterUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applyWaterUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applyWaterUpgrade(currTower, 0);
				applyWaterUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applyWaterUpgrade(currTower, 0);
				applyWaterUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applyWaterUpgrade(currTower, 1);
				applyWaterUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applyWaterUpgrade(currTower, 1);
				applyWaterUpgrade(currTower, 1);
			}
			break;
		case TT_Techno:
			currTower = techno_spawn(pos);
			if (upgradeID == 1) {
				applyTechnoUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applyTechnoUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applyTechnoUpgrade(currTower, 0);
				applyTechnoUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applyTechnoUpgrade(currTower, 0);
				applyTechnoUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applyTechnoUpgrade(currTower, 1);
				applyTechnoUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applyTechnoUpgrade(currTower, 1);
				applyTechnoUpgrade(currTower, 1);
			}
			break;
		case TT_Snowglobe:
			currTower = snowglobe_spawn(pos);
			if (upgradeID == 1) {
				applySnowUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applySnowUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applySnowUpgrade(currTower, 0);
				applySnowUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applySnowUpgrade(currTower, 0);
				applySnowUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applySnowUpgrade(currTower, 1);
				applySnowUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applySnowUpgrade(currTower, 1);
				applySnowUpgrade(currTower, 1);
			}
			break;
		case TT_Music:
			currTower = music_spawn(pos);
			if (upgradeID == 1) {
				applyMusicUpgrade(currTower, 0);
			}
			else if (upgradeID == 2) {
				applyMusicUpgrade(currTower, 1);
			}
			else if (upgradeID == 3) {
				applyMusicUpgrade(currTower, 0);
				applyMusicUpgrade(currTower, 0);
			}
			else if (upgradeID == 4) {
				applyMusicUpgrade(currTower, 0);
				applyMusicUpgrade(currTower, 1);
			}
			else if (upgradeID == 5) {
				applyMusicUpgrade(currTower, 1);
				applyMusicUpgrade(currTower, 0);
			}
			else if (upgradeID == 6) {
				applyMusicUpgrade(currTower, 1);
				applyMusicUpgrade(currTower, 1);
			}
			break;
		case TT_Power_Speed_Totem:
			currTower = speed_totem_spawn(pos);
			break;
		default:
			currTower = NULL;
			break;
		}
		setAllyBuckets(currTower);
	}
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

	gf2d_entity_free_all();
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

	tempJson = sj_object_get_value(levelJson, "nextLevel");
	if (tempJson) {
		level->nextLevel = sj_get_string_value(tempJson);
	}
	else {
		level->nextLevel = "levels/test.json";
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
	level->fileName = levelFile;
	return level;
}

void level_save_level(char* fileName, Level* level) {
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

void level_addRegen(int amountPerRound) {
	LOADED_LEVEL->regenPerRound += amountPerRound;
}

void level_loadNext()
{
	Level* level;
	hideRewardWindow();
	level = level_load(LOADED_LEVEL->nextLevel);
	level_addCash(100.0f);
	if (getPlayer()->perks[PN_Money]) {
		level_addCash(150.0f);
	}
	level_addLife(100);
}

void level_update()
{
	if (LOADED_LEVEL->roundOver == true) { return; }
	//if no more enemies and no more to spawn, 
	if (LOADED_LEVEL->playerHealth <= 0) {
		slog("U ded");
		LOADED_LEVEL->roundOver = true;
		showGameOver();
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
	int reward;
	int reward2;
	//do regen at round end
	level_addLife(LOADED_LEVEL->regenPerRound);
	//show go button
	showGoButton();
	LOADED_LEVEL->roundOver = true;
	//if it's the last round, bring up the rewards screen
	if (LOADED_LEVEL->round == 19) {
		reward = rand() % (3 + 1);
		getPlayer()->powerInventory[reward] += 1;
		if (reward == 0) {
			reward = TT_Power_Bee_Swarm;
		}
		else if (reward == 1) {
			reward = TT_Power_Time_Warp;
		}
		else if (reward == 2) {
			reward = TT_Power_Speed_Totem;
		}
		else if (reward == 3) {
			reward = TT_Power_Cash_Drop;
		}
		
		if (!getPlayer()->perks[0]) {
			reward2 = 0;
			getPlayer()->perks[0] = true;
		}
		else if (!getPlayer()->perks[1]) {
			reward2 = 1;
			getPlayer()->perks[1] = true;
		}
		else if (!getPlayer()->perks[2]) {
			reward2 = 2;
			getPlayer()->perks[2] = true;
		}
		else if (!getPlayer()->perks[3]) {
			reward2 = 3;
			getPlayer()->perks[3] = true;
		}
		else if (!getPlayer()->perks[4]) {
			reward2 = 4;
			getPlayer()->perks[4] = true;
		}
		else {
			reward2 = -1;
		}
		showRewardWindow(reward, reward2);
	}
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
	hideGoButton();
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
