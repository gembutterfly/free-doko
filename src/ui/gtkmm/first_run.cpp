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

#include "first_run.h"

#include "ui.h"
#include "translations.h"

#include "widgets/stock_button.h"
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/texttag.h>
#include <gtkmm/alignment.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent    the parent object
 ** @param     message   the message for the first run
 **
 ** @return    -
 **/
FirstRun::FirstRun(Base* const parent,
                   Translator::Translation const& message) :
  Base(parent),
  Dialog("FirstRun"),
  message(message),
  text_buffer(nullptr)
{
  this->ui->add_window(*this);

  this->ui->translations->add(*this, ::translation("FreeDoko first run"));
  this->set_icon(this->ui->icon);

  this->set_default_size(static_cast<int>(this->ui->logo->get_width() * 1.5),
                         static_cast<int>(this->ui->logo->get_height() * 2.5));
#ifdef POSTPONED
  this->get_window()->set_decorations(Gdk::DECOR_BORDER
                                      | Gdk::DECOR_RESIZEH
                                      | Gdk::DECOR_TITLE
                                      | Gdk::DECOR_MENU);
#endif

  { // close button
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
    close_button->show_all();

    close_button->grab_default();
    // signals
    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  } // close button


  { // the image
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->set_border_width(parent->ui->logo->get_width() / 20);
    auto image = Gtk::manage(new Gtk::Image(parent->ui->logo));
    alignment->add(*image);

    this->get_vbox()->pack_start(*alignment, Gtk::PACK_SHRINK);
  } // the image
  { // the text
    auto text = Gtk::manage(new Gtk::TextView());
    this->text_buffer = text->get_buffer();
    {
      auto tag_title = this->text_buffer->create_tag("title");
      //tag_title->set_property("weight", Pango::WEIGHT_BOLD);
      tag_title->set_property("scale", Pango::SCALE_X_LARGE);
      tag_title->set_property("justification", Gtk::JUSTIFY_CENTER);

      auto tag_endline = this->text_buffer->create_tag("endline");
      tag_endline->set_property("style", Pango::STYLE_ITALIC);
      tag_endline->set_property("justification", Gtk::JUSTIFY_CENTER);

    }


    auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
    text_window->add(*text);
    text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    text->set_editable(false);
    text->set_wrap_mode(Gtk::WRAP_WORD);
    text->set_cursor_visible(false);

    this->get_vbox()->pack_start(*text_window);
  } // the text

  this->language_update();
  this->signal_show().connect(sigc::mem_fun(*this,
                                            &FirstRun::language_update));
  this->show_all_children();

  return ;
} // FirstRun::FirstRun(Base* parent, Translator::Translation message)

/**
 ** update the language
 **
 ** @param     -
 **
 ** @return    -
 **/
void
FirstRun::language_update()
{
  if (!this->get_realized())
    return ;

  this->text_buffer->erase(this->text_buffer->begin(),
                           this->text_buffer->end());

  Glib::ustring message_text = this->message.translation();
  size_t pos = 0;
  { // title
    size_t const title_pos = message_text.find("\n\n") ;
    if (title_pos == message_text.find("\n")) {
      // found the title!

      this->text_buffer->insert_with_tag(this->text_buffer->begin(),
                                         "\n" +
                                         Glib::ustring(message_text,
                                                       0, title_pos + 2),
                                         "title");
      pos = title_pos + 2;
    } // if (title found)
  } // title
  { // end line
    auto const end_pos = message_text.rfind("\n\n") ;
    if (end_pos == message_text.rfind("\n") - 1) {
      // found the end line!

      this->text_buffer->insert(this->text_buffer->end(),
                                Glib::ustring(message_text,
                                              pos,
                                              end_pos - pos + 1));
      this->text_buffer->insert_with_tag(this->text_buffer->end(),
                                         Glib::ustring(message_text,
                                                       end_pos + 1),
                                         "endline");
    } else { // if !(end line found)
      this->text_buffer->insert(this->text_buffer->end(),
                                Glib::ustring(message_text, pos));
    } // if !(end line found)
  } // end line

  return ;
} // void FirstRun::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
