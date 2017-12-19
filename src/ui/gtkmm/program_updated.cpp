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

#include "program_updated.h"

#include "ui.h"
#include "translations.h"

#include "../../party/party.h"
#include "../../player/player.h"
#include "../../player/aiconfig.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/texttag.h>
#include <gtkmm/alignment.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent        the parent object
   ** @param     old_version   last version
   **
   ** @return    -
   **/
  ProgramUpdated::ProgramUpdated(Base* const parent,
                                 DK::Utils::Version const& old_version) :
    Base(parent),
    Dialog("ProgramUpdated"),
    old_version(old_version)
  {
    this->ui->add_window(*this);

    this->ui->translations->add(*this,
				::translation("FreeDoko program updated"));
    this->set_icon(this->ui->icon);

    this->set_default_size(static_cast<int>(this->ui->logo->get_width() * 1.5),
			   static_cast<int>(this->ui->logo->get_height() * 2.5));
#ifdef POSTPONED
    this->get_window()->set_decorations(Gdk::DECOR_BORDER
					| Gdk::DECOR_RESIZEH
					| Gdk::DECOR_TITLE
					| Gdk::DECOR_MENU);
#endif

    { // action area
      { // reset ais button
	auto reset_ais_button
	  = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "reset ais"));
	this->ui->translations->add(*reset_ais_button,
				    ::translation("reset ais"));
	this->add_action_widget(*reset_ais_button, Gtk::RESPONSE_NONE);

	// signals
	reset_ais_button->signal_clicked().connect(sigc::mem_fun(*this, &ProgramUpdated::reset_ais));
      } // reset ais button
      { // close button
	auto close_button
	  = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
	this->ui->translations->add(*close_button,
				    ::translation("close"));
	this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

	close_button->grab_default();
	// signals
	close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
      } // close button
    } // action area


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
      auto const text = Gtk::manage(new Gtk::TextView());
      this->text_buffer = text->get_buffer();
      {
	auto tag_title = this->text_buffer->create_tag("title");
	//tag_title->set_property("weight", Pango::WEIGHT_BOLD);
	tag_title->set_property("scale", Pango::SCALE_X_LARGE);
	tag_title->set_property("justification", Gtk::JUSTIFY_CENTER);

	auto tag_subtitle = this->text_buffer->create_tag("subtitle");
	tag_title->set_property("weight", Pango::WEIGHT_BOLD);
      }


      auto text_window = Gtk::manage(new Gtk::ScrolledWindow());
      text_window->add(*text);
      text_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
      text->set_editable(false);
      text->set_wrap_mode(Gtk::WRAP_WORD);
      text->set_cursor_visible(false);

      this->get_vbox()->pack_end(*text_window);
    } // the text

    this->language_update();
    this->signal_show().connect(sigc::mem_fun(*this,
                                              &ProgramUpdated::language_update));
    this->show_all_children();

    return ;
  } // ProgramUpdated::ProgramUpdated(Base* parent, DK::Utils::Version old_version)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  ProgramUpdated::~ProgramUpdated()
  { }

  /**
   ** update the language
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    ProgramUpdated::language_update()
    {
      if (!this->get_realized())
	return ;

      this->text_buffer->erase(this->text_buffer->begin(),
			       this->text_buffer->end());

      { // title
	this->text_buffer->insert_with_tag(this->text_buffer->begin(),
					   "\n"
					   + ::translation("Greeting::%sversion%", ::version->number_to_string()).translation(),
					   "title");
      } // title
      for (auto v = ::all_versions.rbegin();
	   ( (v != ::all_versions.rend())
	     && (this->old_version < **v) );
	   ++v) {
	this->text_buffer->insert_with_tag(this->text_buffer->end(),
					   "\n\n"
					   + ::translation("ChangeLog::for %sversion%", (*v)->number_to_string()).translation(),
					   "subtitle");
	this->text_buffer->insert(this->text_buffer->end(),
				  "\n"
				  + ::translation("ChangeLog::" + (*v)->number_to_string()).translation());
      } // for (v \in ::all_versions)

      return ;
    } // void ProgramUpdated::language_update()

  /**
   ** reset the ais
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    ProgramUpdated::reset_ais()
    {
      unsigned n = 0;
      for (auto p : this->ui->party().players())
	if (dynamic_cast<Aiconfig*>(p))
	  dynamic_cast<Aiconfig*>(p)->reset_to_hardcoded(n);

      this->hide();

      return ;
    } // void ProgramUpdated::reset_ais()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
