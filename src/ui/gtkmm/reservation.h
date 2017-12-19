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
 **********************************************************************/

#ifdef USE_UI_GTKMM

#ifndef GTKMM_RESERVATION_HEADER
#define GTKMM_RESERVATION_HEADER

#include "htin.h"
#include "translations.h"

#include "../../game/reservation.h"

#include "widgets/sticky_dialog.h"
namespace Gtk {
class StockButton;
class CheckButton;
class RadioButton;
class Notebook;
class Label;
class Table;
} // namespace Gtk

namespace UI_GTKMM_NS {
class Table;

/**
 ** @brief	the reservation
 **
 ** @todo	ordering of the swines
 **/
class Reservation : public HTIN, public Gtk::StickyDialog {
  public:
    // constructor (logical parent, player for the reservation)
    Reservation(Table* table, Position position);
    // destructor
    ~Reservation();

    // the corresponding player
    Player& player();
    // the corresponding game
    Game& game();

    // show the reservation for selecting before it is the turn of the player
    void show_for_selection();

    // get a reservation
    ::Reservation get();

    // dummy function
    void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update);

    // set the default reservation
    void set_default();
    // the gametype has changed (by the user)
    void gametype_changed(int gametype);
    // the marriage selector has changed (by the user)
    void marriage_selector_changed(int marriage_selector);
    // the swines butten has changed (by the user)
    void swines_changed();

    // update the game/swines announcement, ... according to the selected reservation
    void update_for_reservation();

    // update the sensitivity (i.e. swines, marriage, poverty)
    void sensitivity_update();

    // update the selected reservation (when the page was changed)
    void update_player_reservation();

  private:
    // initialize the window
    void init();

    // the selected page has changed
    void switch_page_event(Widget*, guint pageno);

    // close the window and announce the reservation
    void announce();

  private:
    Gtk::StockButton* announce_button = nullptr;

    Gtk::Label* bock_label = nullptr;

    Gtk::Notebook* notebook = nullptr;
    Gtk::CheckButton* swines_button = nullptr;
    Gtk::CheckButton* hyperswines_button = nullptr;
    Gtk::CheckButton* solo_swines_button = nullptr;
    Gtk::CheckButton* solo_hyperswines_button = nullptr;
    vector<Gtk::RadioButton*> gametype_buttons;
    vector<Gtk::RadioButton*> marriage_selector_buttons;

    Gtk::Label* remaining_rounds_label = nullptr;
    Gtk::Label* remaining_rounds_number = nullptr;
    Gtk::Label* remaining_games_label = nullptr;
    Gtk::Label* remaining_games_number = nullptr;
    Gtk::Label* remaining_points_label = nullptr;
    Gtk::Label* remaining_points_number = nullptr;
    Gtk::Label* duty_free_soli_label = nullptr;
    Gtk::Label* duty_free_soli_number = nullptr;
    Gtk::Label* duty_color_soli_label = nullptr;
    Gtk::Label* duty_color_soli_number = nullptr;
    Gtk::Label* duty_picture_soli_label = nullptr;
    Gtk::Label* duty_picture_soli_number = nullptr;

    Gtk::CheckButton* offer_duty_solo_button = nullptr;

    // whether this reservation is already announced
    bool announced = false;
    // whether the window is in an update mode
    bool in_update = false;

  private: // unused
    Reservation() = delete;
    Reservation(Reservation const&) = delete;
    Reservation& operator=(Reservation const&) = delete;
}; // class Reservation : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_RESERVATION_HEADER

#endif // #ifdef USE_UI_GTKMM
