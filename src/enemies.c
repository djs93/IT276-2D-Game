#include "enemies.h"
#include "level.h"
#include "bucket.h"
#include "gf2d_graphics.h"

Entity* commonSpawn(Entity* parent);

Entity* commonSpawn(Entity* parent) {
	Entity* ent;
	Vector2D deltas;
	Line2D* line;
	ent = gf2d_entity_new();
	if (parent) {
		ent->path = parent->path;
		ent->data = parent->data;
		vector2d_copy(deltas, parent->velocity);
		vector2d_normalize(&deltas);
		vector2d_copy(ent->position, parent->position);
		vector2d_copy(ent->boundingBox.position, parent->boundingBox.position);
		ent->distanceLeft = parent->distanceLeft;
	}
	else {
		ent->path = nextPath();
		ent->data = 0; //enemy data will keep track of what line of the path they're on
		line = gfc_list_get_nth(ent->path->lines, 0);
		vector2d_sub(deltas, line->end, line->start);
		vector2d_normalize(&deltas);
		vector2d_copy(ent->position, line->start);
		vector2d_copy(ent->boundingBox.position, line->start);
		ent->distanceLeft = LengthLine2D(*line);
	}
	vector2d_copy(ent->velocity, deltas);
	ent->health = 1;
	ent->boundingBox.radius = 20.0f;
	ent->type = Type_Enemy;
	bucket_update(ent, NULL);
	return ent;
}

Entity* redSpawn(Entity* parent)
{
	Entity* ent;
	ent = commonSpawn(parent);
	gf2d_actor_load(&ent->actor, "actors/enemies/red.actor");
	ent->speed = 0.8f;
	ent->maxSpeed = 0.8f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	ent->name = "red";
	ent->die = redDie;
	ent->think = redThink;
	return ent;
}

Entity* blueSpawn(Entity* parent)
{
	Entity* ent;
	ent = commonSpawn(parent);
	gf2d_actor_load(&ent->actor, "actors/enemies/blue.actor");
	ent->speed = 1.2f;
	ent->maxSpeed = 1.2f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	ent->name = "blue";
	ent->die = blueDie;
	ent->think = blueThink;
	return ent;
}

Entity* greenSpawn(Entity* parent)
{
	Entity* ent;
	ent = commonSpawn(parent);
	gf2d_actor_load(&ent->actor, "actors/enemies/green.actor");
	ent->speed = 1.4f;
	ent->maxSpeed = 1.4f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	ent->name = "green";
	ent->die = greenDie;
	ent->think = greenThink;
	return ent;
}

Entity* yellowSpawn(Entity* parent)
{
	Entity* ent;
	ent = commonSpawn(parent);
	gf2d_actor_load(&ent->actor, "actors/enemies/yellow.actor");
	ent->speed = 1.8f;
	ent->maxSpeed = 1.8f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	ent->name = "yellow";
	ent->die = yellowDie;
	ent->think = yellowThink;
	return ent;
}

Entity* superSpawn(Entity* parent)
{
	Entity* ent;
	ent = commonSpawn(parent);
	gf2d_actor_load(&ent->actor, "actors/enemies/super.actor");
	ent->speed = 0.3f;
	ent->maxSpeed = 0.3f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->health = 50;
	ent->move = flowerMove;
	ent->boundingBox.radius = 45.0f;
	ent->name = "super";
	ent->die = superDie;
	ent->think = superThink;
	return ent;
}

void redThink(Entity* self)
{
	if (self->health <= 0) {
		self->die(self);
	}
	if (self->cooldown <= 0.0f && self->speed != self->maxSpeed) {
		vector2d_normalize(&self->velocity);
		self->speed = self->maxSpeed;
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void blueThink(Entity* self)
{
	if (self->health <= 0) {
		self->die(self);
	}
	if (self->cooldown <= 0.0f) {
		vector2d_normalize(&self->velocity);
		self->speed = self->maxSpeed;
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void greenThink(Entity* self)
{
	if (self->health <= 0) {
		self->die(self);
	}
	if (self->cooldown <= 0.0f) {
		vector2d_normalize(&self->velocity);
		self->speed = self->maxSpeed;
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void yellowThink(Entity* self)
{
	if (self->health <= 0) {
		self->die(self);
	}
	if (self->cooldown <= 0.0f) {
		vector2d_normalize(&self->velocity);
		self->speed = self->maxSpeed;
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void superThink(Entity* self)
{
	if (self->health <= 0) {
		self->die(self);
	}
	if (self->cooldown <= 0.0f) {
		vector2d_normalize(&self->velocity);
		self->speed = self->maxSpeed;
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;
	}
	else {
		self->cooldown -= gf2d_graphics_get_milli_delta();
	}
}

void flowerMove(Entity* self)
{
	float moveDistLeft, overflowDist;
	int index;
	Line2D* line;
	Vector2D deltas;
	Vector2D newPos;
	line = NULL;
	moveDistLeft = self->distanceLeft - self->speed;
	if (moveDistLeft <= 0.000001f) { // go onto next line
		//grab next line and set vel
		moveDistLeft = fabsf(moveDistLeft);
		overflowDist = moveDistLeft;
		while (moveDistLeft >= 0.000001f) {
			index = (int)self->data;
			self->data = index + 1;
			index += 1;
			if (index >= gfc_list_get_count(self->path->lines)) {
				//reached end of path, do damage
				gf2d_entity_free(self);
				return;
			}
			else {
				line = gfc_list_get_nth(self->path->lines, index);
				moveDistLeft -= LengthLine2D(*line);
			}
		}
		self->distanceLeft = fabsf(moveDistLeft);
		vector2d_sub(deltas, line->end, line->start);
		vector2d_normalize(&deltas);
		vector2d_copy(self->velocity, deltas);
		self->velocity.x *= self->speed;
		self->velocity.y *= self->speed;

		newPos.x = line->start.x + deltas.x * overflowDist;
		newPos.y = line->start.y + deltas.y * overflowDist;
		vector2d_copy(self->position, newPos);
	}
	else { //same line
		self->distanceLeft = moveDistLeft;
		vector2d_add(self->position, self->position, self->velocity);
	}
	vector2d_copy(self->boundingBox.position, self->position);
}

void blueMove(Entity* self)
{
}

void greenMove(Entity* self)
{
}

void yellowMove(Entity* self)
{
}

void superMove(Entity* self)
{
}

Entity* redDie(Entity* self)
{
	level_addCash(1.0f);
	gf2d_entity_free(self);
	return NULL;
}

Entity* blueDie(Entity* self)
{
	Entity* ent;
	level_addCash(1.0f);
	ent = redSpawn(self);
	gf2d_entity_free(self);
	return ent;
}

Entity* greenDie(Entity* self)
{
	Entity* ent;
	level_addCash(1.0f);
	ent = blueSpawn(self);
	gf2d_entity_free(self);
	return ent;
}

Entity* yellowDie(Entity* self)
{
	Entity* ent;
	level_addCash(1.0f);
	ent = greenSpawn(self);
	gf2d_entity_free(self);
	return ent;
}

Entity* superDie(Entity* self)
{
	Entity* ent;
	level_addCash(50.0f);
	yellowSpawn(self);
	ent = yellowSpawn(self);
	ent->move(self);
	ent = yellowSpawn(self);
	ent->move(self);
	ent->move(self);
	ent = yellowSpawn(self);
	ent->move(self);
	ent->move(self);
	ent->move(self);
	gf2d_entity_free(self);
	return NULL;
}
