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

#include "human.h"
#ifdef POSTPHONED
#include "humanDb.h"
#endif

#include "../../party/rule.h"
#include "../../misc/setting.h"
#include "../../ui/ui.h"
#include "../../os/bug_report_replay.h"
#include "../../game/gameplay_actions.h"

/**
 ** Construktor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Human::Human() :
  Ai()
{
  // *** HACK
  this->set_type(HUMAN);
  this->set_name("");
#ifdef POSTPHONED
  this->db_ = new HumanDb();
#endif

  return ;
} // Human::Human()

/**
 ** construktor
 **
 ** @param     istr   stream with the infos
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Human::Human(istream& istr) :
  Ai(istr)
{
  this->set_type(HUMAN);

  return ;
} // Human::Human(istream& istr)

/**
 ** copy-constructor
 **
 ** @param     player	player to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Human::Human(Player const& player) :
  Ai(player)
{
  this->set_type(HUMAN);

  return ;
} // Human::Human(Player const& player)

/**
 ** copy-constructor
 **
 ** @param     player		player to copy
 ** @param     aiconfig	configuration to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Human::Human(Player const& player, Aiconfig const& aiconfig) :
  Ai(player, aiconfig)
{
  this->set_type(HUMAN);

  return ;
} // Human::Human(Player const& player, Aiconfig const& aiconfig)

/**
 ** copy-constructor
 **
 ** @param     ai	ai to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Human::Human(Ai const& ai) :
  Ai(static_cast<Ai const&>(ai))
{
  this->set_type(HUMAN);

  return ;
} // Human::Human(Ai const& ai)

/**
 ** copy-constructor
 **
 ** @param     human	human to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Human::Human(Human const& human) :
  Ai(static_cast<Ai const&>(human))
{
  this->set_type(HUMAN);

  return ;
} // Human::Human(Human const& human)

/**
 ** clone the player
 **
 ** @param       -
 **
 ** @return      pointer of a clone
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.3
 **/
Human*
Human::clone() const
{  
  return new Human(*this);
} // virtual Human* Human::clone() const

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @param     -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Human::~Human()
{ }

/**
 ** returns the reservation of the player
 ** (taken from the ui)
 **
 ** @param     is_duty		whether the player has to play a duty solo
 **				(default: false)
 **
 ** @return    reservation of the player
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Reservation const&
Human::reservation_get(bool const is_duty)
{ 
  DEBUG_ASSERTION(!this->game().isvirtual(),
		  "Human::reservation_get():\n"
		  "  Game is virtual");

  { // bug report replay
    if (   ::bug_report_replay
	&& ::bug_report_replay->auto_action()
	&& (::bug_report_replay->current_action().type
	    == GameplayAction::RESERVATION)
	&& (static_cast<GameplayAction::Reservation const&>(::bug_report_replay->current_action()).player
	    == this->game().no(*this))
       ) {
      this->reservation() = static_cast<GameplayAction::Reservation const&>(::bug_report_replay->current_action()).reservation;
      return this->reservation();
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER) {
    this->reservation() = this->Ai::reservation_get(is_duty);
    return this->reservation();
  }

  (void)is_duty;

  this->reservation() = ::ui->reservation_get(*this);

  return this->reservation();
} // Reservation const& Human::reservation_get(bool const is_duty = false)

/**
 ** the game is started
 **
 ** @param       -
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.3
 **/
void
Human::game_start() 
{ 
  this->Ai::game_start();
  if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY)) {
    if (this->game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN))
      if (this->game().swines_announcement_valid(*this))
	this->game().swines_announce(*this);

  } // if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY))

  return ;
} // void Human::game_start()

/**
 ** @param     -
 **
 ** @return    no announcement
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Announcement
Human::announcement_request() const
{
  { // bug report replay
    if (   ::bug_report_replay
	&& ::bug_report_replay->auto_action()
	&& (::bug_report_replay->current_action().type
	    == GameplayAction::ANNOUNCEMENT)
	&& (static_cast<GameplayAction::Announcement const&>(::bug_report_replay->current_action()).player
	    == this->game().no(*this))
       ) {
      return static_cast<GameplayAction::Announcement const&>(::bug_report_replay->current_action()).announcement;
    } // if (auto execute)
  } // bug report replay

  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement Human::announcement_request() const

/**
 ** returns the card played by the human
 **
 ** @param     -
 **
 ** @return    the card choosen by a human player
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
HandCard
Human::card_get()
{
  DEBUG_ASSERTION(!this->game().isvirtual(),
		  "Human::card_get():\n"
		  "  Game is virtual");

  { // bug report replay
    if (   ::bug_report_replay
	&& ::bug_report_replay->auto_action()
       ) {
      if ( (::bug_report_replay->current_action().type
	    == GameplayAction::SWINES)
	  && (static_cast<GameplayAction::Swines const&>(::bug_report_replay->current_action()).player 
	    == this->game().no(*this)))
	this->game().swines_announce(*this);

      if ( (::bug_report_replay->current_action().type
	    == GameplayAction::HYPERSWINES)
	  && (static_cast<GameplayAction::Hyperswines const&>(::bug_report_replay->current_action()).player
	    == this->game().no(*this)))
	this->game().hyperswines_announce(*this);

      if ( (::bug_report_replay->current_action().type
	    == GameplayAction::CARD_PLAYED)
	  && (static_cast<GameplayAction::CardPlayed const&>(::bug_report_replay->current_action()).player
              == this->game().no(*this))) {
        this->lastHeuristic_ = Aiconfig::BUG_REPORT;
        return HandCard(this->hand(), static_cast<GameplayAction::CardPlayed const&>(::bug_report_replay->current_action()).card);
      }
    } // if (auto execute card play)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER) {
    HandCard const card = this->Ai::card_suggestion();
    if (!card.is_empty()
        && card.isvalid(this->game().trick_current())) {
      return card;
      return this->Ai::card_get();
    }
  }

  this->lastHeuristic_ = Aiconfig::MANUAL;
  HandCard const card = ::ui->card_get(*this);

  if (::game_status != GAMESTATUS::GAME_PLAY)
    return HandCard(this->hand(), Card());

  DEBUG_ASSERTION(!card.is_empty(),
                  "Human::card_get():\n"
                  "  result of 'ui->card_get()' is an empty card");

  if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY)) {
    if (card.possible_swine()
        && !this->game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN)
        && this->game().swines_announcement_valid(*this)) {
      if (this->game().rule()(Rule::SWINE_ONLY_SECOND)) {
        if (this->hand().numberoftrumpaces() == 1)
          this->game().swines_announce(*this);
      } else {
        this->game().swines_announce(*this);
      }
    }
    if (card.possible_hyperswine()
        && !this->game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN)
        && this->game().hyperswines_announcement_valid(*this))
      this->game().hyperswines_announce(*this);
  } // if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY))

  return card;
} // HandCard Human::card_get()

/**
 ** updates the team information
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Human::teaminfo_update()
{
  this->Ai::teaminfo_update();
  if (::setting(Setting::SHOW_AI_INFORMATION_TEAMS)
      && !this->game().isvirtual())
    for (vector<Player*>::const_iterator p = this->game().players_begin();
         p != this->game().players_end();
         ++p)
      ::ui->teaminfo_changed(**p);

  return ;
} // void Human::teaminfo_update()

/**
 ** returns which cards the player shifts
 **
 ** @param     -
 **
 ** @return    the cards that are to be shifted
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
HandCards
Human::poverty_shift()
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()
        && (::bug_report_replay->current_action().type
            == GameplayAction::POVERTY_SHIFT)
        && (static_cast<GameplayAction::PovertyShift const&>(::bug_report_replay->current_action()).player
            == this->game().no(*this))
       ) {
      HandCards const cards(this->hand(),
                            static_cast<GameplayAction::PovertyShift const&>(::bug_report_replay->current_action()).cards);
      this->sorted_hand().remove(cards);
      return cards;
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER)
    return this->Ai::poverty_shift();

  return ::ui->poverty_shift(*this);
} // void Human::poverty_shift(Player const& player)

/**
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param     cardno	the number of shifted cards
 **
 ** @return    whether to accept the cards
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
bool
Human::poverty_take_accept(unsigned const cardno)
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()) {
      if (   (::bug_report_replay->current_action().type
              == GameplayAction::POVERTY_ACCEPTED)
          && (static_cast<GameplayAction::PovertyAccepted const&>(::bug_report_replay->current_action()).player
              == this->game().no(*this)) )
        return true;
      if (   (::bug_report_replay->current_action().type
              == GameplayAction::POVERTY_DENIED)
          && (static_cast<GameplayAction::PovertyDenied const&>(::bug_report_replay->current_action()).player
              == this->game().no(*this)) )
        return false;
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER)
    return this->Ai::poverty_take_accept(cardno);

  return ::ui->poverty_take_accept(*this, cardno);
} // void Human::poverty_take_accept(unsigned const cardno)

/**
 ** changes the cards from the poverty-player
 **
 ** @param     cards	the cards that are given to the player
 **
 ** @return    the cards that are returned to the poverty-player
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
HandCards
Human::poverty_cards_change(HandCards const& cards)
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()) {
      if (::bug_report_replay->current_action().type
          == GameplayAction::POVERTY_RETURNED) {
        this->sorted_hand().add(cards);
        HandCards const cards_returned(this->hand(),
                                       static_cast<GameplayAction::PovertyReturned const&>(::bug_report_replay->current_action()).cards);
        this->sorted_hand().remove(cards_returned);
#ifdef WORDAROUND
        // the ai has to update the cards
        this->set_hand(this->hand());
#endif
        return cards_returned;
      }
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER)
    return this->Ai::poverty_cards_change(cards);

#ifdef WORKAROUND
  // the ai has to update the cards
  HandCards const cards_returned
    = ::ui->poverty_cards_change(*this, cards);
  this->Ai::set_hand(this->hand());

  return cards_returned;
#else
  return ::ui->poverty_cards_change(*this, cards);
#endif
} // HandCards Human::poverty_cards_change(HandCards const& cards)

/**
 ** gets the cards from the partner and add them to the hand
 **
 ** @param     cards	the cards that are given to the player
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Human::poverty_cards_get_back(HandCards const& cards)
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()) {
      this->Ai::poverty_cards_get_back(cards);
      return ;
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER) {
    this->Ai::poverty_cards_get_back(cards);
    return ;
  }

  ::ui->poverty_cards_get_back(*this, cards);
#ifdef WORKAROUND
  // the ai has to update the cards
  this->Ai::set_hand(this->hand());
#endif

  return ;
} // void Human::poverty_cards_get_back(HandCards const& cards)

/**
 ** 'player' has announce hyperswines
 **
 ** @param      player	player that has announced the hyperswines
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **/
void
Human::swines_announced(Player const& player)
{ 
  this->Ai::swines_announced(player);

  if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY)) {
    if (this->game().hyperswines_announcement_valid(*this)
        && this->game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN))
      this->game().hyperswines_announce(*this);

  } // if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY))

  return ;
} // void Human::swines_announced(Player const& player)

/**
 ** genscher: -> result
 **
 ** @param     -
 **
 ** @return    pointer to the new teammate,
 **            NULL if the genscher is not to be announced
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Player const*
Human::genscher_partner()
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()
        && (::bug_report_replay->current_action().type
            == GameplayAction::GENSCHER)
        && (static_cast<GameplayAction::Genscher const&>(::bug_report_replay->current_action()).player
            == this->game().no(*this))
       ) {
      unsigned const partner_no
        = static_cast<GameplayAction::Genscher const&>(::bug_report_replay->current_action()).partner;
      if (partner_no == UINT_MAX)
        return NULL;
      else
        return &this->game().player(partner_no);
    } // if (auto execute)
  } // bug report replay

  if (FAST_PLAY & FAST_NS::PLAYER)
    return this->Ai::genscher_partner();

  return ::ui->genscher_partner();
} // Player const* Human::genscher_partner()
