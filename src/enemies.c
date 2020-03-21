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
	return ent;
}

Entity* redSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/red.actor");
	ent->velocity.x *= 5;
	ent->velocity.y *= 5;
	return ent;
}

Entity* blueSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/blue.actor");
	ent->velocity.x *= 7;
	ent->velocity.y *= 7;
	return ent;
}

Entity* greenSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/green.actor");
	ent->velocity.x *= 10;
	ent->velocity.y *= 10;
	return ent;
}

Entity* yellowSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/yellow.actor");
	ent->velocity.x *= 14;
	ent->velocity.y *= 14;
	return ent;
}

Entity* superSpawn()
{
	Entity* ent;
	ent = commonSpawn();
	gf2d_actor_load(&ent->actor, "actors/enemies/super.actor");
	ent->velocity.x *= 4;
	ent->velocity.y *= 4;
	ent->health = 50;
	return ent;
}

void redThink(Entity* self)
{
	//You were here
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

void redMove(Entity* self)
{
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
