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

#include "license.h"

#include "ui.h"
#include "translations.h"

#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/alignment.h>
#include <gtkmm/notebook.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   **
   ** @return    -
   **/
  License::License(Base* const parent) :
    Base(parent),
  StickyDialog("License", false)
  {
    this->ui->add_window(*this);
    this->signal_realize().connect(sigc::mem_fun(*this,
					      &License::init));

    return ;
  } // License::License(Base* const parent)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  License::~License()
  { }

  /**
   ** create all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    License::init()
    {
      this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
						      ::translation("~license")));
      this->set_icon(this->ui->icon);

      this->set_skip_taskbar_hint();

      this->program_text = Gtk::manage(new Gtk::TextView());
      this->ui->translations->add(*this->program_text,
				    ::translation("%stext%", ::GPL_string));
      this->cardset_text = Gtk::manage(new Gtk::TextView());
      this->iconset_text = Gtk::manage(new Gtk::TextView());

      this->close_button
	= Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
      this->ui->translations->add(*(this->close_button),
				    ::translation("close"));
      this->add_action_widget(*(this->close_button), Gtk::RESPONSE_CLOSE);
      this->close_button->show_all();

      this->close_button->grab_default();

      this->set_default_size((this->ui->logo->get_width() * 3) / 2,
			     (this->ui->logo->get_height() * 5) / 2);
#ifdef POSTPONED
      this->get_window()->set_decorations(Gdk::DECOR_BORDER
					  | Gdk::DECOR_RESIZEH
					  | Gdk::DECOR_TITLE
					  | Gdk::DECOR_MENU);
#endif

      { // the image
	auto alignment
	  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
					   0, 0));
	alignment->set_border_width(parent->ui->logo->get_width() / 20);
        Gtk::Image* image = Gtk::manage(new Gtk::Image(parent->ui->logo));
        alignment->add(*image);

        this->get_vbox()->pack_start(*alignment, Gtk::PACK_SHRINK);
      } // the image
      { // a notebook whith the different licenses
        auto notebook = Gtk::manage(new Gtk::Notebook);
        //notebook->set_tab_pos(Gtk::POS_LEFT);
        { // the program text
          auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
          text_window->add(*(this->program_text));
          text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
          this->program_text->set_editable(false);
          this->program_text->set_wrap_mode(Gtk::WRAP_WORD);
          this->program_text->set_cursor_visible(false);

          Gtk::Label* label = Gtk::manage(new Gtk::Label("program license"));
          this->ui->translations->add(*label, ::translation("program license"));
          notebook->append_page(*text_window, *label);
        } // the program text
        { // the cardset text
          auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
          text_window->add(*(this->cardset_text));
          text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
          this->cardset_text->set_editable(false);
          this->cardset_text->set_wrap_mode(Gtk::WRAP_WORD);
          this->cardset_text->set_cursor_visible(false);

          auto label = Gtk::manage(new Gtk::Label("iconset license"));
          this->ui->translations->add(*label, ::translation("cardset license"));
          notebook->append_page(*text_window, *label);
        } // the cardset text
        { // the iconset text
          auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
          text_window->add(*(this->iconset_text));
          text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
          this->iconset_text->set_editable(false);
          this->iconset_text->set_wrap_mode(Gtk::WRAP_WORD);
          this->iconset_text->set_cursor_visible(false);

          auto label = Gtk::manage(new Gtk::Label("iconset license"));
          this->ui->translations->add(*label, ::translation("iconset license"));
          notebook->append_page(*text_window, *label);
        } // the iconset text
        this->get_vbox()->pack_start(*notebook);
      } // a notebook whith the different licenses

      this->license_update();
      this->show_all_children();

      // signals
      this->close_button->signal_clicked().connect(sigc::mem_fun(*this, &License::hide));

      return ;
    } // void License::init()

  /**
   ** updates the license
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    License::license_update()
    {
      if (!this->get_realized())
        return ;

      if (::setting(Setting::CARDSET_LICENSE).empty())
        this->cardset_text->get_buffer()->set_text("???");
      else
        this->cardset_text->get_buffer()->set_text(::setting(Setting::CARDSET_LICENSE));

      if (::setting(Setting::ICONSET_LICENSE).empty())
        this->iconset_text->get_buffer()->set_text("???");
      else
        this->iconset_text->get_buffer()->set_text(::setting(Setting::ICONSET_LICENSE));

      return ;
    } // void License::license_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
