#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <gfc_types.h>
#include <gfc_list.h>

typedef struct {
	Bool isPlacing;
	List* currLinePoints;
	TextBlock inputString;
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
#endif