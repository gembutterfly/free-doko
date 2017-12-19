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

#ifndef GTKMM_GENSCHER_SELECTION_HEADER
#define GTKMM_GENSCHER_SELECTION_HEADER

#include "base.h"

class Player;

#include "widgets/sticky_dialog.h"
namespace Gtk {
class Label;
class Button;
}; // namespace Gtk

namespace UI_GTKMM_NS {
class Table;

/**
 ** @brief	genscher selection dialog
 **
 ** @author	Diether Knof
 **/
class GenscherSelection : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    GenscherSelection(Table* table);
    // destructor
    ~GenscherSelection();

    // select a partner
    Player const* select_partner(Player const& genscher);

    // the name of a player has changed
    void name_changed(Player const& player);

  private:
    // initialize the window
    void init();

    // a partner is selected
    void select_partner_event(Player const* const partner);

  private:
    Gtk::Label* info_label = nullptr;
    vector<Gtk::Button*> partner_button;
    Gtk::Button* no_partner_change_button = nullptr;

    Player const* genscher = nullptr;
    Player const* partner = nullptr;

  private: // unused
    GenscherSelection() = delete;
    GenscherSelection(GenscherSelection const&) = delete;
    GenscherSelection& operator=(GenscherSelection const&) = delete;
}; // class GenscherSelection : public GameinfoDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GENSCHER_SELECTION_HEADER

#endif // #ifdef USE_UI_GTKMM
