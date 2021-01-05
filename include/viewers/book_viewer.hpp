// Copyright (c) 2020 Guy Turcotte
//
// MIT License. Look at file licenses.txt for details.

#ifndef __BOOK_VIEWER_HPP__
#define __BOOK_VIEWER_HPP__

#include <mutex>

#if EPUB_LINUX_BUILD
#else
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/semphr.h"
#endif

#include <vector>
#include <string>
#include <unordered_map>

#include "global.hpp"
#include "pugixml.hpp"
#include "viewers/page.hpp"
#include "models/epub.hpp"
#include "models/page_locs.hpp"
#include "models/css.hpp"
#include "models/fonts.hpp"

using namespace pugi;

class BookViewer
{
  private:
    static constexpr char const * TAG = "BookViewer";

    std::mutex mutex;
    #if EPUB_LINUX_BUILD
    #else
      //static SemaphoreHandle_t mutex;
      //static StaticSemaphore_t mutex_buffer;
      //inline static void enter() { xSemaphoreTake(mutex, portMAX_DELAY); }
      //inline static void leave() { xSemaphoreGive(mutex); }
    #endif

    int32_t           current_offset;          ///< Where we are in current item
    int32_t           start_of_page_offset;
    int32_t           end_of_page_offset;
    int16_t           page_bottom;
    bool              show_images;
    PageLocs::PageId  current_page_id;
    CSS::Properties * last_props;

    // BREAK is BR... A defined BR in esp-idf is the cause of this!!
    enum class Element { BODY, P, LI, BREAK, H1, H2, H3, H4, H5, H6, 
                         B, I, A, IMG, IMAGE, EM, DIV, SPAN, PRE,
                         BLOCKQUOTE };
    
    typedef std::unordered_map<std::string, Element> Elements;
    Elements elements;
    bool start_of_paragraph;  ///< Required to manage paragraph indentation at beginning of new page.
    bool indent_paragraph;

    bool                get_image(std::string & filename, Page::Image & image);
    void            adjust_format(pugi::xml_node node, Page::Format & fmt, CSS::Properties * element_properties);
    void adjust_format_from_suite(Page::Format & fmt, const CSS::PropertySuite & suite);
    bool        page_locs_recurse(pugi::xml_node node, Page::Format fmt);
    bool       page_locs_end_page(Page::Format & fmt);
    bool       build_page_recurse(pugi::xml_node node, Page::Format fmt);
    void            build_page_at(const PageLocs::PageId & page_id);
    int16_t       get_pixel_value(const CSS::Value & value, const Page::Format & fmt, int16_t ref);
    int16_t       get_point_value(const CSS::Value & value, const Page::Format & fmt, int16_t ref);
    float        get_factor_value(const CSS::Value & value, const Page::Format & fmt, float ref);

    inline void reset_font_index(Page::Format & fmt, Fonts::FaceStyle style) {
      if (style != fmt.font_style) {
        int16_t idx = -1;
        if ((idx = fonts.get_index(fonts.get_name(fmt.font_index), style)) == -1) {
          // LOG_E("Font not found 2: %s %d", fonts.get_name(fmt.font_index), style);
          idx = fonts.get_index("Default", style);
        }
        if (idx == -1) {
          fmt.font_style = Fonts::FaceStyle::NORMAL;
          fmt.font_index = 1;
        }
        else {
          fmt.font_style = style;
          fmt.font_index = idx;
        }
      }
    };
  public:

    BookViewer() :
      elements{{"p",     Element::P}, {"div",     Element::DIV}, {"span", Element::SPAN}, {"br",  Element::BREAK}, {"h1",                 Element::H1},  
               {"h2",   Element::H2}, {"h3",       Element::H3}, {"h4",     Element::H4}, {"h5",     Element::H5}, {"h6",                 Element::H6}, 
               {"b",     Element::B}, {"i",         Element::I}, {"em",     Element::EM}, {"body", Element::BODY}, {"a",                   Element::A},
               {"img", Element::IMG}, {"image", Element::IMAGE}, {"li",     Element::LI}, {"pre",   Element::PRE}, {"blockquote", Element::BLOCKQUOTE}}
      { 
        #if EPUB_LINUX_BUILD
          //mutex = PTHREAD_MUTEX_INITIALIZER;
        #else
          //mutex = xSemaphoreCreateMutexStatic(&mutex_buffer);
        #endif 
      }

    ~BookViewer() { }

    void init() { current_page_id = PageLocs::PageId(-1, -1); }

    /**
     * @brief Build the pages location vector
     * 
     * The vector is used to quicly direct page preparation and display. It is expected to
     * be called once a book locations refresh is required by the BooksDir class. The information
     * is put in the ebooks list database and retrieved when the user select a book
     * to read. The process of building the list is a long process, better be done
     * only once per book at application load time.
     * 
     * ToDo: Make it runs as a thread.
     * 
     */
    //bool build_page_locs();
    bool build_page_locs(int16_t itemref_index);

    /**
     * @brief Show a page on the display.
     * 
     * @param page_nbr The page number to show (First ebook page = 0, cover = -1)
     */
    void show_page(const PageLocs::PageId & page_id);

    void line_added_at(int16_t ypos) {
      LOG_D("Line added: %d %d", current_offset, ypos);
    }
};

#if __BOOK_VIEWER__
  BookViewer book_viewer;
#else
  extern BookViewer book_viewer;
#endif

#endif