/*
 * SoundLoader.cpp
 *
 *  Created on: Apr 9, 2016
 *      Author: alex
 */

// TODO: Handle channels too!
#include "SoundLoaderPlayer.h"

#include <assert.h>
#include <SDL_mixer.h>
#include <string>
#include "HashMap.h"
#include "WordReader.h"

SoundLoaderPlayer *soundsPlaying [8] = {NULL};


static HashMap_STR_PTR<Mix_Chunk> *hashMap = NULL;


static int getFreeChannel()
{
	for (int i = 0; i < 8; i++)
	{
		if (soundsPlaying[i] == NULL)
		{
			return i;
		}
	}
	return -1;
}

/*
int regSoundLoader(SoundLoader *soundsLoader)
{
	int i=getFreeChannel();

	if (i>=0)
	{
		soundsPlaying[i]=soundsLoader;
	}
	else
	{
		printf("No channel to play %s\n", soundsLoader->getName()); fflush(stdout);
	}
	return i;
}
*/

// TODO SoundLoader crashed if file was neither ogg or mpg
int SoundLoaderPlayer::playSoundFile(const char* filename)
{
	int i=getFreeChannel();
	if (i>=0)
	{
		// sl shall be deleted by channelDone when sound finished playing
		SoundLoaderPlayer *sl=new SoundLoaderPlayer();
		sl->channelIndex = i;
		if (sl->load_Sound(filename)!=0)
		{
			delete sl;
			return 1;
		}

		if (sl->play_Sound()!=0)
		{
			delete sl;
			return 1;
		}

		soundsPlaying[i]=sl;
	}
	else
	{
		printf("No channel to play %s\n", filename);
		return 1;
	}
	return 0;
}

static void channelDone (int channel)
{
	SoundLoaderPlayer *sl=soundsPlaying[channel];
	assert(sl!=NULL);
	assert(channel==sl->getChannelIndex());
	delete sl;
	soundsPlaying[channel] = NULL;
	printf("Channel done, %i\n", channel); fflush(stdout);
}

// Retruns zero if OK
int SoundLoaderInitAudio()
{
	printf("Init audio! \n");
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() ); fflush(stdout);
		return -1;
	}
	fflush(stdout);
	Mix_AllocateChannels(8);
	Mix_ChannelFinished(channelDone);

	hashMap = new HashMap_STR_PTR<Mix_Chunk>();
	return 0;
}


void SoundLoaderCleanupAudio()
{
	int c=hashMap->capacity();
	for (int i=0;i<c;++i)
	{
		Mix_Chunk* sound=hashMap->getValueByIndex(i);
		if (sound!=NULL)
		{
			Mix_FreeChunk(sound);
		}
	}

	delete hashMap;
}


SoundLoaderPlayer::SoundLoaderPlayer() {
	channelIndex=0;
	sound = NULL;
	music = NULL;
}

SoundLoaderPlayer::~SoundLoaderPlayer() {

	//printf("play_Sound: delete %s\n", soundName);

	// Remove anything that is set
	/*if (sound != NULL)
	{
		Mix_FreeChunk(sound);
		sound = NULL;
	}*/
	if (music != NULL)
	{
		Mix_FreeMusic(music);
		music = NULL;
	}
}


/*int SoundLoader::pause_Sound ()
{

}*/

// Returns 0 if OK
int SoundLoaderPlayer::play_Sound ()
{
	printf("play_Sound: %s\n", soundName);
	// if we managed to play anything.
	int succes = 0;
	if (sound != NULL)
	{
		// Play.
		Mix_PlayChannel(channelIndex, sound, 0);
		succes = 1;
	}
	if (music != NULL)
	{
		// Play.
		Mix_PlayMusic(music, 0);
		succes = 1;
	}
	if (succes == 0)
	{
		// Error
		printf ("No sound was loaded into SoundLoader\n\r"); fflush(stdout);
		channelDone(channelIndex);
		return 1;
	}

	return 0;
}

// Returns 0 if OK
int SoundLoaderPlayer::load_Sound(const char* filename)
{
	//printf("Loading sound: %s\n", filename);

	assert(sound == NULL);
	assert(music == NULL);

	strncpy(soundName, filename, sizeof(soundName)-1);
	soundName[sizeof(soundName)-1]=0;

	WordReader wr(filename); // Create a word reader

	wr.separatorChar = '.'; // Set the separator

	// TODO this will fail if the '.' is not the only '.' in the file name.
	wr.readToken(); // Remove the name


	const char* extension = wr.readToken(); // Then get the extension

	// Identifie extension. TODO: Use magic number instead.
	if (strcmp(extension, "ogg") == 0)
	{
		sound=hashMap->get(filename);
		if (sound!=NULL)
		{
			// Already loaded
		}
		else
		{
			printf ("Loading OGG format\n");
			sound = Mix_LoadWAV(filename);
			if (!sound)
			{
				printf("Mix_LoadMUS(%s): %s\n", filename, Mix_GetError());
				return 1;
			}
			hashMap->put(filename, sound);
		}
	}
	else if (strcmp(extension, "mp3") == 0)
	{
		printf ("Loading MP3 format\n");
		music = Mix_LoadMUS(filename);
		if (!music)
		{
			printf("Mix_LoadMUS(%s): %s\n", filename, Mix_GetError());
			return 1;
		}
	}
	else
	{
		printf ("Unsupported sound: %s, use ogg or mp3 instead in filename: %s\n", extension, filename); fflush(stdout);
		return 1;
	}

	return 0;
}
