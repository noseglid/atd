#include "Audio.h"
#include "Debug.h"
#include "Exception.h"

/* Change the RML file aswell if changing these */
#define VOL_INITIAL_SFX 128
#define VOL_INITIAL_MUSIC 32

Audio::Audio() : sfxvol(VOL_INITIAL_SFX)
{
  Mix_VolumeMusic(VOL_INITIAL_MUSIC);
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

void
Audio::play_current_music()
{
  play(current_music);
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

/**
 * Helper function to work with Mix_HookMusicFinished
 */
static void
__start()
{
  Audio::instance().play_current_music();
}

void
Audio::play(Mix_Music *audio)
{
  current_music = audio;

  auto start = [audio]() {
    if (-1 == Mix_FadeInMusic(audio, -1, 2000)) {
      DBGERR(Mix_GetError());
      throw Exception("Could not start music.");
    }
  };

  if (Mix_PlayingMusic()) {
    Mix_FadeOutMusic(2000);
    Mix_HookMusicFinished(__start);
  } else {
    start();
  }
}

void
Audio::stop_music()
{
  Mix_HaltMusic();
}
