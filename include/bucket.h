#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "Geometry2D.h"
#include "level.h"

typedef struct Bucket_S {
	Shape2D shape;
    List* entities;
}Bucket;

typedef struct {
	Bucket** bucket_array;
	int rows;
	int columns;
	Uint32 bucket_width;
	Uint32 bucket_height;
}BucketManager;

static BucketManager bucket_manager = { 0 };
/**
 * @brief initializes the bucket subsystem
 * @param bucket_width how wide each bucket should be
 * @param bucket_height how tall each bucket should be
 */
void bucket_manager_init(Uint32 bucket_width, Uint32 bucket_height);

/**
* @brief debug function to draw all bucket rectangles
*/
void draw_buckets();

/**
* @brief debug function to draw all optimal bucket rectangles
*/
void draw_buckets_optimal();

/**
* @brief update buckets for a specified entity
* @param ent the entity to update buckets for
* @param context not used, only so gfc_list_foreach operates normally
*/
void bucket_update(Entity* entity, void* context);

/**
* @brief removes all entities that have moved from buckets before calculating new buckets
* @note all entities with zero velocity are not removed from their buckets
*/
void bucket_precalc();

/**
* @brief calculates all buckets for entities within level.needUpdate
*/
void bucket_calc();

/**
* @brief calculates which buckets intersect with a line
* @param data pointer to the line to calculate buckets for
* @param context the level pointer to store the buckets in
*/
void calcOptimalLineBuckets(void* data, void* context);


#endif