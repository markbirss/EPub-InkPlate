// Copyright (c) 2020 Guy Turcotte
//
// MIT License. Look at file licenses.txt for details.

#pragma once

#include "global.hpp"
#include "controllers/event_mgr.hpp"
#include "models/epub.hpp"
#include "models/page_locs.hpp"

#include <string>

class BookController
{
  public:
    BookController() {
      current_page_id = PageLocs::PageId(0, 0);
    }
    
    void key_event(EventMgr::KeyEvent key);
    void enter();
    void leave(bool going_to_deep_sleep = false);
    bool open_book_file(std::string & book_title, 
                        std::string & book_filename, 
                        const PageLocs::PageId & page_id,
                        bool cover_too_large);
    void put_str(const char * str, int xpos, int ypos);

    inline bool is_cover_too_large() { return cover_is_too_large; }
    inline const PageLocs::PageId & get_current_page_id() { return current_page_id; }

  private:
    static constexpr char const * TAG = "BookController";

    PageLocs::PageId current_page_id;
    bool cover_is_too_large;
};

#if __BOOK_CONTROLLER__
  BookController book_controller;
#else
  extern BookController book_controller;
#endif
