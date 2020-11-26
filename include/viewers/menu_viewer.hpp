// Copyright (c) 2020 Guy Turcotte
//
// MIT License. Look at file licenses.txt for details.

#ifndef __MENU_VIEWER_HPP__
#define __MENU_VIEWER_HPP__

#include "controllers/event_mgr.hpp"

class MenuViewer
{
  public:
    static constexpr uint8_t MAX_MENU_ENTRY = 10;

    enum Icon { RETURN, REFRESH, BOOK, BOOK_LIST, MAIN_PARAMS, FONT_PARAMS, POWEROFF, WIFI, INFO, END_MENU };
    char icon_char[END_MENU] = { '@', 'R', 'E', 'F', 'C', 'A', 'Z', 'S', 'I' };
    struct MenuEntry {
      Icon icon;
      const char * caption;
      void (*func)();
    };
    void  show(MenuEntry * the_menu);
    bool event(EventMgr::KeyEvent key);
    
  private:
    static constexpr char const * TAG = "MenuViewer";

    uint8_t current_entry_index;
    uint8_t max_index;
    struct EntryLoc {
      int16_t x, y;
      uint8_t width, height;
    } entry_locs[MAX_MENU_ENTRY];
    MenuEntry * menu;
};

#if __MENU_VIEWER__
  MenuViewer menu_viewer;
#else
  extern MenuViewer menu_viewer;
#endif

#endif