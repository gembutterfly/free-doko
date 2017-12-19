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

#include "chatter.h"

#include "ui.h"
#include "translations.h"
#include "cards.h"

#include "../../party/party.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>

namespace UI_GTKMM_NS {

// how long an urgency hint is shown (in milisec)
unsigned const Chatter::URGENCY_HINT_TIMEOUT = 2000;

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **
 ** @return    -
 **/
Chatter::Chatter(Base* const parent) :
  Base(parent),
  StickyDialog("Chatter", false)
{
  this->ui->add_window(*this);
  this->init();
} // Chatter::Chatter(Base* parent)

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
Chatter::~Chatter()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Chatter::init()
{
  this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                   ::translation("~chatter")));
  this->set_icon(this->ui->icon);
  {
    Gdk::Geometry geometry;
    geometry.min_width = 2 * this->ui->cards->height();
    geometry.min_height = static_cast<int>(2.5 * this->ui->cards->height());
    this->set_geometry_hints(*this, geometry, Gdk::HINT_MIN_SIZE);
  }

  { // close button
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
    close_button->show_all();

    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
    close_button->grab_default();
  } // close button

  { // text view
    this->text = Gtk::manage(new Gtk::TextView());
    auto buffer = this->text->get_buffer();
    buffer->create_tag("0")->set_property("foreground", Glib::ustring("blue"));
    buffer->create_tag("1")->set_property("foreground", Glib::ustring("darkgreen"));
    buffer->create_tag("2")->set_property("foreground", Glib::ustring("red"));
    buffer->create_tag("3")->set_property("foreground", Glib::ustring("brown"));
    buffer->create_tag("default")->set_property("foreground", Glib::ustring("black"));
  }
#if 0
  this->ui->translations->add(*this->text,
                              ::translation("Version: %sversion%\n%t_about_%",
                                            static_cast<string>(::version),
                                            ::translation("_about_")));
#endif


  this->set_default_size(this->ui->logo->get_width(),
                         this->ui->logo->get_height() * 2);
#ifdef POSTPONED
  this->get_window()->set_decorations(Gdk::DECOR_BORDER
                                      | Gdk::DECOR_RESIZEH
                                      | Gdk::DECOR_TITLE
                                      | Gdk::DECOR_MENU);

  { // the image
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->set_border_width(parent->ui->logo->get_width() / 20);
    auto image = Gtk::manage(new Gtk::Image(parent->ui->logo));
    alignment->add(*image);

    this->get_vbox()->pack_start(*alignment, Gtk::PACK_SHRINK);
  } // the image
#endif
  { // the text
    auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
    text_window->add(*this->text);
    text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    this->text->set_editable(false);
    this->text->set_wrap_mode(Gtk::WRAP_WORD);
    this->text->set_cursor_visible(false);

    this->get_vbox()->add(*text_window);
  } // the text
  { // the chat line
    this->chat_line = Gtk::manage(new Gtk::Entry());
    this->get_vbox()->pack_start(*this->chat_line, false, false);
    this->chat_line->signal_activate().connect(sigc::mem_fun(*this, &Chatter::chat_now));
    this->chat_line->grab_focus();
  } // the chat line
  { // send button
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER,
                                       0, 0));
    auto send_button
      = Gtk::manage(new Gtk::StockButton("send"));
    this->ui->translations->add(*send_button,
                                ::translation("send"));
    alignment->add(*send_button);

    this->get_vbox()->pack_end(*alignment, Gtk::PACK_SHRINK);
    send_button->signal_clicked().connect(sigc::mem_fun(*this, &Chatter::chat_now));
  } // send button

  this->show_all_children();

  return ;
} // void Chatter::init()

/**
 ** 'player' has chatted 'message'
 **
 ** @param     player   player who has chatted
 ** @param     message   message of the player
 **
 ** @return    -
 **/
void
Chatter::chat(string const& player, string const& message)
{
  auto buffer = this->text->get_buffer();
  if (player.empty())
    buffer->insert_with_tag(buffer->end(),
                            message + '\n',
                            "default");
  else
    buffer->insert_with_tag(buffer->end(),
                            player + ": " + message + '\n',
                            this->tag(player));
  auto end = buffer->end();
  //this->text->scroll_to(buffer->get_iter_at_line(-1));
  this->text->scroll_to(end);

#if GTKMM_VERSION_GE(2,8)
  if (::setting(Setting::NETWORK_CHAT_URGENCY_HINT)) {
    this->set_urgency_hint();
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &Chatter::stop_urgency_hint),
                                   URGENCY_HINT_TIMEOUT);
  } // if (::setting(Setting::NETWORK_CHAT_URGENCY_HINT))
#endif

  return ;
} // void Chatter::chat(string player, string message)

/**
 ** the player chats the text in 'entry'
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Chatter::chat_now()
{
  ::ui->chat(::setting(Setting::NAME), this->chat_line->get_text());
  this->chat_line->set_text("");
  this->chat_line->grab_focus();
  return ;
} // void Chatter::chat_now()

/**
 ** -> result
 **
 ** @param     player   player
 **
 ** @return    tag for the player
 **/
string
Chatter::tag(string const& player)
{
  size_t p;
  for (p = 0; p < this->ui->party().playerno(); ++p)
    if (this->ui->party().player(p).name() == player)
      break;

  switch(p) {
  case 0:
    return "0";
  case 1:
    return "1";
  case 2:
    return "2";
  case 3:
    return "3";
  default:
    return "default";
  } // switch(p)
} // string Chatter::tag(string const& player)

/**
 ** stop the urgency hint
 **
 ** @param     -
 **
 ** @return    false (stop the timeout signal)
 **/
bool
Chatter::stop_urgency_hint()
{
#if GTKMM_VERSION_GE(2,8)
  this->set_urgency_hint(false);
#endif
  return false;
} // bool Chatter::stop_urgency_hint()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
