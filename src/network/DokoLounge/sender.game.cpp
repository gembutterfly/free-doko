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
 ********************************************************************/

#include "constants.h"

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#include "sender.h"

#include "../server.h"
#include "parser/parser.h"
#include "../../game/game.h"
#include "../../game/gameplay.h"
#include "../../game/gameplay_actions.h"
#include "../../player/player.h"

#include "../../utils/string.h"

namespace Network {
  namespace DokoLounge {

   /**
    ** a gameplay action
    **
    ** @param     action    gameplay action
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::gameplay_action(GameplayAction const& action)
     {
       this->send("gameplay action: " + action.str());

       if (action.player == UINT_MAX)
         return ;
       if (this->game().player(action.player).type() == Player::NETWORK)
         return;

       Player const& player = this->game().player(action.player);
       switch (action.type) {
       case GameplayAction::RESERVATION: {
         Reservation const& reservation
           = dynamic_cast<GameplayAction::Reservation const&>(action).reservation;
         string text;
         switch (reservation.game_type) {
         case GAMETYPE::NORMAL:
           text = "Gesund";
           break;

         case GAMETYPE::THROWN_NINES:
         case GAMETYPE::THROWN_KINGS:
           this->send_name_command("schmeissen");
           text = "Gesund";
           break;

         case GAMETYPE::THROWN_NINES_AND_KINGS:
         case GAMETYPE::THROWN_RICHNESS:
         case GAMETYPE::FOX_HIGHEST_TRUMP:
           // Nicht in den Regeln
           break;

         case GAMETYPE::POVERTY:
           text = "Armut";
           break;

         case GAMETYPE::GENSCHER:
           // Nicht in den Regeln
           break;

         case GAMETYPE::MARRIAGE:
           text = "Hochzeit";
           break;

         case GAMETYPE::MARRIAGE_SOLO:
         case GAMETYPE::MARRIAGE_SILENT:
           break;

         case GAMETYPE::SOLO_MEATLESS:
           text = "Fleischlos-Solo";
           break;

         case GAMETYPE::SOLO_JACK:
           text = "Buben-Solo";
           break;

         case GAMETYPE::SOLO_QUEEN:
           text = "Buben-Solo";
           break;

         case GAMETYPE::SOLO_KING:
         case GAMETYPE::SOLO_QUEEN_JACK:
         case GAMETYPE::SOLO_KING_JACK:
         case GAMETYPE::SOLO_KING_QUEEN:
         case GAMETYPE::SOLO_KOEHLER:
           // Nicht in den Regeln
           break;

         case GAMETYPE::SOLO_CLUB:
           text = "Kreuz-Solo";
           break;

         case GAMETYPE::SOLO_HEART:
           text = "Herz-Solo";
           break;

         case GAMETYPE::SOLO_SPADE:
           text = "Pik-Solo";
           break;

         case GAMETYPE::SOLO_DIAMOND:
           text = "Karo-Solo";
           break;
         } // switch (reservation.game_type)
         this->send_name_command("Melden", text);
         break;
       }

       case GameplayAction::POVERTY_SHIFT:
         // ToDo
         // ka = "<<armutskarten>><<armutsname>>" + MeinName + "<</armutsname>><<armutsk>>" + aKartenStr + "<</armutsk>><</armutskarten>>"
         break;

       case GameplayAction::POVERTY_ACCEPTED:
         this->send_name_command("armutmitnehmen", "Mitnehmen");
         break;

       case GameplayAction::POVERTY_RETURNED:
         // ToDo
         // ka = "<<mitnehmerkartenback>><<mitnehmername>>" + MeinName + "<</mitnehmername>><<mitnehmerk>>" + aKartenStr + "<</mitnehmerk>><</mitnehmerkartenback>>"
         break;

       case GameplayAction::POVERTY_DENIED:
         this->send_name_command("armutmitnehmen", "Ablehnen");
         break;

       case GameplayAction::POVERTY_DENIED_BY_ALL:
         // Nichts zu tun
         break;

       case GameplayAction::CARD_PLAYED: {
         Card const& card
           = dynamic_cast<GameplayAction::CardPlayed const&>(action).card;
         // Beide Kartennummern ausprobieren, eine wird schon passen
         this->send_command("KartenLegen",
                            "(" + DK::Utils::String::to_string(::lounge->table_of_player(::lounge->account()) + 1) + ")"
                            + ::lounge->account() + "||"
                            + Parser::number1(card)
                            );
         this->send_command("KartenLegen",
                            "(" + DK::Utils::String::to_string(::lounge->table_of_player(::lounge->account()) + 1) + ")"
                            + ::lounge->account() + "||"
                            + Parser::number2(card)
                            );
         //<<KartenLegen>>(" + Format(MeinTisch) + ")" + MeinName + "||" + Format(n) + ">|<</KartenLegen>>"
         break;
       }

       case GameplayAction::ANNOUNCEMENT: {
         string text; // Text für die Ansage
         if (player.team() == TEAM::RE)
           text = "RE";
         else
           text = "KONTRA";
         switch (dynamic_cast<GameplayAction::Announcement const&>(action).announcement) {
         case ANNOUNCEMENT::NOANNOUNCEMENT:
           // Entfällt
           break;
         case ANNOUNCEMENT::NO120:
           // Nichts zu tun
           break;
         case ANNOUNCEMENT::NO90:
           text += " keine 90";
           break;
         case ANNOUNCEMENT::NO60:
           text += " keine 60";
           break;
         case ANNOUNCEMENT::NO30:
           text += " keine 30";
           break;
         case ANNOUNCEMENT::NO0:
           text += " SCHWARZ";
           break;
         case ANNOUNCEMENT::REPLY:
         case ANNOUNCEMENT::NO120_REPLY:
         case ANNOUNCEMENT::NO90_REPLY:
         case ANNOUNCEMENT::NO60_REPLY:
         case ANNOUNCEMENT::NO30_REPLY:
         case ANNOUNCEMENT::NO0_REPLY:
           // not allowed
           break;

         } // switch (dynamic_cast<GameplayAction::Announcement const&>(action).announcement)
         this->send_command("Ansage",
                            this->name_command("Spieler")
                            + this->command("Ans", text));
         //mess = "<<Ansage>><<Spieler>>" + MeinName + "<</Spieler>><<Ans>>" + s + "<</Ans>><</Ansage>>"
         break;
       }

       case GameplayAction::SWINES:
       case GameplayAction::HYPERSWINES:
         // Wird vom Server automatisch angesagt
         break;

       case GameplayAction::MARRIAGE:
         // Nichts zu tun
         break;

       case GameplayAction::GENSCHER:
         // Nicht in den Regeln
         break;

       case GameplayAction::TRICK_OPEN:
       case GameplayAction::TRICK_FULL:
         // Nichts zu tun
         break;

       case GameplayAction::TRICK_TAKEN:
         // Stich einsammeln
         if (this->game().trick_current().winnerplayer().type() != Player::NETWORK)
           this->send_name_command("MeinStich");

       case GameplayAction::CHECK:
         // Nichts zu tun
         break;
       } // switch (action.type)

       return ;
     } // void Interpreter::Sender::gameplay_action(GameplayAction action)

#if 0
   /**
    ** the game is opened
    **
    ** @param     game   game that is opened
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::game_open(Game const& game)
     {
       this->send("game: open");
       this->send("game: startplayer: "
                  + DK::Utils::String::to_string(game.startplayer().no())
                 );
       this->send("game: seed: "
                  + DK::Utils::String::to_string(game.seed())
                 );

       return ;
     } // void Interpreter::Sender::game_open(Game const& game)

   /**
    ** the cards are distributed
    **
    ** @param     hands   distributed hands
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::game_distribute_cards(vector<Hand const*> hands)
     {
       for (unsigned i = 0; i < hands.size(); ++i) {
         ostringstream ostr;
         ostr << "game: hand " << i << ": ";
         HandCards const& cards = hands[i]->cards();
         auto c = cards.begin();
         if (c != cards.end()) {
           ostr << *c;
           for (; c != cards.end(); ++c)
             ostr << ", " << *c;
         }
         this->send(ostr.str());
       }

       return ;
     } // void Interpreter::Sender::game_distribute_cards(vector<Hand const*> hands)

   /**
    ** the game is started
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::game_start()
     {
       this->send("game: start");
       return ;
     } // void Interpreter::Sender::game_start()

   /**
    ** the game is finished
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::game_finish()
     {
       this->send("game: finish");
       return ;
     } // void Interpreter::Sender::game_finish()

   /**
    ** synchronize the game
    ** send the gameplay actions
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.5
    **/
   void
     Interpreter::Sender::synchronize_game()
     {
       ostringstream ostr;
       ostr << "sync: gameplay actions\n"
         << "{\n";
       for (auto a : this->connection().server().game().gameplay().actions())
         ostr << **a << '\n';
       ostr << "}\n";

       this->send_lines(ostr.str());
       return ;
     } // void Interpreter::Sender::synchronize_game()
#endif

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
