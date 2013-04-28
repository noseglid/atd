#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL/SDL_mixer.h>
#include <map>
#include <string>

#ifndef MUSICVOL
#define MUSICVOL 16
#endif

#ifndef SFXVOL
#define SFXVOL 32
#endif

class Audio
{
	std::map<std::string, Mix_Chunk *> sfx;
	std::map<std::string, Mix_Music *> music;

	Audio();
	~Audio();

public:
	static Audio& instance();

	Mix_Chunk *load_sfx(const std::string& file);
	Mix_Music *load_music(const std::string& file);

	void play(Mix_Chunk *audio) const;
	void play(Mix_Music *audio) const;
};

#endif
