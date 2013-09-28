#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL/SDL_mixer.h>
#include <map>
#include <string>

class Audio
{
  int sfxvol;

  Mix_Music *current_music;
  std::map<std::string, Mix_Chunk *> sfx;
  std::map<std::string, Mix_Music *> music;

  Audio();
  ~Audio();

public:
  static Audio& instance();

  Mix_Chunk *load_sfx(const std::string& file);
  Mix_Music *load_music(const std::string& file);

  void set_sfx_volume(int volume);
  void set_music_volume(int volume);

  void play(Mix_Chunk *audio, int loops = 0) const;
  void play(Mix_Music *audio);
  void play_current_music();
  void stop_music();

  bool music_is_playing() const;
};

#endif
