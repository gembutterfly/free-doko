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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "label_card_selector.h"

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelCardSelector::LabelCardSelector() :
    LabelTypeSelector<Card>()
  {  }

  /**
   ** constructor
   **
   ** @param	label    text for the label
   **
   ** @return	-
   **/
  LabelCardSelector::LabelCardSelector(Glib::ustring const& label) :
    LabelTypeSelector<Card>(label)
  {  }

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelCardSelector::~LabelCardSelector()
  { }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the card
   **/
  Card
    LabelCardSelector::card()
    {
      return this->type();
    }

  /**
   ** set the card
   **
   ** @param	card   new card
   **
   ** @return	-
   **/
  void
    LabelCardSelector::set_card(Card const& card)
    {
      this->set_type(card);
      return ;
    }

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
