#include "enemies.h"
#include "level.h"

Entity* commonSpawn();

Entity* commonSpawn() {
	Entity* ent;
	Vector2D deltas;
	Line2D* line;
	ent = gf2d_entity_new();
	ent->path = nextPath();
	ent->data = 0; //enemy data will keep track of what line of the path they're on
	line = gfc_list_get_nth(ent->path->lines, 0);
	vector2d_sub(deltas, line->end, line->start);
	vector2d_normalize(&deltas);
	vector2d_copy(ent->velocity, deltas);
	ent->health = 1;
	ent->distanceLeft = LengthLine2D(*line);
	ent->boundingBox.radius = 20.0f;
	return ent;
}

Entity* redSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/red.actor");
	ent->speed = 0.8f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	return ent;
}

Entity* blueSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/blue.actor");
	ent->speed = 1.2f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	return ent;
}

Entity* greenSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/green.actor");
	ent->speed = 1.4f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	return ent;
}

Entity* yellowSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/yellow.actor");
	ent->speed = 1.8f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->move = flowerMove;
	return ent;
}

Entity* superSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/super.actor");
	ent->speed = 0.3f;
	ent->velocity.x *= ent->speed;
	ent->velocity.y *= ent->speed;
	ent->health = 50;
	ent->move = flowerMove;
	ent->boundingBox.radius = 45.0f;
	return ent;
}

void redThink(Entity* self)
{
}

void blueThink(Entity* self)
{
}

void greenThink(Entity* self)
{
}

void yellowThink(Entity* self)
{
}

void superThink(Entity* self)
{
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
	return NULL;
}

Entity* blueDie(Entity* self)
{
	return NULL;
}

Entity* greenDie(Entity* self)
{
	return NULL;
}

Entity* yellowDie(Entity* self)
{
	return NULL;
}

Entity* superDie(Entity* self)
{
	return NULL;
}
