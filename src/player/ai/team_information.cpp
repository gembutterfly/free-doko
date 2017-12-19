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

#include "team_information.h"
#include "team_information.heuristics.h"

#include "constants.h"

#include "ai.h"
#include "cards_information.of_player.h"

#include "../../card/trick.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/gameplay.h"
#include "../../misc/setting.h"
#include "../../ui/ui.h"

#ifdef CHECK_RUNTIME
#include "../../runtime.h"
#endif

// For configuration values search for '*Value*'

/*
 * TeamInformation handles the information a player has which team the other
 * players are / can be.
 * If the team of a player is not known, the actions of the player are rated
 * (pro/contra 're'/'contra'). The rating of the unknown players are compared
 * and if they do not lie close together the teams are set to the 'maybe' team.
 *
 * Certain information:
 * the certain information is (mostly) taken from 'game', but I will lay down
 * here the ideas.
 * The only interesting case is a normal game, in every other case the team
 * information is known (marriage is a special case till the determination).
 * - announcement 're'/'contra'
 *   the team of the player is 're'/'contra'
 * - playing of a club queen
 *   the team of the player is 're'
 * - not serving trump
 *   the team of the player is 'contra' if it is not already 're'
 * - information from 'CardsInformation'
 *   if 'CardsInformation' can say something about having / not having the club
 *   queen we add this information.
 * - counting 're's and 'contra's
 *   if there are two re players, the remaining players must be contra
 *   if there are three contra players, the remaining player must be re
 *     (remember the silent marriage!)
 *   if there are two contra players, the player itself is re and plays no
 *     silent marriage, the remaining player is re
 *
 * Uncertain information:
 * According to some heuristics (see 'TeamInformationHeuristic') the actions
 * of the players are rated (here we assume the other players do play
 * intelligently). Positive means pro re, negative means pro contra.
 * When the rating of the unknown players differ enough the player(s) with the
 * higher value is set to 'maybe re', the other(s) to 'maybe contra'
 * (see 'TeamInformation::update_team()' for more explicit information).
 * I do also check for a silent marriage:
 *   If there are already two contra and one re player, the remaining player is
 *   generally set to 'maybe re', but if he has a strong negative value he is
 *   also set to 'maybe contra'.
 */

/**
 ** constructor
 **
 ** @param    player   	corresponding player
 **
 ** @return   -
 **/
TeamInformation::TeamInformation(Ai const& player) :
  player_(&player),
  team_values_(player.game().playerno(), 0),
  teams_(player.game().playerno(), TEAM::UNKNOWN),
  known_teams_no_(0)
{ this->reset(); }

/**
 ** destructor
 **
 ** @param    -
 **
 ** @return   -
 **/
TeamInformation::~TeamInformation()
{ }

/**
 ** writes 'team_information' in 'ostr'
 **
 ** @param    ostr               output stream
 ** @param    team_information   object to write into 'ostr'
 **
 ** @return   the output stream
 **/
ostream&
operator<<(ostream& ostr, TeamInformation const& team_information)
{
  team_information.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, TeamInformation team_information)

/**
 ** writes 'team_information' in 'ostr'
 **
 ** @param    ostr   output stream
 **
 ** @return   -
 **/
void
TeamInformation::write(ostream& ostr) const
{
  ostr << "team information:\n"
    << "{\n";

  for (unsigned p = 0; p < this->game().playerno(); ++p)
    ostr << p << ": "
      << setw(5)
      << this->team_value(p) << ' '
      << this->team(p)
      << '\n';

  ostr << "}\n";
} // void TeamInformation::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the corresponding player
 **/
Ai const&
TeamInformation::player() const
{ return *this->player_; }

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   values for the team: positive -> re, negative -> contra
 **/
vector<int> const&
TeamInformation::team_values() const
{ return this->team_values_; }

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the team (including maybe)
 **/
vector<Team> const&
TeamInformation::teams() const
{ return this->teams_; }

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the number of known teams (including the own)
 **/
unsigned
TeamInformation::known_teams_no() const
{ return this->known_teams_no_; }


/**
 ** changes the corresponding player
 **
 ** @param    player   new player
 **
 ** @return   -
 **/
void
TeamInformation::set_player(Ai const& player)
{
  this->player_ = &player;
} // void TeamInformation::set_player(Ai player)

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the corresponding game
 **/
Game const&
TeamInformation::game() const
{
  return this->player().game();
}

/**
 ** -> result
 **
 ** @param    player   player
 **
 ** @return   the team value of the player (can be maybe)
 **/
int
TeamInformation::team_value(Player const& player) const
{
  return this->team_value(player.no());
}

/**
 ** -> result
 **
 ** @param    playerno   player number
 **
 ** @return   the team value of the player (can be maybe)
 **/
int
TeamInformation::team_value(unsigned const playerno) const
{
  if (::game_status >= GAMESTATUS::GAME_PLAY)
    return (this->team_values_[playerno]
            + this->player().cards_information().of_player(playerno).weighting(Card::CLUB_QUEEN, false) / 10);
  else
    return this->team_values_[playerno];
} // int TeamInformation::team_value(unsigned playerno) const

/**
 ** -> result
 **
 ** @param    player   player
 **
 ** @return   the team of the player (can be maybe)
 **/
Team const&
TeamInformation::team(Player const& player) const
{
  return this->teams_[player.no()];
} // Team const& TeamInformation::team(Player const& player) const

/**
 ** -> result
 **
 ** @param    playerno   player number
 **
 ** @return   the team of the player (can be maybe)
 **/
Team const&
TeamInformation::team(unsigned const playerno) const
{
  return this->teams_[playerno];
} // Team const& TeamInformation::team(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param    player   player
 **
 ** @return   the team of the player (can be maybe)
 **/

Team&
TeamInformation::team(Player const& player)
{
  return this->teams_[player.no()];
}

/**
 ** -> result
 **
 ** @param    playerno   player number
 **
 ** @return   the team of the player (can be maybe)
 **/
Team&
TeamInformation::team(unsigned const playerno)
{
  return this->teams_[playerno];
}

/**
 ** -> result
 **
 ** @param    player   player
 **
 ** @return   the guessed team of the player (cannot be maybe)
 **/

Team
TeamInformation::guessed_team(Player const& player) const
{
  return this->guessed_team(player.no());
}

/**
 ** -> result
 **
 ** @param    playerno   player number
 **
 ** @return   the guessed team of the player (cannot be maybe)
 **/
Team
TeamInformation::guessed_team(unsigned const playerno) const
{
  if (this->team(playerno) == TEAM::UNKNOWN)
    return TEAM::opposite(this->player().team());
  else
    return TEAM::maybe_to_team(this->team(playerno));
} // Team TeamInformation::guessed_team(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the guessed partner of the player
 **             NULL, if none or more than one found
 **/
Player const*
TeamInformation::guessed_partner() const
{
  Player const* partner = nullptr;
  // current player
  unsigned p;
  // search possible partner
  for (p = 0; p < this->game().playerno(); ++p) {
    if (   (p != this->player().no())
        && (this->guessed_team(p) == this->player().team())) {
      partner = &this->game().player(p);
      break;
    } // if (partner found)
  } // for (p < playerno)

  // check it is the only possible partner
  for (p += 1; p < this->game().playerno(); ++p) {
    if (   (p != this->player().no())
        && (this->guessed_team(p) == this->player().team())) {
      return nullptr;
    } // if (second partner found)
  } // for (p < playerno)

  return partner;
} // Player const* TeamInformation::guessed_partner() const

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the teams of the players guessed to re/contra
 **/
vector<Team>
TeamInformation::guessed_teams() const
{
  vector<Team> teams;
  for (unsigned p = 0; p < this->game().playerno(); ++p)
    teams.push_back(this->guessed_team(p));

  return teams;
} // vector<Team> TeamInformation::guessed_teams() const

/**
 ** sets the teams according to the parameter
 **
 ** @param    teams    teams
 **
 ** @return   -
 **/
void
TeamInformation::set_teams(vector<Team> const& teams)
{
  auto const teams_bak = this->teams_;

  this->teams_ = teams;

  // recalculate the number of known teams
  this->known_teams_no_ = 0;
  for (auto const& t : this->teams())
    if (::is_real(t))
      this->known_teams_no_ += 1;

  // update the ai teams information in the ui
  if (!this->game().isvirtual()
      && ::setting(Setting::SHOW_AI_INFORMATION_TEAMS)
      && (this->player().type() == Player::HUMAN)) {
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         player++)
      if (this->team(**player) != teams_bak[(*player)->no()]) {
        ::ui->teaminfo_changed(**player);
      }
  } // if (show ai teams information)
} // void set_teams(vector<Team> teams)

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   whether all teams are known
 **/
bool
TeamInformation::all_known() const
{
  return (this->known_teams_no() == this->game().playerno());
}

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the certainty of the decision,
 **             0 if at least one team not guessed,
 **             2000000 if all known
 **/
int
TeamInformation::certainty() const
{
  if (this->all_known())
    return 2000000;

  // minimal values for the team decision
  int re_min = 1000000;
  int contra_min = -1000000;
  for (unsigned p = 0; p < this->game().playerno(); ++p) {
    switch (this->team(p)) {
    case TEAM::NOTEAM:
    case TEAM::UNKNOWN:
      return 0;
    case TEAM::RE:
    case TEAM::CONTRA:
      break;
    case TEAM::MAYBE_RE:
      re_min = min(re_min, this->team_value(p));
      break;
    case TEAM::MAYBE_CONTRA:
      contra_min = max(contra_min, this->team_value(p));
      break;
    } // switch (this->team(p))
  } // for (p < this->game().playerno())

  return (re_min - contra_min);
} // int TeamInformation::certainty() const

/**
 ** the game starts
 **
 ** @param    -
 **
 ** @return   -
 **/
void
TeamInformation::game_start()
{
  this->reset();

  // set the own team
  if (this->game().type() == GAMETYPE::NORMAL)
    this->team(this->player())
      = ( (this->player().hand().numberofclubqueens() > 0)
         ? TEAM::RE
         : TEAM::CONTRA);

  this->update_teams();

  return ;
} // void TeamInformation::game_start()

/**
 ** the trick is full
 **
 ** @param      trick   full trick
 **
 ** @return     -
 **
 ** @todo       perhaps check whether any other player has had more information
 **             than we thought at his turn
 **/
void
TeamInformation::trick_full(Trick const& trick)
{ 
} // void TeamInformation::trick_full(Trick trick)

/**
 ** 'card' has been played - update the information
 **
 ** @param    card   played card
 **
 ** @return   -
 **
 ** @todo     all: analyse the card and check for help/bother another player
 **/
void
TeamInformation::card_played(HandCard const& card)
{
  using namespace TeamInformationHeuristic;

  // No information from unknown cards
  if (card.is_unknown())
    return ;

  if (this->game().type() != GAMETYPE::NORMAL)
    return ;

#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai team information"].start_stop_proxy();
#endif

  // take the cards information to check
  // whether the player can still have a club queen
  if (   (this->game().type() == GAMETYPE::NORMAL)
      && !::is_real(this->team(card.player()))
      && (card != Card::CLUB_QUEEN)
      && (this->player().cards_information().of_player(card.player()).can_have(Card::CLUB_QUEEN) == 0) ) {
    this->team_values_[card.player().no()] = TEAM::CONTRA;
  }

  this->team_values_[card.player().no()]
    += TeamInformationHeuristic::card_played(card, this->game().trick_current(),
                                             this->player());

  this->update_teams();
  this->recalc_team_values();

  return ;
} // void TeamInformation::card_played(HandCard card)

/**
 ** an announcement has been made - update the information
 **
 ** @param    announcement   made announcement
 ** @param    player         player who has made the announcement
 **
 ** @return   -
 **/
void
TeamInformation::announcement_made(Announcement const& announcement,
                                   Player const& player)
{
  this->update_teams();
} // void TeamInformation::announcement_made(Announcement announcement, Player player)

/**
 ** the marriage is determined
 ** if the bride is the bridegroom, the bridegroom must play a solo
 **
 ** @param    bridegroom   the player with the marriage
 ** @param    bride        the bride
 **
 ** @return   -
 **/
void
TeamInformation::marriage(Player const& bridegroom, Player const& bride)
{
  for (unsigned p = 0; p < this->game().playerno(); ++p)
    this->team(p) = (   (p == bridegroom.no())
                     || (p == bride.no())
                     ? TEAM::RE
                     : TEAM::CONTRA );

  this->known_teams_no_ = this->game().playerno();
} // void TeamInformation::marriage(Player bridegroom, Player bride)

/**
 ** player 'genscher' has a genscher
 **
 ** @param    genscher   player with the genscher
 ** @param    partner    selected partner
 **
 ** @return   -
 **/
void
TeamInformation::genscher(Player const& genscher, Player const& partner)
{
  for (unsigned p = 0; p < this->game().playerno(); ++p)
    this->team(p) = (   (p == genscher.no())
                     || (p == partner.no())
                     ? TEAM::RE
                     : TEAM::CONTRA );

  this->known_teams_no_ = this->game().playerno();
} // void TeamInformation::genscher(Player genscher, Player partner)

/**
 ** reset all
 **
 ** @param    -
 **
 ** @return   -
 **/
void
TeamInformation::reset()
{
  this->team_values_ = vector<int>(this->game().playerno(), 0); 
  this->teams_ = vector<Team>(this->game().playerno(), TEAM::UNKNOWN); 
  this->known_teams_no_ = 0;

  return ;
} // void TeamInformation::reset()

/**
 ** updates the teams information
 **
 ** @param    -
 **
 ** @return   -
 **/
void
TeamInformation::update_teams()
{
  using namespace TEAM;

  if (   this->all_known()
      && !this->game().isvirtual())
    // all teams are known
    return ;

#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai team information"].start_stop_proxy();
#endif


  // the old value of the teams
  auto const teams_bak = this->teams();

  // in an undetermined marriage set the contra players to 'maybe contra'
  if (this->game().is_undetermined_marriage()) {
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         ++player) {
      if (this->game().is_soloplayer(**player))
        this->team(**player) = RE;
      else if (*player == &this->player())
        this->team(**player) = MAYBE_CONTRA;
      else
        this->team(**player) = MAYBE_RE;
    }
    return ;
  } // if (undetermined marriage)


  // Update with the teaminfo of 'game'
  // and add information from 'CardsInformation'.
  // Also count the 're's and 'contra's
  unsigned re_no = 0; // number of re
  unsigned contra_no = 0; // number of re
  for (auto player = this->game().players_begin();
       player != this->game().players_end();
       ++player) {

    // information from the game
    this->team(**player) = this->game().teaminfo(**player);

    // the player knows his own team
    if (   !::is_real(this->team(**player))
        && (*player == &this->player()) )
      this->team(**player) = this->player().team();

    // use also the cards information
    if (!::is_real(this->team(**player))) {
      if (this->player().value(Aiconfig::HANDS_KNOWN)) {
        // If the hands are known, the checks are made against trump, not against the club queen. Else, the team would be known from the start on.
        if (this->player().cards_information().of_player(**player).played(Card::CLUB_QUEEN))
          this->team(**player) = RE;
        else if (   this->player().cards_information().of_player(**player).does_not_have(Card::TRUMP)
                 && !this->player().cards_information().of_player(**player).played(Card::CLUB_QUEEN))
          // Bug: as soon as the player has played his last trump, his team is known. But without the hands known, it would be the first time, he has not serverd trump.
          this->team(**player) = CONTRA;
      } else { // if !(this->player().value(Aiconfig::HANDS_KNOWN))
        if (   this->player().cards_information().of_player(**player).must_have(Card::CLUB_QUEEN)
            || this->player().cards_information().of_player(**player).played(Card::CLUB_QUEEN))
          this->team(**player) = RE;
        else if (   this->player().cards_information().of_player(**player).cannot_have(Card::CLUB_QUEEN)
                 && !this->player().cards_information().of_player(**player).played(Card::CLUB_QUEEN))
          this->team(**player) = CONTRA;
      } // if !(this->player().value(Aiconfig::HANDS_KNOWN))
    } // if (!::is_real(this->team(**player)))

    switch (this->team(**player)) {
    case RE:
      re_no += 1;
      break;
    case CONTRA:
      contra_no += 1;
      break;
    default:
      break;
    } // switch (this->team(**player)
  }

  // check whether all teams are known
  this->known_teams_no_ = re_no + contra_no;

  if (this->known_teams_no() < this->game().playerno()) {
    // all not re players must be contra
    if (re_no == this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM)) {
      for (unsigned p = 0; p < this->game().playerno(); ++p)
        if (this->team(p) != RE)
          this->team(p) = CONTRA;
      this->known_teams_no_ = this->game().playerno();
    } // if (re_no == this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM))
  } // all not re players must be contra

  if (this->known_teams_no() < this->game().playerno()) {
    // there must be at least one re player
    if (contra_no == this->game().playerno() - 1) {
      for (unsigned p = 0; p < this->game().playerno(); ++p)
        if (this->team(p) != CONTRA)
          this->team(p) = RE;
      this->known_teams_no_ = this->game().playerno();
    } // if (contra_no == this->game().playerno() - 1)
  } // there must be at least one re player

  if (this->known_teams_no() < this->game().playerno()) {
    // there must be another re player but me
    if (   this->player_
        && (contra_no == this->game().playerno() - 2)
        && (this->team(this->player()) == RE)
        && (this->player().hand().numberofall(Card::CLUB_QUEEN)
            < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
       ) {
      for (unsigned p = 0; p < this->game().playerno(); ++p)
        if (this->team(p) != CONTRA)
          this->team(p) = RE;
      this->known_teams_no_ = this->game().playerno();
    } // if (last unknown player must be re)
  } // there must be another re player but me

  // set the team of the players to maybe according to the team value
  if (this->known_teams_no() < this->game().playerno()) {
    switch (this->known_teams_no()) {
    case 0:
    case 1: { // (almost) no information
      // Idea:
      // |  re  | unknown | contra |
      // with enough distance between the two extreme values

      int min_value = INT_MAX;
      int max_value = INT_MIN;
      for (unsigned p = 0; p < this->game().playerno(); ++p) {
        if (!::is_real(this->team(p))) {
          if (this->team_value(p) < min_value)
            min_value = this->team_value(p);
          if (this->team_value(p) > max_value)
            max_value = this->team_value(p);
        }
      } // for (p)

      if (max_value - min_value < 15) // *Value*
        break;

      // count how many possible contra and re there are
      for (unsigned p = 0; p < this->game().playerno(); ++p) {
        if (!::is_real(this->team(p))) {
          if (this->team_value(p) >= max_value - (max_value - min_value) / 3)
            re_no += 1;
          else if (this->team_value(p) <= min_value + (max_value - min_value) / 3)
            contra_no += 1;
        }
      } // for (p)

      if (   (contra_no <= this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM))
          && (re_no <= this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM)) ) {
        for (unsigned p = 0; p < this->game().playerno(); ++p) {
          if (!::is_real(this->team(p))) {
            if (this->team_value(p)
                >= max_value - (max_value - min_value) / 3)
              this->team(p) = TEAM::MAYBE_RE;
            else if (this->team_value(p)
                     <= min_value + (max_value - min_value) / 3)
              this->team(p) = TEAM::MAYBE_CONTRA;
          }
        } // for (p)
      } // if (contra_no, re_no <= 2)

      break;
    } // case 0, 1

    case 2: {
      switch (contra_no) {
      case 0: // contra_no
        DEBUG_ASSERTION(false,
                        "TeamInformation::update_teams()\n"
                        "  contra no = 0");
        break;
      case 1: { // contra_no
        // one re and one contra player are known
        int min_value = INT_MAX;
        int max_value = INT_MIN;
        for (unsigned p = 0; p < this->game().playerno(); ++p) {
          if (!::is_real(this->team(p))) {
            if (this->team_value(p) < min_value)
              min_value = this->team_value(p);
            if (this->team_value(p) > max_value)
              max_value = this->team_value(p);
          }
        } // for (p)

        // enough difference to set the teams
        if (max_value - min_value < (this->player().value(Aiconfig::TRUSTING)
                                     ? 10
                                     : 20) ) // *Value*
          break;

        for (unsigned p = 0; p < this->game().playerno(); ++p) {
          if (!::is_real(this->team(p))) {
            if (this->team_value(p) == max_value)
              this->team(p) = TEAM::MAYBE_RE;
            else // == min_value
              this->team(p) = TEAM::MAYBE_CONTRA;
          }
        }
        break;
      } // one re and one contra player are known
      case 2: // contra_no
        // both other players are re
        // but in the case that someone plays a silent marriage
        for (unsigned p = 0; p < this->game().playerno(); ++p)
          if (!::is_real(this->team(p))) {
            // I think here we can be very strict
            if (this->team_value(p) >= -5) // *Value*
              this->team(p) = TEAM::MAYBE_RE;
            else if (this->team_value(p) <= -25) // *Value*
              this->team(p) = TEAM::MAYBE_CONTRA;
          } // switch(contra_no)

        break;
      default:
        break;
      } // switch (contra_no)

      break;
    } // case 2

    case 3: {
      // this case can only be when there are two contra and one re,
      // so the question is whether the re player plays a silent marriage.

      Player const* re_player = NULL;
      for (unsigned p = 0; p < this->game().playerno(); ++p) {
        if (this->team(p) == TEAM::RE) {
          re_player = &this->game().player(p);
          break;
        }
      }
      DEBUG_ASSERTION(re_player != NULL,
                      "TeamInformation::update_team()\n"
                      "  three known players but no re player\n");
      for (unsigned p = 0; p < this->game().playerno(); ++p)
        if (!::is_real(this->team(p))) {
          // I think here we can be very strict
          if (   (this->team_value(p) >= -20) // *Value*
              || (this->game().points_of_player(*re_player)
                  < this->game().played_points() / 2) )
            this->team(p) = TEAM::MAYBE_RE;
          else
            this->team(p) = TEAM::MAYBE_CONTRA;
          break;
        }
      break;
    } // case 3

    case 4:
      DEBUG_ASSERTION(false,
                      "TeamInformation::update_teams()\n"
                      "  should have left the method earlier because all teams "
                      "are known:\n"
                      "  known teams no = " << this->known_teams_no()
                      << " == " << this->game().playerno()
                      << " = player number in game");
      break;

    default:
      DEBUG_ASSERTION(false,
                      "TeamInformation::update_teams()\n"
                      "  known teams number is '" << this->known_teams_no()
                      << "' > 4 that is the maximum (players in game = "
                      << this->game().playerno());
      break;
    } // switch (this->known_teams_no())
  } // if (this->known_teams_no() < this->game().playerno())

  { // check for two re players

    unsigned re_no = 0;
    unsigned contra_no = 0;
    // count the players of the team
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         player++) {
      if (::maybe_to_team(this->team(**player)) == TEAM::RE)
        re_no += 1;
      else if (::maybe_to_team(this->team(**player)) == TEAM::CONTRA)
        contra_no += 1;
    } // for (player \in this->game().players)

    if (   (re_no + contra_no < this->game().playerno())
        && (re_no == this->game().rule()(Rule::NUMBER_OF_PLAYERS_PER_TEAM)) ) {
      // all unknown players must be contra
      for (auto player = this->game().players_begin();
           player != this->game().players_end();
           player++)
        if (this->team(**player) == TEAM::UNKNOWN)
          this->team(**player) = TEAM::MAYBE_CONTRA;
    } // if (all re players known
  } // check for two re players
  // update the ai teams information in the ui
  if (!this->game().isvirtual()
      && ::setting(Setting::SHOW_AI_INFORMATION_TEAMS)
      && (this->player().type() == Player::HUMAN)) {
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         player++)
      if (this->team(**player) != teams_bak[(*player)->no()]) {
        ::ui->teaminfo_changed(**player);
      }
  } // if (show ai teams information)

  return ;
} // void TeamInformation::update_teams()

/**
 ** updates the estimated hands
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo      work in progress
 **/
void
TeamInformation::recalc_team_values()
{
  return ;


  if (this->all_known())
    return ;

  if (this->in_recalcing_)
    return ;

  auto const lv = set_local_value(this->in_recalcing_, true);

#ifdef CHECK_RUNTIME
  auto const ssp = (  (this == &this->player().team_information())
                    ? ::runtime["ai recalc team weightings"].start_stop_proxy()
                    : nullptr);
#endif
  // overview
  // * Rebuild a new team information for the start of the game
  // * Replay the game and update the weighting (using the information of further played cards)

  // create a virtual game

  // virtual players
  vector<Player*> player_virt;
  Ai* ai = this->player().Ai::clone();
  {
    // Create new players.
    // The hand is set during the recursion.
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         player++) {
      if (**player == this->player())
        player_virt.push_back(ai);
      else
        player_virt.push_back(this->player().Player::clone());
      player_virt.back()->set_name((*player)->name());
      player_virt.back()->set_team(this->player().team_information().guessed_team(player_virt.size() - 1));
    } // for (player)

  } // create a virtual game
  Game virt_game(this->game(), player_virt);
  virt_game.reset_to_first_trick();
  // set the cards information
  ai->cards_information().reset();
  CLOG << virt_game;
  CLOG << ai->cards_information();
  for (unsigned p = 0; p < virt_game.playerno(); ++p) {
    ai->cards_information().of_player(p).add_must_have(this->player().cards_information().of_player(p).played_cards());
  }

  // build a new cards information for the start of the game
  TeamInformation team_information(*player_virt[this->game().no(this->player())]);

  // replay the game (with announcements!)
  virt_game.play(this->game().gameplay());

  this->team_values_ = ai->team_information().team_values();
  for (auto& p : player_virt)
    delete p;
} // void TeamInformation::recalc_team_values()

/**
 ** -> result
 **
 ** @param    team_information_a   first object
 ** @param    team_information_b   second object
 **
 ** @return   whether the two objects are equal (the game may differ)
 **/
bool
operator==(TeamInformation const& team_information_a,
           TeamInformation const& team_information_b)
{
  return (   (team_information_a.team_values_
              == team_information_b.team_values_)
          && (team_information_a.teams_
              == team_information_b.teams_) );
} // bool operator==(TeamInformation team_information_a, TeamInformation team_information_b)

/**
 ** -> result
 **
 ** @param    team_information_a   first object
 ** @param    team_information_b   second object
 **
 ** @return   whether the two objects are different
 **/
bool
operator!=(TeamInformation const& team_information_a,
           TeamInformation const& team_information_b)
{
  return !(team_information_a == team_information_b);
} // bool operator!=(TeamInformation team_information_a, TeamInformation team_information_b)
