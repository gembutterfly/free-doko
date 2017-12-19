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

#include "preferences.h"
#include "ui.h"
#include "translations.h"
#include "cards.h"

#include "../../misc/setting.h"

#include <gtkmm/notebook.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/table.h>
#include <gtkmm/arrow.h>
namespace UI_GTKMM_NS {

  /**
   ** creates all subelements for the cardsorder
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Preferences::init_cardsorder()
    {
      auto vbox = this->add_group_vbox("cards order");
      vbox->set_border_width(2 EX);

      { // the cards
	auto table = Gtk::manage(new Gtk::Table(3, Card::NUMBER_OF_TCOLORS));
	vbox->add(*table);
	table->set_col_spacings(10);

	for (unsigned p = 0; p < Card::NUMBER_OF_TCOLORS; ++p) {
	  { // left/right button
	    auto hbox = Gtk::manage(new Gtk::HBox(true));
	    table->attach(*hbox, p, p + 1, 0, 1);

	    auto left_button = Gtk::manage(new Gtk::Button());
	    left_button->add(*Gtk::manage(new Gtk::Arrow(Gtk::ARROW_LEFT,
                                                         Gtk::SHADOW_OUT)));
	    hbox->add(*left_button);
	    left_button->signal_clicked().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Preferences::cards_order_pos_to_left), p));

            auto right_button = Gtk::manage(new Gtk::Button());
	    right_button->add(*Gtk::manage(new Gtk::Arrow(Gtk::ARROW_RIGHT,
							  Gtk::SHADOW_OUT)));
	    hbox->add(*right_button);
	    right_button->signal_clicked().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Preferences::cards_order_pos_to_right), p));
	  } // left/right button
	  { // the image
	    if (::setting(Setting::CARDS_ORDER).tcolor(p) == Card::TRUMP)
	      this->sorting_tcolor_image.push_back(this->ui->cards->new_managed_image(Card::CLUB_QUEEN));
	    else
	      this->sorting_tcolor_image.push_back(this->ui->cards->new_managed_image(Card(::setting(Setting::CARDS_ORDER).tcolor(p), Card::ACE)));
	    table->attach(*(this->sorting_tcolor_image.back()), p, p + 1, 1, 2);
	  } // the image
	  { // up/down buttons
	    auto vbox2 = Gtk::manage(new Gtk::VBox());
	    table->attach(*vbox2, p, p + 1, 2, 3);

	    Gtk::RadioButton::Group group;
	    this->sorting_up_button.push_back(Gtk::manage(new Gtk::RadioButton(group,
									       "up")));
	    this->ui->translations->add(*(this->sorting_up_button.back()),
				       ::translation(Setting::CardsOrder::UP));
	    vbox2->add(*(this->sorting_up_button.back()));

	    this->sorting_down_button.push_back(Gtk::manage(new Gtk::RadioButton(group,
										 "down")));
	    this->ui->translations->add(*(this->sorting_down_button.back()),
				       ::translation(Setting::CardsOrder::DOWN));
	    vbox2->add(*(this->sorting_down_button.back()));

	    // events
	    this->sorting_up_button.back()->set_active(::setting(Setting::CARDS_ORDER).direction(p) == Setting::CardsOrder::UP);
	    this->sorting_down_button.back()->set_active(::setting(Setting::CARDS_ORDER).direction(p) == Setting::CardsOrder::DOWN);
	    this->sorting_up_button.back()->signal_toggled().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Preferences::cards_order_direction_set), p));
	    this->sorting_down_button.back()->signal_toggled().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Preferences::cards_order_direction_set), p));
	  } // up/down buttons
	} // for (c < Card::NUMBER_OF_TCOLORS)
      } // the cards

      { // sorted/unsorted
	auto alignment2
	  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
					   0, 0));
	vbox->add(*alignment2);

	auto vbox2 = Gtk::manage(new Gtk::VBox(false));
	alignment2->add(*vbox2);

	Gtk::RadioButton::Group group;

	this->sorted_button
	  = Gtk::manage(new Gtk::RadioButton(group, "sorted"));
	this->ui->translations->add(*(this->sorted_button),
				   ::translation("Setting::CardsOrder::sorted"));
	vbox2->add(*(this->sorted_button));

	this->unsorted_button
	  = Gtk::manage(new Gtk::RadioButton(group, "unsorted"));
	this->ui->translations->add(*(this->unsorted_button),
				   ::translation("Setting::CardsOrder::unsorted"));
	vbox2->add(*(this->unsorted_button));

	if (::setting(Setting::CARDS_ORDER).mixed())
	  this->unsorted_button->set_active();
	else
	  this->sorted_button->set_active();

	this->sorted_button->signal_toggled().connect(sigc::mem_fun(*this, &Preferences::cards_order_set_sorted));
	this->unsorted_button->signal_toggled().connect(sigc::mem_fun(*this, &Preferences::cards_order_set_sorted));
      } // sorted/unsorted

      { // mix the cards
	auto alignment2
	  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
					   0, 0));
	vbox->add(*alignment2);

	this->mix_cards_button = Gtk::manage(new Gtk::Button("mix cards"));
	alignment2->add(*(this->mix_cards_button));
	this->ui->translations->add(*(this->mix_cards_button),
				   ::translation("Setting::CardsOrder::mix cards"));
	this->mix_cards_button->signal_clicked().connect(sigc::mem_fun(*this, &Preferences::cards_order_mix));
      } // mix the cards

      return ;
    } // void Preferences::init_cardsorder()

  /**
   ** update the cards order
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Preferences::update_cards_order()
    {
      if (::setting(Setting::CARDS_ORDER).mixed()) {
	this->unsorted_button->set_active();
      } else { // if !(::setting(Setting::CARDS_ORDER).mixed())
	this->sorted_button->set_active();

	for (unsigned p = 0; p < Card::NUMBER_OF_TCOLORS; ++p) {
	  if (::setting(Setting::CARDS_ORDER).tcolor(p) == Card::TRUMP)
	    this->ui->cards->change_managed(this->sorting_tcolor_image[p],
                                            Card::CLUB_QUEEN);
	  else
	    this->ui->cards->change_managed(this->sorting_tcolor_image[p],
                                            Card(::setting(Setting::CARDS_ORDER).tcolor(p), Card::ACE));

	  if (::setting(Setting::CARDS_ORDER).direction(p)
              == Setting::CardsOrder::UP)
	    this->sorting_up_button[p]->set_active();
	  else
	    this->sorting_down_button[p]->set_active();

	} // for (n < Card::NUMBER_OF_TCOLORS)
      } // if !(::setting(Setting::CARDS_ORDER).mixed())

      return ;
    } // void Preferences::update_cards_order()

  /**
   ** show the cards order
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Preferences::show_cards_order()
    {
      this->realize();
      this->group_notebook->set_current_page(5);
      this->present();

      return ;
    } // void Preferences::show_cards_order()

  /**
   ** event: set the sorting direction
   **
   ** @param     pos   the position which is changed
   **
   ** @return    -
   **/
  void
    Preferences::cards_order_direction_set(unsigned const pos)
    {
      ::setting(Setting::CARDS_ORDER).sorted_set(true);
      if (this->sorting_up_button[pos]->get_active())
	::setting(Setting::CARDS_ORDER).direction_set(pos,
                                                      Setting::CardsOrder::UP);
      else
	::setting(Setting::CARDS_ORDER).direction_set(pos,
                                                      Setting::CardsOrder::DOWN);

      return ;
    } // void Preferences::cards_order_direction_set(unsigned pos)

  /**
   ** event: shift the type at 'pos' one position to the left
   **
   ** @param     pos   the position which is shifted
   **
   ** @return    -
   **/
  void
    Preferences::cards_order_pos_to_left(unsigned const pos)
    {
      ::setting(Setting::CARDS_ORDER).sorted_set(true);
      ::setting(Setting::CARDS_ORDER).pos_to_left(pos);

      return ;
    } // void Preferences::cards_order_pos_to_left(unsigned pos)

  /**
   ** event: shift the type at 'pos' one position to the right
   **
   ** @param     pos   the position which is shifted
   **
   ** @return    -
   **/
  void
    Preferences::cards_order_pos_to_right(unsigned const pos)
    {
      ::setting(Setting::CARDS_ORDER).sorted_set(true);
      ::setting(Setting::CARDS_ORDER).pos_to_right(pos);

      return ;
    } // void Preferences::cards_order_pos_to_right(unsigned pos)

  /**
   ** event: set the sorted state
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Preferences::cards_order_set_sorted()
    {
      ::setting(Setting::CARDS_ORDER).sorted_set(this->sorted_button->get_active());

      return ;
    } // void Preferences::cards_order_set_sorted()

  /**
   ** event: mix the cards
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Preferences::cards_order_mix()
    {
      if (::setting(Setting::CARDS_ORDER).sorted() == true)
	::setting(Setting::CARDS_ORDER).sorted_set(false);
      else
	::setting(Setting::CARDS_ORDER).mix();

      return ;
    } // void Preferences::cards_order_mix()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
