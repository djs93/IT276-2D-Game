#include "bucket.h"
#include "simple_logger.h"

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
			//TODO: Initialize all rectangles
		}
	}

	slog("Bucket system initialized!");
}
