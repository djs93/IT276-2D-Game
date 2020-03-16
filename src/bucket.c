#include "bucket.h"
#include "simple_logger.h"
#include <level.h>

void bucket_check_remove(void* entity, void* entList);
void draw_buckets_optimal_foreach(void* data, void* context);


void bucket_manager_init(Uint32 bucket_width, Uint32 bucket_height)
{
	int columns, rows, i, r, c;
	int window_width, window_height;
	SDL_DisplayMode currentWindow;
	SDL_GetCurrentDisplayMode(0, &currentWindow);
	window_width = currentWindow.w;
	window_height = currentWindow.h;
	columns = (int)round((float)window_width / bucket_width);
	rows = (int)round((float)window_height / bucket_height);

	bucket_manager.bucket_array = gfc_allocate_array(sizeof(Bucket*), columns);
	for (i = 0; i < columns; i++) {
		bucket_manager.bucket_array[i] = gfc_allocate_array(sizeof(Bucket), rows);
	}

	for (r = 0; r < rows; r++) {
		for (c = 0; c < columns; c++) {
			bucket_manager.bucket_array[r][c].shape.s.r = rectangle2d(point2d(c * bucket_width, r * bucket_height),vector2d(bucket_width,bucket_height));
			bucket_manager.bucket_array[r][c].shape.type = ST_RECT;
			bucket_manager.bucket_array[r][c].entities = gfc_list_new();
		}
	}

	bucket_manager.rows = rows;
	bucket_manager.columns = columns;
	bucket_manager.bucket_width = bucket_width;
	bucket_manager.bucket_height = bucket_height;
	slog("Bucket system initialized!");
}

void draw_buckets() {
	SDL_Rect rect;
	int r, c;
	rect.h = bucket_manager.bucket_height;
	rect.w = bucket_manager.bucket_width;
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
	for (r = 0; r < bucket_manager.rows; r++) {
		for (c = 0; c < bucket_manager.columns; c++) {
			rect.x = bucket_manager.bucket_array[r][c].shape.s.r.origin.x;
			rect.y = bucket_manager.bucket_array[r][c].shape.s.r.origin.y;
			SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
		}
	}
}

void draw_buckets_optimal() {
	gfc_list_foreach(get_loaded_level()->optimalBuckets, draw_buckets_optimal_foreach, NULL);
}

void draw_buckets_optimal_foreach(void* data, void* context) {
	SDL_Rect rect;
	Bucket* bucket;
	int r, c;
	bucket = data;
	rect.h = bucket->shape.s.r.size.y;
	rect.w = bucket->shape.s.r.size.x;
	rect.x = bucket->shape.s.r.origin.x;
	rect.y = bucket->shape.s.r.origin.y;
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 0, 255, 0, 255);
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
}

void bucket_precalc()
{
	int r, c, i;
	Bucket* currBucket;
	for (r = 0; r < bucket_manager.rows; r++) {
		for (c = 0; c < bucket_manager.columns; c++) {
			currBucket = &bucket_manager.bucket_array[r][c];
			if(currBucket->entities->count>0){
				gfc_list_foreach(currBucket->entities, bucket_check_remove, currBucket->entities);
			}
		}
	}
}

void bucket_check_remove(void* entity, void* entList) {
	Entity* ent = (Entity*)entity;
	List* entityList = (List*)entList;
	if (vector2d_equal(vector2d(0, 0), ent->velocity)) {
		return;
	}
	gfc_list_delete_data(entityList, entity);
}

void bucket_update(Entity* entity) {
	int r, c, i;
	Bucket* currBucket;
	for (r = 0; r < bucket_manager.rows; r++) {
		for (c = 0; c < bucket_manager.columns; c++) {
			currBucket = &bucket_manager.bucket_array[r][c];
			//do collision between rect of bucket and circle of ent
			//add if colliding, add entity to that bucket's list
		}
	}
}

void calcOptimalLineBuckets(void* data, void* context) {
	int r, c, i;
	Bucket* currBucket;
	Line2D* line = data;
	Level* level = context;
	for (r = 0; r < bucket_manager.rows; r++) {
		for (c = 0; c < bucket_manager.columns; c++) {
			currBucket = &bucket_manager.bucket_array[r][c];
			//do collision between rect of bucket and line
			if (LineRectangle(*line, currBucket->shape.s.r)) {
				//add if colliding, add bucket to level's optimal list
				level->optimalBuckets = gfc_list_append(level->optimalBuckets, currBucket);
			}
		}
	}
}
