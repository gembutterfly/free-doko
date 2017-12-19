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

#include "gameplay_actions/announcement.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../card/trick.h"
#include "../ui/ui.h"
#ifdef USE_NETWORK
#include "../network/server.h"
#endif

/**
 **
 ** -> result
 **
 ** @param	team	the team
 **
 ** @return	the announcement of the team 'team'
 **
 ** @version	0.6.1
 **
 ** @author	Diether Knof
 **
 **/
Game::AnnouncementWithTrickno
Game::announcement_of_team(Team const& team) const
{                                                                          
  DEBUG_ASSERTION(((team == TEAM::RE)
		   || (team == TEAM::CONTRA)
                   || this->is_undetermined_marriage()),
		  "Game::announcement_of_team(team)\n"
		  "  invalid team = " << team);

  // In an undetermined marriage the contra team cannot have announced.
  // This case is treated here, because in this case there does not
  // exists a player of the team 'contra' so the assertion later on would fail.
  if (   this->is_undetermined_marriage()
      && (team == TEAM::CONTRA) )
    return AnnouncementWithTrickno();

  // the player of the team, that has made the highest announcement
  Player const* player = NULL;

  // take the highest announcement
  for(vector<Player*>::const_iterator p = this->players().begin();
      p != this->players().end();
      p++) {
    if (   this->rule()(Rule::KNOCKING)
        && !TEAM::is_real(this->teaminfo(**p)))
      continue;

    if ((*p)->team() == team)
      if (!player
          || (this->announcement_of_player(**p).announcement
              > this->announcement_of_player(*player).announcement))
        player = *p;
  } // for (p)

  if (   this->rule()(Rule::KNOCKING)
      && (player == NULL))
    return AnnouncementWithTrickno();

  if (player == NULL) {
#ifdef USE_NETWORK
    if (!::server && !::server->has_parent_connection())
#endif
      DEBUG_ASSERTION(this->isvirtual(),
                      "Game::announcement_of_team(team):\n"
                      "  no player of team '" << team << "' found!");
    return AnnouncementWithTrickno();
  }

  return this->announcement_of_player(*player);
} // Game::AnnouncementWithTrickno Game::announcement_of_team(Team const& team) const

/**
 ** -> result
 **
 ** @param	playerno   the number of the player
 **
 ** @return	the highest announcement of player 'playerno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Game::AnnouncementWithTrickno const&
Game::announcement_of_player(unsigned const playerno) const
{                                                                          
  DEBUG_ASSERTION((playerno < this->number_of_players()),
                  "Game::announcement_of_player(playerno)\n"
                  << "  illegal playerno = " << playerno);

  return this->announcement_[playerno].back();
} // Game::AnnouncementWithTrickno const& Game::announcement_of_player(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	the player
 **
 ** @return	the highest announcement of player 'player'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
Game::AnnouncementWithTrickno const&
Game::announcement_of_player(Player const& player) const
{                                                                          
  DEBUG_ASSERTION((this->no(player) < this->number_of_players()),
                  "Game::announcement_of_player(player)\n"
                  << "  illegal playerno of player " << player.name() << " = " << this->no(player));

  return this->announcement_[this->no(player)].back();
} // Game::AnnouncementWithTrickno const& Game::announcement_of_player(Player const& player) const

/**
 **
 ** -> result
 **
 ** @param	player	the player
 **
 ** @return	all announcements of player 'player'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **
 **/
vector<Game::AnnouncementWithTrickno> const&
Game::announcements_of_player(Player const& player) const
{
  DEBUG_ASSERTION((this->no(player) < this->number_of_players()),
                  "Game::announcement_of_player(player)\n"
                  << "  illegal playerno of player " << player.name() << " = " << this->no(player));

  return this->announcement_[this->no(player)];
} // vector<Game::AnnouncementWithTrickno> const& Game::announcements_of_player(Player const& player) const

/**
 ** request announcements from the players
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Game::announcements_request()
{
  bool again = true; // whether to ask the players again

  while (again) {
    again = false;

    // ask for announcements
    for (vector<Player*>::const_iterator
         p = this->players().begin();
         p != this->players().end();
         p++) {
      Player& player = **p;
      if (this->valid_announcement(player)) {
        Announcement const announcement = player.announcement_request();
        if (this->announcement_valid(announcement, player)) {
          this->make_announcement(announcement, player);
          if (this->announcement_of_player(player) == announcement)
            again = true;
        } // if (this->announcement_valid(announcement, player))
      } // if (this->valid_announcement(*p))
    } // for (p < this->number_of_players())
  } // while (again)

  return ;
} // void Game::announcements_request()

/**
 ** -> result
 **
 ** @param     player   player to check
 **
 ** @result    valid announcement for 'player'
 **            NOANNOUNCEMENT, if 'player' cannot announce anymore
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Announcement
Game::valid_announcement(Player const& player) const
{
  if (!TEAM::is_real(player.team()))
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  if (ANNOUNCEMENT::is_real(this->announcement_of_team(TEAM::opposite(player.team())))
      && this->announcement_valid(ANNOUNCEMENT::to_reply(this->announcement_of_team(TEAM::opposite(player.team()))), player))
    return ANNOUNCEMENT::to_reply(this->announcement_of_team(TEAM::opposite(player.team())));

  if (this->announcement_valid(player.next_announcement(), player))
    return player.next_announcement();

  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement Game::valid_announcement(Player player) const


/**
 **
 ** 'player' announces 'announcement'
 **
 ** @param	announcement	announcement made
 ** @param	player		player who announces
 **
 ** @result	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
void
Game::make_announcement(Announcement announcement,
                        Player const& player)
{
  // test whether the announcement is valid
  // when necessary, a 're' is changed to a 'reply'
  announcement = announcement_valid(announcement, player);
  if (!announcement)
    return ;

#ifdef USE_NETWORK
  // check, that the server has send the announcement
  if (   (::server->has_parent_connection())
      && (announcement > this->announcement_over_network_[this->no(player)]) ) {
    // send the announcement (request) over the network
    ::server->gameplay_action(GameplayAction::Announcement(this->no(player),
                                                           announcement));
    return ;
  } // if (::server->has_parent_connection())
#endif

  // set the announcement
  this->announcement_[this->no(player)].push_back(AnnouncementWithTrickno(announcement, this->trick_current_no()));
  // update the team info
  if (!this->rule()(Rule::KNOCKING))
    this->set_teaminfo(player, player.team());

  // tell all players, that the announcement is made
  for(vector<Player*>::const_iterator p = this->players().begin();
      p != this->players().end();
      p++)
    (*p)->announcement_made(announcement, player);

  // tell the ui, that the announcement is made
  if (!this->isvirtual()) {
    ::ui->announcement_made(announcement, player);
    ::ui->gameplay_action(GameplayAction::Announcement(this->no(player),
                                                       announcement));
  }

  return ;
} // void Game::make_announcement(Announcement announcement, Player player)


#ifdef USE_NETWORK
/**
 ** make an announcement from the network
 ** This method is used, so that the server checks, whether an announcement
 ** is valid. So when an announcement is made locally, it is pushed to the
 ** server. The server checks and sends the announcement over the network.
 ** Then the client gets the announcement from the server and can announce.
 **
 ** @param	announcement	announcement made
 ** @param	player		player who announces
 **
 ** @result	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **
 ** @todo       fix it
 **/
void
Game::make_announcement_from_network(Announcement announcement,
                                     Player const& player)
{
  DEBUG_ASSERTION((this->announcement_over_network_[this->no(player)]
                   <= announcement),
                  "Game::make_announcement_from_network(announcement, player)\n"
                  "  announcement from the network = "
                  << this->announcement_over_network_[this->no(player)] 
                  << " >= " << announcement
                  << " = announcement to make");
  // Probleme:
  // * Server hört nicht (läßt sich z.B. noch den Spieltyp anzeigen)
  // * Ansagen werden vom Server nicht übernommen/akzeptiert,
  //   Beispiel, wenn der Server noch das Anzeigefenster offen hat und der
  //   Client schon seine nächste Ansage tätigt.
  //
  // * Umgehung: Ansage nur zulassen, wenn Stich voll ist oder der Spieler selber am Zug ist.

#ifdef DKNOF
  COUT << (::server->has_parent_connection() ? "child" : "parent")
    << ": make announcement from network: "
    << player.name() << ": " << announcement << endl;
  COUT << "valid: " << this->announcement_valid(announcement, player) << endl;
#endif

  this->announcement_over_network_[this->no(player)] = announcement;
  this->make_announcement(announcement, player);

  return ;
} // void Game::make_announcement_from_network(Announcement announcement, Player player)
#endif // #ifdef USE_NETWORK

/**
 ** -> result
 **
 ** @param	announcement	the announcement
 ** @param	player		the player, who asks to make the announcement
 **
 ** @return	the announcement, if it is valid (but 'reply' for 're', if it is only allowed as reply)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
Game::announcement_valid(Announcement announcement,
                         Player const& player) const
{
  // test, whether status is in a game
  if ( !::in_running_game())
    return ANNOUNCEMENT::NOANNOUNCEMENT;
  if (this->tricks_.empty())
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  // no announcement after the 'Rule::ANNOUNCEMENT_LAST' trick
  if (this->tricks_remaining_no()
      < (this->rule()(Rule::ANNOUNCEMENT_LAST)
         + (this->trick_current().isfull() ? 1 : 0)))
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  // as long as the marriage is not determined, no announcement is valid
  if (   (this->marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)
      && (this->marriage_selector() != MARRIAGE_SELECTOR::SILENT))
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  // unknown teams can appear in a network game
  if (player.team() == TEAM::UNKNOWN)
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  Announcement const& last_announcement
    = ( (   this->rule()(Rule::KNOCKING)
         && GAMETYPE::is_with_unknown_teams(this->type()))
       ? this->announcement_of_player(player).announcement
       : this->announcement_of_team(player.team()).announcement);

  if (ANNOUNCEMENT::is_reply(last_announcement)) {
    return ANNOUNCEMENT::NOANNOUNCEMENT;
  }

  // change the announcement
  if (announcement == ANNOUNCEMENT::REPLY) {
    if (   (this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT))
        && ANNOUNCEMENT::is_real(this->announcement_of_team(opposite(player.team())).announcement) ) {
      announcement
        = ANNOUNCEMENT::to_reply(this->announcement_of_team(::opposite(player.team())
                                                           ).announcement);
    } else {
      announcement = ANNOUNCEMENT::NO120;
    }
  } // if (announcement == ANNOUNCEMENT::REPLY)


  // the announcement must be a better one
  if (last_announcement >= announcement) {
    return ANNOUNCEMENT::NOANNOUNCEMENT;
  }


  unsigned const marriage_deferral
    = (  (   (this->type() == GAMETYPE::MARRIAGE)
          || (this->type() == GAMETYPE::MARRIAGE_SOLO))
       ? (this->marriage_determination_trickno()
          - 1)
       : 0);
  // whether a normal announcement is valid
  bool valid = (((this->rule()(Rule::ANNOUNCEMENT_TILL_FULL_TRICK)
                  ? this->real_tricks_remaining_no()
                  : player.hand().cardsnumber())
                 + marriage_deferral)
                >= this->rule().remaining_cards(announcement));

  // change a 're' to a 'reply', if no futher announcement can be made
  if (!valid
      && (announcement == ANNOUNCEMENT::NO120)
      && !this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT))
    announcement = ANNOUNCEMENT::REPLY;

  if (ANNOUNCEMENT::is_reply(announcement)) {
    if (!this->rule()(Rule::ANNOUNCEMENT_REPLY)) {
      return ANNOUNCEMENT::NOANNOUNCEMENT;
    }

    if (last_announcement != ANNOUNCEMENT::NOANNOUNCEMENT) {
      return ANNOUNCEMENT::NOANNOUNCEMENT;
    }

    Announcement const& opposite_announcement
      = this->announcement_of_team(opposite(player.team()));

    if (   (announcement != ANNOUNCEMENT::REPLY)
        && (ANNOUNCEMENT::from_reply(announcement)
            != opposite_announcement) ) {
      return ANNOUNCEMENT::NOANNOUNCEMENT;
    }

    // till one trick after the last possible moment
    // - I can make my announcement
    // Remark: 'this->trick_current_no()' can be different from
    //         'this->trick_.size()'
    if (((this->rule()(Rule::ANNOUNCEMENT_TILL_FULL_TRICK)
          ? this->real_tricks_remaining_no()
          : player.hand().cardsnumber())
         + marriage_deferral)
        + 1
        >= this->rule().remaining_cards(opposite_announcement)) {
      if (   (announcement == ANNOUNCEMENT::REPLY)
          && this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT))
        announcement
          = ANNOUNCEMENT::to_reply(announcement_of_team(opposite(player.team())
                                                       ).announcement);
      return announcement;
    }

    return ANNOUNCEMENT::NOANNOUNCEMENT;
  } // if (announcement == ANNOUNCEMENT::REPLY)

  // verify that all announcements before are valid
  if (   !this->rule()(Rule::ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT)
      && (announcement > ANNOUNCEMENT::NO120)
      && (last_announcement
          != ANNOUNCEMENT::previous(announcement) ) ) {
    valid = valid && (ANNOUNCEMENT::previous(announcement) ==
                      this->announcement_valid(ANNOUNCEMENT::previous(announcement), player));
  }

  return (valid ? announcement : ANNOUNCEMENT::NOANNOUNCEMENT);
} // Announcement Game::announcement_valid(Announcement const& announcement, Player const& player)

/**
 ** -> result
 **
 ** @param	announcement	the announcement
 ** @param	player		the player, who asks to make the announcement
 **
 ** @return	whether this is the last possibility to announce 'announcement'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Game::last_chance_to_announce(Announcement const& announcement,
                              Player const& player) const
{
  if (!this->announcement_valid(announcement, player))
    return false;

  unsigned const marriage_deferral = (  (this->type() == GAMETYPE::MARRIAGE)
                                      ? (this->marriage_determination_trickno()
                                         - 1)
                                      : 0);

  // re as a reply
  if (   (announcement == ANNOUNCEMENT::NO120
          && !this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT)) ) {
    Announcement const& opposite_announcement
      = this->announcement_of_team(::opposite(player.team()));
    if (opposite_announcement != ANNOUNCEMENT::NOANNOUNCEMENT)
      return (((this->rule()(Rule::ANNOUNCEMENT_TILL_FULL_TRICK)
                ? this->real_tricks_remaining_no()
                : player.hand().cardsnumber())
               + marriage_deferral) + 1
              == this->rule().remaining_cards(opposite_announcement));
  } // if (re as reply)


  return (((this->rule()(Rule::ANNOUNCEMENT_TILL_FULL_TRICK)
            ? this->real_tricks_remaining_no()
            : player.hand().cardsnumber())
           + marriage_deferral)
          == this->rule().remaining_cards(announcement));
} // bool Game::last_chance_to_announce(Announcement announcement, Player player) const
