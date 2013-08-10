#include "Audio.h"
#include "Debug.h"
#include "Exception.h"

Audio::Audio() : sfxvol(32)
{
  Mix_VolumeMusic(4);
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
  std::string rfile = std::string("resources/sound/sfx/") + file;
  auto it = sfx.find(rfile);
  if (it != sfx.end()) return it->second;

  Mix_Chunk *audio = Mix_LoadWAV(rfile.c_str());
  if (NULL == audio) {
    DBGERR(Mix_GetError());
    throw Exception(std::string("Could not load audio: ") + rfile);
  }

  sfx.insert(std::make_pair(rfile, audio));
  return audio;
}

Mix_Music *
Audio::load_music(const std::string& file)
{
  std::string rfile = std::string("resources/sound/music/") + file;
  auto it = music.find(rfile);
  if (it != music.end()) return it->second;

  Mix_Music *audio = Mix_LoadMUS(rfile.c_str());
  if (NULL == audio) {
    DBGERR(Mix_GetError());
    throw Exception(std::string("Could not load music: ") + rfile);
  }

  music.insert(std::make_pair(rfile, audio));
  return audio;
}

void
Audio::set_sfx_volume(int volume)
{
  sfxvol = volume;
}

void
Audio::set_music_volume(int volume)
{
  Mix_VolumeMusic(volume);
}

void
Audio::play(Mix_Chunk *audio, int loops) const
{
  Mix_VolumeChunk(audio, sfxvol);

  if (-1 == Mix_PlayChannel(-1, audio, loops)) {
    DBGERR(Mix_GetError());
    throw Exception("Could not play audio.");
  }
}

void
Audio::play(Mix_Music *audio) const
{
  if (-1 == Mix_PlayMusic(audio, -1)) {
    DBGERR(Mix_GetError());
    throw Exception("Could not start music.");
  }
}
