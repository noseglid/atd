#ifndef __ROCKETSDL_H__
#define __ROCKETSDL_H__

#include <Rocket/Core.h>
#include <SDL/SDL.h>

class RocketSDL
{
public:

  static Rocket::Core::Input::KeyModifier
  sdl2rocket_modifier(SDLMod sdl_mod)
  {
    using Rocket::Core::Input::KeyModifier;
    return (KeyModifier)(
      ((sdl_mod & KMOD_CTRL)  ? KeyModifier::KM_CTRL     : 0) |
      ((sdl_mod & KMOD_SHIFT) ? KeyModifier::KM_SHIFT    : 0) |
      ((sdl_mod & KMOD_ALT)   ? KeyModifier::KM_ALT      : 0) |
      ((sdl_mod & KMOD_META)  ? KeyModifier::KM_META     : 0) |
      ((sdl_mod & KMOD_CAPS)  ? KeyModifier::KM_CAPSLOCK : 0) |
      ((sdl_mod & KMOD_NUM)   ? KeyModifier::KM_NUMLOCK  : 0)
    );
  }

  static int
  sdl2rocket_button_index(SDL_MouseButtonEvent ev)
  {
    /* Boy, that's lucky */
    return ev.button - 1;
  }

  static Rocket::Core::Input::KeyIdentifier
  sdl2rocket_key(SDL_KeyboardEvent ev)
  {
    using namespace Rocket::Core::Input;
    static std::map<SDLKey, KeyIdentifier> map = {
      { SDLK_UNKNOWN,      KI_UNKNOWN },
      { SDLK_SPACE,        KI_SPACE },
      { SDLK_0,            KI_0 },
      { SDLK_1,            KI_1 },
      { SDLK_2,            KI_2 },
      { SDLK_3,            KI_3 },
      { SDLK_4,            KI_4 },
      { SDLK_5,            KI_5 },
      { SDLK_6,            KI_6 },
      { SDLK_7,            KI_7 },
      { SDLK_8,            KI_8 },
      { SDLK_9,            KI_9 },
      { SDLK_a,            KI_A },
      { SDLK_b,            KI_B },
      { SDLK_c,            KI_C },
      { SDLK_d,            KI_D },
      { SDLK_e,            KI_E },
      { SDLK_f,            KI_F },
      { SDLK_g,            KI_G },
      { SDLK_h,            KI_H },
      { SDLK_i,            KI_I },
      { SDLK_j,            KI_J },
      { SDLK_k,            KI_K },
      { SDLK_l,            KI_L },
      { SDLK_m,            KI_M },
      { SDLK_n,            KI_N },
      { SDLK_o,            KI_O },
      { SDLK_p,            KI_P },
      { SDLK_q,            KI_Q },
      { SDLK_r,            KI_R },
      { SDLK_s,            KI_S },
      { SDLK_t,            KI_T },
      { SDLK_u,            KI_U },
      { SDLK_v,            KI_V },
      { SDLK_w,            KI_W },
      { SDLK_x,            KI_X },
      { SDLK_y,            KI_Y },
      { SDLK_z,            KI_Z },
      { SDLK_SEMICOLON,    KI_OEM_1 },
      { SDLK_PLUS,         KI_OEM_PLUS },
      { SDLK_COMMA,        KI_OEM_COMMA },
      { SDLK_MINUS,        KI_OEM_MINUS },
      { SDLK_PERIOD,       KI_OEM_PERIOD },
      { SDLK_SLASH,        KI_OEM_2 },
      { SDLK_BACKQUOTE,    KI_OEM_3 },
      { SDLK_LEFTBRACKET,  KI_OEM_4 },
      { SDLK_BACKSLASH,    KI_OEM_5 },
      { SDLK_RIGHTBRACKET, KI_OEM_6 },
      { SDLK_QUOTEDBL,     KI_OEM_7 },
      { SDLK_KP0,          KI_NUMPAD0 },
      { SDLK_KP1,          KI_NUMPAD1 },
      { SDLK_KP2,          KI_NUMPAD2 },
      { SDLK_KP3,          KI_NUMPAD3 },
      { SDLK_KP4,          KI_NUMPAD4 },
      { SDLK_KP5,          KI_NUMPAD5 },
      { SDLK_KP6,          KI_NUMPAD6 },
      { SDLK_KP7,          KI_NUMPAD7 },
      { SDLK_KP8,          KI_NUMPAD8 },
      { SDLK_KP9,          KI_NUMPAD9 },
      { SDLK_KP_ENTER,     KI_NUMPADENTER },
      { SDLK_KP_MULTIPLY,  KI_MULTIPLY },
      { SDLK_KP_PLUS,      KI_ADD },
      { SDLK_KP_MINUS,     KI_SUBTRACT },
      { SDLK_KP_PERIOD,    KI_DECIMAL },
      { SDLK_KP_DIVIDE,    KI_DIVIDE },
      { SDLK_KP_EQUALS,    KI_OEM_NEC_EQUAL },
      { SDLK_BACKSPACE,    KI_BACK },
      { SDLK_TAB,          KI_TAB },
      { SDLK_CLEAR,        KI_CLEAR },
      { SDLK_RETURN,       KI_RETURN },
      { SDLK_PAUSE,        KI_PAUSE },
      { SDLK_CAPSLOCK,     KI_CAPITAL },
      { SDLK_PAGEUP,       KI_PRIOR },
      { SDLK_PAGEDOWN,     KI_NEXT },
      { SDLK_END,          KI_END },
      { SDLK_HOME,         KI_HOME },
      { SDLK_LEFT,         KI_LEFT },
      { SDLK_UP,           KI_UP },
      { SDLK_RIGHT,        KI_RIGHT },
      { SDLK_DOWN,         KI_DOWN },
      { SDLK_INSERT,       KI_INSERT },
      { SDLK_DELETE,       KI_DELETE },
      { SDLK_HELP,         KI_HELP },
      { SDLK_LSUPER,       KI_LWIN },
      { SDLK_RSUPER,       KI_RWIN },
      { SDLK_F1,           KI_F1 },
      { SDLK_F2,           KI_F2 },
      { SDLK_F3,           KI_F3 },
      { SDLK_F4,           KI_F4 },
      { SDLK_F5,           KI_F5 },
      { SDLK_F6,           KI_F6 },
      { SDLK_F7,           KI_F7 },
      { SDLK_F8,           KI_F8 },
      { SDLK_F9,           KI_F9 },
      { SDLK_F10,          KI_F10 },
      { SDLK_F11,          KI_F11 },
      { SDLK_F12,          KI_F12 },
      { SDLK_F13,          KI_F13 },
      { SDLK_F14,          KI_F14 },
      { SDLK_F15,          KI_F15 },
      { SDLK_NUMLOCK,      KI_NUMLOCK },
      { SDLK_SCROLLOCK,    KI_SCROLL },
      { SDLK_LSHIFT,       KI_LSHIFT },
      { SDLK_RSHIFT,       KI_RSHIFT },
      { SDLK_LCTRL,        KI_LCONTROL },
      { SDLK_RCTRL,        KI_RCONTROL },
      { SDLK_LALT,         KI_LMENU },
      { SDLK_RALT,         KI_RMENU },
      { SDLK_LMETA,        KI_LMETA },
      { SDLK_RMETA,        KI_RMETA }
    };

    return map[ev.keysym.sym];
  }

};

#endif
