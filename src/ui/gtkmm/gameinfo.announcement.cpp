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

#include "gameinfo.announcement.h"

#include "ui.h"
#include "translations.h"
#include "icons.h"

#include "../../player/player.h"
#include "../../game/game.h"
#include "../../party/rule.h"

#include "../../utils/string.h"

#include <gtkmm/image.h>
#include <gtkmm/label.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent         the parent object
   ** @param     player         the player
   ** @param     announcement   the announcement made
   **
   ** @return    -
   **/
  GameInfoDialog::Information::Announcement::Announcement(Base* const parent,
                                                          Player const& player,
                                                          ANNOUNCEMENT::Announcement const
                                                          announcement) :
    GameInfoDialog::Information(parent, ::translation("announcement")),
    player(&player),
    announcement(announcement)
  {
    
    if (   !this->ui->game().rule()(Rule::KNOCKING)
        && (   (announcement == ANNOUNCEMENT::NO120)
            || (announcement == ANNOUNCEMENT::REPLY) ) ) {
      this->ui->icons->change_managed(this->icon, player.team());
      this->title = ::translation(player.team());
    } else {
      this->ui->icons->change_managed(this->icon, player.announcement());
      this->title = ::translation(announcement);
    }

    this->init();

    return ;
  } // GameInfoDialog::Information::Announcement::Announcement(Base parent, Game game)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  GameInfoDialog::Information::Announcement::~Announcement()
  { }

  /**
   ** update all texts
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::Information::Announcement::update_texts()
    {
      if (   !this->ui->game().rule()(Rule::KNOCKING)
          && (   (this->announcement == ANNOUNCEMENT::NO120)
              || (this->announcement == ANNOUNCEMENT::REPLY) ) ) {
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% announces '%tannouncement%'.",
                                                     this->player->name(),
                                                     ::translation(this->player->team())
                                                    ));

      } else {
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% announces '%tannouncement%'.",
                                                     this->player->name(),
                                                     ::translation(this->announcement)
                                                    ));
      }

      return ;
    } // void GameInfoDialog::Information::Announcement::update_texts()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
