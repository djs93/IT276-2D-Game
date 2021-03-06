#include <SDL.h>
#include <game.h>
#include "gfc_audio.h"
#include "gf2d_graphics.h"
#include "gf2d_draw.h"
#include "simple_logger.h"
#include "local.h"
#include "bucket.h"
#include "gf2d_elements.h"
#include "gf2d_font.h"
#include "gfc_input.h"
#include "gf2d_element_button.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_entry.h"
#include "gf2d_mouse.h"
#include "enemies.h"
#include "sound_handler.h"
#include "editor.h"

Entity* entity_list;
void draw_normal_entities();
void draw_level();
void esc_press();
void exit_press();
void toggle_towers();
void toggle_powers();
void sound_test();
void perkMoneyTest();
void loadMainMenu();
void loadLevelSelect();
void loadEditor();
void loadPerks();
void showEditorControls();
void hideEditorControls();
void popBGInput();
void popBGMInput();
void popSaveInput();
void popCustomInput();
void loadCustom();
void yesPress();
void editorSaveAcceptPress();
void loadButton(char* text);
Vector2D vector2d_zero;
int mx, my;
Entity* selectedEntity;
Entity* placementEntity;
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
Window* mainMenuWindow;
Window* levelSelectWindow;
Window* perkWindow;
Bool windowPress;
Window* exitWindow;
Window* toMainMenuWindow;
Window* editorControlsWindow;
Window* editorCoordWindow;
Window* editorBGMInputWindow;
Window* editorBackgroundInputWindow;
Window* saveInputWindow;
Window* customInputWindow;
Window* inputInvalidMenu;
int done;
Player* player;
int uiState;
Editor* editor;

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
    uiState = 0;
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
    gfc_audio_init(256, 16, 4, 1, 1, 1);
    sound_manager_init(
        "sfx/pop.wav", 
        "sfx/fire.mp3",
        "sfx/laser.mp3",
        "sfx/trumpet.mp3",
        "sfx/wave.mp3",
        "sfx/snow.mp3",
        "bgm/anttisinstrumentals_allaboardthefunkytrainvwkinstrumental.mp3",
        "sfx/buy.mp3",
        "sfx/game_win.mp3");
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    
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
    editor = editor_new();

    editor->currLinePoints = gfc_list_new();
    editor->isPlacing = false;

    ui = gf2d_window_load("config/yes_no_window2.json");
    setPrices(ui);
    ui->hide = 1;

    powerUI = gf2d_window_load("config/power_window.json");
    powerUI->hide = 1;

    cashUI = gf2d_window_load("config/cash_UI.json");
    cashUI->no_draw_generic = 1;
    cashUI->hide = 1;

    upgradeUI = gf2d_window_load("config/yes_no_window.json");
    upgradeUI->hide = 1;

    lifeUI = gf2d_window_load("config/life_UI.json");
    lifeUI->no_draw_generic = 1;
    lifeUI->hide = 1;

    roundUI = gf2d_window_load("config/round_UI.json");
    roundUI->no_draw_generic = 1;
    roundUI->hide = 1;

    goButtonUI = gf2d_window_load("config/go_button_window.json");
    goButtonUI->no_draw_generic = 1;
    goButtonUI->hide = 1;

    towerButtonUI = gf2d_window_load("config/tower_button_window.json");
    towerButtonUI->no_draw_generic = 1;
    towerButtonUI->hide = 1;

    powerButtonUI = gf2d_window_load("config/power_button_window.json");
    powerButtonUI->no_draw_generic = 1;
    powerButtonUI->hide = 1;

    rewardWindow = gf2d_window_load("config/reward_window.json");
    rewardWindow->hide = 1;

    gameOverWindow = gf2d_window_load("config/gameover_window.json");
    gameOverWindow->hide = 1;

    editorBackgroundInputWindow = gf2d_window_load("config/input_background_window.json");
    editorBackgroundInputWindow->hide = 1;
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(editorBackgroundInputWindow, 2), &editor->inputString, 500);

    editorBGMInputWindow = gf2d_window_load("config/input_background_music_window.json");
    editorBGMInputWindow->hide = 1;
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(editorBGMInputWindow, 2), &editor->inputString, 500);

    saveInputWindow = gf2d_window_load("config/input_save_name_window.json");
    saveInputWindow->hide = 1;
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(saveInputWindow, 2), &editor->inputString, 500);

    inputInvalidMenu = gf2d_window_load("config/input_invalid_window.json");
    inputInvalidMenu->hide = 1;

    editorControlsWindow = gf2d_window_load("config/editor_controls.json");
    editorControlsWindow->hide = 1;

    editorCoordWindow = gf2d_window_load("config/editor_coords.json");
    editorCoordWindow->hide = 1;
    editorCoordWindow->no_draw_generic = 1;

    mainMenuWindow = gf2d_window_load("config/main_menu.json");
    mainMenuWindow->hide = 0;
    mainMenuWindow->no_draw_generic = 1;

    levelSelectWindow = gf2d_window_load("config/level_select.json");
    levelSelectWindow->hide = 1;
    levelSelectWindow->no_draw_generic = 1;

    customInputWindow = gf2d_window_load("config/input_custom_level_window.json");
    customInputWindow->hide = 1;
    gf2d_element_entry_set_text_pointer(gf2d_window_get_element_by_id(customInputWindow, 2), &editor->inputString, 500);

    perkWindow = gf2d_window_load("config/perk_menu.json");
    perkWindow->hide = 1;
    perkWindow->no_draw_generic = 1;

    exitWindow = gf2d_window_load("config/exit_window.json");
    exitWindow->hide = 1;

    toMainMenuWindow = gf2d_window_load("config/to_main_menu_window.json");
    toMainMenuWindow->hide = 1;
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
    //level_load_from_save("saves/level.json");
    gfc_input_set_callbacks("test", perkMoneyTest, NULL, NULL, NULL, NULL);
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
    gfc_input_set_callbacks("modePrev", targetModePrev, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("modeNext", targetModeNext, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("mainMenu", loadMainMenu, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("level1", loadButton, NULL, NULL, NULL, "levels/test.json");
    gfc_input_set_callbacks("level2", loadButton, NULL, NULL, NULL, "levels/level1.json");
    gfc_input_set_callbacks("levelSelect", loadLevelSelect, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("loadSave", level_load_from_save, NULL, NULL, NULL, "saves/level.json");
    gfc_input_set_callbacks("loadEditor", loadEditor, NULL, NULL, NULL, "saves/level.json");
    gfc_input_set_callbacks("loadPerks", loadPerks, NULL, NULL, NULL, "saves/level.json");
    gfc_input_set_callbacks("perk1", tryBuyPerk, NULL, NULL, NULL, PN_Pierce);
    gfc_input_set_callbacks("perk2", tryBuyPerk, NULL, NULL, NULL, PN_Speed);
    gfc_input_set_callbacks("perk3", tryBuyPerk, NULL, NULL, NULL, PN_Discount);
    gfc_input_set_callbacks("perk4", tryBuyPerk, NULL, NULL, NULL, PN_Money);
    gfc_input_set_callbacks("perk5", tryBuyPerk, NULL, NULL, NULL, PN_StingerUps);
    gfc_input_set_callbacks("bginputyes", background_input_process, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("bgminputyes", background_music_input_process, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("setBG", popBGInput, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("setBGM", popBGMInput, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("widthIncrease", increasePathWidth, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("widthDecrease", decreasePathWidth, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("yes", yesPress, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("drawPath", startPlacement, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("undo", editor_undo, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("saveEditor", editorSaveAcceptPress, NULL, NULL, NULL, NULL);
    gfc_input_set_callbacks("popSave", popSaveInput, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("popCustomInput", popCustomInput, NULL, NULL, NULL, editor);
    gfc_input_set_callbacks("customLevelLoad", loadCustom, NULL, NULL, NULL, editor);

    gfc_sound_play(sound_get(ST_BGM), -1, sound_get(ST_BGM)->volume, sound_get(ST_BGM)->defaultChannel, 0);

    state = GS_MainMenu;
    
    loadMainMenu();
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
        if (state!=GS_MainMenu) {
            draw_level();
            if (state == GS_InGame) {
                level_update();
            }
            if (state == GS_InContentEditor) {
                editorUpdate(editor);
            }
            gf2d_entity_update_all();
            draw_normal_entities();
        }	
		//draw_buckets();
        //draw_buckets_optimal();
        //draw_buckets_ally();
        if (state == GS_InContentEditor) { 
            drawPaths(); 
            editor_draw(editor);
        }
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
    if ((get_loaded_level() && get_loaded_level()->background)) {
        gf2d_sprite_draw(get_loaded_level()->background, vector2d_zero, NULL, NULL, NULL, NULL, NULL, 0);
    }
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

Window* getMainMenuWindow() {
    return mainMenuWindow;
}

void esc_press() {
    //Entity* placeEnt;
    //placeEnt = find_entity("placement");
    if (state == GS_MainMenu) {
        if (mainMenuWindow->hide == 0) {
            if (exitWindow->hide) {
                exitWindow->hide = 0;
            }
            else {
                gf2d_mouse_consume_input(0);
                exitWindow->hide = 1;
            }
        }
        else if (levelSelectWindow->hide == 0) {
            if (customInputWindow->hide == 0) {
                customInputWindow->hide = 1;
                gf2d_mouse_consume_input(0);
            }
            else {
                loadMainMenu();
            }
        }
        else if (perkWindow->hide == 0) {
            loadMainMenu();
        }
        
    }
    else if (state == GS_InGame || state == GS_InContentEditor) {
        if (placementEntity) {
            gf2d_entity_free(placementEntity);
            placementEntity = NULL;
        }
        else {
            if (inputInvalidMenu->hide == 1) {
                if (saveInputWindow->hide == 1) {
                    if (editorBackgroundInputWindow->hide == 1) {
                        if (editorBGMInputWindow->hide == 1) {
                            if (toMainMenuWindow->hide) {
                                toMainMenuWindow->hide = 0;
                            }
                            else {
                                toMainMenuWindow->hide = 1;
                            }
                        }
                        else {
                            editorBGMInputWindow->hide = 1;
                        }
                    }
                    else {
                        editorBackgroundInputWindow->hide = 1;
                    }
                }
                else {
                    saveInputWindow->hide = 1;
                }
            }
            else {
                inputInvalidMenu->hide = 1;
            }
        }
    }
}

void exit_press() {
    done = 1; // exit condition
    //save stuff
    if (state != GS_InContentEditor && state != GS_MainMenu) {
        level_save("saves/level.json");
        player_save("saves/player.json");
    }
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

void showRewardWindow(TowerTypes power, int perkMoney) {
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
    /*
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
    */
    sprintf(str, "Obtained %i perk cash!", perkMoney);
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

void reloadPrices() {
    setPrices(ui);
}

void sound_test() {
    sound_change_bgm("bgm/anttisinstrumentals_allaboardthefunkytrainvwkinstrumental.mp3");
}

void perkMoneyTest() {
    player->perkMoney += 1000;
    updatePerkUI();
}

void loadMainMenu() {
    mainMenuWindow->hide = 0;
    levelSelectWindow->hide = 1;
    perkWindow->hide = 1;
    toMainMenuWindow->hide = 1;
    rewardWindow->hide = 1;
    gameOverWindow->hide = 1;
    hideEditorControls();
    if (state != GS_MainMenu && state != GS_InContentEditor) {
        sound_change_bgm("bgm/anttisinstrumentals_allaboardthefunkytrainvwkinstrumental.mp3");
    }
    if (state == GS_InGame) {
        level_save("saves/level.json");
        player_save("saves/player.json");
    }
    state = GS_MainMenu;
    hideInGameGUI();
    gf2d_mouse_consume_input(0);
}

void loadLevelSelect(){
    levelSelectWindow->hide = 0;
    mainMenuWindow->hide = 1;
    gf2d_mouse_consume_input(0);
}

void hideLevelSelect() {
    levelSelectWindow->hide = 1;
    gf2d_mouse_consume_input(0);
}

void loadEditor() {
    gf2d_mouse_consume_input(0);
    level_load("");
    hideInGameGUI();
    hideGoButton();
    state = GS_InContentEditor;
    showEditorControls();
    updateWidthUI();
}

void showEditorControls() {
    editorControlsWindow->hide = 0;
    editorCoordWindow->hide = 0;
}
void hideEditorControls() {
    editorControlsWindow->hide = 1;
    editorCoordWindow->hide = 1;
}

void loadPerks() {
    perkWindow->hide = 0;
    mainMenuWindow->hide = 1;
    gf2d_mouse_consume_input(0);
    updatePerkUI();
}

void showInGameGUI() {
    cashUI->hide = 0;
    lifeUI->hide = 0;
    roundUI->hide = 0;
    powerButtonUI->hide = 0;
    towerButtonUI->hide = 1;
    powerUI->hide = 1;
    ui->hide = 0;
    uiState = 1;
}

void hideInGameGUI() {
    cashUI->hide = 1;
    lifeUI->hide = 1;
    roundUI->hide = 1;
    powerButtonUI->hide = 1;
    towerButtonUI->hide = 1;
    ui->hide = 1;
    powerUI->hide = 1;
    uiState = 0;
}

void toggleInGameGUI() {
    
    if (uiState == 0) {
        //enable
        cashUI->hide = 0;
        lifeUI->hide = 0;
        roundUI->hide = 0;
        powerButtonUI->hide = 0;
        ui->hide = 0;
        uiState = 1;
    }
    else {
        //disable
        cashUI->hide = 1;
        lifeUI->hide = 1;
        roundUI->hide = 1;
        powerButtonUI->hide = 1;
        towerButtonUI->hide = 1;
        ui->hide = 1;
        powerUI->hide = 1;
        uiState = 0;
    }
}

void updatePerkUI() {
    Element* list;
    Element* currLabel;
    Element* currButton;
    TextLine str;
    currLabel = gf2d_window_get_element_by_id(perkWindow, 7);
    sprintf(str, "$%i", player->perkMoney);
    gf2d_element_label_set_text(currLabel, str);

    list = gf2d_window_get_element_by_id(perkWindow, 5);
    currButton = gf2d_element_list_get_item_by_id(list, 51);
    currLabel = ((ButtonElement*)currButton->data)->label;
    if (player->perks[0] == true) {
        sprintf(str, "Bought!");
        gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
    }
    else {
        sprintf(str, "$%i", get_perk_cost(PN_Pierce));
        if (player->perkMoney < get_perk_cost(PN_Pierce)) {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
        }
        else {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
        }
    }
    
    gf2d_element_label_set_text(currLabel, str);

    currButton = gf2d_element_list_get_item_by_id(list, 52);
    currLabel = ((ButtonElement*)currButton->data)->label;
    if (player->perks[1] == true) {
        sprintf(str, "Bought!");
        gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
    }
    else {
        sprintf(str, "$%i", get_perk_cost(PN_Speed));
        if (player->perkMoney < get_perk_cost(PN_Speed)) {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
        }
        else {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
        }
    }
    gf2d_element_label_set_text(currLabel, str);

    currButton = gf2d_element_list_get_item_by_id(list, 53);
    currLabel = ((ButtonElement*)currButton->data)->label;
    if (player->perks[2] == true) {
        sprintf(str, "Bought!");
        gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
    }
    else {
        sprintf(str, "$%i", get_perk_cost(PN_Discount));
        if (player->perkMoney < get_perk_cost(PN_Discount)) {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
        }
        else {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
        }
    }
    gf2d_element_label_set_text(currLabel, str);

    currButton = gf2d_element_list_get_item_by_id(list, 54);
    currLabel = ((ButtonElement*)currButton->data)->label; 
    if (player->perks[3] == true) {
        sprintf(str, "Bought!");
        gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
    }
    else {
        sprintf(str, "$%i", get_perk_cost(PN_Money));
        if (player->perkMoney < get_perk_cost(PN_Money)) {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
        }
        else {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
        }
    }
    gf2d_element_label_set_text(currLabel, str);

    currButton = gf2d_element_list_get_item_by_id(list, 55);
    currLabel = ((ButtonElement*)currButton->data)->label;
    if (player->perks[4] == true) {
        sprintf(str, "Bought!");
        gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
    }
    else {
        sprintf(str, "$%i", get_perk_cost(PN_StingerUps));
        if (player->perkMoney < get_perk_cost(PN_StingerUps)) {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(120.0f, 120.0f, 120.0f, 255.0f);
        }
        else {
            gf2d_element_actor_get_actor(((ButtonElement*)currButton->data)->actor)->color = vector4d(255.0f, 255.0f, 255.0f, 255.0f);
        }
    }
    gf2d_element_label_set_text(currLabel, str);
}

Window* getEditorCoordWindow() {
    return editorCoordWindow;
}

void popBGInput() {
    editorBackgroundInputWindow->hide = 0;
    gf2d_mouse_consume_input(0);
}

void popInvalidInput() {
    inputInvalidMenu->hide = 0;
    gf2d_mouse_consume_input(0);
}

void popSaveInput() {
    saveInputWindow->hide = 0;
    gf2d_mouse_consume_input(0);
}

void hideBackgroundInputWindow() {
    editorBackgroundInputWindow->hide = 1;
    gf2d_mouse_consume_input(0);
}

void popBGMInput() {
    editorBGMInputWindow->hide = 0;
    gf2d_mouse_consume_input(0);
}

void hideBackgroundMusicInputWindow() {
    editorBGMInputWindow->hide = 1;
    gf2d_mouse_consume_input(0);
}

void updateWidthUI() {
    Element* currLabel;
    TextLine str;
    currLabel = gf2d_window_get_element_by_id(editorCoordWindow, 2);
    sprintf(str, "Width:%i", (int)get_loaded_level()->pathDistance);
    gf2d_element_label_set_text(currLabel, str);
}

void yesPress() {
    if (GS_InContentEditor) {
        if (editor->isPlacing) {
            editor->isPlacing = false;
            editor_confirm_line(editor);
        }
    }
}

void editorSaveAcceptPress() {
    saveInputWindow->hide = 1;
    editor_save(editor);
    gf2d_mouse_consume_input(0);
}

void popCustomInput() {
    customInputWindow->hide = 0;
    gf2d_mouse_consume_input(0);
}

void loadCustom() {
    SJson* test;
    char* teststring = gfc_allocate_array(sizeof(char), 512);
    gf2d_mouse_consume_input(0);
    sprintf(teststring, "levels/%s.json", editor->inputString);
    test = sj_load(teststring);
    if (test) {
        level_load_new(teststring);
        customInputWindow->hide = 1;
    }
    else {
        popInvalidInput();
    }
}

void loadButton(char* text) {
    if (customInputWindow->hide == 1) {
        level_load_new(text);
    }
}
/*eol@eof*/
