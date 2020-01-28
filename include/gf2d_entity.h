#ifndef __GF2D_ENTITY_H__
#define __GF2D_ENTITY_H__

#include "gf2d_sprite.h"
//#include "local.h"
#include "Geometry2D.h"

typedef enum
{
	ES_Idle = 0,
	ES_Dying = 1,
	ES_Dead = 2
}EntityState;

typedef struct Entity_S
{
	char* name;			/**<name of the entity*/
	char* type;			/**Type of the entitiy*/
	int           _inuse;         /**<flag to keep track if this isntance is in use and should not be reassigned*/
	Sprite* sprite;          /**<the 3d model for this entity*/
	Vector3D         position;       /**<DO NOT DIRECTLY MODIFY - position of the entity in 3d space*/
	Vector3D         velocity;       /**<velocity of the entity in 3d space*/
	Vector3D         acceleration;   /**<acceleration of the entity in 3d space*/
	Vector3D         rotation;       /**<yaw, pitch, and roll of the entity*/
	Vector3D         scale;          /**<*please default to 1,1,1*/
	EntityState     state;          /**<current state of the entity*/
	void (*prethink)(struct Entity_S* self);   /**<function called before entity think*/
	void (*think)(struct Entity_S* self);   /**<function called on entity think*/
	void (*touch)(struct Entity_S* self, struct Entity_S* other);   /**<function called when an entity touches another*/
	void (*die)(struct Entity_S* self);   /**<function called when an entity dies*/
	float           health;
	float           healthmax;
//	int movetype;					/**<type of movement*/
	float nextthink;
	int frame;
	Vector3D origin;
	Vector3D old_origin;
	struct Entity_S* groundentity;
	int linkcount;
	int groundentity_linkcount;

	int flags;
	int svflags;

	OrientedRectangle boundingBox;
	Vector3D maxspeed;

	float specFloat1;		/**<used for jumpheight in player*/
	void* data;                     /**<additional entity specific data*/
	void* data2;                     /**<additional entity specific data*/

}Entity;

typedef struct
{
	Entity* entity_list;
	Uint32  entity_max;
	Uint32	num_ents;
}EntityManager;

static EntityManager gf2d_entity_manager = { 0, 100 };
/**
 * @brief initializes the entity subsystem
 * @param entity_max maximum number of simultaneous entities you wish to support
 */
void gf2d_entity_manager_init(Uint32 entity_max);

/**
 * @brief get an empty entity from the system
 * @return NULL on out of space or a pointer to an entity otherwise
 */
Entity* gf2d_entity_new();

/**
 * @brief free an active entity
 * @param self the entity to free
 */
void    gf2d_entity_free(Entity* self);
void gf2d_entity_free_all();

/**
* @brief get the index of the named entity
* @param name the name of the entity to find
* @return the index of the entity in the entity list
*/
Entity* find_entity(char* name);
Entity* get_last_entity();

void rotate_entity(Entity* entity, float radians, Vector3D axis);

Entity* modeled_entity_animated(char* modelName, char* entityName, int startFrame, int numFrames);
Entity* modeled_entity(char* modelName, char* entityName);

//Vector3D getAngles(Matrix4 mat);

Entity* load_entity_json(char* entityType);

void save_entity_layout_json(Entity* entity);

Entity* gf2d_nonanimated_entity_copy(Entity* entity);

void save_all_content_editor();

int get_type_count(char* type);
float getLowestPoint();
#endif
