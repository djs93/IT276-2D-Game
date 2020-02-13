#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "local.h"
#include "gf2d_entity.h"

Entity* entity_list;
void draw_normal_entities(int frame);


int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};

	Entity* mouseEnt;
	Entity* backgroundEnt;
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);

	gf2d_entity_manager_init(300);

	backgroundEnt = gf2d_entity_new();
	backgroundEnt->sprite = sprite;
	backgroundEnt->position = vector2d(0, 0);
	backgroundEnt->scale = vector2d(1, 1);
	backgroundEnt->scale = vector2d(1, 1);
	backgroundEnt->colorShift = vector4d(255, 255, 255, 255);

	mouseEnt = gf2d_entity_new();
	mouseEnt->sprite = mouse;
	mouseEnt->position = vector2d(0, 0);
	mouseEnt->scale = vector2d(1, 1);
	mouseEnt->colorShift = vector4d(255, 255, 255, 255);

	List* testLines = gfc_list_new();
	Line2D line1 = line2d(point2d(0, 0), point2d(0, 1));
	Line2D line2 = line2d(point2d(0, 1), point2d(1, 1));
	testLines = gfc_list_append(testLines, (void*)&line1);
	testLines = gfc_list_append(testLines, (void*)&line2);

	Path2D testPath = path2d(testLines);
	slog("%f", testPath.totalLength);

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        //if (mf >= 16.0)mf = 0;
		mouseEnt->position.x = mx;
		mouseEnt->position.y = my;
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            //gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            //UI elements last
            //gf2d_sprite_draw(mouse, vector2d(mx,my), NULL, NULL, NULL, NULL, &mouseColor, (int)mf);
		draw_normal_entities((int)mf);
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
		

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}

void draw_normal_entities(int frame) {
	int i = 0;
	Entity* ent;
	while (i < gf2d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}

		if (ent->sprite) {
			//if (!ent->name || strcmp(ent->name, "axes_attach") != 0) { //These are checks just in case there are specific things we don't want to draw
			gf2d_sprite_draw(ent->sprite, ent->position, &ent->scale, &ent->scaleCenter, &ent->rotation, &ent->flip, &ent->colorShift, (Uint32)frame % ent->sprite->frame_count);
			//}
		}

		i++;
	}
}
/*eol@eof*/
