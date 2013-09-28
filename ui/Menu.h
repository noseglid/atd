#ifndef __UI__MENU_H__
#define __UI__MENU_H__

#include "ui/UI.h"
#include "Audio.h"

B_NS_UI

class Menu
{
public:
  enum ANIMDIR {
    ANIM_LEFT,
    ANIM_RIGHT
  };

private:
  bool anim_pending;
  int anim_pos_start, anim_pos_end;
  float anim_time_start, anim_duration;
  std::function<void()> anim_done;

  Rocket::Core::Element *animate;

  Mix_Chunk *sfx_change_menu;

  void tick(const engine::Event& ev);

protected:
  Rocket::Core::ElementDocument *document;

  Menu(std::string docpath);

public:
  virtual void show();
  virtual void show(int ms, ANIMDIR dir);
  virtual void show(int ms, std::function<void()> ondone, ANIMDIR dir);
  virtual void hide();
  virtual void hide(int ms, ANIMDIR dir);
  virtual void hide(int ms, std::function<void()> ondone, ANIMDIR dir);
  virtual void toggle();

  virtual bool is_visible() const;

  void display(bool visible = true);
  void feedback(std::string msg = "", bool error = true);
};

E_NS_UI

#endif
