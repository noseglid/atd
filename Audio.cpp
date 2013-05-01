#include "Audio.h"
#include "Debug.h"
#include "Exception.h"

Audio::Audio()
{
	Mix_VolumeMusic(MUSICVOL);
}

Audio::~Audio()
{
	auto it_sfx = sfx.begin();
	while (it_sfx != sfx.end()) {
		Mix_FreeChunk(it_sfx->second);
		sfx.erase(it_sfx++);
	}

	auto it_music = music.begin();
	while (it_music != music.end()) {
		Mix_FreeMusic(it_music->second);
		music.erase(it_music++);
	}
}

Audio&
Audio::instance()
{
	static Audio instance;
	return instance;
}

Mix_Chunk *
Audio::load_sfx(const std::string& file)
{
	auto it = sfx.find(file);
	if (it != sfx.end()) return it->second;

	Mix_Chunk *audio = Mix_LoadWAV(file.c_str());
	if (NULL == audio) {
		DBGERR(Mix_GetError());
		throw Exception(std::string("Could not load audio: ") + file);
	}

	Mix_VolumeChunk(audio, SFXVOL);

	sfx.insert(make_pair(file, audio));
	return audio;
}

Mix_Music *
Audio::load_music(const std::string& file)
{
	auto it = music.find(file);
	if (it != music.end()) return it->second;

	Mix_Music *audio = Mix_LoadMUS(file.c_str());
	if (NULL == audio) {
		DBGERR(Mix_GetError());
		throw Exception(std::string("Could not load music: ") + file);
	}

	music.insert(make_pair(file, audio));
	return audio;
}

void
Audio::play(Mix_Chunk *audio, int loops) const
{
	if (-1 == Mix_PlayChannel(-1, audio, loops)) {
		DBGERR(Mix_GetError());
		throw Exception("Could not play audio.");
	}
}

void
Audio::play(Mix_Music *audio) const
{
	if (-1 == Mix_FadeInMusic(audio, -1, 5000)) {
		DBGERR(Mix_GetError());
		throw Exception("Could not start music.");
	}

	Mix_VolumeMusic(MUSICVOL);
}
