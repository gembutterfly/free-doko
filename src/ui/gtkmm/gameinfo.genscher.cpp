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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "gameinfo.genscher.h"

#include "ui.h"
#include "translations.h"
#include "icons.h"

#include "../../game/game.h"
#include "../../player/player.h"

#include "../../utils/string.h"

#include <gtkmm/image.h>
#include <gtkmm/label.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent     the parent object
   ** @param     genscher   the player with the genscher
   ** @param     partner    the new partner
   **
   ** @return    -
   **/
  GameInfoDialog::Information::Genscher::Genscher(Base* const parent,
               Player const& genscher,
               Player const& partner) :
    GameInfoDialog::Information(parent, ::translation("genscher")),
    genscher(&genscher),
    partner(&partner)
    {
      this->ui->icons->change_managed(this->icon,
                                      this->genscher->game().type());

      this->init();

      return ;
    } // GameInfoDialog::Information::Genscher::Genscher(Base parent, Game game)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  GameInfoDialog::Information::Genscher::~Genscher()
  { }

  /**
   ** update all texts
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::Information::Genscher::update_texts()
    {
      this->ui->translations->change(*this->label,
                                     ::translation("Genscher: %splayer% chooses %spartner%.",
                                                   this->genscher->name(),
                                                   this->partner->name()
                                                  ));

      return ;
    } // void GameInfoDialog::Information::Genscher::update_texts()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
