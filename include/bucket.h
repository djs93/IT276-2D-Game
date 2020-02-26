#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "Geometry2D.h"

typedef struct Bucket_S {
	Shape shape;
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

#endif