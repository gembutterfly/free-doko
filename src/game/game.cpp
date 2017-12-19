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

#include "game.h"

#include "gameplay.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../player/player.h"
#include "../card/trick.h"

#include "../ui/ui.wrap.h"

/**
 ** constructor
 **
 ** @param	party		party, the game belongs to
 ** @param	startplayer	the startplayer of the game
 **
 ** @return	-
 **/
Game::Game(Party& party, Player& startplayer) :
  party_(&party),
  gameplay_(new Gameplay),
  players_(party.players_),
  startplayer_(&startplayer),
  player_current_(&startplayer),
  seed_(party.seed()),
  type_(GAMETYPE::NORMAL),
  cards_(Card::NUMBER_OF_TCOLORS),
  marriage_selector_(MARRIAGE_SELECTOR::TEAM_SET),
  marriage_determination_trickno_(0),
  poverty_cards_(),
  poverty_cards_returned_(),
  poverty_shifted_(false),
  tricks_(),
  teaminfo_(party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME), TEAM::UNKNOWN),
  human_teaminfo_(party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME), TEAM::UNKNOWN),
  announcement_(party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)),
#ifdef USE_NETWORK
  announcement_over_network_(party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)),
#endif
  swines_announced_(false),
  hyperswines_announced_(false),
  finished_(false)
{
  dynamic_cast<UI_Wrap*>(::ui)->prepend(this->gameplay_);

  for (auto& player : this->players())
    player->new_game(*this);

  return ;
} // Game::Game(Party& party, Player const& startplayer)

/**
 ** constructor
 **
 ** @param	game	game to copy
 ** @param	players	the players for the new game
 **
 ** @return	-
 **/
Game::Game(Game const& game, vector<Player*> const& players) :
  party_(game.party_),
  players_(players),
  startplayer_(game.startplayer_),
  player_current_(game.player_current_),
  seed_(game.seed_),
  type_(game.type_),
  cards_(game.cards_),
  soloplayer_(game.soloplayer_),
  marriage_selector_(game.marriage_selector_),
  marriage_determination_trickno_(game.marriage_determination_trickno_),
  poverty_cards_(game.poverty_cards_),
  poverty_cards_returned_(game.poverty_cards_returned_),
  poverty_shifted_(game.poverty_shifted_),
  tricks_(game.tricks_),
  teaminfo_(game.teaminfo_),
  human_teaminfo_(game.human_teaminfo_),
  announcement_(game.announcement_),
#ifdef USE_NETWORK
  announcement_over_network_(game.announcement_over_network_),
#endif
  swines_owner_(game.swines_owner_),
  first_fox_catcher_(game.first_fox_catcher_),
  swines_announced_(game.swines_announced_),
  hyperswines_owner_(game.hyperswines_owner_),
  hyperswines_announced_(game.hyperswines_announced_),
  finished_(game.finished_)
{
  // set the players
  if (game.startplayer_)
    this->startplayer_ = players[game.no(*game.startplayer_)];
  if (game.player_current_)
    this->player_current_ = players[game.no(*game.player_current_)];
  if (game.soloplayer_)
    this->soloplayer_ = players[game.no(*game.soloplayer_)];
  if (game.swines_owner_)
    this->swines_owner_ = players[game.no(*game.swines_owner_)];
  if (game.first_fox_catcher_)
    this->first_fox_catcher_ = players[game.no(*game.first_fox_catcher_)];
  if (game.hyperswines_owner_)
    this->hyperswines_owner_ = players[game.no(*game.hyperswines_owner_)];

  // update the pointers of the tricks
  for (auto& trick : this->tricks()) {
    trick = new Trick(*trick);
    trick->set_game(*this);
  }

  // set the game of the players
  for (auto& p : this->players())
    p->set_game(*this);

  DEBUG_ASSERT(this->self_check());

  return ;
} // Game::Game(Game game, vector<Players*> players)

/**
 ** constructor
 **
 ** @param	game	game to copy
 ** @param	players	the players for the new game
 **
 ** @return	-
 **/
Game::Game(Game const& game, vector<unique_ptr<Player>> const& players) :
  party_(game.party_),
  gameplay_(nullptr),
  players_(),
  seed_(game.seed_),
  type_(game.type_),
  cards_(game.cards_),
  marriage_selector_(game.marriage_selector_),
  marriage_determination_trickno_(game.marriage_determination_trickno_),
  poverty_cards_(game.poverty_cards_),
  poverty_cards_returned_(game.poverty_cards_returned_),
  poverty_shifted_(game.poverty_shifted_),
  tricks_(game.tricks_),
  teaminfo_(game.teaminfo_),
  human_teaminfo_(game.human_teaminfo_),
  announcement_(game.announcement_),
#ifdef USE_NETWORK
  announcement_over_network_(game.announcement_over_network_),
#endif
  swines_announced_(game.swines_announced_),
  hyperswines_announced_(game.hyperswines_announced_),
  finished_(game.finished_)
{
  // set the players
  this->players_.reserve(players.size());
  for (auto& p : players)
    this->players_.push_back(p.get());
  if (game.startplayer_)
    this->startplayer_ = this->players_[game.no(*game.startplayer_)];
  if (game.player_current_)
    this->player_current_ = this->players_[game.no(*game.player_current_)];
  if (game.soloplayer_)
    this->soloplayer_ = this->players_[game.no(*game.soloplayer_)];
  if (game.swines_owner_)
    this->swines_owner_ = this->players_[game.no(*game.swines_owner_)];
  if (game.first_fox_catcher_)
    this->first_fox_catcher_ = this->players_[game.no(*game.first_fox_catcher_)];
  if (game.hyperswines_owner_)
    this->hyperswines_owner_ = this->players_[game.no(*game.hyperswines_owner_)];

  // update the pointers of the tricks
  for (auto& trick : this->tricks()) {
    trick = new Trick(*trick);
    trick->set_game(*this);
  }

  // set the game of the players
  for (auto& p : this->players_)
    p->set_game(*this);

  DEBUG_ASSERT(this->self_check());
} // Game::Game(Game game, vector<unique_ptr<Players>> players)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **/
Game::~Game()
{
  if (this->gameplay_) {
    dynamic_cast<UI_Wrap*>(::ui)->remove(this->gameplay_);
    delete this->gameplay_;
  }

  for (auto& t : this->tricks())
    delete t;
} // Game::~Game()

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param	-
 **
 ** @return	wether the self-check was successful (no error)
 **/
bool
Game::self_check() const
{
  if (!this->isvirtual())
    DEBUG_ASSERTION((this == &::party.game()),
		    "Game::self_check()\n"
		    "  this game is not virutal and not the game of the party");

  DEBUG_ASSERTION(this->party_,
		  "Game::self_check()\n"
		  "  no pointer to a party\n"
		  "  this = " << this);

  DEBUG_ASSERTION((this->party_ == &::party),
		  "Game::self_check()\n"
		  "  different party: "
		  "this->party  = " << this->party_
		  << " != " << &::party << " = ::party");

  if (!this->isvirtual())
  for (vector<Player*>::const_iterator p = this->players().begin();
       p != this->players().end();
       ++p) {
    DEBUG_ASSERTION((this == &(*p)->game()),
		    "Game::self_check()\n"
		    "  a player is not of this game");
    DEBUG_ASSERTION((*p)->self_check(),
		    "Game::self_check()\n"
		    "  self check of a player returned 'false'");
  }

  if (!this->isvirtual())
  for (auto const& t : this->tricks()) {
    DEBUG_ASSERTION((this == &t->game()),
		    "Game::self_check()\n"
		    "  a trick is not of this game");

    DEBUG_ASSERTION(t->self_check(),
		    "Game::self_check()\n"
		    "  self check of a trick returned 'false'");
  } // for (t : this->tricks())

  if (!this->isvirtual())
  if (this->startplayer_)
    DEBUG_ASSERTION((this == &this->startplayer_->game()),
		    "Game::self_check()\n"
		    "  the startplayer is not of the same game");

  if (this->player_current_)
    DEBUG_ASSERTION((this == &this->player_current_->game()),
		    "Game::self_check()\n"
		    "  the current player is not of the same game");

  if (this->soloplayer_)
    DEBUG_ASSERTION((this == &this->soloplayer_->game()),
		    "Game::self_check()\n"
		    "  the soloplayer is not of the same game");
  if (this->swines_owner_)
    DEBUG_ASSERTION((this == &this->swines_owner_->game()),
		    "Game::self_check()\n"
		    "  the swines owner is not of the same game");
  if (this->first_fox_catcher_)
    DEBUG_ASSERTION((this == &this->first_fox_catcher_->game()),
		    "Game::self_check()\n"
		    "  the first fox catcher is not of the same game");
  if (this->hyperswines_owner_)
    DEBUG_ASSERTION((this == &this->hyperswines_owner_->game()),
		    "Game::self_check()\n"
		    "  the hyperswines owner is not of the same game");


  return true;
} // bool Game::self_check() const

/**
 ** output of the game
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **/
ostream&
Game::write(ostream& ostr) const
{
  ostr << "Game:\n"
    << "-----\n"
    << "\n"
    << "seed" << ": " << this->seed() << "\n"
    << "current trick number: " << this->trick_current_no() << "\n"
    << "type: " << this->type() << "\n";

#ifdef OUTDATED
  // DK 2005-03-31
  // this information is saved in the party

  ostr << "\n"
    << "players" << ":\n";
  for (auto const& player : this->players())
    ostr << *player << "\n";
#endif

  ostr << "\n"
    << "teams" << ": \n";
  for (auto const& player : this->players())
    ostr << this->no(*player) << ": " << this->team(*player) << "\n";

  ostr << "\n"
    << "teaminfo" << ": \n";
  for (auto const& player : this->players())
    ostr << this->no(*player) << ": " << this->teaminfo(*player) << "\n";

  ostr << '\n'
    << "tricks:\n"
    << '\n';
  unsigned i = 0;
  for (auto const& trick : this->tricks()) {
    ostr << i << ". trick:\n"
      << *trick << '\n';
    i += 1;
  }

  ostr << '\n';

  return ostr;
} // ostream& Game::write(ostream& ostr) const

/**
 **
 ** output of the tricks
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **/
ostream&
Game::write_tricks(ostream& ostr) const
{
  ostr << '\n'
    << "tricks:\n"
    << '\n';
  unsigned i = 0;
  for (auto const& trick : this->tricks()) {
    ostr << i << ". trick:\n"
      << *trick << '\n';
    i += 1;
  }

  ostr << '\n';

  return ostr;
} // ostream& Game::write_tricks(ostream& ostr) const

/**
 ** output of the hands
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **/
ostream&
Game::write_hands(ostream& ostr) const
{
  ostr << "\n"
    << "hands:\n"
    << "\n";
  for (auto const& p : this->players()) {
    ostr << this->no(*p) << ' ' << p->name() << '\n'
      << p->hand()
      << '\n';
  }

  return ostr;
} // ostream& Game::write_hands(ostream& ostr) const

/**
 ** output of the game
 **
 ** @param	ostr	output stream
 ** @param	game	game to be printed
 **
 ** @return	output stream
 **/
ostream&
operator<<(ostream& ostr,  Game const& game)
{
  game.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, Game game)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is a virtual game
 **/
bool
Game::isvirtual() const
{
  return (this != &this->party().game());
} // bool Game::isvirtual() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the game is finished
 **/
bool
Game::is_finished() const
{
  return this->finished_;
} // bool Game::is_finished() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the rules of the game
 **/
Party&
Game::party()
{
  DEBUG_ASSERTION((this->party_ == &::party),
                  "Game::party()\n"
                  "  different party: "
                  "this->party  = " << this->party_
                  << " != " << &::party << " = ::party");
  return *this->party_;
} // Party& Game::party()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the rules of the game
 **/
Rule const&
Game::rule() const
{
  return this->party().rule();
} // Rule const& Game::rule() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the bock multiplier
 **/
int
Game::bock_multiplier() const
{
  return this->party().current_bock_multiplier();
} // int Game::bock_multiplier() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	current game-type
 **
 ** @todo	show 'MARRIAGE_SILENT' if both club queens have been played
 **/
GameType
Game::type() const
{
  if (   !this->is_finished()
      && (this->type_ == GAMETYPE::MARRIAGE_SILENT)
      && (this->marriage_selector() == MARRIAGE_SELECTOR::SILENT) )
    return GAMETYPE::NORMAL;

  return this->type_;
} // GameType Game::type() const

/**
 **
 ** sets the gametype
 **
 ** @param	type	new gametype
 **
 ** @return	-
 **/
void
Game::set_type(GameType const type)
{
  this->type_ = type;
  this->update_cards();
} // void Game::set_type(GameType type)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is a solo
 **/
bool
Game::is_solo() const
{
  return GAMETYPE::is_solo(this->type());
} // bool Game::is_solo()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is a real solo (solo, but no silent marriage)
 **/
bool
Game::is_real_solo() const
{
  return GAMETYPE::is_real_solo(this->type());
} // bool Game::is_real_solo()

/**
 ** sets whether this is a duty solo
 **
 ** @param	-
 **
 ** @return	whether this is a duty solo
 **/
bool
Game::set_is_duty_solo()
{
  this->is_duty_solo_ = false;

  if (   this->party().is_duty_soli_round()
      && (::game_status <= GAMESTATUS::GAME_RESERVATION)) {
    this->is_duty_solo_ = true;
    return true;
  }

  if (this->is_real_solo()) {
    Player const& soloplayer = (  (::game_status <= GAMESTATUS::GAME_RESERVATION)
                                ? this->player_current()
                                : this->soloplayer());
    if (   GAMETYPE::is_color_solo(this->type())
        && this->party().remaining_duty_color_soli(soloplayer))
      this->is_duty_solo_ = true;

    if (   GAMETYPE::is_picture_solo(this->type())
        && this->party().remaining_duty_picture_soli(soloplayer))
      this->is_duty_solo_ = true;

    if (this->party().remaining_duty_free_soli(soloplayer))
      this->is_duty_solo_ = true;
  }

  return this->is_duty_solo();
} // bool Game::set_is_duty_solo()

/**
 ** set the startplayer
 **
 ** @param     startplayer   the startplayer
 **
 ** @return    -
 **/
void
Game::set_startplayer(unsigned const startplayer)
{
  DEBUG_ASSERTION(::game_status <= GAMESTATUS::GAME_INIT,
                  "Game::set_startplayer(" << startplayer << ")\n"
                  "  game_status = " << ::game_status << " > GAME_INIT");

  this->startplayer_ = &this->player(startplayer);
} // void Game::set_startplayer(unsigned startplayer)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the startplayer is not incremented (t.i. duty solo but not in the duty solo round or lust solo and lustsolo player leads)
 **/
bool
Game::startplayer_stays() const
{
  if (!GAMETYPE::is_real_solo(this->type()))
    return false;
  // some solo
  if (this->party().is_duty_soli_round())
    return (   (::game_status == GAMESTATUS::GAME_RESERVATION)
            || (this->soloplayer() != this->party().player(this->party().startplayer())));
  if (this->is_duty_solo())
    return true;
  // some lust solo
  return this->rule()(Rule::LUSTSOLO_PLAYER_LEADS);
} // bool Game::startplayer_stays() const

/**
 ** updates the cards according to the game type
 **
 ** @param	-
 **
 ** @return	-
 **/
void
Game::update_cards()
{
  this->cards_ = vector<vector<Card> >(5);

  // first all color cards

  for (auto const& c : this->rule().card_colors())
    for (auto const& v : this->rule().card_values())
      if (!Card(c, v).istrump(*this))
        this->cards_[c].insert(this->cards_[c].begin(), Card(c, v));


  // second all trumps (manual fill)

  vector<Card>& trumps = this->cards_[Card::TRUMP];
  switch (this->type()) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::POVERTY:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_DIAMOND:
    if (this->rule()(Rule::WITH_NINES))
      trumps.push_back(Card(this->trumpcolor(), Card::NINE));
    trumps.push_back(Card(this->trumpcolor(), Card::KING));
    if (!(   (this->trumpcolor() == Card::HEART)
          && this->rule()(Rule::DOLLEN)))
      trumps.push_back(Card(this->trumpcolor(), Card::TEN));
    trumps.push_back(Card(this->trumpcolor(), Card::ACE));
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    if (this->rule()(Rule::DOLLEN))
      trumps.push_back(Card::DOLLE);
    break;
  case GAMETYPE::THROWN_NINES:
    trumps.push_back(Card(Card::DIAMOND, Card::NINE));
    trumps.push_back(Card(Card::HEART,   Card::NINE));
    trumps.push_back(Card(Card::SPADE,   Card::NINE));
    trumps.push_back(Card(Card::CLUB,    Card::NINE));
    break;
  case GAMETYPE::THROWN_KINGS:
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  case GAMETYPE::THROWN_NINES_AND_KINGS:
    trumps.push_back(Card(Card::DIAMOND, Card::NINE));
    trumps.push_back(Card(Card::HEART,   Card::NINE));
    trumps.push_back(Card(Card::SPADE,   Card::NINE));
    trumps.push_back(Card(Card::CLUB,    Card::NINE));
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  case GAMETYPE::THROWN_RICHNESS:
    trumps.push_back(Card(Card::CLUB,    Card::ACE));
    trumps.push_back(Card(Card::SPADE,   Card::ACE));
    trumps.push_back(Card(Card::HEART,   Card::ACE));
    trumps.push_back(Card(Card::DIAMOND, Card::ACE));
    trumps.push_back(Card(Card::CLUB,    Card::TEN));
    trumps.push_back(Card(Card::SPADE,   Card::TEN));
    trumps.push_back(Card(Card::HEART,   Card::TEN));
    trumps.push_back(Card(Card::DIAMOND, Card::TEN));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    if (this->rule()(Rule::WITH_NINES)) {
      trumps.push_back(Card(Card::CLUB,    Card::NINE));
      trumps.push_back(Card(Card::SPADE,   Card::NINE));
      trumps.push_back(Card(Card::HEART,   Card::NINE));
      trumps.push_back(Card(Card::DIAMOND, Card::NINE));
    }
    break;
  case GAMETYPE::SOLO_MEATLESS:
    break;
  case GAMETYPE::SOLO_JACK:
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    break;
  case GAMETYPE::SOLO_QUEEN:
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    break;
  case GAMETYPE::SOLO_KING:
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  case GAMETYPE::SOLO_QUEEN_JACK:
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    break;
  case GAMETYPE::SOLO_KING_JACK:
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  case GAMETYPE::SOLO_KING_QUEEN:
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  case GAMETYPE::SOLO_KOEHLER:
    trumps.push_back(Card(Card::DIAMOND, Card::JACK));
    trumps.push_back(Card(Card::HEART,   Card::JACK));
    trumps.push_back(Card(Card::SPADE,   Card::JACK));
    trumps.push_back(Card(Card::CLUB,    Card::JACK));
    trumps.push_back(Card(Card::DIAMOND, Card::QUEEN));
    trumps.push_back(Card(Card::HEART,   Card::QUEEN));
    trumps.push_back(Card(Card::SPADE,   Card::QUEEN));
    trumps.push_back(Card(Card::CLUB,    Card::QUEEN));
    trumps.push_back(Card(Card::DIAMOND, Card::KING));
    trumps.push_back(Card(Card::HEART,   Card::KING));
    trumps.push_back(Card(Card::SPADE,   Card::KING));
    trumps.push_back(Card(Card::CLUB,    Card::KING));
    break;
  }; // switch (this->type())

  DEBUG_ASSERTION((this->trumps().size()
                   == (this->trumps_no()
                       / this->rule()(Rule::NUMBER_OF_SAME_CARDS))),
                  "Game::update_cards()\n"
                  "  trumps size " << this->trumps().size()
                  << " does not match trumps number "
                  << (this->trumps_no()
                      / this->rule()(Rule::NUMBER_OF_SAME_CARDS)));

  return ;
} // void Game::update_cards()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of cards, that are shifted by the poverty player
 **/
unsigned
Game::poverty_cardno() const
{
  return this->poverty_cards().size();
} // unsigned Game::poverty_cardno() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of cards, that are shifted by the poverty player
 **/
unsigned
Game::poverty_trumpno_returned() const
{
  return this->poverty_cards_returned().numberoftrumps();
} // unsigned Game::poverty_trumpno_returned() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the shifted poverty cards
 **/
HandCards const&
Game::poverty_cards() const
{
  DEBUG_ASSERTION((this->type() == GAMETYPE::POVERTY),
                  "Game::poverty_cards():\n"
                  "  invalid gametype");

  return this->poverty_cards_;
} // HandCards const& Game::poverty_cards() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the returned poverty cards
 **/
HandCards const&
Game::poverty_cards_returned() const
{
  DEBUG_ASSERTION((this->type() == GAMETYPE::POVERTY),
                  "Game::poverty_cards():\n"
                  "  invalid gametype '" << this->type() << "'");

  return this->poverty_cards_returned_;
} // HandCards const& Game::poverty_cards_returned() const

/**
 ** -> result
 **
 ** @param	game1	game 1 to compare
 ** @param	game2	game 2 to compare
 **
 ** @return	whether the games are equal (they lie in the same memory)
 **/
bool
operator==(Game const& game1, Game const& game2)
{
  return (&game1 == &game2);
} // bool operator==(Game game1, Game game2)

/**
 ** -> result
 **
 ** @param	game1	game 1 to compare
 ** @param	game2	game 2 to compare
 **
 ** @return	whether the games are different
 **/
bool
operator!=(Game const& game1, Game const& game2)
{
  return !(game1 == game2);
} // bool operator!=(Game game1, Game game2)
