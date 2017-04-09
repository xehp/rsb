/*
 * SoundLoader.h
 *
 *  Created on: Apr 9, 2016
 *      Author: alex
 */
//#include <SDL2/SDL_mixer.h>
#include <SDL_mixer.h>


#ifndef SOUNDLOADER_H_
#define SOUNDLOADER_H_



class SoundLoaderPlayer {
private:
	Mix_Chunk* sound;
	Mix_Music* music;
	char soundName[20];
	int channelIndex;
public:
	static int playSoundFile (const char* filename);
	SoundLoaderPlayer();
	virtual int play_Sound ();
	virtual int load_Sound(const char* filename);
public:
	virtual char* getName() {return soundName;}
	virtual int getChannelIndex() {return channelIndex;}
	virtual ~SoundLoaderPlayer();
};


int SoundLoaderInitAudio();
void SoundLoaderCleanupAudio();


#endif /* SOUNDLOADER_H_ */
