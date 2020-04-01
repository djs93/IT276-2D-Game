#include <SDL.h>
#include "gf2d_graphics.h"
#include "gf2d_draw.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "local.h"
#include "gf2d_entity.h"
#include "level.h"
#include "bucket.h"
#include "Geometry2D.h"
#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_font.h"
#include "gfc_input.h"
#include "gf2d_element_button.h"
#include "gf2d_element_label.h"
#include "gf2d_element_actor.h"
#include "gf2d_mouse.h"
#include "tower.h"
#include "enemies.h"
#include "player.h"

Entity* entity_list;
void draw_normal_entities();
void draw_level();
void esc_press();
void exit_press();
void toggle_towers();
void toggle_powers();
Vector2D vector2d_zero;
int mx, my;
Entity* selectedEntity;
Window* cashUI;
Window* upgradeUI;
Window* lifeUI;
Window* roundUI;
Window* goButtonUI;
Window* powerButtonUI;
Window* towerButtonUI;
Window* ui;
Window* powerUI;
Window* rewardWindow;
Window* gameOverWindow;
Bool windowPress;
Window* exitWindow;
int done;
Player* player;

int main(int argc, char * argv[])
{
    /*variable declarations*/
    done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    char windowTitle [30];
    
    mx = 0;
    my = 0;
    float mf = 0;
    //Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};

	//Entity* mouseEnt;
	Entity* backgroundEnt;

    vector2d_zero = vector2d(0, 0);
    
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
    //mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    gf2d_action_list_init(128);
	gf2d_entity_manager_init(400);
    bucket_manager_init(100, 100);
    /*
	backgroundEnt = gf2d_entity_new();
    gf2d_actor_load(&backgroundEnt->actor, "actors/mouse.actor");
	backgroundEnt->position = vector2d(0, 0);
	backgroundEnt->scale = vector2d(1, 1);
	backgroundEnt->scale = vector2d(1, 1);
	backgroundEnt->colorShift = vector4d(255, 255, 255, 255);
    */

	//mouseEnt = gf2d_entity_new(); 
    //gf2d_actor_load(&mouseEnt->actor, "actors/mouse.actor");
	//mouseEnt->position = vector2d(0, 0);
	//mouseEnt->scale = vector2d(1, 1);
	//mouseEnt->colorShift = vector4d(255, 255, 255, 255);

    player = player_load("saves/player.json");

    gf2d_font_init("config/font.cfg");
    gfc_input_init("config/input.cfg");
    gf2d_windows_init(32);

    ui = gf2d_window_new();

    ui = gf2d_window_load("config/yes_no_window2.json");
    setPrices(ui);

    powerUI = gf2d_window_load("config/power_window.json");
    powerUI->hide = 1;

    cashUI = gf2d_window_load("config/cash_UI.json");
    cashUI->no_draw_generic = 1;

    upgradeUI = gf2d_window_load("config/yes_no_window.json");
    upgradeUI->hide = 1;

    lifeUI = gf2d_window_load("config/life_UI.json");
    lifeUI->no_draw_generic = 1;

    roundUI = gf2d_window_load("config/round_UI.json");
    roundUI->no_draw_generic = 1;

    goButtonUI = gf2d_window_load("config/go_button_window.json");
    goButtonUI->no_draw_generic = 1;

    towerButtonUI = gf2d_window_load("config/tower_button_window.json");
    towerButtonUI->no_draw_generic = 1;
    towerButtonUI->hide = 1;

    powerButtonUI = gf2d_window_load("config/power_button_window.json");
    powerButtonUI->no_draw_generic = 1;

    rewardWindow = gf2d_window_load("config/reward_window.json");
    rewardWindow->hide = 1;

    gameOverWindow = gf2d_window_load("config/gameover_window.json");
    gameOverWindow->hide = 1;

    exitWindow = gf2d_window_load("config/exit_window.json");
    exitWindow->hide = 1;
	/*
	List* testLines = gfc_list_new();
	Line2D line1 = line2d(point2d(0, 0), point2d(0, 1));
	Line2D line2 = line2d(point2d(0, 1), point2d(1, 1));
	testLines = gfc_list_append(testLines, (void*)&line1);
	testLines = gfc_list_append(testLines, (void*)&line2);

	Path2D testPath = path2d(testLines);
	slog("%f", testPath.totalLength);
	*/
    gf2d_mouse_load("actors/mouse.actor");
    level_load_from_save("saves/level.json");
    gfc_input_set_callbacks("test", level_reload_test, NULL, NULL, NULL, "saves/level.json");
    gfc_input_set_callbacks("buyStinger", stinger_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buySlingshot", slingshot_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buyLaser", laser_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buyWater", water_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buyTechno", techno_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buySnowglobe", snowglobe_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("buyMusic", music_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("upgradeOne", upgradeOne_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("upgradeTwo", upgradeTwo_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("no", esc_press, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("exit", exit_press, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("startRound", level_start_round, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("powers", toggle_powers, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("towers", toggle_towers, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("power1", swarm_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("power2", timeWarp_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("power3", speedTotem_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("power4", cashDrop_buy, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("loadNext", level_loadNext, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("reload", level_reload, NULL, NULL, NULL, NULL);
    /*main game loop*/
    while(!done)
    {
        gfc_input_update();
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        //if (mf >= 16.0)mf = 0;
		//mouseEnt->position.x = mx;
		//mouseEnt->position.y = my;
        gf2d_mouse_update();
        gf2d_windows_update_all();
        if(gf2d_mouse_button_state(0) && gf2d_mouse_in_rect(upgradeUI->dimensions)){
            windowPress = true;
        }
        else {
            windowPress = false;
        }
        //slog("%i %i", mx, my);
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            //gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            //UI elements last
            //gf2d_sprite_draw(mouse, vector2d(mx,my), NULL, NULL, NULL, NULL, &mouseColor, (int)mf);
        draw_level();
        level_update();
        gf2d_entity_update_all();		
		//draw_buckets();
        //draw_buckets_optimal();
        draw_normal_entities();
        //draw_buckets_ally();
		//drawPaths();
        gf2d_windows_draw_all();
        gf2d_mouse_draw();
        //gf2d_sprite_draw(mouseEnt->actor.sprite, mouseEnt->position, &mouseEnt->scale, &mouseEnt->scaleCenter, &mouseEnt->rotation, &mouseEnt->flip, &mouseEnt->colorShift, (Uint32)mouseEnt->actor.frame);
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
		
        
        //if (keys[SDL_SCANCODE_ESCAPE])done = 1; 
        sprintf(windowTitle, "gf2d - %f fps", gf2d_graphics_get_frames_per_second());
        gf2d_graphics_set_title(windowTitle);
    }
    slog("---==== END ====---");
    return 0;
}

void draw_normal_entities() {
	int i = 0;
    int j;
	Entity* ent;
    Vector2D newPos;
    SDL_Rect rect;
    Bucket* bucket;
	while (i < gf2d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}
        /*
        if (ent->seekBuckets) {
            SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 0, 0, 255, 255);
            for (j = 0; j < ent->seekBuckets->count; j++) {
                bucket = gfc_list_get_nth(ent->seekBuckets, j);
                rect.h = bucket->shape.s.r.size.y;
                rect.w = bucket->shape.s.r.size.x;
                rect.x = bucket->shape.s.r.origin.x;
                rect.y = bucket->shape.s.r.origin.y;
                
                SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
               
            }
            SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
        }
        if (ent->shootRadius.radius > 0.1f) {
            gf2d_draw_circle(ent->shootRadius.position, ent->shootRadius.radius, vector4d(255.0f, 255.0f, 0.0f, 255.0f));
        }
        */
		if (ent->actor.sprite) {
			//if (!ent->name || strcmp(ent->name, "axes_attach") != 0) { //These are checks just in case there are specific things we don't want to draw
            vector2d_copy(newPos, ent->position);
            newPos.x -= ent->actor.sprite->frame_w/2* ent->actor.al->scale.x;
            newPos.y -= ent->actor.sprite->frame_h/2 * ent->actor.al->scale.y;
			gf2d_sprite_draw(ent->actor.sprite, newPos, &ent->actor.al->scale, &ent->scaleCenter, &ent->rotation, &ent->flip, &ent->colorShift, (Uint32)ent->actor.frame);
			//}
		}
        /*
        if (ent->boundingBox.radius > 0.1f) {
            gf2d_draw_circle(ent->boundingBox.position, ent->boundingBox.radius, vector4d(0.0f, 255.0f, 0.0f, 255.0f));
        }
        */
		i++;
	}
    ent = gf2d_entity_get_selected();
    if (ent !=NULL) {
        if(ent->shootRadius.radius > 0.1f){ gf2d_draw_circle(ent->shootRadius.position, ent->shootRadius.radius, vector4d(0.0f, 255.0f, 0.0f, 255.0f)); }
        else{ gf2d_draw_circle(ent->boundingBox.position, ent->boundingBox.radius, vector4d(0.0f, 255.0f, 0.0f, 255.0f)); }
        
    }
}

void draw_level() {
    gf2d_sprite_draw(get_loaded_level()->background, vector2d_zero, NULL, NULL, NULL, NULL, NULL, 0);
}

Window* getCashWindow() {
    return cashUI;
}
Window* getUpgradeWindow() {
    return upgradeUI;
}
Window* getLifeWindow() {
    return lifeUI;
}
void setWindowPressed(Bool state) {
    windowPress = state;
}
Bool getWindowPressed() {
    return windowPress;
}

void esc_press() {
    Entity* placeEnt;
    placeEnt = find_entity("placement");
    if (placeEnt) {
        gf2d_entity_free(placeEnt);
    }
    else {
        if (exitWindow->hide) {
            exitWindow->hide = 0;
        }
        else {
            exitWindow->hide = 1;
        }
    }
}

void exit_press() {
    done = 1; // exit condition
    //save stuff
    level_save("saves/level.json");
    player_save("saves/player.json");
}

void hideGoButton() {
    goButtonUI->hide = 1;
}

void showGoButton() {
    goButtonUI->hide = 0;
}

Player* getPlayer() {
    return player;
}

void toggle_towers() {
    powerButtonUI->hide = 0;
    towerButtonUI->hide = 1;
    powerUI->hide = 1;
    ui->hide = 0;
}

void toggle_powers() {
    towerButtonUI->hide = 0;
    powerButtonUI->hide = 1;
    powerUI->hide = 0;
    ui->hide = 1;    
}

void showRewardWindow(TowerTypes power, PerkNumbers perk) {
    Element* list;
    Element* currLabel;
    TextLine str;
    list = gf2d_window_get_element_by_id(rewardWindow, 0);
    currLabel = gf2d_element_list_get_item_by_id(list, 2);
    switch (power)
    {
    case TT_Power_Bee_Swarm:
        sprintf(str, "Obtained power: Bee Swarm (Now have %i)", player->powerInventory[0]);
        break;
    case TT_Power_Time_Warp:
        sprintf(str, "Obtained power: Time Warp (Now have %i)", player->powerInventory[1]);
        break;
    case TT_Power_Speed_Totem:
        sprintf(str, "Obtained power: Speed Totem (Now have %i)", player->powerInventory[2]);
        break;
    case TT_Power_Cash_Drop:
        sprintf(str, "Obtained power: Cash Drop (Now have %i)", player->powerInventory[3]);
        break;
    default:
        break;
    }
    gf2d_element_label_set_text(currLabel, str);

    currLabel = gf2d_element_list_get_item_by_id(list, 3);
    switch (perk)
    {
    case PN_Pierce:
        sprintf(str, "Obtained perk: +1 pierce");
        break;
    case PN_Speed:
        sprintf(str, "Obtained perk: +10%% speed");
        break;
    case PN_Discount:
        sprintf(str, "Obtained perk: -20%% cost");
        break;
    case PN_Money:
        sprintf(str, "Obtained perk: +$150 starting money");
        break;
    case PN_StingerUps:
        sprintf(str, "Obtained perk: Stinger first upgrades free");
        break;
    default:
        sprintf(str, " ");
        break;
    }
    gf2d_element_label_set_text(currLabel, str);
    rewardWindow->hide = 0;
}

void hideRewardWindow() {
    rewardWindow->hide = 1;
}


void showGameOver() {
    gameOverWindow->hide = 0;
}

void hideGameOver() {
    gameOverWindow->hide = 1;
}

void setRoundUI(int round) {
    Element* list;
    Element* currLabel;
    TextLine str;
    currLabel = gf2d_window_get_element_by_id(roundUI, 0);
    sprintf(str, "Round: %i", round+1);
    gf2d_element_label_set_text(currLabel, str);
}
/*eol@eof*/
