#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <gfc_types.h>
#include <gfc_list.h>

typedef struct {
	TextBlock inputString;
	Bool isPlacing;
	List* currLinePoints;
}Editor;

/**
* @brief Updates the editor, including coordinates, doing math on the paths, etc
* @param editor The Editor to update
*/
void editorUpdate(Editor* editor);

/**
* @brief Draws the editors current placement lines
* @param editor The Editor to draw placement lines for
* @note After completing a line, the drawing will be handled by drawPaths
*/
void drawEditorLines(Editor* editor);

/**
* @brief Allocates a new Editor
* @return A pointer to the new Editor
*/
Editor* editor_new();

/**
* @brief Processes input for the background image input, setting the background if the image exists, popping the invalid input window if not
* @param editor The Editor to process input for
*/
void background_input_process(Editor* editor);

/**
* @brief Processes input for the background music input, setting the music if the file exists, popping the invalid input window if not
* @param editor The Editor to process input for
*/
void background_music_input_process(Editor* editor);

/**
* @brief increase width of ALL paths in level
*/
void increasePathWidth();

/**
* @brief decrease width of ALL paths in level
*/
void decreasePathWidth();

/**
* @brief Adds the currently drawn line to the level's paths
* @param editor The Editor to save the line for
*/
void editor_confirm_line(Editor* editor);

/**
* @brief Draw both the editor lines and the point attached to the mouse
* @param editor The Editor to draw components
*/
void editor_draw(Editor* editor);

/**
* @brief Enable placement mode on an editor
* @param editor The Editor to draw components
*/
void startPlacement(Editor* editor);

/**
* @brief Undo the last step in the editor
* @param editor the Editor to undo the last step in
* @note Cannot step back into an already confirmed path, will delete path instead. There's no redo, so be careful!
*/
void editor_undo(Editor* editor);

/**
* @brief Save the editor's contents to a file
* @param editor The Editor to save
* @note Outputs to the levels folder
*/
void editor_save(Editor* editor);
#endif