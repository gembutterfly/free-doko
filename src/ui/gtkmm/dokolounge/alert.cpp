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

#include "alert.h"

#include "../ui.h"
#include "../translations.h"

#include "../widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/alignment.h>

namespace UI_GTKMM_NS {
  namespace DokoLounge {

    /**
     ** constructor
     **
     ** @param     parent   the parent object
     **
     ** @return    -
     **/
    Alert::Alert(Base* const parent) :
      Base(parent),
      StickyDialog("Alert", false)
    {
      this->ui->add_window(*this);
      this->signal_realize().connect(sigc::mem_fun(*this,
                                                   &Alert::init));

      return ;
    } // Alert::Alert(Base* const parent)

    /**
     ** destruktor
     **
     ** @param     -
     **
     ** @return    -
     **/
    Alert::~Alert()
    { }

    /**
     ** create all subelements
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Alert::init()
      {
        this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                         ::translation("~DokoLounge: Alert")));
        this->set_icon(this->ui->icon);

        this->set_skip_taskbar_hint();

        this->text = Gtk::manage(new Gtk::TextView());

        auto close_button
          = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
        this->ui->translations->add(*close_button,
                                    ::translation("close"));
        this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
        close_button->show_all();

        close_button->grab_default();
        // signals
        close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));


        this->set_default_size((this->ui->logo->get_width() * 3) / 2,
                               (this->ui->logo->get_height() * 5) / 2);
#ifdef POSTPONED
        this->get_window()->set_decorations(Gdk::DECOR_BORDER
                                            | Gdk::DECOR_RESIZEH
                                            | Gdk::DECOR_TITLE
                                            | Gdk::DECOR_MENU);
#endif

#if 0
        { // the image
          Gtk::Alignment* alignment
            = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                             0, 0));
          alignment->set_border_width(parent->ui->logo->get_width() / 20);
          Gtk::Image* image = Gtk::manage(new Gtk::Image(parent->ui->logo));
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
        this->text->get_buffer()->create_tag("title")->property_weight()
          = PANGO_WEIGHT_BOLD;

          this->get_vbox()->add(*text_window);
        } // the text

        this->show_all_children();

        return ;
      } // void Alert::init()

    /**
     ** show the alert and update the text
     **
     ** @param     text   new text to show
     **
     ** @return    -
     **/
    void
      Alert::show(string const& title, string const& text)
      {
        if (!this->get_realized())
          this->realize();
        this->text->get_buffer()->set_text(text);
        this->text->get_buffer()->insert_with_tag(this->text->get_buffer()->begin(), title + "\n\n", "title");
        this->Gtk::StickyDialog::show();
        return;
      } // void Alert::show(string title, string text)

    /**
     ** update the alert
     **
     ** @param     text   new text to show
     **
     ** @return    -
     **/
    void
      Alert::update_text(string const& title, string const& text)
      {
        this->text->get_buffer()->set_text(text);
        return;
      } // void Alert::update_text(string title, string text)

  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
