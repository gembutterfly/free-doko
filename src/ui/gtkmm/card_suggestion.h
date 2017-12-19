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

#ifndef GTKMM_CARD_SUGGESTION_HEADER
#define GTKMM_CARD_SUGGESTION_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"

#include "../../card/card.h"
class Player;

#include <gdk/gdkevents.h>
namespace Gtk {
class EventBox;
class Image;
class Label;
class TextView;
class StockButton;
}; // namespace Gtk

namespace UI_GTKMM_NS {
class Table;

/**
 ** @brief	card suggestion dialog
 **
 ** @author	Diether Knof
 **/
class CardSuggestion : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    CardSuggestion(Table* table);
    // destructor
    ~CardSuggestion() = default;

    // the corresponding player
    ::Player const& player();

    // show the suggested card
    void show_information(bool show_window);
    // play the suggested card
    void play_card();

    // a card has been played
    void card_played();

  private:
    // initialize the window
    void init();
    // get a suggestion
    void get_suggestion();
    // the card shall be played
    bool play_card_signal(GdkEventButton*);

  public:
    // a mouse click on the table
    virtual void mouse_click_on_table();

  private:
    Gtk::EventBox* card_box = nullptr;
    Gtk::Image* card_image = nullptr;
    Gtk::Label* rationals_text = nullptr;
    Gtk::TextView* rationals_description = nullptr;

    Gtk::StockButton* play_button = nullptr;

  public:
    Card suggested_card;

  private:
    bool thinking = false;

  private: // unused
    CardSuggestion() = delete;
    CardSuggestion(CardSuggestion const&) = delete;
    CardSuggestion& operator=(CardSuggestion const&) = delete;
}; // class CardSuggestion : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_CARD_SUGGESTION_HEADER

#endif // #ifdef USE_UI_GTKMM
