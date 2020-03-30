#ifndef __GF2D_ENTITY_H__
#define __GF2D_ENTITY_H__

#include "gf2d_sprite.h"
#include "gf2d_actor.h"
//#include "local.h"
#include "Geometry2D.h"
//#include "bucket.h"

typedef enum
{
	ES_Idle = 0,
	ES_Dying = 1,
	ES_Dead = 2,
	ES_Firing = 3
}EntityState;

typedef enum
{
	Type_Decor = 0,
	Type_Tower = 1,
	Type_Enemy = 2,
	Type_Projectile = 3
}EntityType;

typedef enum
{
	FL_PLACEABLE = 0x00000001
} EntityFlags;

typedef struct Entity_S
{
	char* name;			/**<name of the entity*/
	EntityType* type;			/**Type of the entitiy*/
	int           _inuse;         /**<flag to keep track if this isntance is in use and should not be reassigned*/
	int health;						/**<Health of the entity*/
	int damage;						/**<damage the entity does*/
	Actor actor;                     /**<animated sprite information*/
	Vector2D         position;       /**<position of the entity in 2d space*/
	Vector2D         velocity;       /**<velocity of the entity in 2d space*/
	float			speed;			/**<magnitude of how fast the entity should move*/
	Vector2D         acceleration;   /**<acceleration of the entity in 3d space*/
	Vector3D         rotation;       /**<x and y of center of rotation, then degrees of rotation*/
	Vector2D         scale;          /**<*please default to 1,1*/
	Vector2D         scaleCenter;          /**<*please default to 0,0*/
	Vector2D         flip;          /**<*1,0 flips on x-axis,  0,1 on the y*/
	Vector4D		colorShift;		/**Color shift*/
	EntityState     state;          /**<current state of the entity*/
	void (*prethink)(struct Entity_S* self);   /**<function called before entity think*/
	void (*think)(struct Entity_S* self);   /**<function called on entity think*/
	void (*touch)(struct Entity_S* self, struct Entity_S* other);   /**<function called when an entity touches another*/
	void* (*die)(struct Entity_S* self);   /**<function called when an entity dies*/
	void (*update)(struct Entity_S* self);  /**<Optional, override normal update*/
	void (*move)(struct Entity_S* self);  /**<defines how the entity moves. Called before updating buckets*/
	float nextThink;				/**<the next time the entity will think (in seconds)*/
	float maxSpeed;					/**<the max speed of the entity*/
	int frame;						/**<the frame of animation the entity is on*/
	Vector3D origin;
	Vector3D old_origin;
	int flags;
	int upgradeID;					/**<ID of the current uprgade stage, used to check later upgrades*/
	Circle boundingBox;
	Circle shootRadius;
	float fireRate;					/**<How long, in seconds, in between tower shots*/
	float cooldown;					/**<How long until next tower shot*/
	Bool dead;						/**<when true, the entity system will delete the entity on the next update*/
	List* seekBuckets;				/**<Buckets that a tower will check for enemies in. Null is every optimal bucket*/
	List* noTouch;					/**<List of entities this entity won't see when touching*/
	Path2D* path;					/**<Path for entity to follow*/
	float distanceLeft;				/**<Distance until the end of the current line on the path*/
	void* data;                     /**<additional entity specific data*/

}Entity;

/**
* @brief Manages all entities, stores them in a list that can be accessed later
*/
typedef struct
{
	Entity* entity_list;
	Uint32  entity_max;
	Uint32	num_ents;
}EntityManager;

static EntityManager gf2d_entity_manager = { 0, 300 };
extern Entity* selectedEntity;
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

/**
* @brief free all entities
*/
void gf2d_entity_free_all();

/**
* @brief get the index of the named entity
* @param name the name of the entity to find
* @return the index of the entity in the entity list
*/
Entity* find_entity(char* name);

/**
* @brief get the last entity in the entity list
* @return the last entity in the entity list
*/
Entity* get_last_entity();

/**
* @brief rotate an entity around an axis
* @param entity the entity to rotate
* @param radians the amount of radians to rotate
* @param axis the axis to rotate around
*/
void rotate_entity(Entity* entity, float radians, Vector3D axis);


//Entity* modeled_entity_animated(char* modelName, char* entityName, int startFrame, int numFrames);
//Entity* modeled_entity(char* modelName, char* entityName);

//Vector3D getAngles(Matrix4 mat);

/**
* @brief load an entity from a json file
* @param entityType the type of entity you wish to load. Loads from <entityType>.json
* @return an Entity pointer to the new entity
*/
Entity* load_entity_json(char* entityType);

/**
* @brief a helper function to save an entity's basic layout to a json file for further manual tweaking. Saves type, sprite (animation data), health, what fram it's on, flags, and bounding box size
* @param entity the entity you wish to save
*/
void save_entity_layout_json(Entity* entity);

//Probably gonna remove this one
Entity* gf2d_nonanimated_entity_copy(Entity* entity);

/**
* @brief saves all needed content from editor to level file (save.json) to load later
*/
void save_all_content_editor();

/**
* @brief get the amount of a certain type of entity in the world
* @param type the type of entity to count
* @return the count of entities of that type
*/
int get_type_count(char* type);

/**
* @brief get the lowest point in the world from ground types
* @return the lowest y position of the world
*/
float getLowestPoint();

/**
* @brief update all entities
*/
void gf2d_entity_update_all();

/**
* @brief update an entity
* @param self the entity to update
*/
void gf2d_entity_update(Entity* self);

/**
* @brief have an entity look at another
* @param self the entity being rotated
* @param lookAt the entity to look at
*/
void gf2d_entity_look_at(Entity* self, Entity* lookAt);

/**
* @brief Get the currently selected entity
* @return the currently selected entity
*/
Entity* gf2d_entity_get_selected();

/**
* @brief Set the currently selected entity
* @param entity the entity to set as currently selected
*/
void gf2d_entity_set_selected(Entity* entity);
#endif
