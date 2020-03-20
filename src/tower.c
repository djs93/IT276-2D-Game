#include "tower.h"
#include "simple_logger.h"
#include "local.h"
//#include "level.h"
#include "gf2d_mouse.h"

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
	self->position.x = mx;
	self->position.y = my;
	self->name = "placement";
	self->data = (int)type;

	return self;
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
	self->position = gf2d_mouse_get_position();
	if (pathCollision(self)) {
		self->actor.color = vector4d(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		self->actor.color = vector4d(0.0f, 1.0f, 0.0f, 1.0f);
	}
}
#pragma endregion

#pragma region PlacementExtras
void placement_detach() {
	Entity* placeEnt = find_entity("placement");
	if (!placeEnt){
		slog("No place entity!");
		return;
	}
	switch ((TowerTypes)placeEnt->data)
	{
	case TT_Stinger:
		placeEnt->think = stinger_think;
		placeEnt->name = "stinger";
		break;
	case TT_Slingshot:
		placeEnt->think = slingshot_think;
		placeEnt->name = "slingshot";
		break;
	case TT_Laser:
		placeEnt->think = laser_think;
		placeEnt->name = "laser";
		break;
	case TT_Water:
		placeEnt->think = water_think;
		placeEnt->name = "water";
		break;
	case TT_Techno:
		placeEnt->think = techno_think;
		placeEnt->name = "techno";
		break;
	case TT_Snowglobe:
		placeEnt->think = snowglobe_think;
		placeEnt->name = "snowglobe";
		break;
	case TT_Music:
		placeEnt->think = music_think;
		placeEnt->name = "music";
		break;
	default:
		placeEnt->think = stinger_think;
		placeEnt->name = "stinger";
		slog("Invalid detach tower type! Defaulting...");
		break;
	}
}
#pragma endregion

