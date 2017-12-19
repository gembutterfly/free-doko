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

#ifdef USE_NETWORK

#include "constants.h"

#include "gameplay_actions.h"
#include "../sender.h"
#include "../../server.h"

#include "../../../game/game.h"
#include "../../../game/gameplay.h"
#include "../../../game/gameplay_action.h"

namespace Network {
  namespace FreeDoko {
    namespace Parser {

      /**
       ** constructor
       **
       ** @param     interpreter   corresponding interpreter
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      GameplayActions::GameplayActions(Connection::Interpreter& interpreter) :
	Lines(interpreter),
        actions()
      { }

      /**
       ** destructor
       **
       ** @param     -
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      GameplayActions::~GameplayActions()
       {
        for (vector<GameplayAction*>::iterator
             action = this->actions.begin();
             action != this->actions.end();
             action++)
          delete *action;
      } // GameplayActions::~GameplayActions()

      /**
       ** interprets the line and save the action
       **
       ** @param     line   line to interpret
       **
       ** @return    whether the line could be interpreted
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      bool
	GameplayActions::interpret_line(string const& line)
	{
          GameplayAction* action = GameplayAction::new_(line);
          if (action == NULL)
            return false;

          this->actions.push_back(action);
	  return true;
	} // bool GameplayActions::interpret_line(string line)

      /**
       ** finish the parsing
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
        GameplayActions::finish()
        {
          this->synchronize_game();
          this->Lines::finish();
          return ;
        } // void GameplayActions::finish()

      /**
        ** synchronize the game according to the actions
       **
       ** @param     -
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **
       ** @todo        all
       **/
        void
          GameplayActions::synchronize_game()
          {
            // the actions of the game
            vector<GameplayAction*> game_actions
              = this->actor().game().gameplay().actions();

            // iterator for the actions of the game
            vector<GameplayAction*>::const_iterator action_g
              = game_actions.begin();
            vector<GameplayAction*>::const_iterator action
              = this->actions.begin();

            while (   (action_g != game_actions.end())
                   && (action != this->actions.end()) ) {

              if (**action_g != **action) {
                // discrepancy between the actions
                // -- do not know how to handle this
                cerr << "Network::FreeDoko::Parser::GameplayActions::synchronize_game()\n"
                  << "Discrepancy between the gameplay actions.\n"
                  << "connection: " << this->interpreter->connection().type()
                  << "local game actions:\n";
                for (vector<GameplayAction*>::const_iterator
                     a = game_actions.begin();
                     a != game_actions.end();
                     ++a)
                  cerr << " " << ((a == action_g) ? '*' : ' ') << **a << '\n';
                cerr << "network game actions:\n";
                for (vector<GameplayAction*>::const_iterator
                     a = this->actions.begin();
                     a != this->actions.end();
                     ++a)
                  cerr << " " << ((a == action) ? '*' : ' ') << **a << '\n';
                cerr << "Ignoring synchronization\n";
                return ;
              }

              action_g++;
              action++;
            } // while (in lists)

            if (   (action_g == game_actions.end())
                && (action == this->actions.end()) ) {
              // the games are synchron
              cout << "games synchron" << endl;
              return ;
            }

            if (action != this->actions.end()) {
              // local game lags behind -- sending actions
              cerr << "local game "
                << "(connection: " << this->interpreter->connection().type() << ") "
                << "lags behind -- sending actions...\n";

              for (; action != this->actions.end(); ++action) {
                cerr << "  " << **action << '\n';
                this->actor().gameplay_action(**action);
              } // for (action \in this->actions)

              return ;
            } // if (action_g == game_actions.end())

            if (action_g != game_actions.end()) {
              cerr << "network game "
                << "(connection: " << this->interpreter->connection().type() << ") "
                << "is not synchron -- sending own game.\n";
              this->interpreter->sender().synchronize_game();
              return ;
            } // if (action_g != game_actions.end())

            return ;
          } // void GameplayActsion::synchronize_game()

    } // namespace Parser
  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
