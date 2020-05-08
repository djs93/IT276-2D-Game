#include "..\include\sound_handler.h"

void sound_manager_init(char* popFile, char* fireFile, char* zapFile, char* trumpetFile, char* waveFile, char* snowFile, char* bgmFile, char* buyFile, char* winFile)
{
	Sound* tempSound;
	tempSound = gfc_sound_load(popFile, 1.0f, 0);
	sound_manager.sound_pop = tempSound;

	tempSound = gfc_sound_load(fireFile, 1.0f, 1);
	sound_manager.sound_fire = tempSound;

	tempSound = gfc_sound_load(zapFile, 1.0f, 2);
	sound_manager.sound_zap = tempSound;

	tempSound = gfc_sound_load(trumpetFile, 1.0f, 3);
	sound_manager.sound_trumpet = tempSound;

	tempSound = gfc_sound_load(waveFile, 1.0f, 4);
	sound_manager.sound_wave = tempSound;

	tempSound = gfc_sound_load(snowFile, 1.0f, 5);
	sound_manager.sound_snow = tempSound;

	tempSound = gfc_sound_load(bgmFile, 1.0f, 6);
	sound_manager.sound_bgm = tempSound;

	tempSound = gfc_sound_load(buyFile, 1.0f, 7);
	sound_manager.sound_buy = tempSound;

	tempSound = gfc_sound_load(winFile, 1.0f, 8);
	sound_manager.sound_win = tempSound;
}

int sound_change_bgm(char* newBGM)
{
	Sound* bgm;
	int chan = -1;
	bgm = gfc_sound_load(newBGM, 1.0f, 6);
	if (bgm) {
		if (sound_manager.sound_bgm) {
			chan = sound_manager.sound_bgm->defaultChannel;
			Mix_HaltChannel(chan);
		}
		sound_manager.sound_bgm = bgm;	
		gfc_sound_play(sound_manager.sound_bgm, -1, sound_manager.sound_bgm->volume, sound_manager.sound_bgm->defaultChannel, 0);
		return 1;
	}
	return 0;
}

Sound* sound_get(SoundTypes soundType)
{
	switch (soundType) {
	case(ST_Pop): 
		return sound_manager.sound_pop;
		break;
	case(ST_Fire):
		return sound_manager.sound_fire;
		break;
	case(ST_Zap):
		return sound_manager.sound_zap;
		break;
	case(ST_Trumpet):
		return sound_manager.sound_trumpet;
		break;
	case(ST_Wave):
		return sound_manager.sound_wave;
		break;
	case(ST_Snow):
		return sound_manager.sound_snow;
		break;
	case(ST_BGM):
		return sound_manager.sound_bgm;
		break;
	case(ST_Buy):
		return sound_manager.sound_buy;
		break;
	case(ST_Win):
		return sound_manager.sound_win;
		break;
	default:
		return NULL;
		break;
	}
}

void sound_play_default(SoundTypes soundType)
{
	Sound* sound = sound_get(soundType);
	if (sound) {
		gfc_sound_play(sound, 0, sound->volume, sound->defaultChannel, 0);
	}
}
