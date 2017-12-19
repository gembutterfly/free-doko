/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Rules as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Rules, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Rules for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Rules
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

#ifndef GTKMM_WIDGET_LABEL_CARD_SELECTOR_HEADER
#define GTKMM_WIDGET_LABEL_CARD_SELECTOR_HEADER

#include "label_type_selector.h"

#include "../../../card/card.h"

namespace Gtk {
  /**
   ** @brief    a selector for cards with a label
   **
   ** @author   Diether Knof
   **/
  class LabelCardSelector : public LabelTypeSelector<Card> {
    public:
      // constructor
      LabelCardSelector();
      // constructor
      LabelCardSelector(Glib::ustring const& label);
      // destructor
      ~LabelCardSelector();

      // returns the selected type
      Card card();
      // set the selected type
      void set_card(Card const& card);

    private: // unused
      LabelCardSelector(LabelCardSelector const&);
      LabelCardSelector& operator=(LabelCardSelector const&);
  }; // class LabelCardSelector : public HBox

} // namespace Gtk

#endif // #ifdef GTKMM_WIDGET_LABEL_CARD_SELECTOR_HEADER

#endif // #ifdef USE_UI_GTKMM
