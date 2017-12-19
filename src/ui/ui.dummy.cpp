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

#include "ui.dummy.h"

#ifndef POSTPONED
#ifndef RELEASE
#include <unistd.h>
#endif
#endif

#ifdef USE_UI_Dummy_TEXT
#include "text/ui.h"
#endif // #ifdef USE_UI_Dummy_TEXT
#ifdef USE_UI_Dummy_GTKMM
#include "gtkmm/ui.h"
#endif // #ifdef USE_UI_Dummy_GTKMM

#include "../party/party.h"
#include "../card/hand.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../os/bug_report.h"
#include "../os/bug_report_replay.h"

/**
 ** Constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
UI_Dummy::UI_Dummy() :
  UI(UI_TYPE::DUMMY)
{ return ; }

/**
 ** Destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
UI_Dummy::~UI_Dummy()
{ return ; }

/**
 ** the party is closed
 ** end the program
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
void
UI_Dummy::party_close()
{
  this->UI::party_close();

  return ;
} // void UI_Dummy::party_close()

/**
 ** gets the reservations of the player 'player'
 ** Here is the first time, the hands can be shown (and should)
 **
 ** @param	player	the player that makes the reservation
 **
 ** @return	Reservation of the player 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Reservation
UI_Dummy::reservation_get(Player const& player)
{ return Reservation(); }

/**
 ** gets the card, the player wishes to play
 ** (don't remove it from the hand!)
 **
 ** @param	player	the player who shall play a card
 **
 ** @return	Card to be played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
HandCard
UI_Dummy::card_get(Player const& player)
{ return HandCard(); }

/**
 ** returns which cards the player shifts
 **
 ** @param	player	the player who shifts the cards
 **
 ** @return	the cards that are to be shifted
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
HandCards
UI_Dummy::poverty_shift(Player& player)
{ return HandCards(); }

/**
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param	player	the player who shifts the cards
 ** @param	cardno	the number of shifted cards
 **
 ** @return	whether to accept the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
bool
UI_Dummy::poverty_take_accept(Player& player, unsigned const cardno)
{ return false; }

/**
 ** changes the cards from the poverty-player
 **
 ** @param	player	the player who has accepted the cards
 ** @param	cards	the cards that are given to the player
 **
 ** @return	the cards that are returned to the poverty-player
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
HandCards
UI_Dummy::poverty_cards_change(Player& player, HandCards const& cards)
{ return HandCards(); }

/**
 ** genscher: -> result
 **
 ** @param	-
 **
 **
 ** @author	Diether Knof
 ** @return	pointer to the new teammate,
 **		NULL if the genscher is not to be announced
 ** @version	0.6.7
 **
 **/
Player const*
UI_Dummy::genscher_partner()
{ return NULL; } 

/**
 ** show the message
 **
 ** @param	message		message
 ** @param	type		type of the information
 ** @param	force_show	whether to force the showing of the massage (default: false)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
UI_Dummy::information(Translator::Translation const& message,
		     INFORMATION::Type const type,
                     bool const force_show)
{
  this->UI::information(message, type, force_show);
  cout << message << endl;

  return ;
}

/**
 ** show the message and quit the program
 **
 ** @param	message		error message
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
UI_Dummy::error(string const& message)
{
  cerr << message << endl;
  if (::bug_report_replay != NULL)
    cerr << "--\n"
      << ::translation("replayed bug report: %sbug report%", ::bug_report_replay->filename()) << '\n';

  { // create bug report
    string const filename = ::bug_report->report(message);

    cerr << ::translation("BugReport::automatic creation, %sfilename%, %scontact%", filename, CONTACT)
      << endl;
  } // create bug report

#ifdef ASSERTION_GENERATES_SEGFAULT
  {
    cerr << ::translation("Creating segmentation fault.") << endl;
    SEGFAULT;
  }
#endif // #ifdef ASSERTION_GENERATES_SEGFAULT

  ::exit(EXIT_FAILURE);

  return ;
} // void UI_Dummy::error(string const& message)
