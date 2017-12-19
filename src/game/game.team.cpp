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

#include "../party/rule.h"
#include "../player/player.h"
#include "../player/human/human.h"
#include "../card/trick.h"
#include "../ui/ui.h"

/**
 ** -> result
 **
 ** @param	player the player
 **
 ** @return	the team of player 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
Team
Game::teaminfo(Player const& player) const
{                                                                          
  DEBUG_ASSERTION(this->no(player) != UINT_MAX,
                  "Game::teaminfo(player)\n"
                  "  player '" << player.name() << ". not found in the game.");
  return this->teaminfo(this->no(player));
} // Team Game::teaminfo(Player player) const

/**
 ** -> result
 **
 ** @param	playerno   the number of the player
 **
 ** @return	the team of player 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
Team
Game::teaminfo(unsigned const playerno) const
{                                                                          
  DEBUG_ASSERTION((playerno < this->teaminfo_.size()),
		  "Game::teaminfo(player):\n"
		  "  playerno is too great");

  return this->teaminfo_[playerno];
} // Team Game::teaminfo(unsigned playerno) const

/**
 **
 ** -> result
 **
 ** @param	player the player
 **
 ** @return	the team of player 'player' as known by the humans
 **
 ** @version	0.6.1
 **
 ** @author	Diether Knof
 **
 **/
Team
Game::teaminfo_for_humans(Player const& player) const
{                                                                          
  return this->human_teaminfo_[this->no(player)];
} // Team Game::teaminfo_for_humans(Player const& player) const

/**
 **
 ** set the teaminfo of the player 'player' to 'team'
 **
 ** @param	player	player
 ** @param	team	team to set
 **
 ** @return	-
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 */
void
Game::set_teaminfo(Player const& player, Team const& team)
{
  DEBUG_ASSERTION(TEAM::is_real(team),
		  "Game::set_teaminfo(player, team):\n"
		  "  'team' not valid: " << team);

  if (this->teaminfo_[this->no(player)] == team)
    return ;

  this->teaminfo_[this->no(player)] = team;
  if (player.team() == TEAM::UNKNOWN)
    this->player(this->no(player)).set_team(team);

  if (!this->isvirtual())
    ::ui->teaminfo_changed(player);

  this->teaminfo_update();
  for (unsigned p = 0; p < this->number_of_players(); ++p)
    this->player(p).teaminfo_update();

  return ;
} // void set_teaminfo(Player const& player, Team const& team);

/**
 ** reset the teaminfo
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
Game::teaminfo_reset()
{                                                                  
  for (unsigned p = 0; p < this->number_of_players(); ++p) {
    this->player(p).set_team(TEAM::UNKNOWN);
  }
  this->teaminfo_ = vector<Team>(this->number_of_players(), TEAM::UNKNOWN);
  this->human_teaminfo_ = vector<Team>(this->number_of_players(), TEAM::UNKNOWN);

  return ;
} // void Game::teaminfo_reset()

/**
 ** sets the teaminfo at the start of the game
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.4.4
 **
 ** @todo      marriage: team of the players (maybe re, maybe contra, ?)
 **/
void
Game::teaminfo_set_at_gamestart()
{                                                                  
  if (this->type() == GAMETYPE::NORMAL) {
    this->teaminfo_ = vector<Team>(this->number_of_players(), TEAM::UNKNOWN);
    for (unsigned p = 0; p < this->number_of_players(); ++p) {
      this->player(p).set_team(this->player(p).hand().contains(Card::CLUB_QUEEN)
                               ? TEAM::RE
                               : this->player(p).hand().contains(Card::UNKNOWN)
                               ? TEAM::UNKNOWN
                               : TEAM::CONTRA);
    }
  } else if (type() == GAMETYPE::POVERTY) {
    // is done in 'this->poverty_shift()'

  } else if (this->type() == GAMETYPE::MARRIAGE) {
    // set all teams to unknown but of the soloplayer
    this->teaminfo_ = vector<Team>(this->number_of_players(), TEAM::UNKNOWN);
    this->teaminfo_[this->no(this->soloplayer())] = TEAM::RE;
    for (unsigned p = 0; p < this->number_of_players(); ++p)
      this->player(p).set_team((this->player(p) == this->soloplayer())
                               ? TEAM::RE
                               : TEAM::MAYBE_CONTRA);

  } else if (GAMETYPE::is_solo(this->type())) {
    // set all teams to contra but of the soloplayer
    this->teaminfo_ = vector<Team>(this->number_of_players(), TEAM::CONTRA);
    this->teaminfo_[this->no(this->soloplayer())] = TEAM::RE;
    for (unsigned p = 0; p < this->number_of_players(); ++p)
      this->player(p).set_team((this->player(p) == this->soloplayer())
                               ? TEAM::RE
                               : TEAM::CONTRA);
  } // if (this->type())

  this->human_teaminfo_ = vector<Team>(this->number_of_players(), TEAM::UNKNOWN);
  this->human_teaminfo_update();

  return ;
} // void Game::teaminfo_set_at_gamestart()

/**
 **
 ** updates the global teaminfo
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
void
Game::teaminfo_update()
{
  if (this->type() != GAMETYPE::NORMAL)
    return ;

  using namespace TEAM;

  if (!this->trick_current().isstartcard()) {

    // The following does not work since when the trick is full
    // the current player is the winner of the trick.
    //   this->player_previous(this->player_current());
    Player const& player
      = this->trick_current().player_of_card(this->trick_current().actcardno() - 1);

    if (   !TEAM::is_real(this->teaminfo(player))
        || this->isvirtual()) {
      // the player has played a club queen
      if (this->trick_current().card_of_player(player) == Card::CLUB_QUEEN) {
        this->set_teaminfo(player, RE);
      } else 

      // the startcard is trump, but the player has not played a trump
      if (   this->trick_current().startcard().istrump()
          && (!this->trick_current().card_of_player(player).istrump())
          && (player.hand().numberofall(Card::CLUB_QUEEN) == 0) ) {
        this->set_teaminfo(player, CONTRA);
      } else

      // it is the last played card of the player
      if (   (player.cards_to_play() == 0)
          && (player.hand().numberofall(Card::CLUB_QUEEN) == 0) ) {
        this->set_teaminfo(player, CONTRA);
      }

    } // if (team of 'player' unknown || virtual game)
  } // if (!this->trick_current().isstartcard())

  { // count the played club queens for a silent marriage
    for (vector<Player*>::const_iterator p = this->players().begin();
         p!= this->players().end();
         p++) {
      Player const& player = **p;
      if (this->teaminfo(player) == TEAM::RE) {
        // We only want to know, whether the player has played his second
        // club queen, so he must already be known as re.
        unsigned no = 0;
        for (vector<Trick*>::const_iterator t = this->tricks().begin();
             t != this->tricks().end();
             ++t) {
          if ((*t)->cardno_of_player(player) < (*t)->actcardno())
            if ((*t)->card_of_player(player) == Card::CLUB_QUEEN)
              no += 1;
        } // for (t \in this->tricks())

        if (no == this->rule()(Rule::NUMBER_OF_SAME_CARDS)) {
          // all other players must be contra
          for (unsigned p = 0; p < this->number_of_players(); ++p)
            if (this->teaminfo(p) != TEAM::RE)
              this->set_teaminfo(this->player(p), TEAM::CONTRA);
        } // if (both club queens played)
      } // if (this->teaminfo_[player] == TEAM::RE)
    } // for (player)
  } // count the played club queens for a silent marriage

  if (this->rule()(Rule::SWINE_ONLY_SECOND)
      && this->swines_announced())
    // the swines owner has to say his team
    this->set_teaminfo(*this->swines_owner(),
                       this->swines_owner()->team());

  { // count the 're's and 'contra's
    unsigned re_no = 0;
    unsigned contra_no = 0;
    for (vector<Team>::const_iterator t = this->teaminfo_.begin();
         t != this->teaminfo_.end();
         t++) {
      if (*t == TEAM::RE)
        re_no += 1;
      else if (*t == TEAM::CONTRA)
        contra_no += 1;
    } // for (t)

    if (re_no == this->rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM)) {
      for (unsigned p = 0; p < this->number_of_players(); ++p)
        if (this->teaminfo(p) != TEAM::RE)
          this->set_teaminfo(this->player(p), TEAM::CONTRA);
    } // if (re_no == 2)
    if (contra_no == this->number_of_players() - 1) {
      for (unsigned p = 0; p < this->number_of_players(); ++p)
        if (this->teaminfo(p) != TEAM::CONTRA)
          this->set_teaminfo(this->player(p), TEAM::RE);

      this->determine_silent_marriage();
    } // if (contra_no == this->number_of_players() - 1)

    if (contra_no == this->number_of_players() - 2) {
      if (!this->trick_current().isstartcard()) {
        Player const& player
          = this->trick_current().player_of_card(this->trick_current().actcardno() - 1);
        if ((this->teaminfo(player) == TEAM::RE)
            && (this->trick_current().startcard().istrump()
                && !this->trick_current().card_of_player(player).istrump())) {
          // the player is re but can play no more trump

          // all players but two are contra, one is re and does not have
          // any more club queen
          // so the remaining player must be re
          for (unsigned p = 0; p < this->number_of_players(); ++p)
            if (this->teaminfo(p) != TEAM::CONTRA)
              this->set_teaminfo(this->player(p), TEAM::RE);
        } // // if (the player is re but can play no more trump)
      } // if (!this->trick_current().isstartcard())
    } // if (contra_no == this->number_of_players() - 2)
  } // count the 're's and 'contra's

  this->human_teaminfo_update();

  return ;
} // void Game::teaminfo_update()


/**
 ** update the teaminfo of all humans
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Game::human_teaminfo_update()
{
  DEBUG_ASSERTION(::game_status & GAMESTATUS::GAME,
                  "Game::update_human_teaminfo()\n"
                  "  not in a game: " << ::game_status);

  // backup of the teaminfo (so changes can be seen)
  vector<Team> const teaminfo_bak = this->human_teaminfo_;

  { // update the teaminfo from the one of 'game'

    // whether some team has changed
    bool changed = false;
    for (unsigned p = 0; p < this->number_of_players(); ++p) {
      Player const& player = this->player(p);
      if (   ::is_real(this->teaminfo(player))
          && (this->teaminfo_for_humans(player) != this->teaminfo(player)) ) {
        this->human_teaminfo_[p] = this->teaminfo(player);
        changed = true;
      }
      if (   (player.type() == Player::HUMAN)
          && (this->teaminfo_for_humans(player) != player.team())
          && ::is_real(player.team()) ) {
        this->human_teaminfo_[p] = player.team();
        changed = true;
      }
    } // for (p < this->number_of_players())

    if (!changed)
      return ;
  } // update the teaminfo from the one of 'game'

  if (this->type() != GAMETYPE::NORMAL)
    return ;

  // the number of re players
  unsigned re_no = 0;
  // the number of human re players
  unsigned human_re_no = 0;
  // the number of club qeens still on the human hands
  unsigned human_club_queens_no = 0;
  // the number of contra players
  unsigned contra_no = 0;

  for (unsigned p = 0; p < this->number_of_players(); ++p) {
    Player const& player = this->player(p);
    if (this->teaminfo_for_humans(player) == TEAM::RE)
      re_no += 1;
    else if (this->teaminfo_for_humans(player) == TEAM::CONTRA)
      contra_no += 1;

    if (this->player(p).type() == Player::HUMAN) {
      if (this->teaminfo_for_humans(player) == TEAM::RE) {
        human_re_no += 1;
        human_club_queens_no += player.hand().numberof(Card::CLUB_QUEEN);
      }
    }
  } // for (p < this->number_of_players())

  // check whether all re players are known
  if (   (re_no == this->rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM))
      || (this->played_no(Card::CLUB_QUEEN) + human_club_queens_no
          == this->rule()(Rule::NUMBER_OF_SAME_CARDS))
     ) {
    for (unsigned p = 0; p < this->number_of_players(); ++p) {
      if (this->human_teaminfo_[p] != TEAM::RE)
        this->human_teaminfo_[p] = TEAM::CONTRA;
    } // for (p < this->number_of_players())
  } // if (all re players known)

  // check whether all contra players are known
  if (   (contra_no == this->number_of_players() - 1)
      || (contra_no == (this->number_of_players() - human_re_no - 1))
     ) {
    for (unsigned p = 0; p < this->number_of_players(); ++p) {
      if (this->human_teaminfo_[p] != TEAM::CONTRA)
        this->human_teaminfo_[p] = TEAM::RE;
    } // for (p < this->number_of_players())
  } // if (all contra players known


  if (!this->isvirtual())
    for (unsigned p = 0; p < this->number_of_players(); ++p)
      if (this->teaminfo_for_humans(this->player(p)) != teaminfo_bak[p]) {
        ::ui->teaminfo_changed(this->player(p));
      }

  return ;
} // void Game::human_teaminfo_update()


/**
 **
 ** -> result
 **
 ** @param	player - the player
 **
 ** @return	the team of player 'player'
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Team
Game::team(Player const& player) const
{
  return player.team();
} // Team Game::team(Player const& player) const
