#include <gf2d_windows.h>
#include <gf2d_draw.h>
#include <simple_logger.h>
#include <game.h>
#include <level.h>
#include <gf2d_mouse.h>
#include <editor.h>
#include <sound_handler.h>

void updateCoords();
void placePoint(Editor* editor, Vector2D position);
void editor_start_line(Editor* editor);

void editorUpdate(Editor* editor) {
    updateCoords();
    if (editor->isPlacing && gf2d_mouse_get_position().y<=608) {
        if (gf2d_mouse_button_pressed(0)) {
            placePoint(editor, gf2d_mouse_get_position());
        }
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
    get_loaded_level()->collisionPaths = gfc_list_new();
    gfc_list_foreach(get_loaded_level()->paths, calcPathBoundaries, get_loaded_level());
}

void decreasePathWidth() {
    if (get_loaded_level()->pathDistance > 1.0f) {
        get_loaded_level()->pathDistance -= 1.0f;
        updateWidthUI();
        get_loaded_level()->collisionPaths = gfc_list_new();
        gfc_list_foreach(get_loaded_level()->paths, calcPathBoundaries, get_loaded_level());
    }
}

void editor_confirm_line(Editor* editor) {
    List* lines;
    int j;
    Point2D* currPoint;
    Point2D* nextPoint;
    Line2D* currLine;
    Path2D* currPath;
    Path2D tempPath;
    slog("Confirm Line");
    if (editor->currLinePoints->count <= 0) {
        return;
    }
    lines = gfc_list_new();
    for (j = 0; j < editor->currLinePoints->count - 1; j++) {
        currPoint = (Point2D*)gfc_list_get_nth(editor->currLinePoints, j);
        nextPoint = (Point2D*)gfc_list_get_nth(editor->currLinePoints, j+1);
        currLine = gfc_allocate_array(sizeof(Line2D), 1);
        currLine->start.x = currPoint->x;
        currLine->start.y = currPoint->y;
        currLine->end.x = nextPoint->x;
        currLine->end.y = nextPoint->y;
        lines = gfc_list_append(lines, currLine);
    }
    currPath = gfc_allocate_array(sizeof(Path2D), 1);
    tempPath = path2d(lines);
    currPath->end = tempPath.end;
    currPath->start = tempPath.start;
    currPath->lines = tempPath.lines;
    currPath->totalLength = tempPath.totalLength;
    get_loaded_level()->paths = gfc_list_append(get_loaded_level()->paths, currPath);
    gfc_list_foreach(get_loaded_level()->paths, calcPathBoundaries, get_loaded_level());
}

void editor_start_line(Editor* editor) {

}

void drawEditorLines(Editor* editor) {
    int i;
    Vector2D pos1;
    Vector2D pos2;

    if (editor->currLinePoints->count >= 1) {
        gf2d_draw_circle(*(Vector2D*)gfc_list_get_nth(editor->currLinePoints, 0), 5, vector4d(0.0f, 255.0f, 0.0f, 255.0f));
    }
    for (i = 1; i < editor->currLinePoints->count; i++) {
        pos1 = *(Vector2D*)gfc_list_get_nth(editor->currLinePoints, i);
        pos2 = *(Vector2D*)gfc_list_get_nth(editor->currLinePoints, i-1);

        gf2d_draw_circle(pos1, 5, vector4d(0.0f, 0.0f, 255.0f, 255.0f));

        SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 0, 0, 255, 255);
        SDL_RenderDrawLineF(gf2d_graphics_get_renderer(), pos2.x, pos2.y, pos1.x, pos1.y);
        SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
    }
}

void editor_draw(Editor* editor) {
    Vector2D position;
    Vector2D position2;
    //drawEditorLines(editor);
    if (editor->isPlacing) {
        position2 = gf2d_mouse_get_position();
        if (editor->currLinePoints->count >= 1) {//connect last point to mouse
            position = *(Vector2D*)gfc_list_get_nth(editor->currLinePoints, editor->currLinePoints->count - 1);
            SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 20, 20, 255);
            SDL_RenderDrawLineF(gf2d_graphics_get_renderer(), position.x, position.y, position2.x, position2.y);
            SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
            gf2d_draw_circle(position2, 5, vector4d(0.0f, 0.0f, 255.0f, 255.0f));
        }
        else {
            gf2d_draw_circle(position2, 5, vector4d(0.0f, 255.0f, 0.0f, 255.0f));
        }
        drawEditorLines(editor);
    }
}

void placePoint(Editor* editor, Vector2D position) {
    Vector2D* pos = gfc_allocate_array(sizeof(Vector2D), 1);
    pos->x = position.x;
    pos->y = position.y;
    editor->currLinePoints = gfc_list_append(editor->currLinePoints, pos);
}

void startPlacement(Editor* editor) {
    editor->isPlacing = true;
    editor->currLinePoints = gfc_list_new();
}

void editor_undo(Editor* editor) {
    if (editor->isPlacing) {
        if (editor->currLinePoints->count > 0) {
            gfc_list_delete_nth(editor->currLinePoints, editor->currLinePoints->count-1);
        }
        else {
            if (get_loaded_level()->paths->count > 0) {
                gfc_list_delete_nth(get_loaded_level()->paths, get_loaded_level()->paths->count - 1);
                get_loaded_level()->collisionPaths = gfc_list_new();
                gfc_list_foreach(get_loaded_level()->paths, calcPathBoundaries, get_loaded_level());
            }
        }
    }
    else {
        if (get_loaded_level()->paths->count > 0) {
            gfc_list_delete_nth(get_loaded_level()->paths, get_loaded_level()->paths->count - 1);
            get_loaded_level()->collisionPaths = gfc_list_new();
            gfc_list_foreach(get_loaded_level()->paths, calcPathBoundaries, get_loaded_level());
        }
    }
}