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

#ifndef GTKMM_HAND_HEADER
#define GTKMM_HAND_HEADER

#include "htin.h"

#include "../../card/card.h"

namespace UI_GTKMM_NS {

/**
 ** @brief	the graphic representation of a hand
 **
 ** @todo	use of 'Gdk::Point' and 'Gdk::Rectangle'
 **/
class Hand : public HTIN {
  public:
    Hand(Table* table, Position position);
    ~Hand() = default;

    using HTIN::draw;
    void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);
    void draw_tricks(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);

    int pos_x(Position position) const;
    int pos_y(Position position) const;
    int width(Position position) const;
    int height(Position position) const;
    int start_x(Position position) const;
    int start_y(Position position) const;
    int margin_x(Position position) const;
    int margin_y(Position position) const;

    int pos_x() const;
    int pos_y() const;
    int width() const;
    int height() const;
    int start_x() const;
    int start_y() const;
    double cards_step_x(unsigned cards_no) const;
    double cards_step_y(unsigned cards_no) const;
    int margin_x() const;
    int margin_y() const;

    unsigned cardno_at_position(int x, int y) const;
    Card card_at_position(int x, int y) const;

  private: // unused
    Hand() = delete;
    Hand(Hand const&) = delete;
    Hand& operator=(Hand const&) = delete;
}; // class Hand : public HTIN

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_HAND_HEADER

#endif // #ifdef USE_UI_GTKMM
