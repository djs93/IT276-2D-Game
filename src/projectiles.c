#include "projectiles.h"
#include "bucket.h"

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
	bolt->distanceLeft = 1500.0f;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = 2;
	bolt->health = 2;
	bolt->die = boltDie;
	return bolt;
}

void boltMove(Entity* self) {
	vector2d_add(self->position, self->position, self->velocity);
	vector2d_copy(self->boundingBox.position, self->position);
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
	bolt->distanceLeft = 2000.0f;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = 1;
	bolt->health = 1;
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
	bolt->speed = 6.0f;
	bolt->velocity.x = direction.x * bolt->speed;
	bolt->velocity.y = direction.y * bolt->speed;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 20.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = 2000.0f;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = 3;
	bolt->health = 4;
	bolt->die = boltDie;
	return bolt;
}