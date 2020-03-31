#include "projectiles.h"
#include "bucket.h"
#include "gf2d_graphics.h"
#include "tower.h"

Entity* stingerBolt_spawn(Entity* parent)
{
	Entity* bolt;
	Vector3D direction;
	bolt = gf2d_entity_new();
	gf2d_actor_load(&bolt->actor, "actors/projectiles/stingerbolt.actor");
	bolt->rotation.x = bolt->actor.sprite->frame_w / 2;
	bolt->rotation.y = bolt->actor.sprite->frame_h / 2;
	bolt->rotation.z = parent->rotation.z+180;
	vector3d_set_angle_by_radians(&direction, (bolt->rotation.z)*GFC_DEGTORAD);
	bolt->speed = 5.0f;
	bolt->velocity.x = direction.x * bolt->speed;
	bolt->velocity.y = direction.y * bolt->speed;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 20.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = parent->distanceLeft;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = parent->damage;
	bolt->health = parent->health;
	bolt->die = boltDie;
	return bolt;
}

void boltMove(Entity* self) {
	vector2d_add(self->position, self->position, self->velocity);
	vector2d_copy(self->boundingBox.position, self->position);
	vector2d_copy(self->shootRadius.position, self->position);
	self->distanceLeft -= vector2d_magnitude_squared(self->velocity);
}

void boltThink(Entity* self) {
	if (self->position.x < 0 || self->position.x>1200 || self->position.y < 0 || self->position.y>720 || self->health<=0 || self->distanceLeft<=0.0f) {
		self->die(self);
	}
}

void boltTouch(Entity* self, Entity* other) {
	Entity* child = NULL;
	int damageLeft = self->damage;
	if (!self || !other) {
		return;
	}
	if (self->health <= 0) {
		return;
	}
	if (other->type != Type_Enemy || other->_inuse!=1) {
		return;
	}
	if (!other->die) {
		return;
	}

	if (other->health <= 0) {
		return;
	}
	if (gfc_list_in_list(self->noTouch, other) >= 0) {
		return;
	}
	other->health -= damageLeft;
	self->health -= 1;
	if (other->health <= 0){
		damageLeft = abs(other->health);
	}
	else {
		self->noTouch = gfc_list_append(self->noTouch, other);
		damageLeft = 0;
	}
	while (damageLeft>0) {
		child = other->die(other);
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
	if (child) {
		self->noTouch = gfc_list_append(self->noTouch, child);
	}
}

void boltDie(Entity* self) {
	gf2d_entity_free(self);
}

Entity* slingerPellet_spawn(Entity* parent)
{
	Entity* bolt;
	Vector3D direction;
	bolt = gf2d_entity_new();
	gf2d_actor_load(&bolt->actor, "actors/projectiles/slingerpellet.actor");
	bolt->rotation.x = bolt->actor.sprite->frame_w / 2;
	bolt->rotation.y = bolt->actor.sprite->frame_h / 2;
	bolt->rotation.z = parent->rotation.z + 180;
	vector3d_set_angle_by_radians(&direction, (bolt->rotation.z) * GFC_DEGTORAD);
	bolt->speed = 8.0f;
	bolt->velocity.x = direction.x * bolt->speed;
	bolt->velocity.y = direction.y * bolt->speed;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 10.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = parent->distanceLeft;
	if (parent->upgradeID == 2 || parent->upgradeID == 5 || parent->upgradeID == 6) {
		bolt->think = homingThink;
	}
	else {
		bolt->think = boltThink;
	}
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = parent->damage;
	bolt->health = parent->health;
	bolt->die = boltDie;
	return bolt;
}

Entity* laserlaser_spawn(Entity* parent)
{
	Entity* bolt;
	Vector3D direction;
	bolt = gf2d_entity_new();
	gf2d_actor_load(&bolt->actor, "actors/projectiles/laserlaser.actor");
	bolt->rotation.x = bolt->actor.sprite->frame_w / 2;
	bolt->rotation.y = bolt->actor.sprite->frame_h / 2;
	bolt->rotation.z = parent->rotation.z + 180;
	vector3d_set_angle_by_radians(&direction, (bolt->rotation.z) * GFC_DEGTORAD);
	bolt->speed = 7.0f;
	bolt->velocity.x = direction.x * bolt->speed;
	bolt->velocity.y = direction.y * bolt->speed;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 20.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = parent->distanceLeft;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = parent->damage;
	bolt->health = parent->health;
	if (parent->upgradeID == 3) {
		bolt->die = laserExplosionDie;
	}
	else {
		bolt->die = boltDie;
	}
	return bolt;
}

void laserExplosionDie(Entity* self) {
	List* targets;
	Entity* target;
	int i;
	self->shootRadius.position = self->position;
	self->shootRadius.radius = 75.0f;
	setSeekBuckets(self);
	targets = inRange(self);
	self->damage = 1;
	if (targets && targets->count > 0) {
		for (i = 0; i < targets->count; i++) {
			target = gfc_list_get_nth(targets, i);
			techno_damage(self, target);
		}
	}
	explosion_spawn(self);
	gf2d_entity_free(self);
}

Entity* waterwave_spawn(Entity* parent)
{
	Entity* wave;
	Vector3D direction;
	wave = gf2d_entity_new();
	gf2d_actor_load(&wave->actor, "actors/projectiles/waterwave.actor");
	wave->position = parent->position;
	wave->move = waveMove;
	wave->distanceLeft = parent->shootRadius.radius;
	wave->think = waveThink;
	wave->type = Type_Projectile;
	wave->touch = NULL;
	wave->die = boltDie;

	wave->fireRate = parent->fireRate * gf2d_graphics_get_frames_per_second();
	return wave;
}

Entity* explosion_spawn(Entity* parent)
{
	Entity* wave;
	wave = gf2d_entity_new();
	gf2d_actor_load(&wave->actor, "actors/projectiles/explosion.actor");
	wave->position = parent->position;
	wave->move = waveMove;
	wave->distanceLeft = parent->shootRadius.radius;
	wave->think = waveThink;
	wave->type = Type_Projectile;
	wave->touch = NULL;
	wave->die = boltDie;

	wave->fireRate = 0.5f * gf2d_graphics_get_frames_per_second();
	return wave;
}

void waveMove(Entity* self) {
	return; //waves don't move
}

void waveThink(Entity* self) {
	float radius;
	float ratio;
	radius = (self->actor.sprite->frame_w / 2) * (self->actor.al->scale.x);
	if (radius < self->distanceLeft) {
		radius += self->distanceLeft / self->fireRate;
		self->actor.al->scale.x = radius/ (self->actor.sprite->frame_w / 2);
		self->actor.al->scale.y = radius/ (self->actor.sprite->frame_w / 2);
	}
	else {
		self->actor.al->scale.x = 1.0f;
		self->actor.al->scale.y = 1.0f;
		self->die(self);
	}
}

Entity* snowwave_spawn(Entity* parent)
{
	Entity* wave;
	Vector3D direction;
	wave = gf2d_entity_new();
	gf2d_actor_load(&wave->actor, "actors/projectiles/snowwave.actor");
	wave->position = parent->position;
	wave->move = waveMove;
	wave->distanceLeft = parent->shootRadius.radius;
	wave->think = waveThink;
	wave->type = Type_Projectile;
	wave->touch = NULL;
	wave->die = boltDie;

	wave->fireRate = parent->fireRate * gf2d_graphics_get_frames_per_second();
	return wave;
}

Entity* musicnote_spawn(Entity* parent)
{
	Entity* bolt;
	Vector3D direction;
	bolt = gf2d_entity_new();
	gf2d_actor_load(&bolt->actor, "actors/projectiles/musicnote.actor");
	bolt->rotation.x = bolt->actor.sprite->frame_w / 2;
	bolt->rotation.y = bolt->actor.sprite->frame_h / 2;
	bolt->rotation.z = parent->rotation.z + 180;
	vector3d_set_angle_by_radians(&direction, (bolt->rotation.z) * GFC_DEGTORAD);
	bolt->speed = 4.0f;
	bolt->velocity.x = direction.x * bolt->speed;
	bolt->velocity.y = direction.y * bolt->speed;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 20.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = parent->distanceLeft;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = parent->damage;
	bolt->health = parent->health;
	bolt->die = boltDie;
	if (bolt->rotation.z >= 360.0f) {
		bolt->rotation.z = fmodf(bolt->rotation.z, 360.0f);
	}
	if (bolt->rotation.z >= 90.0f && bolt->rotation.z <= 270.0f) {
		bolt->flip = vector2d(1, 1);
	}
	else {
		bolt->flip = vector2d(0, 0);
	}
	return bolt;
}
/*
Entity* musicTrap_spawn(Entity* parent) {
	Path2D* path;
	//calculate lines in area

	path = gfc_list_get_nth(get_loaded_level()->paths, get_loaded_level()->currPath);
	//set initial direction, velocity, and distance left
	//set think to trap think and move to bolt move
}

void musicTrap_think(Entity* self) {
	//initial think checks to see if we're at point yet
	//if not, do nothing
	//if so, set touch to bolt touch
	//if so, set think to bolt think
	//if so, set velocity to zero
}
*/
void homingThink(Entity* self) {
	Entity* target;
	Vector3D direction;
	target = findClosest(self);
	if (!target) { return; }
	gf2d_entity_look_at(self, target);
	vector3d_set_angle_by_radians(&direction, (self->rotation.z+180) * GFC_DEGTORAD);
	self->velocity.x = direction.x * self->speed;
	self->velocity.y = direction.y * self->speed;
	boltThink(self);
}