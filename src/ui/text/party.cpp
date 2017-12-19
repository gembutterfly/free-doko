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

#ifdef USE_UI_TEXT

#include "party.h"

#include "../../party/party.h"
#include "../../player/ai/ai.h"
#include "../../misc/translations.h"
#include "../../utils/string.h"

namespace UI_TEXT_NS {

  /**
   ** constructor
   **
   ** @param	ui	pointer to the ui
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  UI_Text::Party::Party(UI_Text* const ui) :
    ui(ui)
  { }

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  UI_Text::Party::~Party()
  { }

  /**
   ** interprets the line read by the ui
   **
   ** @param	-
   **
   ** @return	whether the line was interpreted
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  bool
    UI_Text::Party::interpret_line()
    {
      // whether the line has been interpreted
      bool interpreted = false;

      if (this->ui->iskeyword("show")) {
        if (::game_status == GAMESTATUS::PARTY_NEW) {
          this->show();
          interpreted = true;
        } else if (::game_status == GAMESTATUS::PARTY_LOADED) {
          this->ui->ostr() << ::party;
          interpreted = true;
        } 

      } else if (this->ui->iskeyword("show party")) {
        this->ui->ostr() << ::party;
        interpreted = true;

      } else if (this->ui->iskeyword("show party points")) {
        // ToDo
        //::party.write_points(this->ui->ostr());
        interpreted = true;

      } else if (this->ui->iskeyword("load")) {
        // ToDo
      } else if (this->ui->iskeyword("save")) {
        // ToDo

      } else if (this->ui->iskeyword("seed")) {
        if (::party.seed() == UINT_MAX)
          this->ui->ostr() << ::translation("Seed: random") << '\n';
        else
          this->ui->ostr() << ::translation("Seed: %useed%", ::party.seed()) << '\n';

        interpreted = true;

      } else if (this->ui->line_name == "seed") {
        if (this->ui->line_value == "random") {
          ::party.set_random_seed();
          interpreted = true;
        } else {
            unsigned s = UINT_MAX;
            DK::Utils::String::from_string(this->ui->line_value, s);
            if (   (s != UINT_MAX)
                && (this->ui->line_value == DK::Utils::String::to_string(s))
               ) {
              ::party.set_seed(s);
              interpreted = true;
            }
        }
        if (!interpreted) {
          this->ui->ostr() << "seed '" << this->ui->line_value << "' is not valid" << '\n';
          interpreted = true;
        }

      } else if (this->ui->iskeyword("startplayer")) {
        if (::party.startplayer() == UINT_MAX)
          this->ui->ostr() << ::translation("Startplayer: random") << '\n';
        else
          this->ui->ostr() << ::translation("Startplayer: %sname%", ::party.player(::party.startplayer()).name()) << '\n';
        interpreted = true;

      } else if (this->ui->line_name == "startplayer") {
        if (this->ui->line_value == "random") {
          ::party.set_random_startplayer();
          interpreted = true;
        } else {
          { // check for a number
            unsigned p = UINT_MAX;
            DK::Utils::String::from_string(this->ui->line_value, p);
            if (   (p != UINT_MAX)
                && (this->ui->line_value == DK::Utils::String::to_string(p))
                && (p > 0)
                && (p <= ::party.playerno())) {
              ::party.set_startplayer(p - 1);
              interpreted = true;
            }
          }
          if (!interpreted) {
            for (unsigned p = 0; p < ::party.playerno(); ++p) {
              if (::party.player(p).name() == this->ui->line_value) {
                ::party.set_startplayer(p);
                interpreted = true;
                break;
              }
            } // for (p < ::party.playerno())
          } // if (!interpreted)

        }
        if (!interpreted) {
          this->ui->ostr() << "startplayer '" << this->ui->line_value << "' is not valid" << '\n';
          interpreted = true;
        }
        // ToDo: set the startplayer
      } // if (this->ui->iskeyword(...)

      return interpreted;
    } // bool UI_Text::Party::interpret_line()

  /**
   ** show the party settings
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  void
    UI_Text::Party::show() const
    {
      for (unsigned p = 0; p < ::party.playerno(); ++p) {
        Player const& player = :: party.player(p);
        this->ui->ostr() << ::translation("Player %unumber%", p+1)
          << ": " << player.name() << " ("
          << ::translation(player.type());
        switch (player.type()) {
        case Player::UNSET:
          break;
        case Player::HUMAN:
        case Player::AI:
          this->ui->ostr() << ", " << static_cast<Ai const&>(player).difficulty();
        case Player::AI_DUMMY:
        case Player::AI_RANDOM:
        case Player::NETWORK:
          break;
        }
        this->ui->ostr() << ")\n";
      } // for (p < ::party.playerno())

      if (::party.seed() == UINT_MAX)
        this->ui->ostr() << ::translation("Seed: random") << '\n';
      else
        this->ui->ostr() << ::translation("Seed: %useed%", ::party.seed()) << '\n';
      if (::party.startplayer() == UINT_MAX)
        this->ui->ostr() << ::translation("Startplayer: random") << '\n';
      else
        this->ui->ostr() << ::translation("Startplayer: %sname%", ::party.player(::party.startplayer()).name()) << '\n';

      return ;
    } // void UI_Text::Party::show() const

  /**
   ** get the party settings
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  void
    UI_Text::Party::get_settings() const
    {
      this->ui->ostr() << ::translation("Set settings for the party.") << '\n';
      this->show();
      while (::game_status == GAMESTATUS::PARTY_NEW) {
        this->ui->getline("start party");
        if (   (this->ui->iskeyword("start"))
            || (this->ui->iskeyword("start party")) )
          break;
        else if (this->ui->iskeyword("show"))
          this->show();
        else
          this->ui->interpret_line();
      }

      return ;
    } // void UI_Text::Party::get_settings() const

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
