#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_entity.h"
#include "local.h"

#include "gf2d_mouse.h"

void gf2d_entity_manager_close()
{
    if(entity_list != NULL)
    {
        free(entity_list);
    }
    memset(&gf2d_entity_manager,0,sizeof(EntityManager));
}

void gf2d_entity_manager_init(Uint32 entity_max)
{
	gf2d_entity_manager.entity_max = entity_max;
    entity_list = (Entity*)gfc_allocate_array(sizeof(Entity),entity_max);
    if (!entity_list)
    {
        slog("failed to allocate entity list");
        return;
    }
    atexit(gf2d_entity_manager_close);
}

Entity *gf2d_entity_new()
{
    Entity *ent = NULL;
    int i;
    for (i = 0; i < gf2d_entity_manager.entity_max; i++)
    {
        if (entity_list[i]._inuse)continue;
        //. found a free entity
        memset(&entity_list[i],0,sizeof(Entity));
		ent = &entity_list[i];
        ent->_inuse = 1;
		gf2d_entity_manager.num_ents++;
		//ent->movetype = MOVETYPE_NONE;
		ent->touch = NULL;
		ent->colorShift = vector4d(255, 255, 255, 255);
		ent->cooldown = 0.0f;
		ent->seekBuckets = NULL;
		ent->noTouch = NULL;
        return &entity_list[i];
    }
    slog("request for entity failed: all full up");
    return NULL;
}

void gf2d_entity_free(Entity *self)
{
    if (!self)
    {
        slog("self pointer is not valid");
        return;
    }
    self->_inuse = 0;
	self->name = NULL;
	gf2d_actor_free(&self->actor);
    if (self->data != NULL)
    {
        slog("warning: data not freed at entity free!");
    }
}

void gf2d_entity_free_all()
{
	memset(entity_list, 0, sizeof(Entity) * gf2d_entity_manager.entity_max);
}

Entity* find_entity(char* name) {
	Entity* from = entity_list;
	if (!entity_list) {
		slog("Tried to find entity before initializing entity list!");
		return NULL;
	}
	for (; from < &entity_list[gf2d_entity_manager.num_ents]; from++)
	{
		if (from->_inuse != 1 || !from->name)
			continue;
		if (!strcmp(from->name, name))
			return from;
	}
	return NULL;
}

Entity* get_last_entity() {
	Entity* ent = NULL;
	int i;
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
	}
	return ent;
}

EntityManager get_entity_manager() {
	return gf2d_entity_manager;
}

/*
Entity* modeled_entity_animated(char* modelName, char* entityName, int frame_w, int numFrames)
{
	Entity* ent = gf2d_entity_new();
	if (!ent) {
		return NULL;
	}
	Model* model = gf2d_model_load_animated(modelName, frame_w, numFrames);
	if (!model) {
		slog("Could not load animated model %s", modelName);
		return NULL;
	}
	ent->model = model;
	Matrix4 modelMat;
	gfc_matrix_identity(modelMat);
	gfc_matrix_copy(ent->modelMat, modelMat);
	ent->boundingBox.size = *ent->model->extents[0];
	ent->name = entityName;
	return ent;
}

Entity* modeled_entity(char* modelName, char* entityName)
{
	Entity* ent = gf2d_entity_new();
	if (!ent) {
		return NULL;
	}
	Model* model = gf2d_model_load(modelName);
	if (!model) {
		slog("Could not load model %s", modelName);
		return NULL;
	}
	ent->model = model;
	Matrix4 modelMat;
	gfc_matrix_identity(modelMat);
	gfc_matrix_copy(ent->modelMat, modelMat);
	ent->boundingBox.size = *ent->model->extents[0];
	ent->name = entityName;
	return ent;
}

void rotate_entity(Entity* entity, float radians, Vector3D axis) {
	if (!entity->modelMat) {
		slog("No model matrix for entity %s", entity->name);
	}
	gfc_matrix_rotate(entity->modelMat, entity->modelMat, radians, axis);
	vector3d_copy(entity->rotation,getAngles(entity->modelMat));
}

Vector3D getAngles(Matrix4 mat) {
	Vector3D res;
	float sy = sqrt(pow(mat[0][0], 2) + pow(mat[1][0], 2));

	Bool singular = sy < 0.000001f;
	if (!singular) {
		res.x = atan2(mat[2][1], mat[2][2]);
		res.y = atan2(-mat[2][0], sy);
		res.z = atan2(mat[1][0], mat[0][0]);
	}
	else {
		res.x = atan2(-mat[1][2], mat[1][1]);
		res.y = atan2(-mat[2][0], sy);
		res.z = 0;
	}
	if (res.x < 0.0f) {
		res.x += GFC_2PI;
	}
	if (res.y < 0.0f) {
		res.y += GFC_2PI;
	}
	if (res.z < 0.0f) {
		res.z += GFC_2PI;
	}
	return res;
}
*/

Entity* load_entity_json(char * entityType)
{
	Entity* result; 
	char* fileName = malloc(strlen("mobs/") + strlen(entityType) + strlen(".json") + 1);
	strcpy(fileName, "mobs/");
	strcat(fileName, entityType);
	strcat(fileName, ".json");
	SJson* file = sj_load(fileName);
	if (!file)return NULL;
	SJson* modelFileObj = sj_object_get_value(file, "modelFile");
	char* modelFileChar = sj_get_string_value(modelFileObj);
	SJson* animatedObj = sj_object_get_value(file, "animated");
	char* animatedChar = sj_get_string_value(animatedObj);
	if (strcmp(animatedChar, "true")==0) {
		SJson* frame_wObj = sj_object_get_value(file, "frame_w");
		int frame_w;
		sj_get_integer_value(frame_wObj, &frame_w);
		SJson* frame_hObj = sj_object_get_value(file, "frame_h");
		int frame_h;
		sj_get_integer_value(frame_hObj, &frame_h);
		//result = modeled_entity_animated(modelFileChar, NULL, frame_w, frame_h);
		result = NULL;
	}
	else {
		//result = modeled_entity(modelFileChar, NULL);
		result = NULL;
	}
	if (!result) {
		slog("could not load %s from json!", modelFileChar);
		return NULL;
	}
	result->name = sj_get_string_value(sj_object_get_value(file, "defaultname"));
	result->type = sj_get_string_value(sj_object_get_value(file, "type"));

	float* health = malloc(sizeof(float));
	
	/*
	sj_get_float_value(sj_object_get_value(file, "health"), health);
	if (health)result->health = *health;
	else result->health = 1.0f;

	float* maxHealth = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "healthmax"), maxHealth);
	if (maxHealth)result->healthmax = *maxHealth;
	else result->healthmax = 1.0f;
	*/

	/*
	int* moveType = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "movetype"), moveType);
	if (moveType)result->movetype = *moveType;
	else result->movetype = MOVETYPE_NONE;
	*/

	float* nextThink = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "nextThink"), nextThink);
	if (nextThink)result->nextThink = *nextThink;
	else result->nextThink = 0.0f;

	int* frame = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "frame"), frame);
	if (frame)result->frame = *frame;
	else result->frame = 1;

	int* flags = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "flags"), flags);
	if (flags)result->flags = *flags;
	else result->flags = 0;

	/*
	int* svflags = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "svflags"), svflags);
	if (svflags)result->svflags = *svflags;
	else result->svflags = 0;

	SJson* aabbSizeArray = sj_object_get_value(file, "AABBSize");
	float* x = malloc(sizeof(float));
	float* y = malloc(sizeof(float));
	sj_get_float_value(sj_array_get_nth(aabbSizeArray, 0), x);
	sj_get_float_value(sj_array_get_nth(aabbSizeArray, 1), y);
	result->boundingBox.halfExtents.x = *x;
	result->boundingBox.halfExtents.y = *y;
	*/

	/*
	SJson* aabbOffsetArray = sj_object_get_value(file, "AABBAdjustments");
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 0), x);
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 1), y);
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 2), z);
	result->model->boudningAdjustment.x = *x;
	result->model->boudningAdjustment.y = *y;
	result->model->boudningAdjustment.z = *z;
	*/

	SJson* maxSpeed = sj_object_get_value(file, "maxSpeed");
	float* maxSpeedFloat = malloc(sizeof(float));
	sj_get_float_value(maxSpeed, maxSpeedFloat);
	result->maxSpeed = *maxSpeedFloat;

	/*
	float* specfloat = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "specFloat1"), specfloat);
	if (specfloat)result->specFloat1 = *specfloat;
	else result->specFloat1 = 0.0f;
	*/

	free(health);
	free(maxSpeedFloat);
	//free(moveType);
	free(nextThink);
	free(frame);
	free(flags);
	//free(svflags);
	//free(x);
	//free(y);
	//free(z);
	//free(specfloat);

	return result;
}

//Helper function to save entity data into a loadable format
void save_entity_layout_json(Entity* entity)
{
	SJson* file = sj_object_new();
	if (entity->name) {
		SJson* name = sj_new_str(entity->name);
		sj_object_insert(file, "defaultname", name);
	}

	if (entity->type) {
		SJson* type = sj_new_str(entity->type);
		sj_object_insert(file, "type", type);
	}

	/*
	SJson* inuse = sj_new_int(entity->_inuse);
	sj_object_insert(file, "_inuse", inuse);
	*/

	if (entity->actor.sprite) {
		SJson* animated = NULL;
		SJson* frame_w = NULL;
		SJson* frame_h = NULL;
		SJson* frames_per_line = NULL;
		if (entity->actor.sprite->frame_count > 1) {
			animated = sj_new_bool(1);
			frame_w = sj_new_int(entity->actor.sprite->frame_w);
			frame_h = sj_new_int(entity->actor.sprite->frame_h);
			frames_per_line = sj_new_int(entity->actor.sprite->frames_per_line);
		}
		else {
			animated = sj_new_bool(0);
		}
		sj_object_insert(file, "animated", animated);
		if (frame_w) {
			sj_object_insert(file, "frame_w", frame_w);
		}
		if (frame_h) {
			sj_object_insert(file, "frame_h", frame_h);
		}
		if (frames_per_line) {
			sj_object_insert(file, "frames_per_line", frames_per_line);
		}
		SJson* modelName = sj_new_str(entity->actor.sprite->filepath);
		sj_object_insert(file, "modelFile", modelName);
	}

	/*
	SJson* matArray = sj_array_new();
	int i,j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			sj_array_append(matArray, sj_new_float(entity->modelMat[i][j]));
		}
	}
	sj_object_insert(file, "modelMat", matArray);
	

	SJson* position = sj_array_new();
	sj_array_append(position, sj_new_float(entity->position.x));
	sj_array_append(position, sj_new_float(entity->position.y));
	sj_array_append(position, sj_new_float(entity->position.z));
	sj_object_insert(file, "position", position);

	SJson* velocity = sj_array_new();
	sj_array_append(velocity, sj_new_float(entity->velocity.x));
	sj_array_append(velocity, sj_new_float(entity->velocity.y));
	sj_array_append(velocity, sj_new_float(entity->velocity.z));
	sj_object_insert(file, "velocity", velocity);

	SJson* acceleration = sj_array_new();
	sj_array_append(acceleration, sj_new_float(entity->acceleration.x));
	sj_array_append(acceleration, sj_new_float(entity->acceleration.y));
	sj_array_append(acceleration, sj_new_float(entity->acceleration.z));
	sj_object_insert(file, "acceleration", acceleration);

	SJson* rotation = sj_array_new();
	sj_array_append(rotation, sj_new_float(entity->rotation.x));
	sj_array_append(rotation, sj_new_float(entity->rotation.y));
	sj_array_append(rotation, sj_new_float(entity->rotation.z));
	sj_object_insert(file, "rotation", rotation);

	SJson* scale = sj_array_new();
	sj_array_append(scale, sj_new_float(entity->scale.x));
	sj_array_append(scale, sj_new_float(entity->scale.y));
	sj_array_append(scale, sj_new_float(entity->scale.z));
	sj_object_insert(file, "scale", scale);

	SJson* state = sj_new_int(entity->state);
	sj_object_insert(file, "state", state);
	

	SJson* health = sj_new_float(entity->health);
	sj_object_insert(file, "health", health);

	SJson* healthMax = sj_new_float(entity->healthmax);
	sj_object_insert(file, "healthmax", healthMax);

	SJson* movetype = sj_new_int(entity->movetype);
	sj_object_insert(file, "movetype", movetype);
	*/

	SJson* nextThink = sj_new_float(entity->nextThink);
	sj_object_insert(file, "nextThink", nextThink);

	SJson* frame = sj_new_int(entity->frame);
	sj_object_insert(file, "frame", frame);

	/*
	SJson* origin = sj_array_new();
	sj_array_append(origin, sj_new_float(entity->origin.x));
	sj_array_append(origin, sj_new_float(entity->origin.y));
	sj_array_append(origin, sj_new_float(entity->origin.z));
	sj_object_insert(file, "origin", origin);

	SJson* old_origin = sj_array_new();
	sj_array_append(old_origin, sj_new_float(entity->old_origin.x));
	sj_array_append(old_origin, sj_new_float(entity->old_origin.y));
	sj_array_append(old_origin, sj_new_float(entity->old_origin.z));
	sj_object_insert(file, "oldorigin", old_origin);
	*/

	SJson* flags = sj_new_int(entity->flags);
	sj_object_insert(file, "flags", flags);
	/*
	SJson* svflags = sj_new_int(entity->svflags);
	sj_object_insert(file, "svflags", svflags);

	SJson* AABBPos = sj_array_new();
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.x));
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.y));
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.z));
	sj_object_insert(file, "AABBPos", AABBPos);
	*/

	SJson* radius = sj_new_float(entity->boundingBox.radius);
	sj_object_insert(file, "radius", radius);
	
	/*
	SJson* AABBAdjustments = sj_array_new();
	sj_array_append(AABBAdjustments, sj_new_float(entity->model->boudningAdjustment.x));
	sj_array_append(AABBAdjustments, sj_new_float(entity->model->boudningAdjustment.y));
	sj_object_insert(file, "AABBAdjustments", AABBAdjustments);
	*/

	SJson* maxSpeed = sj_new_float(entity->maxSpeed);
	sj_object_insert(file, "radius", maxSpeed);
	/*
	SJson* specFloat1 = sj_new_float(entity->specFloat1);
	sj_object_insert(file, "specFloat1", specFloat1);
	*/
	//be sure to get special cases like think functions and additional data per ent-specific load!
	char* fileName = malloc(strlen("mobs/")+strlen(entity->type)+ strlen(".json") +1);
	strcpy(fileName, "mobs/");
	strcat(fileName, entity->type);
	strcat(fileName, ".json");
	sj_save(file, fileName);
	free(fileName);
}

SJson* save_entity_content_editor_json(Entity* entity) {
	SJson* entJson = sj_object_new();
	SJson* positionArray = sj_array_new();
	sj_object_insert(entJson, "type", sj_string_to_value(sj_string_new_text(entity->type)));
	sj_array_append(positionArray, sj_new_float(entity->position.x));
	sj_array_append(positionArray, sj_new_float(entity->position.y));
	sj_object_insert(entJson, "position", positionArray);

	return entJson;
}

void save_all_content_editor() {
	Entity* ent;
	int i;
	SJson* saveFile = sj_object_new();
	SJson* entArray = sj_array_new();
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
		if (!ent->name || (strcmp(ent->name, "copied entity") != 0 && strcmp(ent->name, "player") != 0))continue;
		sj_array_append(entArray, save_entity_content_editor_json(ent));
	}
	sj_object_insert(saveFile, "entities", entArray);
	sj_object_insert(saveFile, "loadNext", sj_new_str("save.json"));
	sj_save(saveFile, "save.json");
}

Entity* gf2d_nonanimated_entity_copy(Entity* entity)
{
	Entity* res = gf2d_entity_new();
	//res->model = gf2d_model_load(entity->model->filename);
	//res->model->texture = entity->model->texture;
	res->type = entity->type;
	if (strcmp(res->type, "player") == 0) {
		res->name = "player";
	}
	else {
		res->name = "copied entity";
	}
	//Matrix4 modelMat;
	//gfc_matrix_identity(modelMat);
	//gfc_matrix_copy(res->modelMat, modelMat);
	return res;
}

int get_type_count(char* type) {
	Entity* ent = NULL;
	int i;
	int count = 0;
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
		if (strcmp(ent->type, type) == 0) {
			count++;
		}
	}
	return count;
}

float getLowestPoint() {
	float result = 100000.0f;
	int i;
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		Entity* ent = &entity_list[i];
		if (!ent->_inuse)continue;
		if (!(strcmp(ent->type, "ground") == 0 || strcmp(ent->type, "platform") == 0))continue;
		if (ent->position.y < result) {
			result = ent->position.y;
		}
	}
	return result;
}

void gf2d_entity_update_all()
{
	int i = 0;
	Entity* currEnt;
	bucket_precalc();
	for (i = 0; i < gf2d_entity_manager.entity_max; i++) //everything moves before everything else
	{
		currEnt = &entity_list[i];
		if (currEnt->_inuse != 1)continue;
		if (currEnt->move) {
			currEnt->move(currEnt);
		}
	}
	bucket_calc();
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		currEnt = &entity_list[i];
		if (currEnt->_inuse != 1)continue;
		if (currEnt->prethink) {
			currEnt->prethink(currEnt);
		}
		if (currEnt->type == Type_Tower && gf2d_mouse_button_pressed(0) && PointInCircle(gf2d_mouse_get_position(), currEnt->boundingBox)) {
			gf2d_entity_set_selected(currEnt);
		}
	}
	do_collisions();
	for (i = 0; i < gf2d_entity_manager.entity_max; i++)
	{
		currEnt = &entity_list[i];
		if (currEnt->_inuse != 1)continue;
		if (currEnt->update) {
			currEnt->update(currEnt);
		}
		else {
			gf2d_entity_update(&entity_list[i]);
		}
	}
}

void gf2d_entity_update(Entity* self)
{
	if (!self)return;
	if (!self->_inuse)return;

	if (self->dead != 0)
	{
		gf2d_entity_free(self);
		return;
	}

	/*collision handles position and velocity*/
	vector2d_add(self->velocity, self->velocity, self->acceleration);

	gf2d_actor_next_frame(&self->actor);

	if (self->think) {
		self->think(self); //Enemy movement will be done in their thinks. This is to easily adjust their positions on the line and change lines if necessary
	}
}

void gf2d_entity_look_at(Entity* self, Entity* lookAt)
{
	float degree;
	Vector2D deltas;

	vector2d_sub(deltas, self->position, lookAt->position);

	degree = atan2f(deltas.y, deltas.x);

	degree = (degree > 0.0f?degree:(GFC_2PI+degree));

	if (deltas.y > 0.0f && deltas.x == 0.0f) {
		degree = -GFC_PI / 2;
	}

	self->rotation.z = degree*GFC_RADTODEG;
}

Entity* gf2d_entity_get_selected() {
	return selectedEntity;
}

void gf2d_entity_set_selected(Entity* entity) {
	selectedEntity = entity;
}
/*eol@eof*/
