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

#include "gameplay_actions/marriage.h"
#include "gameplay_actions/genscher.h"

#include "../party/rule.h"
#include "../player/player.h"
#include "../card/trick.h"
#include "../ui/ui.h"

/**
 ** test, whether a genscher is played
 **
 ** @param	played_card   the played card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Game::check_for_genscher(HandCard const& played_card)
{
  if (   !GAMETYPE::is_solo(this->type())
      && (this->type() != GAMETYPE::POVERTY)
      && this->rule()(Rule::GENSCHER)
      && played_card.istrumpking()
      && (this->player_current().hand().numberoftrumpkings()
	  == this->rule()(Rule::NUMBER_OF_SAME_CARDS))
     ) {
    Player const* const genscher_partner
      = this->player_current().genscher_partner();

    if (genscher_partner != NULL) {
      this->soloplayer_ = &(this->player_current());
      // set the teams: 'RE' for the genscher players, 'CONTRA' for the others
      for (vector<Player*>::const_iterator player = this->players_begin();
	   player != this->players_end();
	   player++) {
        this->teaminfo_[this->no(**player)] = ( (   (**player
                                                     == this->player_current())
                                                 || (**player
                                                     == *genscher_partner))
                                               ? TEAM::RE
                                               : TEAM::CONTRA);
        (*player)->set_team(this->teaminfo(**player));
      } // for (player \in this->player)
      this->set_type(GAMETYPE::GENSCHER);

      // remove all announcement but 're' and 'contra'
      for (unsigned p = 0; p < this->number_of_players(); p++) {
        vector<AnnouncementWithTrickno>& announcements
          = this->announcement_[p];
        DEBUG_ASSERTION(!announcements.empty(),
                        "Game::check_for_genscher()\n"
                        "  announcements of player '" << p << "' is empty"
                        " but should contain a 'no announcement'");
        // the first announcement is always a 'no announcement'
        if (announcements.size() == 1)
          continue;
        if (ANNOUNCEMENT::is_reply(announcements.front().announcement)) {
          announcements.erase(announcements.begin() + 1,
                              announcements.end());
          continue;
        }
        // the player has made an announcement
        announcements.erase(announcements.begin() + 2,
                            announcements.end());
        announcements[1].announcement = ANNOUNCEMENT::NO120;
        if (!this->isvirtual())
          ::ui->announcement_made(ANNOUNCEMENT::NO120, this->player(p));
      } // for (p < this->number_of_players())


      if (!this->isvirtual()) {
        for (vector<Player*>::const_iterator player = this->players_begin();
             player != this->players_end();
             player++)
          ::ui->teaminfo_changed(**player);
      } // if (!this->isvirtual())
      this->human_teaminfo_update();

      // tell all players about the genscher
      for (vector<Player*>::const_iterator player = this->players_begin();
           player != this->players_end();
           player++)
        (*player)->genscher(this->player_current(), *genscher_partner);

      if (!this->isvirtual()) {
        ::ui->genscher(this->soloplayer(), *genscher_partner);
        ::ui->gameplay_action(GameplayAction::Genscher(this->no(this->soloplayer()),
                                                       this->no(*genscher_partner)));
      }
    } // if (genscher_partner != NULL)
  } // if (genscher)

  return ;
} // void Game::check_for_genscher(HandCard played_card)


/**
 ** test, whether a silent marriage is played
 **
 ** @param	-
 **
 ** @return	
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
void
Game::check_for_silent_marriage()
{
  if (::game_status == GAMESTATUS::GAME_REDISTRIBUTE)
    return ;

  DEBUG_ASSERTION((::game_status == GAMESTATUS::GAME_RESERVATION),
                  "Game::check_for_silent_marriage()\n"
                  "  game status is not 'reservation' "
                  "but '" << ::game_status << "'");

  if (this->type() != GAMETYPE::NORMAL)
    return ;

  for (vector<Player*>::const_iterator p = this->players().begin();
       p != this->players().end();
       ++p) {
    if ((*p)->hand().numberofclubqueens()
        == this->rule()(Rule::NUMBER_OF_SAME_CARDS)) {
      this->soloplayer_ = *p;
      this->set_type(GAMETYPE::MARRIAGE_SILENT);
      this->marriage_selector_ = MARRIAGE_SELECTOR::SILENT;
      this->marriage_determination_trickno_ = UINT_MAX;
      break;
    } // if (silent marriage)
  } // for (p \in this->players())

  return ;
} // void Game::check_for_silent_marriage()

/**
 ** test, if the silent marriage is solved
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
Game::determine_silent_marriage()
{
  if (   (this->type_ != GAMETYPE::MARRIAGE_SILENT)
      || (this->marriage_selector() != MARRIAGE_SELECTOR::SILENT))
    return ;

  // whether the silent marriage is known to all
  bool solved = false;

  { // check whether both club queens have been played
    if (this->played_no(Card::CLUB_QUEEN)
        == this->rule()(Rule::NUMBER_OF_SAME_CARDS) )
      solved = true;
  } // check whether both club queens have been played

  { // check whether all other players are known to be contra
    unsigned contra_no = 0;
    for (vector<Player*>::const_iterator p = this->players().begin();
         p != this->players().end();
         ++p)
      if (this->teaminfo(**p) == TEAM::CONTRA)
        contra_no += 1;

    if (contra_no == this->number_of_players() - 1)
      solved = true;
  } // check whether all other players are known to be contra

  if (solved) {
    this->marriage_selector_ = MARRIAGE_SELECTOR::TEAM_SET;
    this->marriage_determination_trickno_ = this->trick_current_no();

    if (!this->isvirtual()) {
      for (vector<Player*>::const_iterator p = this->players().begin();
           p != this->players().end();
           ++p)
        ::ui->teaminfo_changed(**p);
    } // if (!this->isvirtual())
  } // if (solved)

  return ;
} // void Game::determine_silent_marriage()

/**
 ** test, if the marriage is solved
 **
 ** @param	-
 **
 ** @return	
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **/
void
Game::determine_marriage()
{
  if (this->is_undetermined_marriage()) {

    // whether the player has to play alone
    // note that 'trick_current_no()' begins at '0'
    // but 'MARRIAGE_DETERMINATION' at '1'
    bool alone = (this->trick_current_no()
                  >= this->rule()(Rule::MARRIAGE_DETERMINATION));

    if (   this->rule()(Rule::MARRIAGE_FIRST_ONE_DECIDES)
        && is_selector(this->trick_current().startcard().tcolor(),
                       this->marriage_selector())
        && (this->trick_current().winnerplayer() == this->soloplayer()) )
      alone = true;


    if (   is_selector(this->trick_current().startcard().tcolor(),
                       this->marriage_selector())
        && (this->trick_current().winnerplayer() != this->soloplayer()) ) {
      // set the teams
      for (vector<Player*>::const_iterator player = this->players_begin();
           player != this->players_end();
           player++) {
        this->set_teaminfo(**player,
                           ((   (**player == this->soloplayer())
                             || (**player
                                 == this->trick_current().winnerplayer()))
                            ? TEAM::RE
                            : TEAM::CONTRA));
        (*player)->set_team(this->teaminfo(**player));
      }
      this->marriage_selector_ = MARRIAGE_SELECTOR::TEAM_SET;
      this->marriage_determination_trickno_ = this->trick_current_no();
      this->human_teaminfo_update();
      // update the teaminfo of the players
      for (vector<Player*>::iterator player = this->players().begin();
           player != this->players().end();
           player++)
        (*player)->marriage(this->soloplayer(),
                            this->trick_current().winnerplayer());

      if (!this->isvirtual()) {
        ::ui->marriage(this->soloplayer(),
                       this->trick_current().winnerplayer());
        ::ui->gameplay_action(GameplayAction::Marriage(this->no(this->soloplayer()),
                                                       this->no(this->trick_current().winnerplayer())));
      }

      alone = false;
    } // if selected trick is caught


    if (   (this->marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)
        && !alone) {
      // test, whether there is a remaining card of the color
      bool has_card = false; // whether a player has a card of the color
      for (vector<Player*>::const_iterator player = this->players_begin();
           player != this->players_end();
           player++) {
        switch(this->marriage_selector()) {
        case MARRIAGE_SELECTOR::SILENT:
        case MARRIAGE_SELECTOR::TEAM_SET:
        case MARRIAGE_SELECTOR::FIRST_FOREIGN:
          has_card = true;
          break;
        case MARRIAGE_SELECTOR::FIRST_TRUMP:
          if ((*player)->hand().contains(Card::TRUMP))
            has_card = true;
          break;
        case MARRIAGE_SELECTOR::FIRST_COLOR:
          if ((*player)->hand().contains(Card::CLUB)
              || (*player)->hand().contains(Card::SPADE)
              || (*player)->hand().contains(Card::HEART))
            has_card = true;
          break;
        case MARRIAGE_SELECTOR::FIRST_CLUB:
          if ((*player)->hand().contains(Card::CLUB))
            has_card = true;
          break;
        case MARRIAGE_SELECTOR::FIRST_SPADE:
          if ((*player)->hand().contains(Card::SPADE))
            has_card = true;
          break;
        case MARRIAGE_SELECTOR::FIRST_HEART:
          if ((*player)->hand().contains(Card::HEART))
            has_card = true;
          break;
        } // switch (this->marriage_selector())
      } // for (p < this->number_of_players())

      if (!has_card)
        alone = true;
    } // if (!alone)

    if (alone) {
      this->marriage_determination_trickno_ = this->trick_current_no();
      this->set_type(GAMETYPE::MARRIAGE_SOLO);
      for (vector<Player*>::const_iterator player = this->players_begin();
           player != this->players_end();
           player++) {
        this->set_teaminfo(**player,
                           (  (**player == this->soloplayer())
                            ? TEAM::RE
                            : TEAM::CONTRA));
        (*player)->set_team(this->teaminfo(**player));
      } // for (player)
      this->marriage_selector_ = MARRIAGE_SELECTOR::TEAM_SET;
      // update the teaminfo of the players
      for (vector<Player*>::iterator player = this->players().begin();
           player != this->players().end();
           player++)
        (*player)->marriage(this->soloplayer(), this->soloplayer());

      if (!this->isvirtual()) {
        ::ui->marriage(this->soloplayer(), this->soloplayer());
        ::ui->gameplay_action(GameplayAction::Marriage(this->no(this->soloplayer()),
                                                       this->no(this->soloplayer()))
                             );
      }
    } // if (this->acttrick() >= this->rule()(Rule::MARRIAGE_DETERMINATION))

    // it can be, that the team of the swines owner or the first fox catcher
    // has changed
    if (this->rule()(Rule::SWINE_ONLY_SECOND)
        && this->swines_owner()
        && this->first_fox_catcher()
        && (this->marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET)){
      this->swines_owner_->hand_sort();
      if (!this->isvirtual())
        ::ui->redraw_all();
    } // if (first fox of swines in trick)
  } // if (type() == GAMETYPE::MARRIAGE)

  return ;
} // void Game::determine_marriage()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the game is a marriage and undetermined
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
bool
Game::is_undetermined_marriage() const
{
  return (   (this->type() == GAMETYPE::MARRIAGE)
          && (this->marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET));
} // bool Game::is_undetermined_marriage() const
