#include "tower.h"
#include "simple_logger.h"
#include "local.h"
#include "level.h"
#include "gf2d_mouse.h"
#include "gf2d_element_button.h"
#include "gf2d_graphics.h"
#include "bucket.h"
#include "projectiles.h"
#include "gf2d_draw.h"
#include <enemies.h>
#include "player.h"
#include "game.h"

Bool allyCollision(Entity* self);
char* getStingerUpgradeDesc(Entity* tower, int upgradeNum);
int getStingerUpgradeCost(Entity* tower, int upgradeNum);
char* getLaserUpgradeDesc(Entity* tower, int upgradeNum);
int getLaserUpgradeCost(Entity* tower, int upgradeNum);
char* getMusicUpgradeDesc(Entity* tower, int upgradeNum);
int getMusicUpgradeCost(Entity* tower, int upgradeNum);
char* getSlingshotUpgradeDesc(Entity* tower, int upgradeNum);
int getSlingshotUpgradeCost(Entity* tower, int upgradeNum);
char* getSnowUpgradeDesc(Entity* tower, int upgradeNum);
int getSnowUpgradeCost(Entity* tower, int upgradeNum);
char* getTechnoUpgradeDesc(Entity* tower, int upgradeNum);
int getTechnoUpgradeCost(Entity* tower, int upgradeNum);
char* getWaterUpgradeDesc(Entity* tower, int upgradeNum);
int getWaterUpgradeCost(Entity* tower, int upgradeNum);

void swarm_kill_all();
void timeWarp_slow_all();


#pragma region Spawns
Entity* stinger_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/stinger.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 150.0f; 
	self->think = stinger_think;
	self->name = "stinger";
	self->fireRate = 0.5f;
	self->distanceLeft = 1500.0f;
	self->health = 2;
	if (getPlayer()->perks[PN_Pierce]) {
		self->health += 1;
	}
	self->damage = 2;
	self->data = (int)TT_Stinger;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* slingshot_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/slingshot.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 225.0f;
	self->think = slingshot_think;
	self->name = "slingshot";
	self->fireRate = 0.25f;
	self->distanceLeft = 2000.0f;
	self->health = 1;
	if (getPlayer()->perks[PN_Pierce]) {
		self->health += 1;
	}
	self->damage = 1;
	self->data = (int)TT_Slingshot;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* laser_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/laser.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 200.0f;
	self->think = laser_think;
	self->name = "laser";
	self->fireRate = 0.5f;
	self->distanceLeft = 2000.0f;
	self->health = 4;
	if (getPlayer()->perks[PN_Pierce]) {
		self->health += 1;
	}
	self->damage = 3;
	self->data = (int)TT_Laser;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* water_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/water.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 90.0f;
	self->think = water_think;
	self->name = "water";
	self->fireRate = 0.75f;
	self->damage = 1;
	self->data = (int)TT_Water;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* techno_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/techno.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 0.0f;
	self->think = techno_think;
	self->name = "techno";
	self->fireRate = 1.0f;
	self->damage = 2;
	self->data = (int)TT_Techno;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	return self;
}

Entity* snowglobe_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/snowglobe.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 100.0f;
	self->think = snowglobe_think;
	self->name = "snowglobe";
	self->fireRate = 1.45f;
	self->damage = 1;
	self->health = 10;
	if (getPlayer()->perks[PN_Pierce]) {
		self->health += 1;
	}
	self->distanceLeft = 3400.0f;
	self->data = (int)TT_Snowglobe;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* music_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/music.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 125.0f;
	self->think = music_think;
	self->name = "music";
	self->fireRate = 0.5f;
	self->distanceLeft = 1400.0f;
	self->health = 1;
	if (getPlayer()->perks[PN_Pierce]) {
		self->health += 1;
	}
	self->damage = 1;
	self->data = (int)TT_Music;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
	return self;
}

Entity* speed_totem_spawn(Vector2D position) {
	Entity* self;
	self = gf2d_entity_new();
	gf2d_actor_load(&self->actor, "actors/speedTotem.actor");
	vector2d_copy(self->position, position);
	vector2d_copy(self->shootRadius.position, position);
	self->shootRadius.radius = 130.0f;
	self->think = speedTotem_think;
	self->name = "speedTotem";
	self->data = (int)TT_Power_Speed_Totem;
	self->type = Type_Tower;
	self->rotation.x = self->actor.sprite->frame_w / 2;
	self->rotation.y = self->actor.sprite->frame_h / 2;
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	setSeekBuckets(self);
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
		self->shootRadius.radius = 0.0f;
		break;
	case TT_Snowglobe:
		gf2d_actor_load(&self->actor, "actors/snowglobe.actor");
		self->shootRadius.radius = 100.0f;
		break;
	case TT_Music:
		gf2d_actor_load(&self->actor, "actors/music.actor");
		self->shootRadius.radius = 125.0f;
		break;
	case TT_Power_Bee_Swarm:
		gf2d_actor_load(&self->actor, "actors/beeSwarm.actor");
		self->shootRadius.radius = 0.0f;
		break;
	case TT_Power_Time_Warp:
		gf2d_actor_load(&self->actor, "actors/timeWarp.actor");
		self->shootRadius.radius = 0.0f;
		break;
	case TT_Power_Speed_Totem:
		gf2d_actor_load(&self->actor, "actors/speedTotem.actor");
		self->shootRadius.radius = 130.0f;
		break;
	case TT_Power_Cash_Drop:
		gf2d_actor_load(&self->actor, "actors/cashDrop.actor");
		self->shootRadius.radius = 0.0f;
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
	self->boundingBox.radius = 10.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;
	self->type = Type_Placement;
	return self;
}
#pragma endregion

#pragma region Buys
void stinger_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Stinger)) {
		//level_addCash(-(float)getPrice(TT_Stinger));
		placement_spawn(TT_Stinger);
	}
}

void slingshot_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Slingshot)) {
		//level_addCash(-(float)getPrice(TT_Slingshot));
		placement_spawn(TT_Slingshot);
	}
}

void laser_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Laser)) {
		//level_addCash(-(float)getPrice(TT_Laser));
		placement_spawn(TT_Laser);
	}
}

void water_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Water)) {
		//level_addCash(-(float)getPrice(TT_Water));
		placement_spawn(TT_Water);
	}
}

void techno_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Techno)) {
		//level_addCash(-(float)getPrice(TT_Techno));
		placement_spawn(TT_Techno);
	}
}

void snowglobe_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Snowglobe)) {
		//level_addCash(-(float)getPrice(TT_Snowglobe));
		placement_spawn(TT_Snowglobe);
	}
}

void music_buy() {
	if (get_loaded_level()->playerCash >= getPrice(TT_Music)) {
		//level_addCash(-(float)getPrice(TT_Music));
		placement_spawn(TT_Music);
	}
}

void swarm_buy() {
	if (getPlayer()->powerInventory[0] >= 1) {
		placement_spawn(TT_Power_Bee_Swarm);
	}
}

void cashDrop_buy() {
	if (getPlayer()->powerInventory[1] >= 1) {
		placement_spawn(TT_Power_Cash_Drop);
	}
}

void speedTotem_buy() {
	if (getPlayer()->powerInventory[2] >= 1) {
		placement_spawn(TT_Power_Speed_Totem);
	}
}

void timeWarp_buy() {
	if (getPlayer()->powerInventory[3] >= 1) {
		placement_spawn(TT_Power_Time_Warp);
	}
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
			if (self->upgradeID == 5) {
				self->rotation.z -= 15.0f;
				stingerBolt_spawn(self);
				self->rotation.z += 30.0f;
				stingerBolt_spawn(self);
				self->rotation.z -= 15.0f;
				stingerBolt_spawn(self);
			}
			else {
				stingerBolt_spawn(self);
			}
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
			slog("Firerate: %f", self->fireRate);
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
				target = techno_damage(self, target);
				if (self->upgradeID == 5) {
					if (!target) { continue; }
					vector2d_normalize(&target->velocity);
					target->speed = target->maxSpeed / 3.0f;
					target->velocity.x *= target->speed;
					target->velocity.y *= target->speed;
					target->cooldown = self->fireRate;
				}
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
	Entity* tempTarget;
	Vector2D firePos;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			vector2d_copy(firePos, target->position);
			tempTarget = techno_damage(self, target);
			SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 20, 20, 255);
			SDL_RenderDrawLineF(gf2d_graphics_get_renderer(), self->position.x, self->position.y, firePos.x, firePos.y);
			SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
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
			if (self->upgradeID == 6) {
				self->noTouch = gfc_list_new();
				if(tempTarget && tempTarget->_inuse == 1){
					self->noTouch = gfc_list_append(self->noTouch, tempTarget);
				}
				target = findClosest(self);
				if (target) {
					vector2d_copy(firePos, target->position);
					tempTarget = techno_damage(self, target);
					SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 20, 20, 255);
					SDL_RenderDrawLineF(gf2d_graphics_get_renderer(), self->position.x, self->position.y, firePos.x, firePos.y);
					SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
					if (tempTarget && tempTarget->_inuse == 1) {
						self->noTouch = gfc_list_append(self->noTouch, tempTarget);
					}

					target = findClosest(self);
					if (target) {
						vector2d_copy(firePos, target->position);
						tempTarget = techno_damage(self, target);
						SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 20, 20, 255);
						SDL_RenderDrawLineF(gf2d_graphics_get_renderer(), self->position.x, self->position.y, firePos.x, firePos.y);
						SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
						if (tempTarget && tempTarget->_inuse == 1) {
							self->noTouch = gfc_list_append(self->noTouch, tempTarget);
						}
					}
				}
				gfc_list_delete(self->noTouch);
				self->noTouch = NULL;
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
	List* targets;
	Entity* target;
	int i;
	int j;
	float speedMod;
	Bucket* bucket;
	List* slowed;
	if (self->cooldown < 0.0000001f) {
		//try to fire
		targets = inRange(self);
		//if fire, reset cooldown to fireRate
		if (targets && targets->count > 0) {
			gf2d_actor_set_action(&self->actor, "fire");
			for (i = 0; i < targets->count; i++) {
				target = gfc_list_get_nth(targets, i);
				techno_damage(self, target);
				vector2d_normalize(&target->velocity);
				speedMod = 2.0f;
				if (self->upgradeID == 2 || self->upgradeID == 5 || self->upgradeID == 6) {
					speedMod *= 1.15f;
				}
				if (self->upgradeID == 5) {
					speedMod *= 1.35f;
				}
				target->speed = target->maxSpeed/ speedMod;
				target->velocity.x *= target->speed;
				target->velocity.y *= target->speed;
				target->cooldown = 2.0f;
			}
			snowwave_spawn(self);
			self->cooldown = self->fireRate;
		}
		else {
			gf2d_actor_set_action(&self->actor, "idle");
		}
		if (self->upgradeID == 3) {
			target = findClosest(self);
			//if fire, reset cooldown to fireRate
			if (target) {
				gf2d_actor_set_action(&self->actor, "fire");
				gf2d_entity_look_at(self, target);
				snowCannon_spawn(self);
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
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
	if (self->upgradeID == 6) {
		if (self->abilityCooldown < 0.0000001f) {
			//try to fire
			slowed = gfc_list_new();
			for (i = 0; i < get_loaded_level()->optimalBuckets->count; i++) {
				bucket = gfc_list_get_nth(get_loaded_level()->optimalBuckets, i);
				if (bucket->entities->count <= 0) { continue; }
				for (j = 0; j < bucket->entities->count; j++) {
					target = gfc_list_get_nth(bucket->entities, j);
					if (target->_inuse != 1 || target->type != Type_Enemy || gfc_list_in_list(slowed, target) >= 0) { continue; }
					vector2d_normalize(&target->velocity);
					target->speed = target->maxSpeed / 5.0f;
					target->velocity.x *= target->speed;
					target->velocity.y *= target->speed;
					target->cooldown = 3.0f;
					slowed = gfc_list_append(slowed, target);
				}
			}
			gfc_list_delete(slowed);
			self->abilityCooldown = self->abilityRate;
		}
		else {
			self->abilityCooldown -= gf2d_graphics_get_milli_delta();
		}
	}
}

void music_think(Entity* self){
	int i,j;
	Entity* currAlly;
	Bucket* currBucket;
	List* allyBuckets;
	List* alreadyBuffed;
	Entity* target;
	allyBuckets = get_loaded_level()->allyBuckets;
	alreadyBuffed = self->noTouch;
	if (!alreadyBuffed) {
		self->noTouch = gfc_list_new();
		alreadyBuffed = (List*)self->noTouch;
	}
	for (i = 0; i < allyBuckets->count; i++) {
		currBucket = gfc_list_get_nth(allyBuckets, i);
		for (j = 0; j < currBucket->entities->count; j++) {
			currAlly = gfc_list_get_nth(currBucket->entities, j);
			if (currAlly->type != Type_Tower || currAlly->_inuse != 1 || currAlly == self || gfc_list_in_list(alreadyBuffed, currAlly)>=0 || !CircleCircle(self->shootRadius, currAlly->boundingBox)) {
				continue;
			}
			if (self->upgradeID == 1 || self->upgradeID == 3 || self->upgradeID == 4) {
				currAlly->fireRate *= 0.75f;
			}
			else {
				currAlly->fireRate *= 0.85f;
			}
			if (self->upgradeID == 2 || self->upgradeID == 5) {
				currAlly->damage += 1;
			}
			if (self->upgradeID == 5) {
				currAlly->damage += 1;
			}
			if (self->upgradeID == 3) {
				currAlly->shootRadius.radius *= 1.15f;
				currAlly->distanceLeft *= 1.10f;
				setSeekBuckets(currAlly);
			}
			if (self->upgradeID == 4) {
				currAlly->distanceLeft *= 1.20f;
				currAlly->distanceLeft *= 1.20f;
			}
			if (self->upgradeID == 6) {
				currAlly->health += 2;
			}
			self->noTouch = gfc_list_append(alreadyBuffed, currAlly);
			alreadyBuffed = self->noTouch;
		}
	}
	if (self->cooldown < 0.0000001f) {
		//try to fire
		target = findClosest(self);
		//if fire, reset cooldown to fireRate
		if (target) {
			gf2d_actor_set_action(&self->actor, "fire");
			gf2d_entity_look_at(self, target);
			musicnote_spawn(self);
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

void placement_think(Entity* self) {
	Bool colliding;
	self->position = gf2d_mouse_get_position();
	if ((TowerTypes)self->data >= TT_Power_Bee_Swarm && (TowerTypes)self->data != TT_Power_Speed_Totem) {
		if (gf2d_mouse_button_pressed(0) && (self->flags & FL_PLACEABLE)) {
			placement_detach(self);
		}
		if (!(self->flags & FL_PLACEABLE) && gf2d_mouse_button_released(0)) {
			self->flags = self->flags | FL_PLACEABLE;
		}
		return;
	}
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

	if (self->boundingBox.radius > 0.1f) {
		gf2d_draw_circle(self->boundingBox.position, self->boundingBox.radius, vector4d(0.0f, 255.0f, 0.0f, 255.0f));
	}
	if (self->shootRadius.radius > 0.1f) {
		gf2d_draw_circle(self->shootRadius.position, self->shootRadius.radius, vector4d(255.0f, 255.0f, 0.0f, 255.0f));
	}
}

void speedTotem_think(Entity* self) {
	int i, j;
	Entity* currAlly;
	Bucket* currBucket;
	List* allyBuckets;
	List* alreadyBuffed;
	Entity* target;
	allyBuckets = get_loaded_level()->allyBuckets;
	alreadyBuffed = self->noTouch;
	if (!alreadyBuffed) {
		self->noTouch = gfc_list_new();
		alreadyBuffed = (List*)self->noTouch;
	}
	for (i = 0; i < allyBuckets->count; i++) {
		currBucket = gfc_list_get_nth(allyBuckets, i);
		for (j = 0; j < currBucket->entities->count; j++) {
			currAlly = gfc_list_get_nth(currBucket->entities, j);
			if (currAlly->type != Type_Tower || currAlly->_inuse != 1 || currAlly == self || gfc_list_in_list(alreadyBuffed, currAlly) >= 0 || !CircleCircle(self->shootRadius, currAlly->boundingBox)) {
				continue;
			}
			currAlly->fireRate *= 0.75f;
			self->noTouch = gfc_list_append(alreadyBuffed, currAlly);
			alreadyBuffed = self->noTouch;
		}
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
		ent->fireRate = 0.5f;
		ent->distanceLeft = 1500.0f;
		ent->health = 2;
		if (getPlayer()->perks[PN_Pierce]) {
			ent->health += 1;
		}
		ent->damage = 2;
		setSeekBuckets(ent);
		break;
	case TT_Slingshot:
		ent->think = slingshot_think;
		ent->name = "slingshot";
		ent->fireRate = 0.25f;
		ent->distanceLeft = 2000.0f;
		ent->health = 1;
		if (getPlayer()->perks[PN_Pierce]) {
			ent->health += 1;
		}
		ent->damage = 1;
		setSeekBuckets(ent);
		break;
	case TT_Laser:
		ent->think = laser_think;
		ent->name = "laser";
		ent->fireRate = 0.5f;
		ent->distanceLeft = 2000.0f;
		ent->health = 4;
		if (getPlayer()->perks[PN_Pierce]) {
			ent->health += 1;
		}
		ent->damage = 3;
		setSeekBuckets(ent);
		break;
	case TT_Water:
		ent->think = water_think;
		ent->name = "water";
		ent->fireRate = 0.75f;
		ent->damage = 1;
		setSeekBuckets(ent);
		break;
	case TT_Techno:
		ent->think = techno_think;
		ent->name = "techno";
		ent->fireRate = 1.0f;
		ent->damage = 2;
		break;
	case TT_Snowglobe:
		ent->think = snowglobe_think;
		ent->name = "snowglobe";
		ent->fireRate = 1.45f;
		ent->damage = 1;
		ent->health = 10;
		if (getPlayer()->perks[PN_Pierce]) {
			ent->health += 1;
		}
		ent->distanceLeft = 3400.0f;
		setSeekBuckets(ent);
		break;
	case TT_Music:
		ent->think = music_think;
		ent->name = "music";
		ent->fireRate = 0.5f;
		ent->distanceLeft = 1400.0f;
		ent->health = 1;
		if (getPlayer()->perks[PN_Pierce]) {
			ent->health += 1;
		}
		ent->damage = 1;
		setSeekBuckets(ent);
		break;
	case TT_Power_Bee_Swarm:
		//kill things on screen
		swarm_kill_all();
		//subtract from player's inventory
		player_consume(TT_Power_Bee_Swarm);
		gf2d_entity_free(ent);
		return;
		break;
	case TT_Power_Cash_Drop:
		level_addCash(250.0f);
		//subtract from player's inventory
		player_consume(TT_Power_Cash_Drop);
		gf2d_entity_free(ent);
		return;
		break;
	case TT_Power_Speed_Totem:
		//subtract from player's inventory
		player_consume(TT_Power_Speed_Totem);
		ent->think = speedTotem_think;
		ent->name = "speedTotem";
		break;
	case TT_Power_Time_Warp:
		//slow things on screen
		timeWarp_slow_all();
		//subtract from player's inventory
		player_consume(TT_Power_Time_Warp);
		gf2d_entity_free(ent);
		return;
		break;
	default:
		ent->think = stinger_think;
		ent->name = "stinger";
		ent->fireRate = 0.5f;
		setSeekBuckets(ent);
		slog("Invalid detach tower type! Defaulting...");
		break;
	}
	level_addCash(-(float)getPrice((TowerTypes)ent->data));
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
		return 0;
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
			if (self->noTouch && gfc_list_in_list(self->noTouch, currEnemy) >= 0) {
				continue;
			}
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
		if (gfc_list_in_list(self->seekBuckets, currBucket)>0) {
			continue;
		}
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
			if (currOther->_inuse != 1 || currOther->type != Type_Tower) { continue; }
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

Entity* techno_damage(Entity* self, Entity* target) {
	int damageLeft;
	damageLeft = self->damage;
	Entity* child = NULL;
	if (target->_inuse != 1) { return; }
	target->health -= damageLeft;
	if (target->health < 0) {
		child = target->die(target);
		damageLeft = abs(target->health);
		while (damageLeft > 0) {
			if (!child) {
				break;
			}
			child->health -= damageLeft;
			if (child->health < 0) {
				damageLeft = abs(child->health);
				child = child->die(child);
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
		return child;
	}
	else if (target->health > 0) {
		return target;
	}
	else {
		return target->die(target);
	}
}

char* getUpgradeOneDesc(Entity* tower)
{
	switch ((TowerTypes)tower->data)
	{
	case TT_Stinger:
		return getStingerUpgradeDesc(tower, 0);
		break;
	case TT_Laser:
		return getLaserUpgradeDesc(tower, 0);
		break;
	case TT_Music:
		return getMusicUpgradeDesc(tower, 0);
		break;
	case TT_Slingshot:
		return getSlingshotUpgradeDesc(tower, 0);
		break;
	case TT_Snowglobe:
		return getSnowUpgradeDesc(tower, 0);
		break;
	case TT_Techno:
		return getTechnoUpgradeDesc(tower, 0);
		break;
	case TT_Water:
		return getWaterUpgradeDesc(tower, 0);
		break;
	default:
		return "No Upgrades";
		break;
	}
}

char* getUpgradeTwoDesc(Entity* tower)
{
	switch ((TowerTypes)tower->data)
	{
	case TT_Stinger:
		return getStingerUpgradeDesc(tower, 1);
		break;
	case TT_Laser:
		return getLaserUpgradeDesc(tower, 1);
		break;
	case TT_Music:
		return getMusicUpgradeDesc(tower, 1);
		break;
	case TT_Slingshot:
		return getSlingshotUpgradeDesc(tower, 1);
		break;
	case TT_Snowglobe:
		return getSnowUpgradeDesc(tower, 1);
		break;
	case TT_Techno:
		return getTechnoUpgradeDesc(tower, 1);
		break;
	case TT_Water:
		return getWaterUpgradeDesc(tower, 1);
		break;
	default:
		return "No Upgrades";
		break;
	}
}

int getUpgradeTwoPrice(Entity* tower)
{
	switch ((TowerTypes)tower->data)
	{
	case TT_Stinger:
		return getStingerUpgradeCost(tower, 1);
		break;
	case TT_Laser:
		return getLaserUpgradeCost(tower, 1);
		break;
	case TT_Music:
		return getMusicUpgradeCost(tower, 1);
		break;
	case TT_Slingshot:
		return getSlingshotUpgradeCost(tower, 1);
		break;
	case TT_Snowglobe:
		return getSnowUpgradeCost(tower, 1);
		break;
	case TT_Techno:
		return getTechnoUpgradeCost(tower, 1);
		break;
	case TT_Water:
		return getWaterUpgradeCost(tower, 1);
		break;
	default:
		return -1;
		break;
	}
}

int getUpgradeOnePrice(Entity* tower)
{
	switch ((TowerTypes)tower->data)
	{
	case TT_Stinger:
		return getStingerUpgradeCost(tower, 0);
		break;
	case TT_Laser:
		return getLaserUpgradeCost(tower, 0);
		break;
	case TT_Music:
		return getMusicUpgradeCost(tower, 0);
		break;
	case TT_Slingshot:
		return getSlingshotUpgradeCost(tower, 0);
		break;
	case TT_Snowglobe:
		return getSnowUpgradeCost(tower, 0);
		break;
	case TT_Techno:
		return getTechnoUpgradeCost(tower, 0);
		break;
	case TT_Water:
		return getWaterUpgradeCost(tower, 0);
		break;
	default:
		return -1;
		break;
	}
}

void swarm_kill_all() {
	Entity* currEntity;
	Bucket* currBucket;
	List* optiBuckets;
	int i, j;
	optiBuckets = get_loaded_level()->optimalBuckets;
	for (i = 0; i < optiBuckets->count; i++) {
		currBucket = gfc_list_get_nth(optiBuckets, i);
		for (j = 0; j < currBucket->entities->count; j++) {
			currEntity = gfc_list_get_nth(currBucket->entities, j);
			if (currEntity->_inuse != 1 || currEntity->type != Type_Enemy) {
				continue;
			}
			switch ((EnemyTypes)currEntity->flags)
			{
			case ET_RED:
				level_addCash(1.0f);
				break;
			case ET_BLUE:
				level_addCash(2.0f);
				break;
			case ET_GREEN:
				level_addCash(3.0f);
				break;
			case ET_YELLOW:
				level_addCash(4.0f);
				break;
			case ET_SUPER:
				level_addCash(50.0f);
				break;
			default:
				break;
			}
			gf2d_entity_free(currEntity);
		}
	}
}

void timeWarp_slow_all() {
	Entity* currEntity;
	Bucket* currBucket;
	List* optiBuckets;
	List* alreadySlowed;
	int i, j;
	optiBuckets = get_loaded_level()->optimalBuckets;
	alreadySlowed = gfc_list_new();
	for (i = 0; i < optiBuckets->count; i++) {
		currBucket = gfc_list_get_nth(optiBuckets, i);
		for (j = 0; j < currBucket->entities->count; j++) {
			currEntity = gfc_list_get_nth(currBucket->entities, j);
			if (currEntity->_inuse != 1 || currEntity->type != Type_Enemy || gfc_list_in_list(alreadySlowed, currEntity)>=0) {
				continue;
			}
			alreadySlowed = gfc_list_append(alreadySlowed, currEntity);

			vector2d_normalize(&currEntity->velocity);
			currEntity->speed = currEntity->maxSpeed / 3.5f;
			currEntity->velocity.x *= currEntity->speed;
			currEntity->velocity.y *= currEntity->speed;
			currEntity->cooldown = 6.0f;
		}
	}
}

#pragma region StingerUpgradeMethods
char* getStingerUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Stinger) {
		slog("Invalid stinger passed to getStingerUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getStingerUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed +15%%";
		}
		else {
			return "Travel +20%%";
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed +25%%";
		}
		else {
			return "Pierce +3";
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			return "+2 shots";
		}
		else {
			return "Range +15%%";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getStingerUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Stinger) {
		slog("Invalid stinger passed to getStingerUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getStingerUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 90;
		}
		else {
			return 75;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return 125;
		}
		else {
			return 175;
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			return 175;
		}
		else {
			return 100;
		}
	}
	else {
		return -1;
	}
}

void applyStingerUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Stinger) {
		slog("Invalid stinger passed to applyStingerUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyStingerUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->fireRate *= 0.85f;
		}
		else {
			tower->upgradeID = 2; 
			tower->distanceLeft *= 1.2f;
			tower->distanceLeft *= 1.2f;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
			tower->fireRate *= 0.75f;
		}
		else {
			tower->upgradeID = 4;
			tower->health += 3;
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
		}
		else {
			tower->upgradeID = 6;
			tower->shootRadius.radius *= 1.15f;
			setSeekBuckets(tower);
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region LaserUpgradeMethods
char* getLaserUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Laser) {
		slog("Invalid laser passed to getLaserUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getLaserUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed +15%%";
		}
		else {
			return "Pierce +1";
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return "Explodo-shot";
		}
		else {
			return "Damage +2";
		}
	}
	else if (tower->upgradeID == 2) {//pierce path
		if (upgradeNum == 0) {
			return "Pierce +2";
		}
		else {
			return "Damage +2";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getLaserUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Laser) {
		slog("Invalid laser passed to getLaserUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getLaserUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 90;
		}
		else {
			return 75;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return 200;
		}
		else {
			return 120;
		}
	}
	else if (tower->upgradeID == 2) {//pierce path
		if (upgradeNum == 0) {
			return 130;
		}
		else {
			return 150;
		}
	}
	else {
		return -1;
	}
}

void applyLaserUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Laser) {
		slog("Invalid laser passed to applyLaserUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyLaserUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->fireRate *= 0.85f;
		}
		else {
			tower->upgradeID = 2;
			tower->health += 1;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
		}
		else {
			tower->upgradeID = 4;
			tower->damage += 2;
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
			tower->health += 2;
		}
		else {
			tower->upgradeID = 6;
			tower->damage += 2;
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region MusicUpgradeMethods
char* getMusicUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Music) {
		slog("Invalid laser passed to getMusicUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getMusicUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed Buff +10%%";
		}
		else {
			return "Damage Buff +1";
		}
	}
	else if (tower->upgradeID == 1) {//speed buff path
		if (upgradeNum == 0) {//first upgrade desc
			return "Range Buff +15%%";
		}
		else {
			return "Travel Buff +20%%";
		}
	}
	else if (tower->upgradeID == 2) {//damage bugg path
		if (upgradeNum == 0) {
			return "Damage Buff +1";
		}
		else {
			return "Pierce Buff +2";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getMusicUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Music) {
		slog("Invalid laser passed to getMusicUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getMusicUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 200;
		}
		else {
			return 250;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return 300;
		}
		else {
			return 250;
		}
	}
	else if (tower->upgradeID == 2) {//damage path
		if (upgradeNum == 0) {
			return 300;
		}
		else {
			return 400;
		}
	}
	else {
		return -1;
	}
}

void applyMusicUpgrade(Entity* tower, int upgradeNum) {
	List* alreadyBuffed = tower->noTouch;
	Entity* currAlly;
	int i;
	if (!tower || (TowerTypes)tower->data != TT_Music) {
		slog("Invalid music bee passed to applyMusicUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyMusicUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->fireRate *= 20.0f / 17.0f;
				currAlly->fireRate *= 0.75f; // this is to undo original 15% boost and properly apply new 25% boost
			}
		}
		else {
			tower->upgradeID = 2;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->damage += 1;
			}
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->shootRadius.radius *= 1.15f;
				currAlly->distanceLeft *= 1.10f;
				setSeekBuckets(currAlly);
			}
		}
		else {
			tower->upgradeID = 4;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->distanceLeft *= 1.20f;
				currAlly->distanceLeft *= 1.20f;
			}
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->damage += 1;
			}
		}
		else {
			tower->upgradeID = 6;
			for (i = 0; i < alreadyBuffed->count; i++) {
				currAlly = gfc_list_get_nth(alreadyBuffed, i);
				currAlly->health += 2;
			}
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region SlingshotUpgradeMethods
char* getSlingshotUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Slingshot) {
		slog("Invalid laser passed to getSlingshotUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getSlingshotUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed +10%%";
		}
		else {
			return "Homing shots";
		}
	}
	else if (tower->upgradeID == 1) {//speed buff path
		if (upgradeNum == 0) {//first upgrade desc
			return "Range +15%%";
		}
		else {
			return "Speed +20%%";
		}
	}
	else if (tower->upgradeID == 2) {//damage bugg path
		if (upgradeNum == 0) {
			return "Damage +1";
		}
		else {
			return "Pierce +2";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getSlingshotUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Slingshot) {
		slog("Invalid laser passed to getSlingshotUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getSlingshotUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 95;
		}
		else {
			return 180;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			return 160;
		}
		else {
			return 145;
		}
	}
	else if (tower->upgradeID == 2) {//homing path
		if (upgradeNum == 0) {
			return 180;
		}
		else {
			return 240;
		}
	}
	else {
		return -1;
	}
}

void applySlingshotUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Slingshot) {
		slog("Invalid slingshot passed to applySlingshotUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applySlingshotUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->fireRate *= 0.90f;
		}
		else {
			tower->upgradeID = 2;
			tower->distanceLeft *= 1.75f;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->shootRadius.radius *= 1.15f;
			tower->distanceLeft *= 1.15f;
			setSeekBuckets(tower);
		}
		else {
			tower->upgradeID = 4;
			tower->fireRate *= 0.80f;
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
			tower->damage += 1;
		}
		else {
			tower->upgradeID = 6;
			tower->health += 2;
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region SnowUpgradeMethods
char* getSnowUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Snowglobe) {
		slog("Invalid laser passed to getSnowUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getSnowUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Range +15%%";
		}
		else {
			return "Slow +15%%";
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {
			return "Ice Cannon";
		}
		else {
			return "Range +20%%";
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return "Slow +35%%";
		}
		else {
			return "Flash Freeze";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getSnowUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Snowglobe) {
		slog("Invalid laser passed to getSnowUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getSnowUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 130;
		}
		else {
			return 165;
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {//first upgrade desc
			return 350;
		}
		else {
			return 400;
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return 250;
		}
		else {
			return 400;
		}
	}
	else {
		return -1;
	}
}

void applySnowUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Snowglobe) {
		slog("Invalid stinger passed to applyStingerUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyStingerUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->shootRadius.radius *= 1.15f;
			setSeekBuckets(tower);
		}
		else {
			tower->upgradeID = 2;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
			tower->fireRate *= 0.8f;
		}
		else {
			tower->upgradeID = 4;
			tower->shootRadius.radius *= 1.35f;
			setSeekBuckets(tower);
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
		}
		else {
			tower->upgradeID = 6;
			tower->abilityRate = 7.0f;
			tower->abilityCooldown = 4.0f;
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region TechnoUpgradeMethods
char* getTechnoUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Techno) {
		slog("Invalid laser passed to getTechnoUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getTechnoUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Speed +25%%";
		}
		else {
			return "Damage +2";
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {
			return "Speed +35%%";
		}
		else {
			return "Recovery Bots";
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return "Damage +3";
		}
		else {
			return "Tri-Shot";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getTechnoUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Techno) {
		slog("Invalid laser passed to getTechnoUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getTechnoUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 230;
		}
		else {
			return 250;
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {//first upgrade desc
			return 350;
		}
		else {
			return 400;
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return 400;
		}
		else {
			return 500;
		}
	}
	else {
		return -1;
	}
}

void applyTechnoUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Techno) {
		slog("Invalid stinger passed to applyStingerUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyStingerUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->fireRate *= 0.75f;
		}
		else {
			tower->upgradeID = 2;
			tower->damage += 2;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
			tower->fireRate *= 0.65f;
		}
		else {
			tower->upgradeID = 4;
			level_addRegen(1);
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
			tower->damage += 3;
		}
		else {
			tower->upgradeID = 6;
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

#pragma region WaterUpgradeMethods
char* getWaterUpgradeDesc(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Water) {
		slog("Invalid laser passed to getWaterUpgradeDesc");
		return "InvStngEnt";
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getWaterUpgradeDesc", upgradeNum);
		return "InvStngNum";
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return "Range +15%%";
		}
		else {
			return "Damage +1";
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {
			return "Healing Wave";
		}
		else {
			return "Speed +30%%";
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return "Slowing wave";
		}
		else {
			return "Speed +15%%";
		}
	}
	else {
		return "Fully Upgraded!";
	}
}

int getWaterUpgradeCost(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Water) {
		slog("Invalid laser passed to getWaterUpgradeCost");
		return -1;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in getWaterUpgradeCost", upgradeNum);
		return -1;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			return 100;
		}
		else {
			return 135;
		}
	}
	else if (tower->upgradeID == 1) {
		if (upgradeNum == 0) {//first upgrade desc
			return 350;
		}
		else {
			return 275;
		}
	}
	else if (tower->upgradeID == 2) {
		if (upgradeNum == 0) {
			return 260;
		}
		else {
			return 320;
		}
	}
	else {
		return -1;
	}
}

void applyWaterUpgrade(Entity* tower, int upgradeNum) {
	if (!tower || (TowerTypes)tower->data != TT_Water) {
		slog("Invalid stinger passed to applyStingerUpgrade");
		return;
	}
	else if (upgradeNum > 1 || upgradeNum < 0) {
		slog("Invalid upgradeNum %i in applyStingerUpgrade", upgradeNum);
		return;
	}

	if (tower->upgradeID == 0) {//base tower state, no upgrades
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 1;
			tower->shootRadius.radius *= 1.15f;
			setSeekBuckets(tower);
		}
		else {
			tower->upgradeID = 2;
			tower->damage += 1;
		}
	}
	else if (tower->upgradeID == 1) {//speed path
		if (upgradeNum == 0) {//first upgrade desc
			tower->upgradeID = 3;
			level_addRegen(1);
		}
		else {
			tower->upgradeID = 4;
			tower->fireRate *= 0.70f;
		}
	}
	else if (tower->upgradeID == 2) {//speed path
		if (upgradeNum == 0) {//travel upgrade desc
			tower->upgradeID = 5;
		}
		else {
			tower->upgradeID = 6;
			tower->fireRate *= 0.20f;
		}
	}
	else {
		return "Fully Upgraded!";
	}
}
#pragma endregion

void upgradeOne_buy() {
	Entity* selected;
	int price;
	selected = gf2d_entity_get_selected();
	if (!selected) {
		slog("No selected entity!");
		return;
	}
	if (selected->type != Type_Tower) {
		slog("Selected entity is not a tower!");
		return;
	}
	price = getUpgradeOnePrice(selected);
	if (get_loaded_level()->playerCash - price < 0.0f) {
		return;
	}
	else {
		level_addCash(-price);
	}

	switch ((TowerTypes)selected->data)
	{
	case TT_Stinger:
		applyStingerUpgrade(selected, 0);
		break;
	case TT_Laser:
		applyLaserUpgrade(selected, 0);
		break;
	case TT_Music:
		applyMusicUpgrade(selected, 0);
		break;
	case TT_Slingshot:
		applySlingshotUpgrade(selected, 0);
		break;
	case TT_Snowglobe:
		applySnowUpgrade(selected, 0);
		break;
	case TT_Techno:
		applyTechnoUpgrade(selected, 0);
		break;
	case TT_Water:
		applyWaterUpgrade(selected, 0);
		break;
	default:
		break;
	}

	gf2d_entity_set_selected(gf2d_entity_get_selected());//update upgrade window
}

void upgradeTwo_buy() {
	Entity* selected;
	int price;
	selected = gf2d_entity_get_selected();
	if (!selected) {
		slog("No selected entity!");
		return;
	}
	if (selected->type != Type_Tower) {
		slog("Selected entity is not a tower!");
		return;
	}
	price = getUpgradeTwoPrice(selected);
	if (get_loaded_level()->playerCash - price < 0.0f) {
		return;
	}
	else {
		level_addCash(-price);
	}

	switch ((TowerTypes)selected->data)
	{
	case TT_Stinger:
		applyStingerUpgrade(selected, 1);
		break;
	case TT_Laser:
		applyLaserUpgrade(selected, 1);
		break;
	case TT_Music:
		applyMusicUpgrade(selected, 1);
		break;
	case TT_Slingshot:
		applySlingshotUpgrade(selected, 1);
		break;
	case TT_Snowglobe:
		applySnowUpgrade(selected, 1);
		break;
	case TT_Techno:
		applyTechnoUpgrade(selected, 1);
		break;
	case TT_Water:
		applyWaterUpgrade(selected, 1);
		break;
	default:
		break;
	}

	gf2d_entity_set_selected(gf2d_entity_get_selected());//update upgrade window
}