#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "Geometry2D.h"
#include "gf2d_entity.h"

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
*/
void bucket_update(Entity* ent);

/**
* @brief removes all entities about to move from buckets before calculating new buckets
* @note all entities with zero velocity are not removed from their buckets
*/
void bucket_precalc();

/**
* @brief calculates which buckets intersect with a line
* @param data pointer to the line to calculate buckets for
* @param context the level pointer to store the buckets in
*/
void calcOptimalLineBuckets(void* data, void* context);


#endif