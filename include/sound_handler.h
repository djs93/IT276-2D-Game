#ifndef __SOUND_HANDLER_H__
#define __SOUND_HANDLER_H__

#include "gfc_audio.h"

typedef struct {
	Sound* sound_pop;
	Sound* sound_fire;
	Sound* sound_zap;
	Sound* sound_trumpet;
	Sound* sound_wave;
	Sound* sound_snow;
	Sound* sound_bgm;
	Sound* sound_buy;
	Sound* sound_win;
}SoundHandler;

typedef enum {
    ST_Pop = 0,
    ST_Fire = 1,
    ST_Zap = 2,
    ST_Trumpet = 3,
    ST_Wave = 4,
    ST_Snow = 5,
    ST_BGM = 6,
    ST_Buy = 7,
    ST_Win = 8
}SoundTypes;

static SoundHandler sound_manager = { 0 };

/**
 * @brief initializes the sound manager system based on the passed in parameters
 * @param popFile the sound file (MP3) used when a flower is killed
 * @param fireFile the sound file (MP3) used when a basic projectile is fired (slingshot and stinger)
 * @param zapFile the sound file (MP3) used when a techno bee fires
 * @param trumpetFile the sound file (MP3) used when a music bee fires
 * @param waveFile the sound file (MP3) used when a water bee fires
 * @param snowFile the sound file (MP3) used when a snow bee fires
 * @param bgmFile the sound file (MP3) used for background music
 * @param buyFile the sound file (MP3) used for when buying a tower
 * @param winFile the sound file (MP3) used for winning a level
 */
void sound_manager_init(char* popFile, char* fireFile, char* zapFile, char* trumpetFile, char* waveFile, char* snowFile, char* bgmFile, char* buyFile, char* winFile);

/**
* @brief changes the background music, useful for level music swaps
* @param newBGM the path to the MP3 file used for the new music
*/
void sound_change_bgm(char* newBGM);

/**
* @brief get the sound associated with a SoundType
* @param soundType the type of sound to get
* @return a pointer to the sound referenced by soundType, NULL if invalid soundType
*/
Sound* sound_get(SoundTypes soundType);

/**
* @breif play a soundType with volume of 1.0f and no loops on its default channel
* @param soundType the type of sound to play
*/
void sound_play_default(SoundTypes soundType);

#endif