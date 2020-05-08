#include <gf2d_windows.h>
#include <simple_logger.h>
#include <game.h>
#include <level.h>
#include <gf2d_mouse.h>
#include <editor.h>
#include <sound_handler.h>

void updateCoords();

void editorUpdate(Editor* editor) {
    updateCoords();
    if (gf2d_mouse_get_position().y>=608 && gf2d_mouse_button_pressed(0)) {
        
    }
}

void updateCoords() {
    Element* currLabel;
    TextLine str;
    currLabel = gf2d_window_get_element_by_id(getEditorCoordWindow(), 0);
    sprintf(str, "X:%i", (int)gf2d_mouse_get_position().x);
    gf2d_element_label_set_text(currLabel, str);

    currLabel = gf2d_window_get_element_by_id(getEditorCoordWindow(), 1);
    sprintf(str, "Y:%i", (int)gf2d_mouse_get_position().y);
    gf2d_element_label_set_text(currLabel, str);
}

Editor* editor_new() {
    Editor* editor = gfc_allocate_array(sizeof(Editor), 1);
}

void background_input_process(Editor* editor){
    Sprite* bgSprite;
    TextBlock str;
    sprintf(str, "images/backgrounds/%s", editor->inputString);
    bgSprite = gf2d_sprite_load_image(str);
    if (bgSprite) {
        get_loaded_level()->background = bgSprite;
        hideBackgroundInputWindow();
    }
    else {
        popInvalidInput();
    }
}

void background_music_input_process(Editor* editor){
    TextBlock str;
    sprintf(str, "bgm/%s", editor->inputString);
    if (sound_change_bgm(str)) {
        hideBackgroundMusicInputWindow();
    }
    else {
        popInvalidInput();
    }
}

void increasePathWidth() {
    get_loaded_level()->pathDistance += 1.0f;
    updateWidthUI();
}

void decreasePathWidth() {
    if (get_loaded_level()->pathDistance > 1.0f) {
        get_loaded_level()->pathDistance -= 1.0f;
        updateWidthUI();
    }
}
