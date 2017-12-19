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

#ifdef USE_UI_TEXT

#ifndef UI_TEXT_HEADER
#define UI_TEXT_HEADER

#include "../ui.h"

namespace UI_TEXT_NS {

  /**
   ** @brief	the UI, a simple text interface
   **
   ** @author	Diether Knof
   **
   ** @todo	(nearly) all
   **/
  class UI_Text : public UI {
    public:
      class General;
      class Help;
      class BugReport;
      class Settings;
      class Party;

      friend class General;
      friend class Help;
      friend class BugReport;
      friend class Settings;
      friend class Party;
    public:
      UI_Text();
      virtual ~UI_Text();

      // the output stream
      virtual ostream& ostr();
      // the input stream
      virtual istream& istr();

      // initialize the ui
      virtual void init(int& argc, char**& argv);

      // sleeps 'sleep_usec' thousands of seconds
      // (< 0 for infinity)
      virtual void sleep(unsigned const sleep_usec);
      // updates the language
      virtual void language_update();

      // the parts of a party
      virtual void party_open();
      virtual void party_get_settings();
      virtual void party_start();
      virtual void party_finish();
      virtual void party_close();

      // the parts of a game
      virtual void game_open();
      Reservation reservation_get(Player const& player);
      virtual void game_start();
      virtual void game_finish();
      virtual void game_close();

      // the parts of a trick
      virtual void trick_open();
      virtual void trick_full(Trick const& trick);
      virtual void trick_move_in_pile(Trick const& trick);
      virtual void trick_close();
      // get a card
      HandCard card_get(Player const& player);
      // the card is played
      virtual void card_played(Card const& card, Player const& player);
      // an announcement is made
      virtual void announcement_made(Announcement const& announcement,
                                     Player const& player);
      // the player has swines
      virtual void swines_announced(Player const& player);
      // the player has hyperswines
      virtual void hyperswines_announced(Player const& player);

      // 'player' shifts 'cardno' cards
      virtual void poverty_shift(Player const& player, unsigned const cardno);
      // ask 'player' whether to accept the poverty
      virtual void poverty_ask(Player const& player, unsigned const cardno);
      // the player 'player' has denied the poverty trumps
      virtual void poverty_take_denied(Player const& player);
      // all players have denied to take the cards
      virtual void poverty_take_denied_by_all();
      // the player 'player' has accepted the poverty trumps
      // and has returned 'cardno' cards with 'trumpno' trumps
      virtual void poverty_take_accepted(Player const& player,
                                         unsigned const cardno,
                                         unsigned const trumpno);
      // 'player' shifts cards
      virtual HandCards poverty_shift(Player& player);
      // returns whether 'player' accepts the poverty
      virtual bool poverty_take_accept(Player& player,
                                       unsigned const cardno);
      // the player changes the cards of the poverty
      virtual HandCards poverty_cards_change(Player& player,
                                             HandCards const& cards);
      // the poverty player 'player' gets 'cards'
      virtual void poverty_cards_get_back(Player& player,
                                          Hand const& cards);

      // return a partner for a genscher
      virtual Player const* genscher_partner();
      // a player has announced a genscher
      virtual void genscher(Player const& genscher, Player const& partner);

      // redrawing
      virtual void redraw_all();
      virtual void gametype_update();
      virtual void trick_update();
      virtual void name_update(Player const& player);
      virtual void hand_update(Player const& player);
      virtual void trick_pile_update(Player const& player);
      virtual void teaminfo_update(Player const& player);
      virtual void announcement_update(Player const& player);


      // change of the rules
      virtual void rule_update(int const type, void const* const old_value);
      // changes of the settings
      virtual void setting_update(int const type, void const* const old_value);

      // load the help page
      virtual void help_load(string const& page);

    private:
      // reads a line from the input stream
      virtual void getline(string const& default_command = "");
      // interprets the line (non special commands)
      virtual bool interpret_line();
      // test, whether the read line is the keyword
      virtual bool iskeyword(string const& keyword) const;
      // test, whether 'text' is the keyword
      virtual bool iskeyword(string const& text, string const& keyword) const;
      // test, whether the first word is the keyword
      virtual bool first_iskeyword(string const& keyword) const;
      // test, whether the first word of 'text' is the keyword
      virtual bool first_iskeyword(string const& text,
                                   string const& keyword) const;

    private:
      // input and output stream
      istream* istr_;
      ostream* ostr_;
    protected:
      // the prompt
      string prompt;
      // read line
      string line;
      // name part of the line
      string line_name; 
      // value part of the line
      string line_value; 

    private:
      General* general;
      Help* help;
      BugReport* bug_report;
      Settings* settings;
      Party* party;

    private: // unused
      UI_Text(UI_Text const& ui);
      UI_Text& operator=(UI_Text const& ui);
  }; // class UI_Text : public UI

} // namespace UI_TEXT_NS
using UI_TEXT_NS::UI_Text;

#endif // #ifdef UI_TEXT_HEADER

#endif // #ifdef USE_UI_TEXT
