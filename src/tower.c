#include "tower.h"
#include "simple_logger.h"
#include "local.h"

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
	self = gf2d_entity_new();
	switch (type)
	{
	case TT_Stinger:
		gf2d_actor_load(&self->actor, "actors/stinger.actor");
		break;
	case TT_Slingshot:
		gf2d_actor_load(&self->actor, "actors/slingshot.actor");
		break;
	case TT_Laser:
		gf2d_actor_load(&self->actor, "actors/laser.actor");
		break;
	case TT_Water:
		gf2d_actor_load(&self->actor, "actors/water.actor");
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
}
#pragma endregion

#pragma region Thinks
void stinger_think(Entity* self){

}

void slingshot_think(Entity* self){

}

void laser_think(Entity* self){

}

void water_think(Entity* self){

}

void techno_think(Entity* self){

}

void snowglobe_think(Entity* self){

}

void music_think(Entity* self){

}

void placement_think(Entity* self) {
	self->position.x = mx;
	self->position.y = my;
}
#pragma endregion