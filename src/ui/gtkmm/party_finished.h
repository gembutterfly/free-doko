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

#ifndef GTKMM_PARTY_FINISHED_HEADER
#define GTKMM_PARTY_FINISHED_HEADER

#include "base.h"
#include "ui.h"
#include "widgets/sticky_dialog.h"

#include <gdk/gdkevents.h>
namespace Gtk {
class DrawingArea;
} // namespace Gtk

class Player;

namespace UI_GTKMM_NS {
/**
 ** @brief	the party finished window
 **/
class PartyFinished : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    PartyFinished(Base* parent);
    // destructor
    ~PartyFinished();

    // the party is finished
    void party_finished();

    // redraw the graph
    void redraw_points_graph();

    // the name of the player has changed
    void name_changed(::Player const& player);

  private:
    // initialize the window
    void init();
    // the window is hidden
    void on_hide();
    // the graph shall be redrawn
    bool on_draw_graph(::Cairo::RefPtr<::Cairo::Context> const& cr);

  private:
    Gtk::DrawingArea* points_graph = nullptr;

    Gtk::StockButton* show_party_points = nullptr;
    Gtk::StockButton* close_party_button = nullptr;

  private: // unused
    PartyFinished() = delete;
    PartyFinished(PartyFinished const&) = delete;
    PartyFinished& operator=(PartyFinished const&) = delete;
}; // class PartyFinished : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PARTY_FINISHED_HEADER

#endif // #ifdef USE_UI_GTKMM
