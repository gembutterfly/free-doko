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

#include "gameinfo.gametype.h"

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
   ** @param     parent   the parent object
   ** @param     game     the corresponding game
   **
   ** @return    -
   **/
  GameInfoDialog::Information::GameType::GameType(Base* const parent,
                                                  Game const& game) :
    GameInfoDialog::Information(parent, ::translation(game.type())),
    game(&game)
  {
    this->ui->icons->change_managed(this->icon, this->game->type());

    this->init();

    return ;
  } // GameInfoDialog::Information::GameType::GameType(Base parent, Game game)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  GameInfoDialog::Information::GameType::~GameType()
  { }

  /**
   ** update all texts
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::Information::GameType::update_texts()
    {
      if (!(::game_status & GAMESTATUS::GAME))
        return ;

      if (   (::game_status != GAMESTATUS::GAME_RESERVATION)
          && (::game_status != GAMESTATUS::GAME_REDISTRIBUTE)
          && (::game_status != GAMESTATUS::GAME_START))
        if (this->ui->game().type() != GAMETYPE::NORMAL)
          return ;

      auto const& game = *this->game;

      switch (game.type()) {
      case GAMETYPE::NORMAL:
        this->ui->translations->change(*this->label,
                                       ::translation(game.type()));
        break;
      case GAMETYPE::THROWN_NINES:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has %tnumber% nines.",
                                                     game.soloplayer().name(),
                                                     ::translation(DK::Utils::String::to_string(game.soloplayer().hand().numberofnines())))
                                      );
        break;
      case GAMETYPE::THROWN_KINGS:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has %tnumber% kings.",
                                                     game.soloplayer().name(),
                                                     ::translation(DK::Utils::String::to_string(game.soloplayer().hand().numberofkings())))
                                      );
        break;
      case GAMETYPE::THROWN_NINES_AND_KINGS:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has %tnumber% nines and kings.",
                                                     game.soloplayer().name(),
                                                     ::translation(DK::Utils::String::to_string(game.soloplayer().hand().numberofnines() + game.soloplayer().hand().numberofkings())))
                                      );
        break;
      case GAMETYPE::THROWN_RICHNESS:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has %upoints% richness.",
                                                     game.soloplayer().name(),
                                                     game.soloplayer().hand().points())
                                      );
        break;
      case GAMETYPE::FOX_HIGHEST_TRUMP:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has %tgametype%.",
                                                     game.soloplayer().name(),
                                                     ::translation(game.type()))
                                      );
        break;
      case GAMETYPE::POVERTY: {
        // error
        ::Translator::Translation
          text = ::translation("%splayer% has a %tgametype%.",
                               game.soloplayer().name(),
                               ::translation(game.type()));
        if (::game_status != GAMESTATUS::GAME_REDISTRIBUTE) {
          for (vector<Player*>::const_iterator player
               = game.players_begin();
               player != game.players_end();
               player++) {
            if (   (**player != game.soloplayer())
                && ((*player)->team() == TEAM::RE)) {
#ifdef POSTPONED
              text +=  ("        \n        "
                        + ::translation("Partner is %splayer%.",
                                        (*player)->name()));
#endif
              if (game.soloplayer().hand().numberoftrumps() == 1)
                text +=  ("        \n        "
                          + ::translation("%splayer% has returned %utrumpno% trump.",
                                          (*player)->name(),
                                          game.soloplayer().hand().numberoftrumps()
                                         ).translation());
              else
                text +=  ("        \n        "
                          + ::translation("%splayer% has returned %utrumpno% trumps.",
                                          (*player)->name(),
                                          game.soloplayer().hand().numberoftrumps()
                                         ).translation());

            } // if (player is the partner)
          } // for (player)
        } // if (::game_status != GAME_REDISTRIBUTE)
        this->ui->translations->change(*this->label, text);
      } break;
      case GAMETYPE::GENSCHER:
        // cannot happen
        break;
      case GAMETYPE::MARRIAGE:
      case GAMETYPE::MARRIAGE_SOLO:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% has a %tgametype%: %tselector%.",
                                                     game.soloplayer().name(),
                                                     ::translation(game.type()),
                                                     ::translation(game.marriage_selector()))
                                      );
        break;
      case GAMETYPE::MARRIAGE_SILENT:
        // cannot happen
        break;
      case GAMETYPE::SOLO_MEATLESS:
      case GAMETYPE::SOLO_JACK:
      case GAMETYPE::SOLO_QUEEN:
      case GAMETYPE::SOLO_KING:
      case GAMETYPE::SOLO_QUEEN_JACK:
      case GAMETYPE::SOLO_KING_JACK:
      case GAMETYPE::SOLO_KING_QUEEN:
      case GAMETYPE::SOLO_KOEHLER:
      case GAMETYPE::SOLO_CLUB:
      case GAMETYPE::SOLO_SPADE:
      case GAMETYPE::SOLO_HEART:
      case GAMETYPE::SOLO_DIAMOND:
        this->ui->translations->change(*this->label,
                                       ::translation("%splayer% plays a %tgametype%.",
                                                     game.soloplayer().name(),
                                                     ::translation(game.type()))
                                      );
        break;
      } // switch (game.type())

      return ;
    } // void GameInfoDialog::Information::GameType::update_texts()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether the information is blocking the gameplay
   **/
  bool
    GameInfoDialog::Information::GameType::is_blocking() const
    {
      return (this->Information::is_blocking() 
              || (::game_status == GAMESTATUS::GAME_REDISTRIBUTE));
    } // virtual bool GameInfoDialog::Information::GameType::is_blocking() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
