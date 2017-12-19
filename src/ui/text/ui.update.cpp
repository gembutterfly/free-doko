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

#ifdef USE_UI_TEXT

#include "ui.h"

#include "../../misc/setting.h"
#include "../../misc/translations.h"

namespace UI_TEXT_NS {

  /**
   **
   ** updates all language elements
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::language_update()
    {
      return ;
    } // void UI_Text::language_update()

  /**
   **
   ** redraw all (of the playfield)
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::redraw_all()
    { 
      return ;
    } // void UI_Text::redraw_all()

  /**
   **
   ** update the gametype icon
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::gametype_update()
    { 
      return ;
    } // void UI_Text::gametype_udpate()

  /**
   **
   ** update the trick
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::trick_update()
    { 
      return ;
    } // void UI_Text::trick_update()

  /**
   **
   ** update the name of 'player'
   **
   ** @param	player	the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::name_update(Player const& player)
    {
      return ;
    } // void UI_Text::name_update(player)

  /**
   **
   ** update the hand of 'player'
   **
   ** @param	player	the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::hand_update(Player const& player)
    { 
      return ;
    } // void UI_Text::hand_update(player)

  /**
   **
   ** update the trickpile of 'player'
   **
   ** @param	player	the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::trick_pile_update(Player const& player)
    { 
      return ;
    } // void UI_Text::trick_pile_udpate(player)


  /**
   **
   ** update the team icon of 'player'
   **
   ** @param	player	the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::teaminfo_update(Player const& player)
    { 
      return ;
    } // void UI_Text::teaminfo_udpate(player)

  /**
   **
   ** update the announcement icon of 'player'
   **
   ** @param	player	the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::announcement_update(Player const& player)
    { 
      return ;
    } // void UI_Text::announcement_udpate(player)

  /**
   **
   ** update the rule
   **
   ** @param	type		the rule, that has changed
   ** @param	old_value	old value
   **
   ** @return	-
   **
   ** @version	0.5.3
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::rule_update(int const type, void const* const old_value)
    {
      return ;
    } // void UI_Text::rule_update(int const type, void const* const old_value)

  /**
   **
   ** show the update
   **
   ** @param	type		the setting, that has changed
   ** @param	old_value	old value
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  void
    UI_Text::setting_update(int const type, void const* const old_value)
    {
      if ((type >= Setting::BOOL_FIRST)
	  && (type <= Setting::BOOL_LAST))
	this->ostr() << ::translation("Setting")
	  << ": " << ::translation(Setting::TypeBool(type))
	  << " = " << ::setting(Setting::TypeBool(type))
	  << " (" << *(static_cast<bool const* const>(old_value)) << ")"
	  << '\n';
      else if ((type >= Setting::UNSIGNED_FIRST)
	  && (type <= Setting::UNSIGNED_LAST))
	this->ostr() << ::translation("Setting")
	  << ": " << ::translation(Setting::TypeUnsigned(type))
	  << " = " << ::setting(Setting::TypeUnsigned(type))
	  << " (" << *(static_cast<unsigned const* const>(old_value)) << ")"
	  << '\n';
      else if ((type >= Setting::STRING_FIRST)
	       && (type <= Setting::STRING_LAST))
	this->ostr() << ::translation("Setting")
	  << ": " << ::translation(Setting::TypeString(type))
	  << " = " << ::setting(Setting::TypeString(type))
	  << " (" << *(static_cast<string const* const>(old_value)) << ")"
	  << '\n';
      else if (type == Setting::CARDS_ORDER)
	this->ostr() << ::translation("Setting")
	  << ": " << ::translation(::name(Setting::TypeCardsOrder(type)))
	  << " = " << ::setting(Setting::TypeCardsOrder(type))
	  << " (" << *(static_cast<Setting::CardsOrder const* const>(old_value)) << ")"
	  << '\n';

      return ;
    } // void UI_Text::setting_update(int type, void const* old_value)

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
