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
	bolt->velocity.x = direction.x * 3.0f;
	bolt->velocity.y = direction.y * 3.0f;
	bolt->boundingBox.position = parent->position;
	bolt->boundingBox.radius = 20.0f;
	bolt->position = parent->position;
	bolt->move = boltMove;
	bolt->distanceLeft = 10000.0f;
	bolt->think = boltThink;
	bolt->type = Type_Projectile;
	bolt->touch = boltTouch;
	bucket_update(bolt, NULL);
	bolt->noTouch = gfc_list_new();
	bolt->damage = 2;
	return bolt;
}

void boltMove(Entity* self) {
	vector2d_add(self->position, self->position, self->velocity);
	vector2d_copy(self->boundingBox.position, self->position);
	self->distanceLeft -= vector2d_magnitude_squared(self->velocity);
}

void boltThink(Entity* self) {
	if (self->position.x < 0 || self->position.x>1200 || self->position.y < 0 || self->position.y>720) {
		gf2d_entity_free(self);
	}
}

void boltTouch(Entity* self, Entity* other) {
	Entity* child;
	int damageLeft = self->damage;
	if (!self || !other) {
		return;
	}
	if (other->type != Type_Enemy || other->_inuse!=1) {
		return;
	}
	if (!other->die) {
		return;
	}
	if (gfc_list_in_list(self->noTouch, other) >= 0) {
		return;
	}
	other->health -= damageLeft;
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
			break;
		}
		else {
			self->noTouch = gfc_list_append(self->noTouch, child);
			damageLeft = 0;
		}
	}
}
