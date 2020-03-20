#include "tower.h"
#include "simple_logger.h"
#include "local.h"
#include "level.h"
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
	self = find_entity("placement");
	if (self) {
		return self;
	}
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
	self->boundingBox.radius = 15.0f * self->actor.al->scale.x;
	self->boundingBox.position = self->position;

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
	Bool colliding;
	self->position = gf2d_mouse_get_position();
	colliding = pathCollision(self);
	if (colliding) {
		self->colorShift = vector4d(255.0f, 0.0f, 0.0f, 255.0f);
	}
	else {
		self->colorShift = vector4d(0.0f, 255.0f, 0.0f, 255.0f);
	}
	self->boundingBox.position = self->position;
	if (!colliding&&gf2d_mouse_button_pressed(0)) {
		placement_detach(self);
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
		break;
	case TT_Slingshot:
		ent->think = slingshot_think;
		ent->name = "slingshot";
		ent->shootRadius.radius = 150.0f;
		break;
	case TT_Laser:
		ent->think = laser_think;
		ent->name = "laser";
		ent->shootRadius.radius = 150.0f;
		break;
	case TT_Water:
		ent->think = water_think;
		ent->name = "water";
		ent->shootRadius.radius = 150.0f;
		break;
	case TT_Techno:
		ent->think = techno_think;
		ent->name = "techno";
		ent->shootRadius.radius = 150.0f;
		break;
	case TT_Snowglobe:
		ent->think = snowglobe_think;
		ent->name = "snowglobe";
		ent->shootRadius.radius = 150.0f;
		break;
	case TT_Music:
		ent->think = music_think;
		ent->name = "music";
		ent->shootRadius.radius = 150.0f;
		break;
	default:
		ent->think = stinger_think;
		ent->name = "stinger";
		ent->shootRadius.radius = 150.0f;
		slog("Invalid detach tower type! Defaulting...");
		break;
	}
	ent->shootRadius.position = ent->position;
	ent->colorShift = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
}
#pragma endregion

