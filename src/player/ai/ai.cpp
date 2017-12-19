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

#include "ai.h"
#include "team_information.h"
#include "cards_information.h"

#include "heuristics.h"
#include "weighting.h"
#include "aiDb.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"
#include "../../game/gameplay_actions.h"
#ifndef OUTDATED
// for 'nextcard()' errors
#include "../../misc/bug_report.h"
#include "../../os/bug_report.h"
#endif
#include "../../os/bug_report_replay.h"
#ifndef WORKAROUND
// for replaying old bug reports
#include "../../utils/version.h"
#endif
#ifdef CHECK_RUNTIME
#include "../../runtime.h"
#endif

//#define DEBUG_PLAYER ((::party.game().trick_current_no() == 8) ? 2 : 22)
#define DEBUG_PLAYER 33

// whether to debug the game decision
#ifndef RELEASE
#define DEBUG_RESERVATION_PLAYER 9
#endif
#ifdef DEBUG_RESERVATION_PLAYER
#define DEBUG_RESERVATION(solo, value, max) \
  if (this->no() == DEBUG_RESERVATION_PLAYER) { \
    COUT << solo << ": " << value << (value == max ? " * " : "") << '\n'; \
  } else (void)0
#else
#define DEBUG_RESERVATION(solo, value, max) (void)0
#endif

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
 **
 ** @todo      using constructor Aiconfig(istr)
 ** @todo      update to the new format
 **/
Ai::Ai(istream& istr) :
  Player(AI),
  Aiconfig(),
  team_information_(NULL),
  cards_information_(NULL),
  teaminfo_p(),
  trick_p(::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)),
  trickno_p(0),
  colorjabbed_p(::party.rule()(Rule::NUMBER_OF_CARD_COLORS),
                vector<bool>(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                             false)),
  low_p(true),
  meatlesscolor_p(Card::NOCARDCOLOR),
  trumpless_(),
  last_trick_to_calculate_p(12),
  lastHeuristic_(Aiconfig::NO_HEURISTIC),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_(false)
{
  delete this->db_;
  this->db_ = new AiDb;

  while (istr.good()) {
    string line;
    getline(istr, line);
    if (istr.fail() || istr.eof())
      break;

    if (*line.rbegin() == '\r')
      line.erase(line.end() - 1);

    if ((line == "")
        || (line[0] == '#'))
      continue;

#ifndef OUTDATED
    // 2006-09-18   0.7.4
    if (   (line == "Aiconfig")
        || (line == "aiconfig") )
#else
      if (line == "aiconfig")
#endif
      {
        // this is the last entry
        this->Aiconfig::read(istr);
        break;
      } else if (line == "Database") {
        this->db_->read(istr);
      } else {
        cerr << "Reading the ai:\n"
          << "found following unknown line:\n"
          << line << '\n'
          << "ignoring it."
          << endl;
#ifndef RELEASE
        exit(EXIT_FAILURE);
#endif
      }
  }; // while (istr.good())

  return ;
} // Ai::Ai(istream& istr)

/**
 ** standard constructor
 **
 ** @param     -
 **
 ** @result    -
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Ai::Ai() :
  Player(AI),
  Aiconfig(),
  team_information_(NULL),
  cards_information_(NULL),
  teaminfo_p(),
  trick_p(::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)),
  trickno_p(0),
  colorjabbed_p(::party.rule()(Rule::NUMBER_OF_CARD_COLORS),
                vector<bool>(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                             false)),
  low_p(true),
  meatlesscolor_p(Card::NOCARDCOLOR),
  trumpless_(),
  last_trick_to_calculate_p(12),
  lastHeuristic_(Aiconfig::NO_HEURISTIC),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_(false)
{
  delete this->db_;
  this->db_ = new AiDb;

  return ;
} // Ai::Ai()

/**
 ** constructor
 **
 ** @param     aiconfig   ai configuration
 **
 ** @result    -
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Ai::Ai(Aiconfig const& aiconfig) :
  Player(AI),
  Aiconfig(aiconfig),
  team_information_(NULL),
  cards_information_(NULL),
  teaminfo_p(),
  trick_p(::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)),
  trickno_p(0),
  colorjabbed_p(::party.rule()(Rule::NUMBER_OF_CARD_COLORS),
                vector<bool>(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                             false)),
  low_p(true),
  meatlesscolor_p(Card::NOCARDCOLOR),
  trumpless_(),
  last_trick_to_calculate_p(12),
  lastHeuristic_(Aiconfig::NO_HEURISTIC),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_(false)
{
  delete this->db_;
  this->db_ = new AiDb;

  return ;
} // Ai::Ai(Aiconfig aiconfig)

/**
 **
 ** copy constructor
 **
 ** @param	player	player to copy
 **
 ** @return	-
 **
 ** @version	0.6.0
 **
 ** @author	Diether Knof
 **      
 **/
Ai::Ai(Player const& player) :
  Player(player),
  Aiconfig(),
  team_information_(NULL),
  cards_information_(NULL),
  teaminfo_p(),
  trick_p(::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)),
  trickno_p(0),
  colorjabbed_p(::party.rule()(Rule::NUMBER_OF_CARD_COLORS),
                vector<bool>(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                             false)
               ) ,
  low_p(true),
  meatlesscolor_p(Card::NOCARDCOLOR),
  trumpless_(),
  last_trick_to_calculate_p(12),
  lastHeuristic_(Aiconfig::NO_HEURISTIC),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_(false)
{
  this->set_type(AI);

  delete this->db_;
  this->db_ = new AiDb(player.db());

  return ;
} // Ai::Ai(Player const& player)

/**
 ** copy constructor
 **
 ** @param	player		player to copy
 ** @param	aiconfig	configuration to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Ai::Ai(Player const& player, Aiconfig const& aiconfig) :
  Player(player),
  Aiconfig(aiconfig),
  team_information_(NULL),
  cards_information_(NULL),
  teaminfo_p(),
  trick_p(::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)),
  trickno_p(0),
  colorjabbed_p(::party.rule()(Rule::NUMBER_OF_CARD_COLORS),
                vector<bool>(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                             false)
               ) ,
  low_p(true),
  meatlesscolor_p(Card::NOCARDCOLOR),
  trumpless_(),
  last_trick_to_calculate_p(12),
  lastHeuristic_(Aiconfig::NO_HEURISTIC),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_(false)
{
  this->set_type(AI);

  if (::game_status & GAMESTATUS::GAME)
    DEBUG_ASSERTION(false,
                    "Ai::Ai(player):\n"
                    "  in game");

  delete this->db_;
  this->db_ = new AiDb(player.db());

  return ;
} // Ai::Ai(Player const& player, Aiconfig const& aiconfig)

/**********************************************************************
 *
 ** Ai::Ai(const Ai& ai)
 *
 ** Parameters:  ai - Ai to copy
 *
 ** Result:      -
 *
 ** Version:     0.4.2
 *
 ** Description: constructor
 **      
 *
 **********************************************************************/
Ai::Ai(const Ai& ai) :
  Player(ai),
  Aiconfig(ai),
team_information_(ai.team_information_
                  ? new TeamInformation(ai.team_information())
                  : NULL),
cards_information_(ai.cards_information_
                   ? new CardsInformation(ai.cards_information())
                   : NULL),
  teaminfo_p(ai.teaminfo_p),
  trick_p(ai.trick_p),
  trickno_p(ai.trickno_p),
  colorjabbed_p(ai.colorjabbed_p),
  low_p(ai.low_p),
  meatlesscolor_p(ai.meatlesscolor_p),
  trumpless_( ai.trumpless_ ),
  last_trick_to_calculate_p(ai.last_trick_to_calculate_p),
  lastHeuristic_(ai.lastHeuristic_),
  vgi_(new VirtualGamesAi(*this)),
  hi_(new HeuristicAi(*this)),
  silentMarriage_( ai.silentMarriage_ )
{
  if (::game_status & GAMESTATUS::GAME)
    DEBUG_ASSERTION((this->trumpless_.size() > 0),
                    "Ai::Ai(ai):\n"
                    "  'trumpless_.size() == 0");

  this->set_type(AI);
  if (this->team_information_)
    this->team_information_->set_player(*this);
  if (this->cards_information_)
    this->cards_information_->set_player(*this);

  delete this->db_;
  if (ai.db())
    this->db_ = new AiDb(*ai.db());
  else
    this->db_ = new AiDb;

  if (::game_status & GAMESTATUS::GAME) {
    for (unsigned i = 0; i < this->colorjabbed_p.size(); ++i) {
      DEBUG_ASSERTION((ai.colorjabbed_p[i].size() == this->game().playerno()),
                      "Ai::Ai(ai)\n"
                      "ai");
      DEBUG_ASSERTION((this->colorjabbed_p[i].size() == this->game().playerno()),
                      "Ai::Ai(ai)\n"
                      "this");
    }
  } // if (::game_status & GAMESTATUS::GAME)

  return ;
} // Ai::Ai(const Ai& ai)

/**********************************************************************
 *
 **    Ai::~Ai()
 *
 **    Parameters:
 *
 **    Result: destroyed AI
 *
 **    Version: Alpha
 *
 **    Description:
 *
 *
 **********************************************************************/

Ai::~Ai()
{
  delete this->vgi_;
  delete this->hi_;
  delete this->team_information_;
  delete this->cards_information_;

  return ;
}

/**
 **
 ** clone the player
 **
 ** @param       -
 **
 ** @return      pointer of a clone
 **
 ** @author      Diether Knof
 **
 ** @version     0.5.3
 **
 **/
Ai*
Ai::clone() const
{  
  return new Ai(*this);
} // virtual Ai* Ai::clone() const

/**********************************************************************
 *
 **    Ai& Ai::operator=(const Ai& ai)
 *
 **    Parameters:  Ai to copy
 *
 **    Result: copy of ai
 *
 **    Version: Alpha
 *
 **    Description: 
 **      
 *
 **********************************************************************/

Ai& Ai::operator=(const Ai& ai)
{
  static_cast<Player&>(*this) = static_cast<const Player&>(ai);
  static_cast<Aiconfig&>(*this) = static_cast<const Aiconfig&>(ai);

  delete this->team_information_;
  this->team_information_ = (ai.team_information_
                             ? new TeamInformation(ai.team_information())
                             : NULL);
  if (this->team_information_)
    this->team_information_->set_player(*this);
  delete this->cards_information_;
  this->cards_information_ = (ai.cards_information_
                              ? new CardsInformation(ai.cards_information())
                              : NULL);
  if (this->cards_information_)
    this->cards_information_->set_player(*this);
  this->teaminfo_p = ai.teaminfo_p;
  this->trick_p = ai.trick_p;
  this->trickno_p=ai.trickno_p;
  this->colorjabbed_p = ai.colorjabbed_p;
  this->low_p=ai.low_p;
  this->meatlesscolor_p=ai.meatlesscolor_p;
  this->trumpless_ = ai.trumpless_;
  this->last_trick_to_calculate_p=ai.last_trick_to_calculate_p;
  this->lastHeuristic_ = ai.lastHeuristic_;
  this->silentMarriage_ = ai.silentMarriage_;
  delete this->db_;
  this->db_ = new AiDb(*(ai.db()));

  return *this;
}

/**
 **
 ** cast to 'VirtualGamesInterface'
 **
 ** @param	-
 **
 ** @return	the cast
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
Ai::operator VirtualGamesInterface const&() const
{
  return *(this->vgi_);
} // virtual Ai::operator VirtualGamesInterface const&() const

/**
 **
 ** cast to 'HeuristicInterface'
 **
 ** @param	-
 **
 ** @return	the cast
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
Ai::operator HeuristicInterface const&() const
{
  return *(this->hi_);
} // virtual Ai::operator HeuristicInterface const&() const

/**
 **
 ** writes the ai into the stream
 **
 ** @param	ostr	stream the ai is to be written in
 **
 ** @return	the output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **
 **/
ostream&
Ai::write(ostream& ostr) const
{
  // output of the name, type and database
  this->Player::write(ostr);

  // output of the configuration
  ostr << '\n'
    << "Aiconfig\n"
    << "{\n";
  this->Aiconfig::write(ostr);
  ostr << "}\n";

  return ostr;
} // virtual ostream& Ai::write(ostream& ostr) const

/**
 **
 ** read the config of the player
 **
 ** @param	config		configuration to read
 ** @param	istr		input stream
 **
 ** @return	whether the configuration was valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
bool
Ai::read(Config const& config, istream& istr)
{
  if (config.separator) {
  } else { // if (config.separator)
#ifndef OUTDATED
    // 2006-09-18   0.7.4
    if (   (config.name == "Aiconfig")
        || (config.name == "aiconfig") )
#else
      if (config.name == "aiconfig")
#endif
      {
        return this->Aiconfig::read(istr);
      }
  } // if (config.separator)

  return this->Player::read(config, istr);

} // virtual bool Ai::read(Config config, istream& istr)

/**
 **
 ** compares the '*this' with 'ai'
 **
 ** @param       ai	ai to compare with
 **
 ** @return      whther 'ai' is equal to '*this'
 **
 ** @author      Diether Knof
 **
 ** @version     0.5.4
 **
 ** @todo	test 'cards_information', 'team_information'
 **/
bool
Ai::isequal(Ai const& ai) const
{
  // ToDo: test team_information
  // ToDo: test cards_information

  // vector<vector<bool> > colorjabbed_p;
  if (this->colorjabbed_p.size() != ai.colorjabbed_p.size())
    return false;

  for (unsigned i = 0; i < this->colorjabbed_p.size(); i++) {
    if (this->colorjabbed_p[i].size() != ai.colorjabbed_p[i].size())
      return false;

    for (unsigned j = 0; j < this->colorjabbed_p[i].size(); j++)
      if (this->colorjabbed_p[i][j] != ai.colorjabbed_p[i][j])
        return false;
  } // for (i < this->colorjabbed_p.size())

  // vector<unsigned> trumpless_p;
  if (this->trumpless_.size() != ai.trumpless_.size())
    return false;

  // vector<bool> trumpless_;
  for (unsigned i = 0; i < this->trumpless_.size(); i++)
    if (this->trumpless_[i] != ai.trumpless_[i])
      return false;

  // unsigned trickno_p;  
  if (this->trickno_p != ai.trickno_p)
    return false;

  // bool low_p;  
  if (this->low_p != ai.low_p)
    return false;

  // Card::Color meatlesscolor_p;  
  if (this->meatlesscolor_p != ai.meatlesscolor_p)
    return false;

  // unsigned last_trick_to_calculate_p;  
  if (this->last_trick_to_calculate_p != ai.last_trick_to_calculate_p)
    return false;

  // Aiconfig::TypeBool lastHeuristic_;  
  if (this->lastHeuristic_ != ai.lastHeuristic_)
    return false;

#if 0
  vector<Trick> trick_p; // last played tricks
#endif

  return true;
} // virtual bool Ai::isequal(Ai ai) const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the aitype for the current trick
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.7
 **
 **/
AiType const&
Ai::aitype() const
{
#ifdef RELEASE
  // Workaround
  if( !( (::game_status >= GAMESTATUS::GAME_PLAY)
        && (::game_status < GAMESTATUS::GAME_FINISHED) ))
    return this->Aiconfig::aitype(0);
#else
  DEBUG_ASSERTION( (::game_status >= GAMESTATUS::GAME_PLAY)
                  && (::game_status < GAMESTATUS::GAME_FINISHED),
                  "Ai::aitype():\n"
                  "  game status not in game: " << ::game_status);
#endif

  return this->Aiconfig::aitype(this->game().trick_current().no());
} // AiType const& Ai::aitype() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the rating type for the current trick
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.3
 **/
Rating::Type const&
Ai::rating() const
{
#ifdef RELEASE
  // Workaround
  if( !( (::game_status >= GAMESTATUS::GAME_PLAY)
        && (::game_status < GAMESTATUS::GAME_FINISHED) ))
    return this->Aiconfig::rating(0);
#else
  DEBUG_ASSERTION( (::game_status >= GAMESTATUS::GAME_PLAY)
                  && (::game_status < GAMESTATUS::GAME_FINISHED),
                  "Ai::rating():\n"
                  "  game status not in game: " << ::game_status);
#endif

  return this->Aiconfig::rating(this->game().trick_current().no());
} // Rating::Type const& Ai::rating() const

/**
 ** sets the cards information
 **
 ** @param     cards_information   new cards information
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Ai::set_cards_information(CardsInformation const& cards_information)
{
  if (this->cards_information_ == &cards_information)
    return ;

  delete this->cards_information_;
  this->cards_information_ = new CardsInformation(cards_information);
  this->cards_information_->set_player(*this);

  return ;
} // void Ai::set_cards_information(CardsInformation cards_information)

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the future limit for the current trick
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.7
 **
 **/
unsigned const
Ai::future_limit() const
{
  DEBUG_ASSERTION( (::game_status >= GAMESTATUS::GAME_PLAY)
                  && (::game_status < GAMESTATUS::GAME_FINISHED),
                  "Ai::future_limit():\n"
                  "  game status not in game: " << ::game_status);

  return this->Aiconfig::future_limit(this->game().trick_current().no());
} // unsigned const Ai::future_limit() const

/**
 ** sets the game of the ai
 **
 ** @param	game	the new game for the ai
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
void
Ai::set_game(Game& game)
{
  bool const old_isvirtual = this->game().isvirtual();
  this->Player::set_game(game);

  if (false)
    if (!old_isvirtual
        && game.isvirtual()) {
      // make the ai not count the cards of its own
      Hand const hand = this->hand();
      this->set_hand(hand);
    }

  return ;
} // Hand const& Ai::set_game(Game& game)

/**
 ** sets the hand of the ai
 **
 ** @param     hand   new hand
 **
 ** @return    the hand
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Hand const&
Ai::set_hand(const Hand& hand)
{

  this->Player::set_hand(hand);
  this->cards_information().set_hand(*this, hand);
  this->team_information().update_teams();

  // how to start in the heuristic 'play low high'
  this->low_p = (this->hand().numberof(Card::JACK) >= this->hand().numberof(Card::QUEEN) +2);

  return this->hand();
} // virtual Hand const& Ai::set_hand(Hand hand)

/**
 ** the rule has changed
 **
 ** @param       type        rule type to have changed
 ** @param       old_value   old value of the rule
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.8
 **/
void
Ai::rule_changed(int const type, void const* const old_value)
{
  this->Aiconfig::rule_changed(type, old_value);
  return ;
} // void Ai::rule_changed(int const type, void const* const old_value)

/**
 **
 ** the game 'game' is opened
 **
 ** @param       game    opened game
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
void
Ai::game_open(Game& game)
{
  this->Player::game_open(game);
  this->team_information().reset();
  this->cards_information().reset();
  return ;
} // virtual void Ai::game_open(Game& game)

/**
 ** the game is started
 **
 ** @param       -
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.9
 **/
void
Ai::game_start() 
{ 
  if (::game_status == GAMESTATUS::GAME_REDISTRIBUTE)
    return ;

  this->Player::game_start();

  { // in a poverty the hand can have changed
    this->cards_information().reset();
    this->cards_information().set_hand(*this, this->hand());
  }

  this->team_information().game_start();
  this->cards_information().game_start();
  this->team_information().update_teams();

  return ;
} // void Ai::game_start()

/**
 **
 ** the game is closed
 **
 ** @param       game	game that is closed
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.9
 **
 **/
void
Ai::game_close(Game const& game) 
{ 
  this->Player::game_close(game);

  delete this->team_information_;
  this->team_information_ = NULL;

  delete this->cards_information_;
  this->cards_information_ = NULL;

  return ;
} // void Ai::game_close(Game game)

/**
 **
 ** -> result
 ** - if 'Aiconfig::HANDS_KNOWN' the real hand of the player is returned
 **   (independent on 'Rule::SHOW_ALL_HANDS')
 ** - if not 'Aiconfig::HANDS_KNOWN' the hand is calculated,
 **   see 'CardsInformation'
 **
 ** @param	player		player whose hand is returned
 **
 ** @result	hand of 'player'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **
 **/
Hand const&
Ai::handofplayer(Player const& player) const
{
  if (   !this->game().isvirtual()
      && (player == *this))
    return this->hand();

  if (this->Aiconfig::value(Aiconfig::HANDS_KNOWN))
    return player.hand();

  // If it is the last trick and the player has already played a card,
  // his hand is empty.
  if (   (::game_status == GAMESTATUS::GAME_PLAY)
      && (this->game().real_tricks_remaining_no() == 1)
      && (this->game().trick_current().cardno_of_player(player)
          < this->game().trick_current().actcardno()))
    return player.hand();

#ifndef RELEASE
  Hand const& hand = this->cards_information().estimated_hand(player);
  hand.self_check();
  return hand;
#else
  return this->cards_information().estimated_hand(player);
#endif
} // Hand Ai::handofplayer(Player const& player) const

/**********************************************************************
 *
 **    Team team(const unsigned i) const; 
 ** 
 *
 **    Parameters:  player
 *
 **    Result: team of player depending on fairplayteams
 *
 **    Version: Alpha
 *
 **    Description: 
 **       
 *
 **********************************************************************/

Team
Ai::teamofplayer(Player const& player) const
{
  if (   !this->Aiconfig::value(Aiconfig::TEAMS_KNOWN)
      || (this->game().type() == GAMETYPE::MARRIAGE))
  {  
    // in the marriage case, as long as the teams are not determined, the player
    // shall think that every other player is against him
    return this->teaminfo( player );
  } else
  {
    return  player.team();
  }
} // Team Ai::teamofplayer(Player player) const

/**
 ** set the teams
 **
 ** @param     teams   teams of the players
 **
 ** @return    -
 **
 ** @author    Diether Knof
 ** @author    Borg Enders
 **
 ** @version   0.7.2
 **/
void
Ai::set_teams(vector<Team> const& teams)
{
  this->team_information().set_teams(teams);
  this->set_team(teams[this->game().no(*this)]);

  return ;
} // void Ai::set_teams(vector<Team> teams)

/**
 **
 ** the trick is opened
 **
 ** @param      trick	opened trick
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.9
 **
 **/
void
Ai::trick_open(Trick const& trick)
{ 
  this->cards_information().trick_open(trick);
  this->team_information().update_teams();

  return ;
} // void Ai::trick_open(Trick const& trick)

/**
 **
 ** the trick is full
 **
 ** @param      trick   full trick
 **
 ** @return     -
 **
 ** @version    0.4.5
 **
 ** @author     Diether Knof
 **
 **/
void
Ai::trick_full(Trick const& trick)
{ 
  DEBUG_ASSERT( trick.isfull() );

  this->team_information().trick_full(trick);
  this->cards_information().trick_full(trick);
  this->team_information().update_teams();

  if( trick.startcard().istrump() ) {
    for ( unsigned i = 0; i < trick.actcardno(); i++) {
      if (!trick.card(i).istrump()) {
        DEBUG_ASSERTION((this->trumpless_.size()
                         > this->game().no(trick.player_of_card(i))),
                        "Ai::trick_full(trick):\n"
                        "  'this->trumpless_' is too small: "
                        << this->trumpless_.size() << " <= "
                        << this->game().no(trick.player_of_card(i)));
        this->trumpless_[ this->game().no(trick.player_of_card(i)) ] = true;
      }
    }
  }

  return ;
} // void Ai::trick_full(Trick const& trick)


/**********************************************************************
 *
 ** Card Ai::card_get()
 *
 ** Parameters:  -
 *
 ** Result:      card played by the ai
 *
 ** Version:     0.4.4
 *
 ** Description: returns the card, the ai wishes to play
 *
 **********************************************************************/
HandCard
Ai::card_get()
{
  HandCard c(this->hand(), this->nextcard(this->game().trick_current()));

  if (   c.possible_swine()
      && game().swines_announcement_valid( *this ) )
    this->game().swines_announce(*this);
  if (   c.possible_hyperswine()
      && game().hyperswines_announcement_valid( *this ) )
    this->game().hyperswines_announce(*this);
  // ToDo: check for last chance to announce
#if 0
  if (this->game().last_chance_to_announce(this->next_announcement(), player)) {
    Ai* virt_ai = this->clone();
    vector<Player*> players(this->game().playerno());
    for (unsigned p = 0; p < this->game().playerno(); ++p)
      players[p] = virt_ai;
    Game virt_game(this->game(), players);
    virt_game.trick_current() += c;
    this->game().make_announcement(virt_ai->announcement_request(), *this);
    delete virt_ai;
  }
#endif

  return c;
} // HandCard Ai::card_get() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	suggested card
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
HandCard
Ai::card_suggestion()
{
  if (::bug_report_replay) {
    static bool bug_report_flip = false;
    if (this->lastHeuristic_ != Aiconfig::BUG_REPORT)
      bug_report_flip = true;
    else
      bug_report_flip ^= true;
    if (bug_report_flip) {
      this->lastHeuristic_ = Aiconfig::BUG_REPORT;
      Card const card = ::bug_report_replay->next_card(*this);
      if (   !card.is_empty()
          && this->hand().contains(card))
        return HandCard(this->hand(), card);
    }
  }
  return HandCard(this->hand(),
                  this->nextcard(this->game().trick_current()));
} // HandCard Ai::card_suggestion()

/**
 **
 ** 'player' has played the card 'card'
 **
 ** @param       card    played card
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.6.3
 **
 **/
void
Ai::card_played(HandCard const& card)
{ 
  this->team_information().card_played(card);
  this->cards_information().card_played(card);
  this->team_information().update_teams();

  if( this->game().no(*this) == DEBUG_PLAYER ) {
    COUT << "card played " << card << endl;
    COUT << "virtual = " << this->game().isvirtual() << endl;
  }

  Player const& player = card.player();
  Card::Color jabbed=Card::NOCARDCOLOR;


  Trick t = player.game().trick_current();

  if (!t.startcard().istrump())
  {
    jabbed=t.startcard().color();
  }


  if (jabbed!=Card::NOCARDCOLOR && card.istrump())
  {
    colorjabbed_p[jabbed][this->game().no(player)]=true;
  }  
  if(   jabbed != Card::NOCARDCOLOR 
     && card.color() != jabbed
     && (this->cards_information().remaining_trumps_others()
         >=  game().tricks_remaining_no() ) )
  {
    colorjabbed_p[jabbed][this->game().no(player)]=true;
  }  


  if (player == *this) {
    switch (this->lastHeuristic_) {
    case Aiconfig::LOW_HIGH:
    case Aiconfig::COLOR_LOW_HIGH:
      if (! (   this->low_p
             && (this->game().trick_current().winnerplayer() == *this)) )
        this->check_low_high(card);
      break;

    default:
      break;
    } // switch (this->lastHeuristic_)
    if(  this->hand().numberof(Card::JACK) >= this->hand().numberof(Card::QUEEN) +2 )
      this->low_p = true;
  } // if (player == *this)



  this->teaminfo_update();

  return ;
} // void Ai::card_played(HandCard const& card)

/**
 ** check a swines announcement at the game start
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.6
 **/
void
Ai::check_swines_announcement_at_game_start()
{
  if (this->type() == Player::AI) {
    // announce swines
    if (this->game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN))
      if (this->game().swines_announcement_valid(*this))
        this->game().swines_announce(*this);

    // hyperswines are announced by 'Ai::swines_announced(player)'
  } // if (this->type() == Player::AI)

  return ;
} // void Ai::check_swines_announcement_at_game_start()

/**
 ** 'player' has announce swines
 ** if possible announce hyperswines
 **
 ** @param      player	player that has announced the swines
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.6
 **/
void
Ai::swines_announced(Player const& player)
{ 
  this->cards_information().swines_announced(player);
  this->team_information().update_teams();

  if (this->type() == Player::AI) {
    if (   this->game().hyperswines_announcement_valid(*this)
        && this->game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN))
      this->game().hyperswines_announce(*this);
  } // if (this->type() == Player::AI)

  return ;
} // void Ai::swines_announced(Player const& player)

/**
 **
 ** 'player' has announce hyperswines
 **
 ** @param      player	player that has announced the hyperswines
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.6
 **
 **/
void
Ai::hyperswines_announced(Player const& player)
{ 
  this->cards_information().hyperswines_announced(player);
  this->team_information().update_teams();

  return ;
} // void Ai::hyperswines_announced(Player const& player)

/**
 ** the marriage partner has found a bride
 ** if the bride is the bridegroom, the bridegroom must play a solo
 **
 ** @param	bridegroom	the player with the marriage
 ** @param	bride		the bride
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
void
Ai::marriage(Player const& bridegroom, Player const& bride)
{
  this->team_information().marriage(bridegroom, bride);
  this->teaminfo_update();

  return ;
} // void Ai::marriage(Player bridegroom, Player bride)

/**
 **
 ** a player has a genscher and selected a new partner
 **
 ** @param	genscher	genscher player
 ** @param	partner		partner of the gensher player
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **
 **/
void
Ai::genscher(Player const& genscher, Player const& partner)
{ 
  this->team_information().genscher(genscher, partner);
  this->cards_information().genscher(genscher, partner);
  this->team_information().update_teams();

  return ;
} // void Ai::genscher(Player genscher, Player partner)

/**
 **
 ** selects a card to play
 **
 ** @param	trick	current trick
 ** 
 ** @return	card to play
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
Card
Ai::nextcard(Trick const& trick)
{
  if (!this->game().isvirtual())
    db()->heuristicInc( Aiconfig::NO_HEURISTIC );

  if (!this->game().isvirtual()) {
    // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()
       ) {
      if ( (::bug_report_replay->current_action().type
            == GameplayAction::SWINES)
          && (static_cast<GameplayAction::Swines const&>(::bug_report_replay->current_action()).player == this->game().no(*this)) )
        this->game().swines_announce(*this);

      if ( (::bug_report_replay->current_action().type
            == GameplayAction::HYPERSWINES)
          && (static_cast<GameplayAction::Hyperswines const&>(::bug_report_replay->current_action()).player == this->game().no(*this)) )
        this->game().hyperswines_announce(*this);

      if ( (::bug_report_replay->current_action().type
            == GameplayAction::CARD_PLAYED)
          && (static_cast<GameplayAction::CardPlayed const&>(::bug_report_replay->current_action()).player == this->game().no(*this)) ) {
        this->lastHeuristic_ = Aiconfig::BUG_REPORT;
        HandCard const card(this->hand(), static_cast<GameplayAction::CardPlayed const&>(::bug_report_replay->current_action()).card);
        if (this->hand().contains(card)
            && trick.isvalid(card))
          return card;
      }
    } // if (auto execute card play)
  } // bug report replay

  { // only valid card
    HandCards const valid_cards = this->hand().validcards(trick);
    DEBUG_ASSERTION((valid_cards.cardsnumber() > 0),
                    "Ai::nextcard(Trick, output):\n"
                    "  found no valid card."
                    "Trick: " << trick
                    << "Hand: " << this->hand());
    // if there is only one valid card that's it
    if (valid_cards.cardsnumber() == 1) {
      Card const card = valid_cards.card(0);
      Aiconfig::Heuristic const heuristic = Aiconfig::ONLY_VALID_CARD;

      if (HEURISTIC_OUTPUT
          && !this->game().isvirtual())
        cout <<  setw(2) << this->game().no(*this) << " "
          << setw(20) << card << " "
          << ::name(heuristic)
          << endl;

      this->lastHeuristic_ = heuristic;
      // only increment, if the ai could select a heuristic
      if (static_cast<int>(this->game().tricks_remaining_no()) >
          this->Aiconfig::value(Aiconfig::LAST_TRICKS_WITHOUT_HEURISTICS ) ) {
        this->db()->heuristicInc(heuristic);
      }

      return card;
    } // if (valid_cards.cardsnumber() == 1)
  } // only valid card

  if (static_cast<int>(this->game().tricks_remaining_no()) >
      this->Aiconfig::value(Aiconfig::LAST_TRICKS_WITHOUT_HEURISTICS ) ) {
#ifdef CHECK_RUNTIME
      auto const ssp = (  !this->game().isvirtual()
                        ? ::runtime["ai heuristics"].start_stop_proxy()
                        : nullptr);
#endif

    // special case: unset marriage and last chance
    if (   (this->game().type() == GAMETYPE::MARRIAGE)
        && (this->game().soloplayer() == *this)
        && (this->game().marriage_selector() != MARRIAGE_SELECTOR::SILENT)
        && (this->game().marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)
        && (this->game().trick_current().isempty())
        && (this->game().trick_current_no() + 1 == this->game().rule()(Rule::MARRIAGE_DETERMINATION)) ) {
      Card const card = Heuristics::play_to_get_married(trick, *this);
      CLOG << card << endl;
      if (card) {
        this->lastHeuristic_ = Aiconfig::PLAY_TO_GET_MARRIED;
        this->db()->heuristicInc(Aiconfig::PLAY_TO_GET_MARRIED);
        return card;
      }
    }

    vector<Aiconfig::HeuristicState> const
      heuristics_vector = (  this->silentMarriage_
                           ? (this->heuristic_states(HeuristicsMap::MARRIAGE_SILENT,
                                                     HeuristicsMap::RE))
                           : this->heuristic_states(*this));
    for (vector<Aiconfig::HeuristicState>::const_iterator
         heuristic_state = heuristics_vector.begin();
         heuristic_state != heuristics_vector.end();
         ++heuristic_state) {
      if (!heuristic_state->active)
        continue;
      Heuristic const heuristic = heuristic_state->heuristic;

      Card card; // card to play
      switch (heuristic) {
      case Aiconfig::NO_HEURISTIC:
      case Aiconfig::MANUAL:
      case Aiconfig::BUG_REPORT:
      case Aiconfig::NETWORK:
      case Aiconfig::ONLY_VALID_CARD:
        DEBUG_ASSERTION(false,
                        "Ai::nextcard(trick)\n"
                        "  heuristic '" << heuristic << "' is not valid");
        break;
      case Aiconfig::PLAY_TO_MARRY:
        card = Heuristics::play_to_marry(trick, *this);
        break;
      case Aiconfig::PLAY_TO_GET_MARRIED:
        card = Heuristics::play_to_get_married(trick, *this);
        break;
      case Aiconfig::CHOOSE_ACE:
        card = Heuristics::choose_ace(trick, *this);
        break;
      case Aiconfig::CHOOSE_TEN:
        card = Heuristics::choose_ten(trick, *this);
        break;
      case Aiconfig::START_WITH_COLOR:
        card = Heuristics::start_with_color(trick, *this);
        break;
      case Aiconfig::START_WITH_LOW_COLOR:
        card = Heuristics::start_with_low_color(trick, *this);
        break;
      case Aiconfig::START_WITH_LOW_TRUMP:
        card = Heuristics::start_with_low_trump(trick, *this);
        break;
      case Aiconfig::RETRY_COLOR:
        card = Heuristics::retry_color(trick, *this);
        break;
      case Aiconfig::PLAY_COLOR_FOR_PARTNER:
        card = Heuristics::play_color_for_partner(trick, *this);
        break;
      case Aiconfig::TRY_COLOR_FOR_PARTNER:
        card = Heuristics::try_color_for_partner(trick, *this);
        break;
      case Aiconfig::PLAY_COLOR_FOR_PARTNER_ACE:
        card = Heuristics::play_color_for_partner_ace(trick, *this);
        break;
      case Aiconfig::PLAY_BAD_COLOR:
        card = Heuristics::play_bad_color(trick, *this);
        break;
      case Aiconfig::CHOOSE_FOR_COLOR_TRICK:
        card = Heuristics::choose_for_color_trick(trick, *this);
        break;
      case Aiconfig::JAB_COLOR_OVER_FOX:
        card = Heuristics::jab_color_over_fox(trick, *this);
        break;
      case Aiconfig::SERVE_COLOR_TRICK:
        card = Heuristics::serve_color_trick(trick, *this);
        break;
      case Aiconfig::SERVE_TRUMP_TRICK:
        card = Heuristics::serve_trump_trick(trick, *this);
        break;
      case Aiconfig::CHOOSE_PFUND_POVERTY:
      case Aiconfig::CHOOSE_PFUND:
        card = Heuristics::choose_pfund(trick, *this);
        break;
      case Aiconfig::CHOOSE_PFUND_BEFORE_PARTNER:
        card = Heuristics::choose_pfund_before_partner(trick, *this);
        break;
      case Aiconfig::JAB_FOR_ACE:
        card = Heuristics::jab_for_ace(trick, *this);
        break;
      case Aiconfig::CREATE_FEHL:
        card = Heuristics::create_fehl(trick, *this);
        break;
      case Aiconfig::BEST_WINNING:
        if (trick.isempty())
          break;

#ifndef OUTDATED
        if (   (maybe_to_team(this->teaminfo(trick.winnerplayer())) == this->team())
            && (   (   !trick.winnercard().istrump()
                    && (trick.winnercard().value() == Card::ACE)
                    && (this->cards_information().played(trick.winnercard().tcolor()) <= trick.actcardno()))
                || (trick.winnercard().istrump()
                    && this->value(Aiconfig::TRUMPLIMIT_NORMAL).less(trick.winnercard()))
               )) {
          break;
        }
#endif

        card = Heuristics::best_winning_card(trick, *this);
        break;
      case Aiconfig::LOW_HIGH:
      case Aiconfig::COLOR_LOW_HIGH:
        // ToDo: Warum extra 'COLOR_LOW_HIGH'?
        card = Heuristics::play_low_high(trick, *this);
        break;
      case Aiconfig::PLAY_TRUMP:
        card = Heuristics::play_trump(trick, *this);
        break;
      case Aiconfig::PLAY_FOR_TEAM:
        card = Heuristics::play_for_team(trick, *this);
        break;
      case Aiconfig::JAB_FOX:
        card = Heuristics::jab_fox(trick, *this);    
        break;
      case Aiconfig::PLAY_FOR_PARTNER_WORRIES:
        card = Heuristics::play_for_partner_worries(trick, *this);
        break;
      case Aiconfig::TRY_FOR_DOPPELKOPF:
        card = Heuristics::try_for_doppelkopf(trick, *this);

        // make an announcement so that the player behind can give points
        if (!card.is_empty()
            && (this->announcement() == ANNOUNCEMENT::NOANNOUNCEMENT)
            && !trick.islastcard()
            && (this->teaminfo(trick.player_of_card(this->game().playerno()))
                != ::opposite(this->team()))
           )
          this->game().make_announcement(ANNOUNCEMENT::NO120, *this);

        break;
      case Aiconfig::PARTNER_BACKHAND_DRAW_TRUMP:
        card = Heuristics::partner_backhand_draw_trump(trick, *this);
        break;
      case Aiconfig::DRAW_TRUMP:
        card = Heuristics::draw_trump(trick, *this);
        break;
      case Aiconfig::PLAY_TO_JAB_LATER:
        card = Heuristics::play_to_jab_later(trick, *this);
        break;
      case Aiconfig::PLAY_HIGHEST_COLOR_CARD_IN_GAME:
        card = Heuristics::play_highest_color_card_in_game(trick, *this);
        break;
      case Aiconfig::GET_TRICK_FOR_ANNOUNCEMENT:
        card = Heuristics::get_trick_for_keeping_announcement(trick, *this);
        break;
      case Aiconfig::GRAB_TRICK:
        card = Heuristics::grab_trick(trick, *this);
        break;
      case Aiconfig::LET_TRICK_PASS:
        card = Heuristics::let_trick_pass(trick, *this);
        break;


      case Aiconfig::POVERTY_SPECIAL_PLAY_PFUND:
        card = Heuristics::poverty_special_play_pfund(trick, *this);
        break;
      case Aiconfig::POVERTY_SPECIAL_GIVE_NO_POINTS:
        card = Heuristics::poverty_special_give_no_points(trick, *this);
        break;
      case Aiconfig::POVERTY_SPECIAL_OFFER_PFUND:
        card = Heuristics::poverty_special_offer_pfund(trick, *this);
        break;
      case Aiconfig::POVERTY_RE_TRUMP_COLOR_TRICK_HIGH:
        card = Heuristics::poverty_re_trump_color_trick_high(trick, *this);
        break;
      case Aiconfig::POVERTY_RE_PLAY_TRUMP:
        card = Heuristics::poverty_re_play_trump(trick, *this);
        break;
      case Aiconfig::POVERTY_CONTRA_PLAY_COLOR:
        card = Heuristics::poverty_contra_play_color(trick, *this);
        break;
      case Aiconfig::POVERTY_CONTRA_TRUMP_COLOR_TRICK_HIGH:
        card = Heuristics::poverty_contra_trump_color_trick_high(trick, *this);
        break;
      case Aiconfig::POVERTY_LEAVE_TO_PARTNER:
        card = Heuristics::poverty_leave_to_partner(trick, *this);
        break;
      case Aiconfig::POVERTY_OVERJAB_RE:
        card = Heuristics::poverty_overjab_re(trick, *this);
        break;
      case Aiconfig::POVERTY_BEST_WINNING_CARD:
        card = Heuristics::poverty_best_winning_card(trick, *this);
        break;


      case Aiconfig::PLAY_COLOR_IN_SOLO:
        card = Heuristics::play_color_in_solo(trick, *this);
        break;
      case Aiconfig::MEATLESS_PLAYHIGHESTCOLOR:
        if (!trick.isstartcard())
          break;
        if (this->game().type() != GAMETYPE::SOLO_MEATLESS)
          break;

        card = this->nextcard_meatless(trick);
        break;
      case Aiconfig::PICTURE_SECONDBESTTRUMP:
        if (!GAMETYPE::is_picture_solo(this->game().type()))
          break;
        // ToDo: Warum nicht einfach nur 'SECONDBESTTRUMP'?
        if (   trick.isstartcard()
            && (this->cards_information().remaining_trumps_others() > 0)
            && (this->hand().numberoftrumps() > 1))
          card = Heuristics::SecondBestTrump(*this);
        break;
      case Aiconfig::CHOOSE_SINGLE_ACE:
        card = Heuristics::choose_single_ace(trick, *this);
        break;
      case Aiconfig::COLOR_JAB_FOR_ACE:
        if (!GAMETYPE::is_color_solo(this->game().type()))
          break;
        // ToDo: Warum extra 'COLOR_JAB_FOR_ACE'?
        card = Heuristics::jab_for_ace(trick, *this);
        break;
      case Aiconfig::CHOOSE_BEST_CARD:
        DEBUG_ASSERT(false);
        break;
      } // switch(*heuristic)

      if (!card.is_empty()) {
        if (   HEURISTIC_OUTPUT
            && !this->game().isvirtual())
          cout <<  setw(2) << this->game().no(*this) << " "
            << setw(20) << card << " "
            << ::name(heuristic)
            << endl;

        this->lastHeuristic_ = heuristic;
        this->db()->heuristicInc(heuristic);

        return card;
      } // if (card)
    } // for (...)
  } // if (actTrick < LASTHEURISTICTRICK)

  { // choosebestcard
#if 0
    if (!this->game().isvirtual()
        && this->game().tricks_remaining_no() > 
        this->Aiconfig::value(Aiconfig::LAST_TRICKS_WITHOUT_HEURISTICS ) )
      ::bug_report.report("no heuristic", "automatic");
#endif

    Aiconfig::Heuristic const heuristic = Aiconfig::CHOOSE_BEST_CARD;
    Card const card = Weighting::best_card(this->aitype(), *this);

    if (HEURISTIC_OUTPUT
        && !this->game().isvirtual())
      cout <<  setw(2) << this->game().no(*this) << " "
        << setw(16) << card << " "
        << setw(39) << ::name(heuristic) << " (" << this->aitype() << ") "
        << endl;

    this->lastHeuristic_ = heuristic;
    // only increment, if the ai could select a heuristic
    if (static_cast<int>(this->game().tricks_remaining_no()) >
        this->Aiconfig::value(Aiconfig::LAST_TRICKS_WITHOUT_HEURISTICS ) ) {
      this->db()->heuristicInc(heuristic);
    }

    if (::game_status != GAMESTATUS::GAME_PLAY)
      return Card();

    if (this->aitype() != AiType::NO_CHOOSEBESTCARD)
      DEBUG_ASSERT(!card.is_empty());

    return card;
  } // choosebestcard
} // Card Ai::nextcard(Trick const& trick)



/**********************************************************************
 *
 **    int Ai::calculate_meatless_color( Color c)
 *
 **    Parameters:  
 **        c color to calculate
 *
 **    Result: value of card 
 *
 **    Version: 0.6.1
 *
 **    Description:
 **      
 *
 **********************************************************************/
int
Ai::calculate_meatless_color( Card::Color const co )
{
  if ( hand().numberof( co ) == 0 ) return -1000;
  int modi = 0;
  if (this->cards_information().remaining_others(Card(co,Card::ACE)))
    modi = -1;

  if (!this->cards_information().remaining(Card(co,Card::ACE))) {
    if (this->cards_information().remaining_others(Card(co, Card::TEN)))
      modi = -1;
    else
      modi = 1;      
  }
  if ( hand().numberof(Card(co,Card::ACE))
      == this->game().rule()(Rule::NUMBER_OF_SAME_CARDS) )
    modi = 1;

  int result = static_cast<int>(((hand().numberof(co, this->game())) + (5*modi)) 
                                * hand().numberof(Card(co,Card::ACE))
                                +  (hand().numberof(co, this->game()) + (4 * modi))
                                * hand().numberof(Card(co,Card::TEN))
                                +  hand().numberof(co, this->game())

                               ) *modi;
  return result;
} // int Ai::calculate_meatless_color( Card::Color const co )

/**
 ** -> result
 **
 ** @param	t	current trick
 **
 ** @return	card to play
 **
 ** @author	Borg Enders
 **
 ** @version	0.6.6
 **
 ** @todo	-> heuristics.cpp
 **/
Card
Ai::nextcard_meatless(const Trick& t)
{
  Hand ha;
  Card c=Card();
  int maxcards=0;
  if (   (this->meatlesscolor_p == Card::NOCARDCOLOR)
      || (   !this->hand().contains(Card(meatlesscolor_p, Card::ACE))
          && this->cards_information().remaining(Card( meatlesscolor_p, Card::ACE))))
  {
    Card::Color co=Card::DIAMOND;

    maxcards=calculate_meatless_color(co);
    meatlesscolor_p=co;

    co=Card::HEART;

    if (calculate_meatless_color(co)>maxcards)
    {
      maxcards=calculate_meatless_color(co);
      meatlesscolor_p=co;
    }

    co=Card::SPADE;

    if (calculate_meatless_color(co)>maxcards)
    {      	
      maxcards=calculate_meatless_color(co);
      meatlesscolor_p=co;
    }

    co=Card::CLUB;

    if (calculate_meatless_color(co)>maxcards)
    {

      maxcards= calculate_meatless_color( co );
      meatlesscolor_p=co;
    }
  } 

  c=Heuristics::play_highest_color(t,*this,meatlesscolor_p);
  if( c==Card() )
    meatlesscolor_p = Card::NOCARDCOLOR;
  if( hand().numberof( c.color() ) == 1 ) //last card of this color
    meatlesscolor_p = Card::NOCARDCOLOR; // trigger new calculation

  return c;
} // Card Ai::nextcard_meatless(Trick t)


/**********************************************************************
 *
 **    bool Ai::colorjabbed(Card::Color c)
 *
 **    Parameters: color of for which should be known if it was jabbed
 *
 **    Result: true if color was jabbed and c is legal
 **            false otherwise
 *
 **    Version: Alpha
 *
 **    Description:
 *
 *
 **********************************************************************/
bool
Ai::colorjabbed(const Card::Color c) const
{
  bool result=false;
  if (!(c==Card::NOCARDCOLOR)) 
    for(unsigned i=0; i<game().playerno();i++) {
      result=result||colorjabbed_p[c][i];
    }

  return result;
}


/**********************************************************************
 *
 **    bool Ai::jabbedbyownteam(Card::Color c)
 *
 **    Parameters: color of for which should be known if it was jabbed by own team
 *
 **    Result: true if color was jabbed by own team and c is legal
 **            false otherwise
 *
 **    Version: Alpha
 *
 **    Description:
 *
 *
 **********************************************************************/

bool
Ai::jabbedbyownteam(const Card::Color c) const
{
  bool res=false;

  if (!(c==Card::NOCARDCOLOR)) 
    for(unsigned i=0; i<game().playerno();i++) {
      if (i!=this->game().no(*this)
          && maybe_to_team(teaminfo( game().player( i )))==team())
        res=res||colorjabbed_p[c][i];
    }

  return res;
}

/**********************************************************************
 *
 **    bool Ai::jabbedbyownteam(Card::Color c)
 *
 **    Parameters: color of for which should be known if it was jabbed by own team
 *
 **    Result: true if color was jabbed aby own team nd c is legal
 **            false otherwise
 *
 **    Version: Alpha
 *
 **    Description:
 *
 *
 **********************************************************************/

bool
Ai::jabbedbyotherteam(const Card::Color c) const
{
  bool res=false;

  if (!(c==Card::NOCARDCOLOR)) 
    for(unsigned i=0; i<game().playerno();i++) {
      if (i!=this->game().no(*this) && maybe_to_team(teaminfo( game().player( i )))!=team())
        res=res||colorjabbed_p[c][i];
    }

  return res;
}

/**
 ** -> result
 **
 ** @param     color   color
 **
 ** @return    how many times the color has run
 **
 ** @author    Diether Knof
 ** @author    Borg Enders
 **
 ** @version   0.7.4
 **/
unsigned
Ai::color_runs(Card::Color const color) const
{
  return this->cards_information().color_runs(color);
} // unsigned Ai::color_runs(Card::Color color) const

/**
 ** -> result
 **
 ** @param     trick   current trick
 **
 ** @return    how many times the color of the trick has run
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
unsigned
Ai::color_runs(Trick const& trick) const
{
  DEBUG_ASSERTION(!trick.isempty(),
                  "Ai::color_runs(trick)\n"
                  "  trick is empty");
  return this->cards_information().color_runs(trick.startcard().tcolor());
} // unsigned Ai::color_runs(Trick trick) const


/**********************************************************************
 *
 **     bool Ai::next_low()
 *
 **    Parameters: 
 *
 **    Result: returns true if next card should be low trump otherwise false
 *
 **    Version: Alpha
 *
 **    Description:
 *
 *
 **********************************************************************/

bool Ai::next_low() const
{
  return this->low_p;
}


/**********************************************************************
 *
 **     void Ai::check_low_high(Card c)
 *
 **    Parameters: next palyed card
 *
 **    Result: 
 *
 **    Version: Alpha
 *
 **    Description:
 **      sets low_p to true if c>=DIAMOND QUEEN to false if card<DIAMOND QUENN
 *
 **********************************************************************/ 
void
Ai::check_low_high(HandCard const& card)
{
  if ( card.istrump()
      && !(card.less(Card(Card::DIAMOND,Card::QUEEN))))
    this->low_p = true;
  else
    this->low_p = false;

  return ;
}

/**********************************************************************   
 **                                                                        
 **    void Ai::new_game()                                             
 **                                                                        
 **    Parameters: none                                                    
 **                                                                        
 **    Result: none                                                        
 **                                                                        
 **    Version: Alpha                                                      
 **                                                                        
 **    Description: inits the player for a new game                        
 **                 (the game-pointer is set in 'Game::Game(Party, int)')  
 **                                                                        
 **********************************************************************/  
void                                                                      
Ai::new_game(Game& game)                                                        
{    
  this->Player::new_game(game);

  delete this->team_information_;
  this->team_information_ = new TeamInformation(*this);

  delete this->cards_information_;
  this->cards_information_ = new CardsInformation(*this);

  this->teaminfo_p = vector<Team>(this->game().playerno(), TEAM::UNKNOWN);

  this->meatlesscolor_p=Card::NOCARDCOLOR;

  this->low_p=true;


  this->trick_p = vector<Trick>(this->game().rule()(Rule::NUMBER_OF_TRICKS_IN_GAME));

  this->trumpless_ = vector<bool>(this->game().playerno(), false );
  this->trickno_p=0;
  for ( unsigned x = 0;
       x < this->game().rule()(Rule::NUMBER_OF_CARD_COLORS); x++)
  {
    this->colorjabbed_p[x]= vector<bool>(this->game().playerno(), false );
    for(unsigned y=0;y<this->game().playerno();y++) {
      this->colorjabbed_p[x][y]=false;
    } // for (y < this->game().playerno())
  } // for (x < this->game().playerno())

  this->silentMarriage_ = false;

  return ;
}


/**********************************************************************
 *
 **    MarriageSelector Ai::get_Marriage() const
 *
 **    Parameters: none
 *
 **    Result: tricktype which determines teams for a marraige
 *
 **    Version: Alpha
 *
 **    Description: 
 **      
 *
 **********************************************************************/

MarriageSelector Ai::get_Marriage() const
{
  bool const isstartplayer = (this->game().startplayer() == *this);
  // ToDo: Test rules
  if (this->game().rule()(Rule::MARRIAGE_FIRST_FOREIGN))
    return MARRIAGE_SELECTOR::FIRST_FOREIGN;

  if (hand().numberoftrumps()<6
      && hand().numberoftrumpaces()==1
      && (   isstartplayer
          || (this->game().rule()(Rule::MARRIAGE_DETERMINATION) >= 4)))
    return MARRIAGE_SELECTOR::FIRST_TRUMP;

  if ( Card(Card::HEART,Card::TEN).istrump(this->game()) )
  { 
    if (hand().numberof(Card::HEART)>3 &&
        (hand().numberof(Card::HEART,Card::ACE)>0)
       )
      //      firstMarriageColor_ = Card::HEART;
      return MARRIAGE_SELECTOR::FIRST_HEART;
  } else
  {
    if(  hand().numberof(Card::HEART) > 4 
       || (   hand().numberof(Card::HEART)<2 
           && (  (   hand().numberof(Card::HEART,Card::TEN)==1  
                  && isstartplayer)
              )
          )
      )
    {           
      //      firstMarriageColor_ = Card::HEART;
      return MARRIAGE_SELECTOR::FIRST_HEART;
    }
  }
  if(  hand().numberof(Card::CLUB) > 4 
     || (   hand().numberof(Card::CLUB)<2 
         && (   (   hand().numberof(Card::CLUB,Card::TEN)==1  
                 && isstartplayer)
            )
        )
    )
  {
    //    firstMarriageColor_ = Card::CLUB;
    return MARRIAGE_SELECTOR::FIRST_CLUB;
  }
  if (  hand().numberof(Card::SPADE) > 4 
      || (   hand().numberof(Card::SPADE)<2 
          &&  (   hand().numberof(Card::SPADE,Card::TEN)==1  
               && isstartplayer)
         )
     )
  {
    //   firstMarriageColor_ = Card::SPADE;
    return MARRIAGE_SELECTOR::FIRST_SPADE;
  }
  if(    hand().numberof(Card::HEART) == 1
     && hand().numberof(Card::HEART,Card::ACE)!=1
     && isstartplayer)
  {           
    //     firstMarriageColor_ = Card::HEART;
    return MARRIAGE_SELECTOR::FIRST_HEART;
  }
  if(    hand().numberof(Card::CLUB) == 1
     && hand().numberof(Card::CLUB,Card::ACE)!=1
     && isstartplayer)
  {           
    //   firstMarriageColor_ = Card::CLUB;
    return MARRIAGE_SELECTOR::FIRST_CLUB;
  }
  if(    hand().numberof(Card::SPADE) == 1
     && hand().numberof(Card::SPADE,Card::ACE)!=1
     && isstartplayer)
  {           
    //     firstMarriageColor_ = Card::SPADE;
    return MARRIAGE_SELECTOR::FIRST_SPADE;
  }



  if (   hand().numberoftrumps()>=8
      && !isstartplayer)
    return MARRIAGE_SELECTOR::FIRST_COLOR;

  return MARRIAGE_SELECTOR::FIRST_FOREIGN;
}

/**
 ** -> result
 **
 ** @param     isDuty   whether the player must play a duty solo
 **                     (has no effect, yet)
 **
 ** @result    the reservation of the ai
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.3
 **/
Reservation const&
Ai::reservation_get( bool const isDuty ) 
{
  this->team_information().reset();
  this->cards_information().reset();

  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()
        && (::bug_report_replay->current_action().type
            == GameplayAction::RESERVATION)
        && (static_cast<GameplayAction::Reservation const&>(::bug_report_replay->current_action()).player
            == this->game().no(*this))
       ) {
      return (this->reservation() = static_cast<GameplayAction::Reservation const&>(::bug_report_replay->current_action()).reservation);
    } // if (auto execute)
  } // bug report replay


  Reservation res = this->reservation_get_default();
  Reservation best_duty_solo_res;
  int solo_best_duty = INT_MIN;

  // always announce swines/hyperswines
  // -- the game will reject it if the announcement is not valid
  best_duty_solo_res.swines
    = this->game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN);
  best_duty_solo_res.hyperswines
    = this->game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN);

  // check, whether to announce 'richness'
  if (res.game_type == GAMETYPE::THROWN_RICHNESS) {
    if (Heuristics::make_announcement(*this))
      res.game_type = GAMETYPE::NORMAL;
  } // if (res.game_type == GAMETYPE::THROWN_RICHNESS)


  int solovalue=0;  
  int solomax=0;
  int duty_soli_mod = 0; // modificator to play more solos for the end of a tournament

  if( this->remaining_duty_soli() > 0) {
    // CRIT: test formula

    // divide remaining duty soli on remaining rounds
    // 2do Aiconfig
    unsigned weighting = 2;
    unsigned rounds_per_solo = 2;
    duty_soli_mod
      = static_cast<unsigned>(round((weighting
                                     * rounds_per_solo
                                     * this->remaining_duty_soli()
                                    ) / std::max(1u, this->game().party().remaining_rounds())
                                   ));
  }

  if ( party.rule()(Rule::SOLO_KOEHLER)) {
    solovalue = checksolo(GAMETYPE::SOLO_KOEHLER);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ( solovalue > this->Aiconfig::value(Aiconfig::TRIPLESOLO) )
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::TRIPLESOLO);
      res.game_type=GAMETYPE::SOLO_KOEHLER; 
    }
    if (this->remaining_duty_picture_soli()
        || this->remaining_duty_free_soli())
    {
      best_duty_solo_res.game_type = GAMETYPE::SOLO_KOEHLER;
      solo_best_duty = solovalue-this->Aiconfig::value(Aiconfig::TRIPLESOLO);
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_KOEHLER, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_KOEHLER))

  if ( party.rule()(Rule::SOLO_QUEEN_JACK)) {
    solovalue=checksolo(GAMETYPE::SOLO_QUEEN_JACK);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::DOUBLESOLO))
        && (solovalue - this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);
      res.game_type=GAMETYPE::SOLO_QUEEN_JACK;
    } 
    if (solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        best_duty_solo_res.game_type=GAMETYPE::SOLO_QUEEN_JACK;
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);
      }

    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_QUEEN_JACK, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_QUEEN_JACK))

  if ( party.rule()(Rule::SOLO_KING_JACK)) {
    solovalue=checksolo(GAMETYPE::SOLO_KING_JACK);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::DOUBLESOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solomax))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);
      res.game_type=GAMETYPE::SOLO_KING_JACK;
    } 
    if ( (solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solo_best_duty))
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);
        best_duty_solo_res.game_type=GAMETYPE::SOLO_KING_JACK;
      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_KING_JACK, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_KING_JACK))

  if ( party.rule()(Rule::SOLO_KING_QUEEN)) {
    solovalue=checksolo(GAMETYPE::SOLO_KING_QUEEN);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::DOUBLESOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);
      res.game_type=GAMETYPE::SOLO_KING_QUEEN;
    }
    if ( (solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO)>=solo_best_duty))
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::DOUBLESOLO);

        best_duty_solo_res.game_type=GAMETYPE::SOLO_KING_QUEEN;
      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_KING_QUEEN, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_KING_QUEEN))

  if ( party.rule()(Rule::SOLO_QUEEN)) {
    solovalue=checksolo(GAMETYPE::SOLO_QUEEN);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::SINGLESOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);
      res.game_type=GAMETYPE::SOLO_QUEEN;
    } 
    if ( solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        best_duty_solo_res.game_type=GAMETYPE::SOLO_QUEEN;
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);

      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_QUEEN, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_QUEEN))

  if ( party.rule()(Rule::SOLO_KING)) {
    solovalue=checksolo(GAMETYPE::SOLO_KING);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::SINGLESOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);
      res.game_type=GAMETYPE::SOLO_KING;
    } 
    if (solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        best_duty_solo_res.game_type=GAMETYPE::SOLO_KING;
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);

      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_KING, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_KING))

  if ( party.rule()(Rule::SOLO_JACK)) {
    solovalue=checksolo(GAMETYPE::SOLO_JACK);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::SINGLESOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);
      res.game_type=GAMETYPE::SOLO_JACK;
    } 
    if (solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_picture_soli()
          || this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::SINGLESOLO);

        best_duty_solo_res.game_type=GAMETYPE::SOLO_JACK;
      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_JACK, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_JACK))

  if ( party.rule()(Rule::SOLO_MEATLESS)) {
    solovalue=checksolo(GAMETYPE::SOLO_MEATLESS);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::MEATLESS))
        && (solovalue-this->Aiconfig::value(Aiconfig::MEATLESS)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::MEATLESS);
      res.game_type=GAMETYPE::SOLO_MEATLESS;
    }
    if ( solovalue-this->Aiconfig::value(Aiconfig::MEATLESS)>=solo_best_duty)
    {
      if (this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::MEATLESS);

        best_duty_solo_res.game_type=GAMETYPE::SOLO_MEATLESS;
      }
    }
    DEBUG_RESERVATION(GAMETYPE::SOLO_MEATLESS, solovalue, solomax);
  } // if ( party.rule()(Rule::SOLO_MEATLESS))

  if (  party.rule()(Rule::SOLO_COLOR)) {
    solovalue=checksolo(GAMETYPE::SOLO_DIAMOND);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;
    if ((solovalue > this->Aiconfig::value(Aiconfig::COLORSOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO))>=solomax )
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
      res.game_type=GAMETYPE::SOLO_DIAMOND;
      // playing silent solo
      if (hand().numberofclubqueens()==2) 
      {
        res.game_type=GAMETYPE::NORMAL;
        silentMarriage_ = true;
      }
    } 
    if ( solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_color_soli()
          || this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);	
        best_duty_solo_res.game_type=GAMETYPE::SOLO_DIAMOND;
      }
    }	
    DEBUG_RESERVATION(GAMETYPE::SOLO_DIAMOND, solovalue, solomax);

    solovalue=checksolo(GAMETYPE::SOLO_SPADE);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::COLORSOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
      res.game_type=GAMETYPE::SOLO_SPADE;
    }
    if ( solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_color_soli()
          || this->remaining_duty_free_soli())
      {
        best_duty_solo_res.game_type=GAMETYPE::SOLO_SPADE;
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);

      }
    }	
    DEBUG_RESERVATION(GAMETYPE::SOLO_SPADE, solovalue, solomax);

    solovalue=checksolo(GAMETYPE::SOLO_CLUB);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if (solovalue > this->Aiconfig::value(Aiconfig::COLORSOLO)
        && (solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO))>=solomax)
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
      res.game_type=GAMETYPE::SOLO_CLUB;
    } 
    if ( solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_color_soli()
          || this->remaining_duty_free_soli())
      {
        best_duty_solo_res.game_type=GAMETYPE::SOLO_CLUB;
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
      }

    }	
    DEBUG_RESERVATION(GAMETYPE::SOLO_CLUB, solovalue, solomax);

    solovalue=checksolo(GAMETYPE::SOLO_HEART);
    //CRIT: "keine normierung"
    solovalue += duty_soli_mod;

    if ((solovalue > this->Aiconfig::value(Aiconfig::COLORSOLO))
        && (solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solomax ))
    {
      solomax=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
      res.game_type=GAMETYPE::SOLO_HEART;
    } 
    if ( solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO)>=solo_best_duty)
    {
      if (this->remaining_duty_color_soli()
          || this->remaining_duty_free_soli())
      {
        solo_best_duty=solovalue-this->Aiconfig::value(Aiconfig::COLORSOLO);
        best_duty_solo_res.game_type=GAMETYPE::SOLO_HEART;
      }
    }	
    DEBUG_RESERVATION(GAMETYPE::SOLO_HEART, solovalue, solomax);
  } // if (  party.rule()(Rule::SOLO_COLOR))

  if( isDuty ) {
    DEBUG_ASSERTION(GAMETYPE::is_real_solo(best_duty_solo_res.game_type),
                    "Ai::reservation_get(duty solo)\n"
                    "  best duty solo reservation is no solo:\n"
                    << best_duty_solo_res);
    return (this->reservation() = best_duty_solo_res);
  }

#ifndef OLD
  if (res.game_type == GAMETYPE::MARRIAGE) {
    res.marriage_selector=get_Marriage();
#ifndef POSTPONED
    // ToDo: to be removed
    if (!this->game().rule()(res.marriage_selector))
      res.marriage_selector = this->reservation_get_default().marriage_selector;
#endif
  }
#endif

  // special case: with swines announced the player does not have a poverty
  if (   (res.game_type == GAMETYPE::POVERTY)
      && (this->hand().numberofpovertycards()
          > this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)) )
    res.swines = false;

  return (this->reservation() = res);
} // Reservation const& Ai::reservation_get( bool const isDuty ) 

/**
 ** -> result
 **
 ** @param       player whose teaminfo is returned
 **
 ** @return      the team of the given player, as far as this player knows it
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.1
 **/
Team const&
Ai::teaminfo(Player const& player) const
{
  return this->team_information().team(player);
} // Team const& Ai::teaminfo(Player const& player) const

/**
 ** updates the information of the teams (uses the current trick)
 **
 ** @param      -
 **
 ** @return     the known teams
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.4.4
 **/
void
Ai::teaminfo_update()
{
  this->team_information().update_teams();
  return ;

#ifdef OUTDATED
  // 0.7.4
  using namespace TEAM;
  { // sure information
    // update with the teaminfo of 'game'
    for(vector<Player*>::const_iterator player = this->game().players_begin();
        player != this->game().players_end();
        player++) {
      if ((this->game().teaminfo(**player) == RE)
          || (this->game().teaminfo(**player) == CONTRA))
        this->teaminfo_p[(*player)->no()] = this->game().teaminfo(**player);
    }

    // set the team of himself
    if ( (this->game().type() == GAMETYPE::NORMAL)
        && (this->teaminfo(*this) == UNKNOWN) ) {
      // look, whether there is a club queen in the hand
      this->teaminfo_p[this->game().no(*this)]
        = ((this->hand().numberofclubqueens() > 0) ? RE : CONTRA);
    }

    { // count the re and contra
      unsigned re_no = 0; // number of re
      unsigned contra_no = 0; // number of re

      for(vector<Player*>::const_iterator player = this->game().players_begin();
          player != this->game().players_end();
          player++) {
        if (this->teaminfo(**player) == RE)
          re_no += 1;
        else if (this->teaminfo(**player) == CONTRA)
          contra_no += 1;
      } // for (player)

      if (re_no + contra_no
          < this->game().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)) {
        // there are no more the two players in the re team
        if (re_no == this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM))
          for(vector<Player*>::const_iterator player = this->game().players_begin();
              player != this->game().players_end();
              player++)
            if (this->teaminfo(**player) != RE) {
              this->teaminfo_p[(*player)->no()] = CONTRA;
            }

        // one player must be re
        if ((contra_no == this->game().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) - 1)
            || ((contra_no
                 == this->game().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) - 2)
                && (this->team() == RE))) {
          // if there are 3 contra, the remaining player must be re
          // if I am re and there are two contra, the remaining player must be re
          for(vector<Player*>::const_iterator player = this->game().players_begin();
              player != this->game().players_end();
              player++)
            if (this->teaminfo(**player) != CONTRA) {
              this->teaminfo_p[(*player)->no()] = RE;
            }
        } // if (contra max)
      } // if (not all teams known)
    } // count the re and contra
  } // sure information

  if( silentMarriage_ )
    for (unsigned i = 0; i < 4; i++)   
      if( i != this->game().no(*this) )
        this->teaminfo_p[ i ] = CONTRA;

  if (::game_status == GAMESTATUS::GAME_RESERVATION) {
    if (this->game().type() == GAMETYPE::MARRIAGE) {
      for (unsigned p = 0; p < this->game().playerno(); p++)
        if (this->teaminfo_p[p] == TEAM::UNKNOWN)
          this->teaminfo_p[p] = TEAM::MAYBE_CONTRA;
    } // if (this->game().type() == GAMETYPE::MARRIAGE)
  } // if (::game_status == GAMESTATUS::RESERVATION)

  if (::game_status >= GAMESTATUS::GAME_PLAY) {

    // without this return in virtual games a player can be set to 'maybe re'
    // when he 'pfunds' the marriage player.
    if (this->game().is_undetermined_marriage())
      return ;

    // set some maybe-teaminfo
    Trick const& trick = this->game().trick_current();
    if (trick.isfull()) {
      bool anyKnown = false;
      for (unsigned i = 0; i < trick.actcardno(); i++) {
        // the team is already set
        if ( (this->game().teaminfo(trick.player_of_card(i)) == RE) ||
            (this->game().teaminfo(trick.player_of_card(i)) == CONTRA) )
          anyKnown = true;
      }

      if (!anyKnown) return;
      for (unsigned i = 0; i < trick.actcardno(); i++) {

        // jab a club queen
        if ( (trick.card(i) == Card::CLUB_QUEEN)
            && (trick.winnerplayer() != trick.player_of_card(i))
            && (this->teaminfo(trick.winnerplayer()) != RE)
            && (this->teaminfo(trick.winnerplayer()) != CONTRA)
           )
        {
          this->teaminfo_p[trick.winnerplayer().no()] = team_to_maybe(CONTRA);
        }

        // the team is already set
        if ( (this->teaminfo(trick.player_of_card(i)) == RE) ||
            (this->teaminfo(trick.player_of_card(i)) == CONTRA) )
          continue;

        // no jab in (color) trick (and there are trumps remaining)
        if ( (trick.card(i).tcolor() != trick.startcard().tcolor())
            && (!trick.card(i).istrump())
            && (trick.winnerplayer() != trick.player_of_card(i))
            && (this->game().trumps_no()
                > (this->game().trumps_played_no()
                   + this->hand().numberoftrumps())
               )
           )
        {

          this->teaminfo_p[trick.player_of_card(i).no()]
            = team_to_maybe(this->teaminfo(trick.winnerplayer()));
        }
        // pfund in a trick
        if ( (trick.card(i).tcolor() == trick.startcard().tcolor())
            &&(trick.winnerplayer() != trick.player_of_card(i))
            && (trick.card(i).value() >= 10)
            && this->value(Aiconfig::TRUSTING)
           ) {
          this->teaminfo_p[trick.player_of_card(i).no()]
            = team_to_maybe(this->teaminfo(trick.winnerplayer()));
        }
      } // for (i < trick.actcard())
    } // if (trick.isfull())
    else
    {
      for (unsigned i = 0; i < trick.actcardno(); i++) {
        // the team is already set


        // jab a club queen
        if ( (trick.card(i) == Card::CLUB_QUEEN)
            && (trick.winnerplayer() != trick.player_of_card(i))
            && 	(this->teaminfo(trick.winnerplayer()) != RE)
            && 	(this->teaminfo(trick.winnerplayer()) != CONTRA)
           )
        {
          this->teaminfo_p[trick.winnerplayer().no()] = team_to_maybe(CONTRA);
        }
      }

      if ( !trick.isstartcard()
          && (this->game().type() == GAMETYPE::MARRIAGE) ) {
        switch (this->game().marriage_selector()) {
        case MARRIAGE_SELECTOR::SILENT:
        case MARRIAGE_SELECTOR::TEAM_SET:
          // the marriage is determined
          break;
        case MARRIAGE_SELECTOR::FIRST_COLOR:
        case MARRIAGE_SELECTOR::FIRST_FOREIGN:
          if (!trick.card(0).istrump()
              && (trick.card(0).value() == Card::ACE)
              && (this->color_runs( trick.card( 0 ).color() ) == 0) )
          {//2do jabbing for colors
            this->teaminfo_p[trick.player_of_card(0).no()] = TEAM::MAYBE_RE;
          }
          if (this->game().marriage_selector()
              == MARRIAGE_SELECTOR::FIRST_COLOR)
            break;
        case MARRIAGE_SELECTOR::FIRST_TRUMP:
          // ToDo
          break;
        case MARRIAGE_SELECTOR::FIRST_CLUB:
          if ( (trick.card(0) == Card(Card::CLUB, Card::ACE))
              && (this->color_runs( trick.card( 0 ).color() ) == 0) )
          {//2do jabbing for colors
            this->teaminfo_p[trick.player_of_card(0).no()] = TEAM::MAYBE_RE;
          }
          break;
        case MARRIAGE_SELECTOR::FIRST_SPADE:
          if ( (trick.card(0) == Card(Card::SPADE, Card::ACE))
              && (this->color_runs( trick.card( 0 ).color() ) == 0) )
          {//2do jabbing for colors
            this->teaminfo_p[trick.player_of_card(0).no()] = TEAM::MAYBE_RE;
          }
          break;
        case MARRIAGE_SELECTOR::FIRST_HEART:
          if ( (trick.card(0) == Card(Card::HEART, Card::ACE))
              && (this->color_runs( trick.card( 0 ).color() ) == 0) )
          {//2do jabbing for colors
            this->teaminfo_p[trick.player_of_card(0).no()] = TEAM::MAYBE_RE;
          }
          break;
        } // switch (this->game().marriage_selector())
      } // if !(marriage && !trick.isstartcard())
    } // if !(trick.isfull())

  } // if (::game_status >= GAMESTATUS::GAME_PLAY)

  return ;
#endif // #ifndef OUTDATED
} // void Ai::teaminfo_update()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the partner of the player, NULL, if not known for sure
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Player const*
Ai::partner(Player const& player) const
{
  if (::game_status < GAMESTATUS::GAME_PLAY)
    return NULL;

  if (!this->team_information().all_known())
    return NULL;

  Player const* partner = NULL;

  for (unsigned p = 0; p < this->game().playerno(); ++p) {
    if (   (p != this->game().no(player))
        && (this->teaminfo(this->game().player(p))
            == this->teaminfo(player))) {
      if (partner != NULL)
        return NULL; // more than one partner
      partner = &this->game().player(p);
    }
  } // for (p)

  return partner;
} // Player const* Ai::partner(Player player) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the points the team has made
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
unsigned
Ai::points_of_team() const
{
  unsigned points = this->game().points_of_player(*this);


  if (this->team_information().all_known()) {
    for (unsigned p = 0; p < this->game().playerno(); ++p)
      if (   (p != this->game().no(*this))
          && (this->team_information().team(p) == this->team()))
        points += this->game().points_of_player(p);
  } else if (this->team_information().guessed_partner() != NULL) {
    // normal game, partner guessed
    points += this->game().points_of_player(*this->team_information().guessed_partner());
  } else { // if (no partner known)
    // add the lowest points of the players
    unsigned lowest_points = 240;
    for (unsigned p = 0; p < this->game().playerno(); ++p)
      if (   (p != this->game().no(*this))
          && (this->game().points_of_player(p) < lowest_points))
        lowest_points = this->game().points_of_player(p);
    points += lowest_points;
  } // if (no partner known)

  return points;
} // unsigned Ai::points_of_team() const

/**
 ** -> result
 ** (uses Heuristics)
 **
 ** @see        Heuristics::make_announcement()
 ** @see        Heuristics::say_no90
 ** @see        Heuristics::say_no60
 ** @see        Heuristics::say_no30
 ** @see        Heuristics::say_no0
 **
 ** @param      -
 **
 ** @return     announcement of the player
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.7.6
 **/
Announcement
Ai::announcement_request() const
{
  if (   ::bug_report_replay
      && ::bug_report_replay->auto_action()) {
    if (   (::bug_report_replay->current_action().type
            == GameplayAction::ANNOUNCEMENT)
        && (static_cast<GameplayAction::Announcement const&>(::bug_report_replay->current_action()).player
            == this->game().no(*this))
       ) {
      return static_cast<GameplayAction::Announcement const&>(::bug_report_replay->current_action()).announcement;
    } // if (current action = ANNOUNCEMENT)
    return ANNOUNCEMENT::NOANNOUNCEMENT;
  } // if (bug report auto execute)


  Announcement res = ANNOUNCEMENT::NOANNOUNCEMENT;

  if (Heuristics::make_announcement(*this))
    res = ANNOUNCEMENT::NO120;

  for (Announcement a
       = std::max(ANNOUNCEMENT::NO90,
                  static_cast<Announcement>(this->announcement() + 1));
       a <= ANNOUNCEMENT::NO0;
       a = static_cast<Announcement>(a+1)) {
    // make the announcement as late as possible
    if (   this->game().announcement_valid(a, *this)
        && (this->game().rule().remaining_cards(a)
            == this->hand().cardsnumber())
        && (&this->game().player_current() == this)
       ) {
      switch(a) {
      case ANNOUNCEMENT::NOANNOUNCEMENT:
      case ANNOUNCEMENT::REPLY:
      case ANNOUNCEMENT::NO120_REPLY:
      case ANNOUNCEMENT::NO90_REPLY:
      case ANNOUNCEMENT::NO60_REPLY:
      case ANNOUNCEMENT::NO30_REPLY:
      case ANNOUNCEMENT::NO0_REPLY:
      case ANNOUNCEMENT::NO120:
        DEBUG_ASSERTION(false,
                        "Ai::announcement_request():\n"
                        "  wrong announcement in 'switch': "
                        << a);
        break;
      case ANNOUNCEMENT::NO90:
        if (Heuristics::say_no90(*this))
          res = a;
        break;
      case ANNOUNCEMENT::NO60:
        if (Heuristics::say_no60(*this))
          res = a;
        break;
      case ANNOUNCEMENT::NO30:
        if (Heuristics::say_no30(*this))
          res = a;
        break;
      case ANNOUNCEMENT::NO0:
        if (Heuristics::say_no0(*this))
          res = a;
        break;
      } // switch(a)
    } // if (last chance for announcement)
  } 
  // for (a \in announcements)

  if ( ( res == ANNOUNCEMENT::NOANNOUNCEMENT )
      && this->game().trick_current().isfull()
      && this->game().announcement_valid(ANNOUNCEMENT::REPLY, *this) )
    res = Heuristics::make_reply(*this);

  return res;
} // Announcement Ai::announcement_request() const

/**
 **
 ** the announcement 'announcement' has been made by player 'player'
 **
 ** @param	announcement	the announcement
 ** @param	player		the player, who has made the announcement
 **
 ** @return	-
 **
 ** @version	0.6.9
 **
 ** @author	Diether Knof
 **
 **/
void
Ai::announcement_made(Announcement const& announcement,
                      Player const& player)
{
  this->team_information().announcement_made(announcement, player);
  this->cards_information().announcement_made(announcement, player);
  this->team_information().update_teams();

  this->teaminfo_update();

  return ;
} // void Ai::announcement_made(Announcement announcement, Player player)

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo(GameType const gametype) const
{
  switch (gametype) {
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    return this->checksolo_single_picture(gametype);
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    return this->checksolo_double_picture(gametype);
  case GAMETYPE::SOLO_KOEHLER:
    return this->checksolo_koehler(gametype);
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
    return this->checksolo_color(gametype);
  case GAMETYPE::SOLO_MEATLESS:
    return this->checksolo_meatless(gametype);
  default:
    DEBUG_ASSERTION(false,
                    "Ai::checksolo(" << gametype << ")\n"
                    "The gametype is no solo\n");
  } // switch (gametype)
  return 0;
} // int Ai::checksolo(GameType const gametype) const

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type (single picture)
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo_single_picture(GameType const gametype) const
{
  DEBUG_ASSERTION(   (gametype == GAMETYPE::SOLO_JACK)
                  || (gametype == GAMETYPE::SOLO_QUEEN)
                  || (gametype == GAMETYPE::SOLO_KING),
                  "Ai::checksolo_single_picture(" << gametype << ")\n"
                  "Gametype does not match, must be either solo jack, solo queen or soo king\n");
  auto const trump = (  (gametype == GAMETYPE::SOLO_JACK)  ? Card::JACK
                      : (gametype == GAMETYPE::SOLO_QUEEN) ? Card::QUEEN
                      : (gametype == GAMETYPE::SOLO_KING)  ? Card::KING
                      : Card::NOCARDVALUE);
  auto const& hand = this->hand();
  int res = 0;
  int high_trump = 0;

  for (auto const& card : hand.cards()) {
    if (card.value() == trump) {
      switch (card.color()) {
      case Card::CLUB:
        res += 4;	 
        high_trump += 1;
        break;
      case Card::SPADE:
        res += 3;	 
        high_trump += 1;
        break;
      case Card::HEART:
        res += 2;	 
        break;
      case Card::DIAMOND:
        res += 1;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == Card::ACE) {
      res += 1;
    } else {
      res -= 1;
    }
  } // for (card : hand.cards())

  if ( high_trump >= 3 )
    res += 4;

  if (hand.numberoftrumps(gametype) > 4)
    res += 9 + hand.numberoftrumps(gametype) - hand.cardsnumber();

  res += hand.numberoffehlcolors(gametype) * 2;
  res -= hand.numberofsingletens(gametype) * 4;

  // color without aces
  for (auto const& c : this->game().rule().card_colors()) {
    if (   hand.numberof(c, gametype) > 0
        && hand.numberof(c, Card::ACE) == 0 ) {
      res -= 7;
    }
  }
  return res;
} // int Ai::checksolo_single_picture(GameType gametype) const

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type (double picture)
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo_double_picture(GameType const gametype) const
{
  DEBUG_ASSERTION(   (gametype == GAMETYPE::SOLO_QUEEN_JACK)
                  || (gametype == GAMETYPE::SOLO_KING_JACK)
                  || (gametype == GAMETYPE::SOLO_KING_QUEEN),
                  "Ai::checksolo_double_picture(" << gametype << ")\n"
                  "Gametype does not match, must be a double picture solo.\n");
  auto const trump1 = (  (gametype == GAMETYPE::SOLO_QUEEN_JACK) ? Card::QUEEN
                       : (gametype == GAMETYPE::SOLO_KING_JACK)  ? Card::QUEEN
                       : (gametype == GAMETYPE::SOLO_KING_QUEEN) ? Card::KING
                       : Card::NOCARDVALUE);
  auto const trump2 = (  (gametype == GAMETYPE::SOLO_QUEEN_JACK) ? Card::JACK
                       : (gametype == GAMETYPE::SOLO_KING_JACK)  ? Card::JACK
                       : (gametype == GAMETYPE::SOLO_KING_QUEEN) ? Card::QUEEN
                       : Card::NOCARDVALUE);

  auto const& hand = this->hand();
  int res = 0;

  for (auto const& card : hand.cards()) {
    if (card.value() == trump1) {
      switch (card.color()) {
      case Card::CLUB:
        res += 8;	 
        break;
      case Card::SPADE:
        res += 7;	 
        break;
      case Card::HEART:
        res += 6;	 
        break;
      case Card::DIAMOND:
        res += 5;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == trump2) {
      switch (card.color()) {
      case Card::CLUB:
        res += 4;	 
        break;
      case Card::SPADE:
        res += 3;	 
        break;
      case Card::HEART:
        res += 2;	 
        break;
      case Card::DIAMOND:
        res += 1;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == Card::ACE) {
      res += 1;
    } else {
      res -= 2;
    }
  } // for (card : hand.cards())

  res += hand.numberoffehlcolors(gametype) * 6;
  res -= hand.numberofsingletens(gametype) * 8;

  // color without aces
  for (auto const& c : this->game().rule().card_colors()) {
    if (   hand.numberof(c, gametype) > 0
        && hand.numberof(c, Card::ACE) == 0 ) {
      res -= 7;
    }
  }

  return res;
} // int Ai::checksolo_double_picture(GameType gametype) const

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type (koehler)
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo_koehler(GameType const gametype) const
{
  DEBUG_ASSERTION(gametype == GAMETYPE::SOLO_KOEHLER,
                  "Ai::checksolo_koehler(" << gametype << ")\n"
                  "Gametype does not match, must be a koehler solo.\n");
  auto const& hand = this->hand();
  int res = 0;

  for (auto const& card : hand.cards()) {
    if (card.value() == Card::KING) {
      switch (card.color()) {
      case Card::CLUB:
        res += 12;	 
        break;
      case Card::SPADE:
        res += 11;	 
        break;
      case Card::HEART:
        res += 10;	 
        break;
      case Card::DIAMOND:
        res += 9;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == Card::QUEEN) {
      switch (card.color()) {
      case Card::CLUB:
        res += 8;	 
        break;
      case Card::SPADE:
        res += 7;	 
        break;
      case Card::HEART:
        res += 6;	 
        break;
      case Card::DIAMOND:
        res += 5;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == Card::JACK) {
      switch (card.color()) {
      case Card::CLUB:
        res += 4;	 
        break;
      case Card::SPADE:
        res += 3;	 
        break;
      case Card::HEART:
        res += 2;	 
        break;
      case Card::DIAMOND:
        res += 1;	 
        break;
      default:
        break;
      } // switch (card.color())
    } else if (card.value() == Card::ACE) {
      res += 1;
    } else {
      res -= 3;
    }
  } // for (card : hand.cards())

  res += hand.numberoffehlcolors(gametype) * 10;
  res -= hand.numberofsingletens(gametype) * 12;

  // color without aces
  for (auto const& c : this->game().rule().card_colors()) {
    if (   hand.numberof(c, gametype) > 0
        && hand.numberof(c, Card::ACE) == 0 ) {
      res -= 5;
    }
  }

  return res;
} // int Ai::checksolo_koehler(GameType gametype) const

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type (color solo)
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo_color(GameType const gametype) const
{
  DEBUG_ASSERTION(   (gametype == GAMETYPE::SOLO_CLUB)
                  || (gametype == GAMETYPE::SOLO_SPADE)
                  || (gametype == GAMETYPE::SOLO_HEART)
                  || (gametype == GAMETYPE::SOLO_DIAMOND),
                  "Ai::checksolo_color(" << gametype << ")\n"
                  "Gametype does not match, must be a color solo.\n");
  auto const trump = (  (gametype == GAMETYPE::SOLO_CLUB)    ? Card::CLUB
                      : (gametype == GAMETYPE::SOLO_SPADE)   ? Card::SPADE
                      : (gametype == GAMETYPE::SOLO_HEART)   ? Card::HEART
                      : (gametype == GAMETYPE::SOLO_DIAMOND) ? Card::DIAMOND
                      : Card::NOCARDCOLOR);
  auto const& hand = this->hand();
  auto const& rule = this->game().rule();
  int res = 0;

  if (   rule(Rule::SWINES_IN_SOLO)
      && (hand.numberof({trump, Card::ACE}) == 2) ) 
    res += 9;

  if (   (trump == Card::HEART)
    && rule(Rule::DOLLEN) )
    res += 2;

  for (auto const& card : hand.cards()) {
    if (card.isdolle()) {
      res += 4;
      if (!rule(Rule::SWINES_IN_SOLO))
        res += 2;
    } else if (card.value() == Card::QUEEN) {
        res += 3;
    } else if (card.value() == Card::JACK) {
        res += 2;
    } else if (card.istrump()) {
    } else if (card.value() == Card::ACE) {
      if (   rule(Rule::DOLLEN)
          && (card.color() == Card::HEART) ) {
        if (hand.numberof(card.color()) < 2)
          res += 1;
      } else {
        if (hand.numberof(card.color()) < 3)
          res += 1;
      }
    } else {
      res -= 1;
    }
  } // for (card : hand.cards())

  res += hand.numberoffehlcolors(gametype) * 2;

  res -= hand.numberofsingletens(gametype) * 4;

  if (hand.numberoffehlcolors(gametype) == 0)
    res += 5;
  else if (hand.numberoffehlcolors(gametype) == 1)
    res += 4;
  else if (hand.numberoffehlcolors(gametype) == 2)
    res += 2;

  // check for fehl color in a normal game
  if (   (trump != Card::DIAMOND)
      && (hand.numberof(trump) == 0))
    res -= 3;

  if (hand.numberofdolle() == 0)
    res -= 1;
  if (hand.numberof(Card::CLUB_QUEEN) == 0)
    res -= 1;
  if (hand.numberof(Card::SPADE_QUEEN) == 0)
    res -= 1;

  return res;
} // int Ai::checksolo_color(GameType gametype) const

/**
 ** returns a value of the hand for the game
 **
 ** @param    gametype   game type (meatless)
 **
 ** @return   value for the game decision
 **/
int
Ai::checksolo_meatless(GameType const gametype) const
{
  DEBUG_ASSERTION(gametype == GAMETYPE::SOLO_MEATLESS,
                  "Ai::checksolo_meatless(" << gametype << ")\n"
                  "Gametype does not match, must be meatless.\n");
  int res=0;
  auto const& hand = this->hand();
  auto const& rule = this->game().rule();

  for (auto const c : rule.card_colors()) {
    res += (hand.numberof(c) * hand.numberof(Card(c, Card::ACE)) +
            hand.numberof(c) * hand.numberof(Card(c, Card::TEN)));
  }

  res -= hand.numberofsingletens(GAMETYPE::SOLO_MEATLESS) * 5;

  for (auto const c : rule.card_colors()) {
      res -= 2 * (2 - hand.numberof(c, Card::ACE) );
  }

  return res;
} // int Ai::checksolo_meatless(GameType gametype) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the player has a single ace of a color
 **            and else only tens of the color
 **/
bool
Ai::single_ace() const
{
  for (auto const c : this->game().rule().card_colors()) {
    if (c == this->game().trumpcolor())
      continue;

    if (   (this->hand().numberof(c, Card::ACE) == 1)
        && (this->hand().numberof(c, Card::ACE) 
            + this->hand().numberof(c, Card::TEN)
            == this-> hand().numberof(c)))
      return true;
  } // for (c : rule->card_colors)

  return false;
} // bool Ai::single_ace() const


/**
 ** -> result
 **
 ** @param      -
 **
 ** @return     modificator for the end depth for calculating tricks
 **             in WVirtualGames
 **
 ** @version    0.4.4
 **
 ** @author     Borg Enders
 **
 **/
unsigned const&
Ai::last_trick_to_calculate() const
{
  return this->last_trick_to_calculate_p;
} // unsigned const& Ai::last_trick_to_calculate() const

/**
 ** sets the parameter to the last trick to calculate
 **
 ** @param      last_trick_to_calculate   new value
 **
 ** @return     modificator for the end depth for calculating tricks
 **             in WVirtualGames
 **
 ** @version    0.4.4
 **
 ** @author     Borg Enders
 **
 **/
void
Ai::set_last_trick_to_calculate(unsigned const last_trick_to_calculate)
{
  this->last_trick_to_calculate_p = last_trick_to_calculate;

  return ;
} // void Ai::set_last_trick_to_calculate(unsigned last_trick_to_calculate)

/**
 ** returns which cards the player shifts
 **
 ** @param      -
 **
 ** @return     the cards that are to be shifted
 **
 ** @author     Borg Enders
 **
 ** @version    0.4.5
 **/
HandCards
Ai::poverty_shift()
{ 
  DEBUG_ASSERTION(this->hand().numberoftrumps() <= 5,
                  "Ai::poverty_shift()\n"
                  "  more then 5 trumps:\n" << this->hand());

  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()
        && (::bug_report_replay->current_action().type
            == GameplayAction::POVERTY_SHIFT)
        && (static_cast<GameplayAction::PovertyShift const&>(::bug_report_replay->current_action()).player == this->game().no(*this))
       ) {
      HandCards const cards(this->hand(),
                            static_cast<GameplayAction::PovertyShift const&>(::bug_report_replay->current_action()).cards);
      this->sorted_hand().remove(cards);
      return cards;
    } // if (auto execute)
  } // bug report replay

  SortedHand& sh = this->sorted_hand();
  Hand const& h = this->hand();

  HandCards res;

  // number of remaining cards to shift
  unsigned rem = this->hand().numberofpovertycards();

  for( unsigned int i = 0; i < h.cardsnumber(); i++ )
    if ( h.card( i ).istrump())
      res.push_back( h.card( i ) );

  sh.remove( res );

  DEBUG_ASSERTION((res.size() <= rem),
                  "Ai::poverty_shift()\n"
                  "  too many trumps: " << res.size() << " > " << rem << '\n'
                  << "  trumps found:\n"
                  << res);
  rem -= res.size();

  if ( !this->game().rule()( Rule::POVERTY_SHIFT_ONLY_TRUMP ) && rem > 0  )
  {
    HandCards addon;

    Card::Color c = Card::HEART ;
    unsigned int size = h.numberof( c );

    if ( h.numberof( Card::SPADE ) < size )
    {
      c = Card::SPADE;
      size = h.numberof( Card::SPADE );      
    }

    if ( h.numberof( Card::CLUB ) < size )
    {
      c = Card::CLUB;
      size = h.numberof( Card::CLUB );      
    }

    unsigned int i = 0;
    while ( rem > 0 )
    {
      if ( i >= h.cardsnumber() )
        i = 0;
      if ( size > 0 )
      {
        if ( h.card( i ).color() == c )
        {
          addon.push_back( h.card( i ) );
          sh.remove( h.card( i ) );
          size--;
          rem--;
        }
      } else
      {
        addon.push_back( h.card( i ) );
        sh.remove( h.card( i ) );
        rem--;
      }
      i += 1;
    }

    res.insert(res.end(), addon.begin(), addon.end());
  }// !this->game().rule()( Rule::POVERTY_SHIFT_ONLY_TRUMP ) && rem > 0

  return res;
} // HandCards Ai::poverty_shift(Player const& player)

/**
 **
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param      cardno  the number of shifted cards
 **
 ** @return     whether to accept the cards
 **
 ** @version    0.4.5
 **
 ** @author     Borg Enders
 **
 **/
bool
Ai::poverty_take_accept(unsigned const cardno)
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()) {
      if (   (::bug_report_replay->current_action().type
              == GameplayAction::POVERTY_ACCEPTED)
          && (static_cast<GameplayAction::PovertyAccepted const&>(::bug_report_replay->current_action()).player == this->game().no(*this)) )
        return true;
      if (   (::bug_report_replay->current_action().type
              == GameplayAction::POVERTY_DENIED)
          && (static_cast<GameplayAction::PovertyDenied const&>(::bug_report_replay->current_action()).player == this->game().no(*this)) )
        return false;
    } // if (auto execute)
  } // bug report replay

  unsigned trumpno_possible = cardno;
  if (!this->game().rule()(Rule::POVERTY_SHIFT_ONLY_TRUMP)) {
    if (!this->game().rule()(Rule::THROW_WITH_ONE_TRUMP))
      trumpno_possible = 2;
    else 
      trumpno_possible = 1;
  }

  int value = Heuristics::CalcHandValue( *this );

  if( this->hand().numberoftrumps() >= 12 - cardno )
    value += this->hand().numberof( Card::QUEEN );

  std::multimap<unsigned, Card::Color> color_nums;
  for (vector<Card::Color>::const_iterator
       c = this->game().rule().card_colors().begin();
       c != this->game().rule().card_colors().end();
       ++c) {
    color_nums.insert(std::make_pair(this->hand().numberof(*c), *c));
  }

  for (std::multimap<unsigned, Card::Color>::const_iterator
       c = color_nums.begin(); 
       c != color_nums.end(); 
       ++c) {
    if (c->first > trumpno_possible)
      break;

    value += 2;
    trumpno_possible -= c->first;
  }
  if( this->hand().numberoftrumps() >= 12 - cardno - 1 )
    value += (this->hand().numberof( Card::QUEEN ) 
              + this->hand().numberoftrumpaces());


  if ( this->game().rule()(Rule::DOLLEN))
    value -= 4 - 2 * this->hand().numberof(Card::HEART, Card::TEN);

  value -= 4 - 2 * this->hand().numberofclubqueens();
  value -= 2 - this->hand().numberof(Card::SPADE, Card::QUEEN);


  //COUT << no() << "\t" << value << "\t" << static_cast<signed>(this->value(Aiconfig::TAKEPOVERTY)) << std::endl;

  return (value >= static_cast<signed>(this->value(Aiconfig::TAKEPOVERTY)));
} // void Ai::poverty_take_accept(unsigned const cardno)

/**
 **
 ** changes the cards from the poverty-player
 **
 ** @param      cards   the cards that are given to the player
 **
 ** @return     the cards that are returned to the poverty-player
 **
 ** @version    0.7.4
 **
 ** @author     Borg Enders
 **
 **/
HandCards
Ai::poverty_cards_change(HandCards const& cards)
{ 
  { // bug report replay
    if (   ::bug_report_replay
        && ::bug_report_replay->auto_action()) {
      if (::bug_report_replay->current_action().type
          == GameplayAction::POVERTY_RETURNED) {
        HandCards const reshifted_cards(this->hand(),
                                        static_cast<GameplayAction::PovertyReturned const&>(::bug_report_replay->current_action()).cards);
        this->sorted_hand().add(cards);
        this->sorted_hand().remove(reshifted_cards);
        return reshifted_cards;
      }
    } // if (auto execute)
  } // bug report replay

  //SortedHand& sh = this->sorted_hand();
  Hand& h = this->hand();

  //sh.add(cards);
  h.add(cards);


  HandCards res;
  unsigned rem = cards.size();

  // create Fehl for Heart
  if( h.numberof( Card::HEART ) > 0 && h.numberof( Card::HEART ) <= rem && this->game().rule()(Rule::DOLLEN))
  {
    HandCards addon;

    for( unsigned int i = 0; i < h.cardsnumber(); i++ )
      if( ( h.card( i ).color() == Card::HEART ) && !h.card(i).istrump() )
        addon.push_back( h.card( i ) );
    h.remove( addon );
    res.insert(res.end(), addon.begin(), addon.end());
    rem-= addon.size();
  }

  // create Fehl for Club
  if( h.numberof( Card::CLUB ) > 0 && h.numberof( Card::CLUB ) <= rem )
  {
    bool ace_removed = false;
    if( h.numberof( Card::CLUB_ACE ) == 2 )
    {
      h.remove (Card::CLUB_ACE); // keep second ace.
      ace_removed = true;
    }

    HandCards addon;
    for( unsigned int i = 0; i < h.cardsnumber(); i++ )
      if ( ( h.card( i ).color() == Card::CLUB ) && !h.card(i).istrump() )
        addon.push_back( h.card( i ) );
    h.remove( addon );
    res.insert(res.end(), addon.begin(), addon.end());
    rem-= addon.size();

    if (ace_removed)
      h.add( Card::CLUB_ACE );
  }


  // create Fehl for Spade
  if( h.numberof( Card::SPADE ) > 0 && h.numberof( Card::SPADE ) <= rem )
  {
    bool ace_removed = false;
    if( h.numberof( Card::SPADE_ACE ) == 2 )
    {
      h.remove (Card::SPADE_ACE); // keep second ace.
      ace_removed = true;
    }

    HandCards addon;
    for( unsigned int i = 0; i < h.cardsnumber(); i++ )
      if( ( h.card( i ).color() == Card::SPADE ) && !h.card(i).istrump() )
        addon.push_back( h.card( i ) );
    h.remove( addon );
    res.insert(res.end(), addon.begin(), addon.end());
    rem-= addon.size();

    if (ace_removed)
      h.add( Card::SPADE_ACE );
  }

  // create Fehl for Heart
  if( h.numberof( Card::HEART ) > 0 && h.numberof( Card::HEART ) <= rem )
  {

    bool ace_removed = false;
    if( h.numberof( Card::HEART_ACE ) == 2 )
    {
      h.remove (Card::HEART_ACE); // keep second ace.
      ace_removed = true;
    }
    HandCards addon;
    for( unsigned int i = 0; i < h.cardsnumber(); i++ )
      if( ( h.card( i ).color() == Card::HEART ) && !h.card(i).istrump() )
        addon.push_back( h.card( i ) );
    h.remove( addon );
    res.insert(res.end(), addon.begin(), addon.end());
    rem-= addon.size();

    if (ace_removed)
      h.add( Card::HEART_ACE );
  }


  while ( rem > 0 ) {
    //adding highest color card not ace
    if ( h.numberoftrumps() < h.cardsnumber()) {
      HandCard c;
      for( unsigned int i = 0; i < h.cardsnumber(); i++ )
        if ( !h.card( i ).istrump() )
          c = h.card( i );

      if( !c.is_empty() )
      {
        for( unsigned int i = 0; i < h.cardsnumber(); i++ )
          if ( !h.card( i ).istrump() && c.less(h.card( i )) )
            c = h.card( i );

        h.remove( c );
        res.push_back( c );
        rem--;      
        continue;
      }
    }

    { // adding color card
      if ( h.numberoftrumps() < h.cardsnumber()) {
        HandCard c;
        for( unsigned int i = 0; i < h.cardsnumber(); i++ )
          if ( !h.card( i ).istrump() )
            c = h.card( i );

        h.remove( c );
        res.push_back( c );
        rem--;      
        continue;
      }
    } // adding color card

    if (rem > 0 ) {
      // add low trump cards
      vector<Card> cards_to_check;
      cards_to_check.push_back(Card(Card::DIAMOND, Card::NINE));
      cards_to_check.push_back(Card(Card::DIAMOND, Card::KING));
      cards_to_check.push_back(Card(Card::DIAMOND, Card::JACK));
      cards_to_check.push_back(Card(Card::DIAMOND, Card::QUEEN));
      cards_to_check.push_back(Card(Card::DIAMOND, Card::TEN));
      cards_to_check.push_back(Card(Card::HEART,   Card::JACK));
      cards_to_check.push_back(Card(Card::SPADE,   Card::JACK));
      cards_to_check.push_back(Card(Card::CLUB,    Card::JACK));
      cards_to_check.push_back(Card(Card::DIAMOND, Card::ACE));  
      cards_to_check.push_back(Card(Card::HEART,   Card::QUEEN));
      cards_to_check.push_back(Card(Card::SPADE,   Card::QUEEN));
      cards_to_check.push_back(Card(Card::CLUB,    Card::QUEEN));

      HandCard c;

      for (vector<Card>::const_iterator card = cards_to_check.begin();
           rem > 0;
           ++card) {
        while ((h.numberof(*card) > 0) && (rem > 0)) {
          c = HandCard( h, *card );
          if (c.possible_swine() || c.possible_hyperswine() )
            break;
          h.remove( c );
          res.push_back( c );
          rem--;
        }
      } // for (card)
    } // add low trump cards

  } // if ( rem > 0 )

  this->cards_information().reset();
  this->set_hand(h); // to update the cards information

  return res;
} // HandCards Ai::poverty_cards_change(HandCards const& cards)


/**
 **
 ** gets the cards from the partner and add them to the hand
 **
 ** @param	cards	the cards that are given to the player
 **
 ** @return	-
 **
 ** @author	Borg Enders
 **
 ** @version	0.6.1
 **
 **/
void
Ai::poverty_cards_get_back(HandCards const& cards)
{ 
  this->sorted_hand().add(cards);
  this->cards_information().reset();
  this->set_hand( this->hand() ); // to update the cards information

  return ;
} // void Ai::poverty_cards_get_back(HandCards const& cards)


/**********************************************************************
 *
 **    Card trump_card_limit()
 *
 **    Parameters:  
 *
 **    Result: limit card for some heuristic decisions
 *
 **    Version: Beta
 *
 **    Description:
 **       Results: 
 **          Normal    : Heart Queen
 **          Color Solo: Heart Queen
 **          Jack Solo : Spade Jack
 **          Queen Solo: Spade Queen
 **          King Solo : Spade King
 **          Queen-Jack: Club Jack
 **          King-Jack : Club Jack
 **          King-Queen: Club Queen
 **          Köhler    : Club Queen
 **          Meatless  : Diamond Ten
 *
 **********************************************************************/

Card Ai::trump_card_limit() const
{
  GameType gt=party.game().type();


  if  ( (gt==GAMETYPE::SOLO_CLUB) ||
       (gt==GAMETYPE::SOLO_HEART) ||
       (gt==GAMETYPE::SOLO_SPADE) ||
       (gt==GAMETYPE::SOLO_DIAMOND) ||
       silentMarriage_ )
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOCOLOR);
  }

  if  (gt==GAMETYPE::SOLO_JACK) 
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOJACK);
  }

  if  (gt==GAMETYPE::SOLO_QUEEN) 
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOQUEEN);
  }

  if  (gt==GAMETYPE::SOLO_KING) 
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOKING);
  }

  if   (gt==GAMETYPE::SOLO_QUEEN_JACK)      
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOJACKQUEEN);
  }
  if      (gt==GAMETYPE::SOLO_KING_JACK)       
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOJACKKING);
  }

  if (gt==GAMETYPE::SOLO_KING_QUEEN)
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOQUEENKING);
  }
  if    (gt==GAMETYPE::SOLO_KOEHLER) 
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_SOLOKOEHLER);
  }

  if (gt==GAMETYPE::SOLO_MEATLESS)
  {
    return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_MEATLESS);
  }

  return this->Aiconfig::value(Aiconfig::TRUMPLIMIT_NORMAL);
}


/**********************************************************************
 *
 **    Card lowest_trump_card_limit()
 *
 **    Parameters:  
 *
 **    Result: limit card for some heuristic decisions
 *
 **    Version: Beta
 *
 **    Description:
 **      Results:
 **        Normal    : Diamond jack
 **        Color Solo: Diamond jack
 **        Jack Solo : Heart jack
 **        Queen Solo: heart Queen
 **        King Solo : heart king
 **        Queen-jack: Spade jack
 **        King-jack : Spade jack
 **        King-Queen: Spade Queen
 **        Koehler   : Spade Queen
 **        Meatless  : Diamond Queen
 *
 *
 **********************************************************************/

Card Ai::lowest_trump_card_limit() const
{
  GameType gt=party.game().type();


  if  ( (gt==GAMETYPE::SOLO_CLUB) ||
       (gt==GAMETYPE::SOLO_HEART) ||
       (gt==GAMETYPE::SOLO_SPADE) ||
       (gt==GAMETYPE::SOLO_DIAMOND) ||
       silentMarriage_ )
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOCOLOR);
  }

  if  (gt==GAMETYPE::SOLO_JACK) 
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACK);
  }

  if  (gt==GAMETYPE::SOLO_QUEEN) 
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOQUEEN);
  }

  if  (gt==GAMETYPE::SOLO_KING) 
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOKING);
  }

  if   (gt==GAMETYPE::SOLO_QUEEN_JACK)      
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACKQUEEN);
  }
  if      (gt==GAMETYPE::SOLO_KING_JACK)       
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACKKING);
  }

  if (gt==GAMETYPE::SOLO_KING_QUEEN)
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOQUEENKING);
  }
  if    (gt==GAMETYPE::SOLO_KOEHLER) 
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_SOLOKOEHLER);
  }

  if (gt==GAMETYPE::SOLO_MEATLESS)
  {
    return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_MEATLESS);
  }

  return this->Aiconfig::value(Aiconfig::LOWEST_TRUMPLIMIT_NORMAL);
}

/**
 ** returns the selected partner for a genscher
 **
 ** @param     -
 **
 ** @return    pointer to the partner
 **            NULL if not to use the genscher
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.4
 **/
Player const*
Ai::genscher_partner()
{
  Player const* res = NULL;

  // first find original teammate
  for ( unsigned i = 0; i < this->game().playerno(); i++ ) {
    if( this->teamofplayer( this->game().player(i) ) == this->team()
       && this->game().player(i) != *this)
    {	
      res = &(this->game().player(i));
      break;
    }
  }
  /// @todo use teams after genscher...

  // check Specialpoints
  std::vector<int> bonusPoints( this->game().playerno(), 0 );
  for( unsigned i = 0; i < trick_p.size(); i++ )
  {
    // DK: if hinzugefuegt
    if (!trick_p[i].isempty())
      bonusPoints[ this->game().no(trick_p[i].winnerplayer()) ] += trick_p[i].specialpoints().size() * 30;
  }

  // secure victory
  unsigned int points = 0;
  bool found = false;
  if ( this->game().points_of_team( this->team() ) < 120 )
    for ( unsigned i = 0; i < this->game().playerno(); i++ ) 
    {
      if(   this->game().points_of_player( this->game().player(i) ) 
         + this->game().points_of_player( *this ) 
         > 120
         && (this->game().player(i) != *this )
         && this->game().points_of_player( this->game().player(i) )
         + bonusPoints[ i ] > points )
      {	
        res = &(this->game().player(i));
        points = this->game().points_of_player( this->game().player(i) )
          + bonusPoints[ i ] ;
        found = true;

      }
    }
  if( found )
    return res;

  for ( unsigned i = 0; i < this->game().playerno(); i++ ) 
  {
    if( this->game().points_of_player( this->game().player(i) )
       + bonusPoints[ i ]  > points &&
       this->game().player(i) != *this )
    {  
      points = this->game().points_of_player( this->game().player(i) )
        + bonusPoints[ i ] ;
      res = &(this->game().player(i));
    }
  }

  Announcement highest_announcement = ANNOUNCEMENT::NOANNOUNCEMENT;
  // calculate highest announcements of this team
  for( unsigned i = 0; i < this->game().playerno(); i++ )
  {
    Player const& pl = this->game().player( i );
    if( this->game().team( this->game().player( i ) ) == this->team() )
    {
      if( pl.announcement() > highest_announcement )
        highest_announcement = pl.announcement();
    }
  }

  if ( highest_announcement >= ANNOUNCEMENT::NO0 &&
      this->game().hastrick( opposite( this->team() ) ) ) 
    return res;

  if ( highest_announcement >= ANNOUNCEMENT::NO30 &&
      this->game().points_of_team( opposite( this->team() ) ) >= 30 )
    return res;

  if ( highest_announcement >= ANNOUNCEMENT::NO60 &&
      this->game().points_of_team( opposite( this->team() ) ) >= 60 )
    return res;

  if ( highest_announcement >= ANNOUNCEMENT::NO90 &&
      this->game().points_of_team( opposite( this->team() ) ) >= 90 )
    return res;

  // don't use genscher
  return NULL;
} // Player const* Ai::genscher_partner()

/**
 **
 ** create a virtual game with virtual players (ais!), uses card_informations and team information
 **
 ** @param       -
 **
 ** @return      the virtual game
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
Game*
VirtualGamesAi::create_virtual_game() const
{
  // the game
  Game const& game = this->game();
#if 0
  CLOG << &this->game().player(0) << endl;
  CLOG << &this->player() << endl;
  CLOG << this->no() << endl;
  CLOG << this->player().name() << endl;
#endif
  // the ai of the virtual games interface
  Ai const& real_ai = static_cast<Ai const&>(game.player(this->no()));

  // the virtual players
  vector<Player*> ai_virt;
  unsigned n = 0;
  for (vector<Player*>::const_iterator player = game.players_begin();
       player != game.players_end();
       ++player, ++n) {
    ai_virt.push_back(new Ai(real_ai));
  }
  // create the virtual game
  Game* const game_virt_p = new Game(this->game(), ai_virt);
  Game& game_virt = *game_virt_p;
  for (unsigned p = 0; p < game_virt.playerno(); ++p) {
    Ai& ai_virtual = static_cast<Ai&>(*ai_virt[p]);

    ai_virtual.set_name(game.player(p).name());

    if (   (n != this->no())
        || game.isvirtual())  {
      // set the cards information
      ai_virtual.set_cards_information(real_ai.cards_information());
      // set the hand
      ai_virtual.set_hand(this->handofplayer(game.player(p)));
      // set the teaminfo
      ai_virtual.set_teams(real_ai.team_information().guessed_teams());
      // change the future limit
    } // if (ai_virtual.no() != real_ai.no())
#ifndef POSTPONED
    // *** workaround
    ai_virtual.set_future_limit_for_all_tricks(real_ai.future_limit() / 10);
#endif // #ifndef POSTPONED

  } // for (player \in game.players())

  return game_virt_p;
} // virtual Game* VirtualGamesAi::create_virtual_game() const;
