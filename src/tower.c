#include "tower.h"
#include "simple_logger.h"
#include "local.h"
#include "level.h"
#include "gf2d_mouse.h"
#include "gf2d_element_button.h"
#include "gf2d_graphics.h"
#include "bucket.h"
#include "projectiles.h"

Entity* findClosest(Entity* self);
void setSeekBuckets(Entity* self);
Bool allyCollision(Entity* self);
void techno_damage(Entity* self, Entity* target);
List* inRange(Entity* self);

#pragma region Spawns
Entity* stinger_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/stinger.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* slingshot_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/slingshot.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* laser_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/laser.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* water_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/water.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* techno_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/techno.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* snowglobe_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/snowglobe.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* music_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/music.actor");
	vector2d_copy(self->position, position);
	return self;
}

Entity* placement_spawn(TowerTypes type) {
	Entity* self;
	self = find_entity("placement");
	if (self) {
		return self;
	}
	self = gf2d_entity_new();
	switch (type)
	{
	case TT_Stinger:
		gf2d_actor_load(&self->actor, "actors/stinger.actor");
		self->shootRadius.radius = 150.0f;
		break;
	case TT_Slingshot:
		gf2d_actor_load(&self->actor, "actors/slingshot.actor");
		self->shootRadius.radius = 225.0f;
		break;
	case TT_Laser:
		gf2d_actor_load(&self->actor, "actors/laser.actor");
		self->shootRadius.radius = 200.0f;
		break;
	case TT_Water:
		gf2d_actor_load(&self->actor, "actors/water.actor");
		self->shootRadius.radius = 90.0f;
		break;
	case TT_Techno:
		gf2d_actor_load(&self->actor, "actors/techno.actor");
		break;
	case TT_Snowglobe:
		gf2d_actor_load(&self->actor, "actors/snowglobe.actor");
		break;
	case TT_Music:
		gf2d_actor_load(&self->actor, "actors/music.actor");
		break;
	default:
		gf2d_actor_load(&self->actor, "actors/stinger.actor");
		slog("Invalid tower type! Defaulting...");
		break;
	}
	self->think = placement_think;
	self->position.x = mx;
	self->position.y = my;
	self->name = "placement";
	self->data = (int)type;
	self->boundingBox.radius = 15.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	return self;
}
#pragma endregion

#pragma region Thinks
void stinger_think(Entity* self){
	Entity* target;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			stingerBolt_spawn(self);
			if (self->rotation.z >= 360.0f) {
				self->rotation.z = fmodf(self->rotation.z, 360.0f);
			}
			if (self->rotation.z >= 90.0f && self->rotation.z <= 270.0f) {
				self->flip = vector2d(0, 1);
			}
			else {
				self->flip = vector2d(0, 0);
			}
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void slingshot_think(Entity* self){
	Entity* target;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			slingerPellet_spawn(self);
			self->rotation.z += 180;
			if (self->rotation.z >= 360.0f) {
				self->rotation.z = fmodf(self->rotation.z, 360.0f);
			}
			if (self->rotation.z >= 90.0f && self->rotation.z <= 270.0f) {
				self->flip = vector2d(0, 1);
			}
			else {
				self->flip = vector2d(0, 0);
			}
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void laser_think(Entity* self){
	Entity* target;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			laserlaser_spawn(self);
			self->rotation.z += 180;
			if (self->rotation.z >= 360.0f) {
				self->rotation.z = fmodf(self->rotation.z, 360.0f);
			}
			if (self->rotation.z >= 90.0f && self->rotation.z <= 270.0f) {
				self->flip = vector2d(0, 1);
			}
			else {
				self->flip = vector2d(0, 0);
			}
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void water_think(Entity* self){
	List* targets;
	Entity* target;
	int i;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		targets = inRange(self);
		//if fire, reset cooldown to fireRate
		if (targets && targets->count>0) {
			gf2d_actor_set_action(&self->actor, "fire");
			for (i = 0; i < targets->count; i++) {
				target = gfc_list_get_nth(targets, i);
				techno_damage(self, target);
			}
			waterwave_spawn(self);
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void techno_think(Entity* self){
	Entity* target;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			techno_damage(self, target);
			self->rotation.z += 180;
			if (self->rotation.z >= 360.0f) {
				self->rotation.z = fmodf(self->rotation.z, 360.0f);
			}
			if (self->rotation.z >= 90.0f && self->rotation.z <= 270.0f) {
				self->flip = vector2d(0, 1);
			}
			else {
				self->flip = vector2d(0, 0);
			}
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void snowglobe_think(Entity* self){

}

void music_think(Entity* self){

}

void placement_think(Entity* self) {
	Bool colliding;
	self->position = gf2d_mouse_get_position();
	colliding = (pathCollision(self)||allyCollision(self));
	if (colliding) {
		self->colorShift = vector4d(255.0f, 0.0f, 0.0f, 255.0f);
	}
	else {
		self->colorShift = vector4d(0.0f, 255.0f, 0.0f, 255.0f);
	}
	self->boundingBox.position = self->position;
	self->shootRadius.position = self->position;
	if (!colliding&&gf2d_mouse_button_pressed(0)&& (self->flags & FL_PLACEABLE)) {
		placement_detach(self);
	}
	if (!(self->flags & FL_PLACEABLE)&&gf2d_mouse_button_released(0)) {
		self->flags = self->flags | FL_PLACEABLE;
	}
}
#pragma endregion

#pragma region PlacementExtras
void placement_detach(Entity* ent) {
	if (!ent){
		slog("No place entity!");
		return;
	}
	switch ((TowerTypes)ent->data)
	{
	case TT_Stinger:
		ent->think = stinger_think;
		ent->name = "stinger";
		ent->shootRadius.radius = 150.0f;
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		break;
	case TT_Slingshot:
		ent->think = slingshot_think;
		ent->name = "slingshot";
		ent->shootRadius.radius = 225.0f;
		ent->fireRate = 0.25f;
		setSeekBuckets(ent);
		break;
	case TT_Laser:
		ent->think = laser_think;
		ent->name = "laser";
		ent->shootRadius.radius = 200.0f;
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		break;
	case TT_Water:
		ent->think = water_think;
		ent->name = "water";
		ent->shootRadius.radius = 90.0f;
		ent->fireRate = 0.75f;
		ent->damage = 1;
		setSeekBuckets(ent);
		break;
	case TT_Techno:
		ent->think = techno_think;
		ent->name = "techno";
		ent->shootRadius.radius = 0.0f;
		ent->fireRate = 1.0f;
		ent->damage = 2;
		break;
	case TT_Snowglobe:
		ent->think = snowglobe_think;
		ent->name = "snowglobe";
		ent->shootRadius.radius = 150.0f;
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		break;
	case TT_Music:
		ent->think = music_think;
		ent->name = "music";
		ent->shootRadius.radius = 150.0f;
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		break;
	default:
		ent->think = stinger_think;
		ent->name = "stinger";
		ent->shootRadius.radius = 150.0f;
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		slog("Invalid detach tower type! Defaulting...");
		break;
	}
	ent->shootRadius.position = ent->position;
	ent->colorShift = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
	ent->type = Type_Tower;
	ent->rotation.x = ent->actor.sprite->frame_w/2;
	ent->rotation.y = ent->actor.sprite->frame_h/2;
	setAllyBuckets(ent);
}
#pragma endregion

void setPrices(Window* window) {
	Element* list;
	Element* currButton;
	Element* currLabel;
	TextLine str;
	list = gf2d_window_get_element_by_id(window, 0);
	currButton = gf2d_element_list_get_item_by_id(list, 50);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Stinger - $%i", getPrice(TT_Stinger));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 51);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Slinger - $%i", getPrice(TT_Slingshot));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 52);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Laser - $%i", getPrice(TT_Laser));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 53);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Water - $%i", getPrice(TT_Water));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 54);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Techno - $%i", getPrice(TT_Techno));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 55);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Snow - $%i", getPrice(TT_Snowglobe));
	gf2d_element_label_set_text(currLabel, str);

	currButton = gf2d_element_list_get_item_by_id(list, 56);
	currLabel = ((ButtonElement*)currButton->data)->label;
	sprintf(str, "Music - $%i", getPrice(TT_Music));
	gf2d_element_label_set_text(currLabel, str);
}

int getPrice(TowerTypes type)
{
	switch (type)
	{
	case TT_Stinger:
		return 100;
	case TT_Slingshot:
		return 175;
	case TT_Laser:
		return 250;
	case TT_Water:
		return 250;
	case TT_Techno:
		return 350;
	case TT_Snowglobe:
		return 150;
	case TT_Music:
		return 200;
	default:
		return 100;
	}
}

Entity* findClosest(Entity* self) {
	int i,j;
	Bucket* bucket;
	Entity* currEnemy;
	Entity* targetEnemy;
	int count;
	float currClosestDist;
	float testDist;
	targetEnemy = NULL;
	currClosestDist = -1.0f;

	count = self->seekBuckets ? self->seekBuckets->count : get_loaded_level()->optimalBuckets->count;
	for (i = 0; i < count; i++) {
		if (!self->seekBuckets) {
			bucket = gfc_list_get_nth(get_loaded_level()->optimalBuckets, i);
		}
		else {
			bucket = gfc_list_get_nth(self->seekBuckets, i);
		}
		for (j = 0; j < bucket->entities->count; j++) { //I think the problem is we're clearing buckets before thinking and not recalcing until after.
			currEnemy = gfc_list_get_nth(bucket->entities, j);
			if (!currEnemy || currEnemy->_inuse!=1) { continue; }
			if (currEnemy->type == Type_Enemy) {
				if (self->shootRadius.radius == 0.0f || CircleCircle(self->shootRadius, currEnemy->boundingBox)) {
					testDist = LengthSqLine2D(line2d(self->shootRadius.position, currEnemy->boundingBox.position));
					if (currClosestDist < 0.0f) {
						targetEnemy = currEnemy;
						currClosestDist = testDist;
					}
					else if (testDist < currClosestDist) {
						targetEnemy = currEnemy;
						currClosestDist = testDist;
					}
				}
			}
		}
	}
	return targetEnemy;
}

void setSeekBuckets(Entity* self) {
	int i;
	Bucket* currBucket;
	List* optiBuckets = get_loaded_level()->optimalBuckets;
	if (!self->seekBuckets) {
		self->seekBuckets = gfc_list_new();
	}
	for (i = 0; i < optiBuckets->count; i++) {
		currBucket = gfc_list_get_nth(optiBuckets, i);
		if (CircleRectangle(self->shootRadius, currBucket->shape.s.r)) {
			self->seekBuckets = gfc_list_append(self->seekBuckets, currBucket);
		}
	}	
}

Bool allyCollision(Entity* self) {
	int i,j;
	Entity* currOther;
	Bucket* bucket;
	List* allyBuckets = get_loaded_level()->allyBuckets;
	for (i = 0; i < allyBuckets->count; i++)
	{
		bucket = gfc_list_get_nth(allyBuckets, i);
		for (j = 0; j < bucket->entities->count; j++) {
			currOther = gfc_list_get_nth(bucket->entities, j);
			if (CircleCircle(self->boundingBox, currOther->boundingBox)) {
				return true;
			}
		}
	}
	return false;
}

List* inRange(Entity* self) {
	int i, j;
	Entity* currOther;
	Bucket* bucket;
	List* seekBuckets;
	List* res;
	seekBuckets = self->seekBuckets;
	if (!seekBuckets) {
		return NULL;
	}
	res = gfc_list_new();
	for (i = 0; i < seekBuckets->count; i++)
	{
		bucket = gfc_list_get_nth(seekBuckets, i);
		for (j = 0; j < bucket->entities->count; j++) {
			currOther = gfc_list_get_nth(bucket->entities, j);
			if (currOther->type != Type_Enemy || currOther->_inuse!=1) { continue; }
			if (CircleCircle(self->shootRadius, currOther->boundingBox)) {
				res = gfc_list_append(res, currOther);
			}
		}
	}
	return res;
}

void techno_damage(Entity* self, Entity* target) {
	int damageLeft;
	damageLeft = self->damage;
	Entity* child = NULL;
	if (target->_inuse != 1) { return; }
	target->health -= damageLeft;
	if (target->health <= 0) {
		damageLeft = abs(target->health);
		while (damageLeft > 0) {
			child = target->die(target);
			if (!child) {
				break;
			}
			child->health -= damageLeft;
			if (child->health < 0) {
				damageLeft = abs(child->health);
			}
			else if (child->health == 0) {
				damageLeft = 0;
				child = child->die(child);
			}
			else {
				damageLeft = 0;
				child = NULL;
			}
		}
	}
}