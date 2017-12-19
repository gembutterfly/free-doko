/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public License as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *   MA  02111-1307  USA
 *
 *  Contact:
 *    Diether Knof dknof@gmx.de
 *    Borg Enders  borg@borgsoft.de
 *
 *********************************************************************/

#include "constants.h"

#ifdef USE_UI_GTKMM
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#include "chat.h"


#include "../translations.h"

#include "../ui.h"
#include "../../../misc/lounge.h"
#include "../../../network/server.h"
#include "../../../network/connection.h"
#include "../../../network/DokoLounge/sender.h"
#include "../../../utils/string.h"

#include <gtkmm/box.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/entry.h>

namespace UI_GTKMM_NS {
  namespace DokoLounge {

    /**
     ** constructor
     **
     ** @param     parent   the parent object
     **
     ** @return    -
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Chat::Chat(Lounge* const parent) :
      Base(parent),
      Frame("Chat"),
      chat_messages(NULL),
      chat_line(NULL)
    {
      this->init();
      return ;
    } // Chat::Chat(Lounge* const parent)

    /**
     ** destruktor
     **
     ** @param     -
     **
     ** @return    -
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Chat::~Chat()
    {  }

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding lounge
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Lounge&
    Chat::lounge()
    {
      return *dynamic_cast<Lounge*>(this->parent);
    } // Lounge& Chat::lounge()

    /**
     ** create all subelements
     **
     ** @param     -
     **
     ** @return    -
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Chat::init()
      {
        this->ui->translations->add(*dynamic_cast<Gtk::Label*>(this->get_label_widget()),
                                    ::translation("Chat"));
        Gtk::VBox* vbox = Gtk::manage(new Gtk::VBox(false, 0));

        this->chat_messages = Gtk::manage(new Gtk::TextView());
        this->chat_messages->set_wrap_mode(Gtk::WRAP_WORD);
        this->chat_messages->set_editable(false);
        this->chat_messages->set_cursor_visible(false);
        Gtk::ScrolledWindow* scroll_window = Gtk::manage(new Gtk::ScrolledWindow());
        scroll_window->add(*this->chat_messages);
        scroll_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        vbox->add(*scroll_window);

        this->chat_line = Gtk::manage(new Gtk::Entry());
        vbox->pack_end(*this->chat_line, Gtk::PACK_SHRINK);
        this->chat_line->grab_focus();

        this->add(*vbox);

        { // signals
          this->chat_line->signal_activate().connect(sigc::mem_fun(*this, &Chat::chat_signal));
        } // signals

        this->chat_messages->grab_focus();
        this->show_all_children();

        return ;
      } // void Chat::init()

    /**
     ** a chat entry has been added
     **
     ** @param     entry    new chat entry
     **
     ** @return    -
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **
     ** @todo      type: bold/italic
     **/
    void
      Chat::entry_added(::LoungeChatEntry const& entry)
      {
        string text = entry.text;
        if (text.empty())
          return;
        if (text[text.size() - 1] != '\n')
          text.append("\n");

        Glib::RefPtr<Gtk::TextBuffer::Tag> tag;
        if (   entry.with_color
            && (entry.type != LoungeChatEntry::PLAYER_FOR_ALL)) {
          Gdk::Color color;
          color.set_rgb_p(entry.color.r, entry.color.g, entry.color.b);
          tag = this->chat_messages->get_buffer()->create_tag();
          tag->property_foreground_gdk() = color;
        } else if (!entry.player.empty()) {
          tag = this->chat_messages->get_buffer()->get_tag_table()->lookup(entry.player + "::" + ::name(entry.type));
        }
        Gtk::TextBuffer::iterator iter = this->chat_messages->get_buffer()->end();

        struct tm const* const time_tm = localtime(&entry.time);
        string time_str;
        if (time_tm->tm_hour < 10)
          time_str += "0";
        time_str += DK::Utils::String::to_string(time_tm->tm_hour);
        time_str += ":";
        if (time_tm->tm_min < 10)
          time_str += "0";
        time_str += DK::Utils::String::to_string(time_tm->tm_min);
        time_str += ":";
        if (time_tm->tm_sec < 10)
          time_str += "0";
        time_str += DK::Utils::String::to_string(time_tm->tm_sec);

        text = time_str + " " + text;
        if (tag == 0) {
          iter = this->chat_messages->get_buffer()->insert(iter, text);
        } else {
          if (entry.player.empty()) {
            iter = this->chat_messages->get_buffer()->insert_with_tag(iter, text, tag);
          }
          else {
            iter = this->chat_messages->get_buffer()->insert_with_tag(iter,
                                                                      text,
                                                                      tag);
          }
        }
        // insert_with_tag(..., tag = entry.player)
        this->chat_messages->scroll_to(iter);
        return;
      } // void Chat::entry_added(::LoungeChatEntry entry)

    /**
     ** update the chat colors
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Chat::update_chat_colors()
      {
        vector< ::Lounge::Player> const& players = ::lounge->players();

        for (unsigned i = 0; i < players.size(); ++i) {
          string const& name = players[i].name;
          if (this->chat_messages->get_buffer()->get_tag_table()->lookup(name + "::" + ::name(LoungeChatEntry::PLAYER)) == 0) {
            Glib::RefPtr<Gtk::TextBuffer::Tag> tag;
            tag = this->chat_messages->get_buffer()->create_tag(name + "::" + ::name(LoungeChatEntry::PLAYER));
            tag->property_foreground_gdk() = this->lounge().player_color(name);
            tag = this->chat_messages->get_buffer()->create_tag(name + "::" + ::name(LoungeChatEntry::PLAYER_WHISPER));
            tag->property_foreground_gdk() = this->lounge().player_color(name);
            tag->property_style() = Pango::STYLE_ITALIC;
            tag = this->chat_messages->get_buffer()->create_tag(name + "::" + ::name(LoungeChatEntry::PLAYER_FOR_ALL));
            tag->property_foreground_gdk() = this->lounge().player_color(name);
            tag->property_weight() = Pango::WEIGHT_BOLD;
          }
        } // for (i)
      } // void Chat::update_chat_colors()

    /**
     ** the player chats the text in 'entry'
     **
     ** @param     -
     **
     ** @return    -
     ** 
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Chat::chat_signal()
      {
        string text = this->chat_line->get_text();
        if (text.empty())
          return;
        if (this == this->lounge().lounge_chat)
          this->lounge().sender->chat_lounge(Glib::convert(text, "latin1", "utf8"));
        else if (this == this->lounge().table_chat)
          this->lounge().sender->chat_table(Glib::convert(text, "latin1", "utf8"));
        this->chat_line->set_text("");
        this->chat_line->grab_focus();
        return ;
      } // void Chat::chat_signal()

  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
