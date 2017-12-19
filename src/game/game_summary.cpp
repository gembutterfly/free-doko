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

#include "game_summary.h"
#include "game.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../player/player.h"
#include "../card/trick.h"
#include "../utils/string.h"

/**
 ** Constructor
 **
 ** @param	game	game the summary is made of
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 ** @todo	remove const cast
 **
 **/
GameSummary::GameSummary(Game const& game) :
  rule_(&game.rule()),
  seed_(game.seed()),
  players_number_(game.number_of_players()),
  party_players_(game.number_of_players()),
  startplayer_no_(game.no(game.startplayer())),
soloplayer_no_( (game.exists_soloplayer()
                 ? game.no(game.soloplayer())
                 : UINT_MAX) ),
  game_type_(game.type()),
  duty_solo_(game.is_duty_solo()),
swines_player_no_( (game.swines_owner()
                    ? game.no(*game.swines_owner())
                    : UINT_MAX) ),
hyperswines_player_no_( (game.hyperswines_owner()
                         ? game.no(*game.hyperswines_owner())
                         : UINT_MAX) ),
  teams_(),
  highest_announcement_re_(ANNOUNCEMENT::NOANNOUNCEMENT),
  highest_announcement_contra_(ANNOUNCEMENT::NOANNOUNCEMENT),
  trick_points_(),
  winnerteam_(TEAM::NOTEAM),
  points_(UINT_MAX),
  specialpoints_(),
  bock_triggers_()
{
  for (unsigned p = 0; p < game.number_of_players(); ++p)
    this->party_players_[p] = game.party().no(game.player(p));
  this->evaluate(game);

  return ;
} // GameSummary::GameSummary(Game const& game) :

/**
 ** Constructor
 **
 ** @param	rule	the corresponding rule
 ** @param	istr	input stream to read the game from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
GameSummary::GameSummary(Rule const& rule, istream& istr) :
  rule_(&rule),
  seed_(UINT_MAX),
  players_number_(UINT_MAX),
  party_players_(),
  startplayer_no_(UINT_MAX),
  soloplayer_no_(UINT_MAX),
  game_type_(GAMETYPE::NORMAL),
  duty_solo_(false),
  swines_player_no_(UINT_MAX),
  hyperswines_player_no_(UINT_MAX),
  teams_(),
  highest_announcement_re_(ANNOUNCEMENT::NOANNOUNCEMENT),
  highest_announcement_contra_(ANNOUNCEMENT::NOANNOUNCEMENT),
  trick_points_(),
  winnerteam_(),
  points_(),
  specialpoints_(),
  bock_triggers_()
{
  this->read(istr);

  return ;
} // GameSummary::GameSummary(istream& istr) :

/**
 ** copy constructor
 **
 ** @param     game_summary   game summary to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
GameSummary::GameSummary(GameSummary const& game_summary) :
  rule_(game_summary.rule_),
  seed_(game_summary.seed_),
  players_number_(game_summary.players_number_),
  party_players_(game_summary.party_players_),
  startplayer_no_(game_summary.startplayer_no_),
  soloplayer_no_(game_summary.soloplayer_no_),
  game_type_(game_summary.game_type_),
  duty_solo_(game_summary.duty_solo_),
  swines_player_no_(game_summary.swines_player_no_),
  hyperswines_player_no_(game_summary.hyperswines_player_no_),
  teams_(game_summary.teams_),
  highest_announcement_re_(game_summary.highest_announcement_re_),
  highest_announcement_contra_(game_summary.highest_announcement_contra_),
  trick_points_(game_summary.trick_points_),
  winnerteam_(game_summary.winnerteam_),
  points_(game_summary.points_),
  specialpoints_(game_summary.specialpoints_),
  bock_triggers_(game_summary.bock_triggers_)
{ }


/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
GameSummary::~GameSummary()
{ }


/**
 ** writes the game summary into 'ostr'
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
GameSummary::write(ostream& ostr) const
{
  ostr << "seed = " << Unsigned(this->seed()) << '\n';
  ostr << "players number = " << Unsigned(this->players_number()) << '\n';
  ostr << "party players =";
  for (unsigned p = 0; p < this->party_players().size(); ++p)
    ostr << " " << this->party_players()[p];
  ostr << '\n';
  ostr << "startplayer number = " << Unsigned(this->startplayer_no()) << '\n';
  ostr << "soloplayer number = " << Unsigned(this->soloplayer_no()) << '\n';
  ostr << "gametype = " << this->game_type() << '\n';
  ostr << "duty solo = " << this->duty_solo() << '\n';
  ostr << "swines player number = " << Unsigned(this->swines_player_no()) << '\n';
  ostr << "hyperswines player number = " << Unsigned(this->hyperswines_player_no()) << '\n';
  ostr << "teams =";
  for (vector<Team>::const_iterator t = this->teams().begin();
       t != this->teams().end();
       ++t) {
    if (t != this->teams().begin())
      ostr << ',';
    ostr << ' ' << *t;
  }
  ostr << '\n';
  ostr << "highest announcement re = " << this->highest_announcement_re() << '\n';
  ostr << "highest announcement contra = " << this->highest_announcement_contra() << '\n';
  ostr << "trick points =";
  for (vector<unsigned>::const_iterator p = this->trick_points().begin();
       p != this->trick_points().end();
       ++p) {
    if (p != this->trick_points().begin())
      ostr << ',';
    ostr << ' ' << *p;
  }
  ostr << '\n';
  ostr << "winnerteam = " << this->winnerteam() << '\n';
  ostr << "points = " << this->points() << '\n';
  ostr << "special points\n"
    << "{\n"
    << this->specialpoints()
    << "}\n";
  if (!this->bock_triggers().empty()) {
    ostr << "bock triggers\n"
      << "{\n";
    for (vector<BockTrigger>::const_iterator c = this->bock_triggers().begin();
         c != this->bock_triggers().end();
         ++c)
      ostr << *c << '\n';
    ostr << "}\n";
  } // if (!this->bock_triggers().empty())

  return ;
} // void GameSummary::write(ostream& ostr) const

/**
 ** reads the game summary from 'ostr'
 **
 ** @param	istr	input stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
GameSummary::read(istream& istr)
{

  Unsigned seed = UINT_MAX;
  Unsigned players_number = UINT_MAX;
  vector<unsigned> party_players;
  Unsigned startplayer_no = UINT_MAX;
  Unsigned soloplayer_no = UINT_MAX;

  GameType game_type = GAMETYPE::NORMAL;
  bool duty_solo = false;
  Unsigned swines_player_no = UINT_MAX;
  Unsigned hyperswines_player_no = UINT_MAX;
  vector<Team> teams;
  Announcement highest_announcement_re = ANNOUNCEMENT::NOANNOUNCEMENT;
  Announcement highest_announcement_contra = ANNOUNCEMENT::NOANNOUNCEMENT;
  vector<unsigned> trick_points;
  Team winnerteam = TEAM::NOTEAM;
  int points = INT_MAX;
  Specialpointsvector specialpoints;
  vector<BockTrigger> bock_triggers;

  { // read the data
    Config config; // the config read
    int depth = 0; // the depth in the blocks
    do { // while (istr)
      istr >> config;

      if (istr.eof())
        break;

      if (istr.fail())
        break;

      if ((config.name == "") && (config.value == ""))
        break;

      if (config.value.empty()) {
        if (config.name == "{") {
          depth += 1;
        } else if (config.name == "}") {
          depth -= 1;
          // finished with the game summary
          if (depth == -1)
            istr.putback('}');
          if (depth <= 0)
            break;

        } else if (config.name == "special points") {
          string line;
          line = DK::Utils::String::remove_blanks(::read_line(istr));
          if (line != "{")
            throw ReadException("Game summary read: "
                                "special points: expected '{', "
                                " found '" + line + "'");

          while (istr.good()
                 && (istr.peek() != '}')) {
            specialpoints.push_back(Specialpoints(istr));
            while (isspace(istr.peek())
                   && istr.good())
              istr.get();
          }

          line = DK::Utils::String::remove_blanks(::read_line(istr));
          if (line != "}")
            throw ReadException("Game summary read: "
                                "special points: expected '}', "
                                " found '" + line + "'");
        } else if (config.name == "bock triggers") {
          string line;
          line = DK::Utils::String::remove_blanks(::read_line(istr));
          if (line != "{")
            throw ReadException("Game summary read: "
                                "bock triggers: expected '{', "
                                " found '" + line + "'");

          while (istr.good()
                 && (istr.peek() != '}')) {
            line = DK::Utils::String::remove_blanks(::read_line(istr));
            bock_triggers.push_back(BOCK_TRIGGER::from_name(line));
            while (isspace(istr.peek())
                   && istr.good())
              istr.get();
          }

          line = DK::Utils::String::remove_blanks(::read_line(istr));
          if (line != "}")
            throw ReadException("Game summary read: "
                                "bock triggers: expected '}', "
                                " found '" + line + "'");
        } else { // if (config.name == ...)
          throw ReadException("Game summary read: unknown line "
                              "'" + config.name + "'");
        } // if (config.name == ...)
      } else { // if !(config.value.empty())
        istringstream istr(config.value);
        if (config.name == "seed") {
          istr >> seed;
        } else if (config.name == "players number") {
          istr >> players_number;
          if (party_players.empty()) {
            for (unsigned p = 0; p < players_number; ++p)
              party_players.push_back(p);
          }
        } else if (config.name == "party players") {
          for (unsigned p = 0; p < party_players.size(); ++p)
            istr >> party_players[p];
        } else if (config.name == "startplayer number") {
          istr >> startplayer_no;
        } else if (config.name == "soloplayer number") {
          istr >> soloplayer_no;
        } else if (config.name == "gametype") {
          game_type = GAMETYPE::from_name(config.value);
        } else if (config.name == "duty solo") {
          duty_solo = (   (config.value == "true")
                       || (config.value == "yes")
                       || (config.value == "1"));
        } else if (config.name == "swines player number") {
          istr >> swines_player_no;
        } else if (config.name == "hyperswines player number") {
          istr >> hyperswines_player_no;
        } else if (config.name == "teams") {
          string::size_type pos_begin = 0;
          while ((pos_begin < config.value.size())
                 && isspace(config.value[pos_begin]))
            pos_begin++;
          string::size_type pos_end = pos_begin;
          while (pos_begin < config.value.size()) {
            pos_end = config.value.find(',', pos_begin);
            string const name(string(config.value,
                                     pos_begin,
                                     ((pos_end == string::npos)
                                      ? string::npos
                                      : (pos_end - pos_begin))));

            teams.push_back(TEAM::from_name(name));
            if (pos_end == string::npos)
              break;
            pos_begin = pos_end + 1;
            while ((pos_begin < config.value.size())
                   && isspace(config.value[pos_begin]))
              pos_begin++;
          } // while (pos_begin < config.value.size())
        } else if (config.name == "highest announcement re") {
          highest_announcement_re = ANNOUNCEMENT::from_name(config.value);
        } else if (config.name == "highest announcement contra") {
          highest_announcement_contra = ANNOUNCEMENT::from_name(config.value);
        } else if (config.name == "trick points") {
          string::size_type pos_begin = 0;
          while ((pos_begin < config.value.size())
                 && isspace(config.value[pos_begin]))
            pos_begin++;
          string::size_type pos_end = pos_begin;
          while (pos_begin < config.value.size()) {
            pos_end = config.value.find(',', pos_begin);
            string const name(string(config.value,
                                     pos_begin,
                                     ((pos_end == string::npos)
                                      ? string::npos
                                      : (pos_end - pos_begin))));

            trick_points.push_back(Unsigned(name));
            if (trick_points.back() == UINT_MAX)
              throw ReadException("Game summary read: invalid trickpoints "
                                  "'" + string(config.value,
                                               pos_begin,
                                               pos_end - pos_begin)
                                  + "'");
            if (pos_end == string::npos)
              break;
            pos_begin = pos_end + 1;
            while ((pos_begin < config.value.size())
                   && isspace(config.value[pos_begin]))
              pos_begin++;
          } // while (pos_begin < config.value.size())
        } else if (config.name == "winnerteam") {
          winnerteam = TEAM::from_name(config.value);
        } else if (config.name == "points") {
          istr >> points;
        } else { // if (config.name == ...)
          throw ReadException("Game summary read: unknown line "
                              "'" + config.name + " = " + config.value + "'");
        } // if (config.name == ...)
        if (!istr.good() && !istr.eof())
          throw ReadException("Game summary read: error loading "
                              "'" + config.name + "'");
      } // if !(config.value.empty())

    } while (istr.good());

    if (!istr.good())
      throw ReadException("Game summary read: unknown error");
  } // read the data

  { // test whether all data was read
    // explicit casts of Unsigned because of problems with MinGW g++-3.2
    if (seed.operator unsigned const&() == UINT_MAX)
      throw ReadException("Game summary read: seed not loaded");
    if (players_number.operator unsigned const&() == UINT_MAX)
      throw ReadException("Game summary read: players number not loaded");
    if (startplayer_no.operator unsigned const&() == UINT_MAX)
      throw ReadException("Game summary read: startplayer number not loaded");
    if (teams.size() != players_number.operator unsigned const&())
      throw ReadException("Game summary read: teams not loaded");
    if (trick_points.size() != players_number.operator unsigned const&())
      throw ReadException("Game summary read: trick points not loaded");
    if (points == INT_MAX)
      throw ReadException("Game summary read: points number not loaded");

  } // test whether all data was read

  { // set the data
    // explicit casts of Unsigned because of problems with MinGW g++-3.2
    this->seed_ = seed.operator unsigned const&();
    this->players_number_ = players_number.operator unsigned const&();
    this->party_players_ = party_players;
    this->startplayer_no_ = startplayer_no.operator unsigned const&();
    this->soloplayer_no_ = soloplayer_no.operator unsigned const&();
    this->game_type_ = game_type;
    this->duty_solo_ = duty_solo;
    this->swines_player_no_ = swines_player_no.operator unsigned const&();
    this->hyperswines_player_no_ = hyperswines_player_no.operator unsigned const&();
    this->teams_ = teams;
    this->highest_announcement_re_ = highest_announcement_re;
    this->highest_announcement_contra_ = highest_announcement_contra;
    this->trick_points_ = trick_points;
    this->winnerteam_ = winnerteam;
    this->points_ = points;
    this->specialpoints_ = specialpoints;
    this->bock_triggers_ = bock_triggers;
  } // set the data

  return ;
} // void GameSummary::read(istream& istr)

/**
 ** -> result
 **
 ** @param	p   number of the player in the game
 **
 ** @return	the number of the player in the party
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
GameSummary::party_player(unsigned const p) const
{
  DEBUG_ASSERTION(p < this->party_players().size(),
                  "GameSummary::party_player(" << p << ")\n"
                  "  There are only " << this->party_players().size() << " players");
  return this->party_players()[p];
} // unsigned GameSummary::party_player(unsigned p) const

/**
 ** -> result
 **
 ** @param	p   number of the player in the party
 **
 ** @return	whether the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
GameSummary::game_player(unsigned const player_no) const
{
  for (unsigned p = 0; p < this->party_players().size(); ++p)
    if (this->party_players()[p] == player_no)
      return p;
  return UINT_MAX;
} // unsigned GameSummary::game_player(unsigned player_no) const

/**
 ** -> result
 **
 ** @param	p   number of the player in the party
 **
 ** @return	whether the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
bool
GameSummary::party_player_has_played(unsigned const player_no) const
{
  return (this->game_player(player_no) != UINT_MAX);
} // bool GameSummary::party_player_has_played(unsigned player_no) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the game was a duty solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
GameSummary::is_solo() const
{
  return GAMETYPE::is_solo(this->game_type());
} // bool GameSummary::is_solo() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the game was a duty solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
bool
GameSummary::is_duty_solo() const
{
  return this->duty_solo();
} // bool GameSummary::is_duty_solo() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the game was a lust solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
bool
GameSummary::is_lust_solo() const
{
  return (   this->is_solo()
          && !this->is_duty_solo());
} // bool GameSummary::is_lust_solo() const

/**
 ** -> result
 ** the check of duty solo round must be made by party
 **
 ** @param	-
 **
 ** @return	whether the startplayer is not incremented (t.i. duty solo or lust solo and lustsolo player leads)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
bool
GameSummary::startplayer_stays() const
{
  if (!GAMETYPE::is_real_solo(this->game_type()))
    return false;
  // some solo
  if (this->is_duty_solo())
    return true;
  // some lust solo
  return this->rule()(Rule::LUSTSOLO_PLAYER_LEADS);
} // bool GameSummary::startplayer_stays() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the next startplayer
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
GameSummary::next_startplayer_no() const
{
  if (this->startplayer_stays())
    return this->startplayer_no();
  else
    return (this->startplayer_no() + 1) % this->players_number();
} // unsigned GameSummary::next_startplayer_no() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the team which gets the points
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Team
GameSummary::team_of_points() const
{
  if (this->winnerteam() == TEAM::NOTEAM) {
    switch(this->rule()(Rule::COUNTING)) {
    case COUNTING_NS::PLUS:
      if (this->points() > 0)
        return TEAM::RE;
      else
        return TEAM::CONTRA;

    case COUNTING_NS::MINUS:
      if (this->points() < 0)
        return TEAM::RE;
      else
        return TEAM::CONTRA;

    case COUNTING_NS::PLUSMINUS:
      return TEAM::RE;

    } // switch(this->rule()(Rule::COUNTING))
  } else { // if !(this->winnerteam() == TEAM::NOTEAM)
    switch(this->rule()(Rule::COUNTING)) {
    case COUNTING_NS::PLUS:
      return this->winnerteam();

    case COUNTING_NS::MINUS:
      return opposite(this->winnerteam());

    case COUNTING_NS::PLUSMINUS:
      return this->winnerteam();
    } // switch(this->rule()(Rule::COUNTING))
  } // if !(this->winnerteam() == TEAM::NOTEAM)

  return TEAM::NOTEAM;
} // Team GameSummary::team_of_points() const

/**
 ** -> result
 **
 ** @param	team	the team
 **
 ** @return	whether the team gets points
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
GameSummary::team_get_points(Team const team) const
{
  if (team == TEAM::NOTEAM)
    return false;

  using namespace COUNTING_NS;
  if (this->winnerteam() == TEAM::NOTEAM) {
    switch(this->rule()(Rule::COUNTING)) {
    case PLUS:
      return ( ( (this->points() > 0)
                && (team == TEAM::RE))
              || ( (this->points() < 0)
                  && (team == TEAM::CONTRA)) );
    case MINUS:
      return ( ( (this->points() < 0)
                && (team == TEAM::RE))
              || ( (this->points() > 0)
                  && (team == TEAM::CONTRA)) );

    case PLUSMINUS:
      return (this->points_ != 0);

    } // switch(this->rule()(Rule::COUNTING))

  } else { // if !(p.winnerteam() == TEAM::NOTEAM)
    switch(this->rule()(Rule::COUNTING)) {
    case PLUS:
    case MINUS:
      return (team == this->team_of_points());

    case PLUSMINUS:
      return true;
    } // switch(this->rule()(Rule::COUNTING))
  } // if !(p.winnerteam() == TEAM::NOTEAM)

  DEBUG_ASSERT(false);
  return false;
} // bool GameSummary::team_get_points(Team const team) const

/**
 ** -> result
 **
 ** @param	team	the team
 **
 ** @return	the points the team gets
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
int
GameSummary::points(Team const team) const
{
  if (team == TEAM::NOTEAM)
    return 0;
  if (!this->team_get_points(team))
    return 0;

  // in a solo game 'points()' returns three times the value of 'points_'
  switch(this->rule().counting()) {
  case COUNTING_NS::PLUS:
    if (   this->is_solo()
        && (   this->rule()(Rule::SOLO_ALWAYS_COUNTS_TRIPLE)
            || (this->winnerteam() == TEAM::RE)) )
      return 3 * this->points();

    if ( (this->winnerteam() == TEAM::NOTEAM)
        && (team == TEAM::CONTRA) )
      return -this->points();

    return this->points();

  case COUNTING_NS::MINUS:
    if (   this->is_solo()
        && (   this->rule()(Rule::SOLO_ALWAYS_COUNTS_TRIPLE)
            || (this->winnerteam() == TEAM::CONTRA)) )
      return 3 * (-this->points());

    if ( (this->winnerteam() == TEAM::NOTEAM)
        && (team == TEAM::RE) )
      return this->points();

    return -this->points();

  case COUNTING_NS::PLUSMINUS:
    if (this->is_solo()) {
      if (this->winnerteam() == TEAM::RE) {
        if (team == this->winnerteam())
          return 3 * this->points();
        else
          return -this->points();
      } else { // if !(this->winnerteam() == TEAM::RE)
        if (team == this->winnerteam())
          return this->points();
        else
          return -3 * this->points();
      } // if !(this->winnerteam() == TEAM::RE)
    } // if (this->is_solo())

    if (team == this->team_of_points())
      return  this->points();
    else
      return -this->points();
  } // switch(this->rule().counting())

  DEBUG_ASSERTION(false,
                  "GameSummary::points(team):\n"
                  "  counting type not known: " << this->rule().counting());
  return 0;
} // int GameSummary::points(Team team) const

/**
 ** -> result
 **
 ** @param	playerno	the player
 **
 ** @return	whether the player gets points
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
GameSummary::player_get_points(unsigned const playerno) const
{
  return team_get_points(this->team(playerno));
} // bool GameSummary::player_get_points(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	the player
 **
 ** @return	whether the player gets points
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
bool
GameSummary::player_get_points(Player const& player) const
{
  return team_get_points(this->team(player));
} // bool GameSummary::player_get_points(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	the player number (party number)
 **
 ** @return	the points the player gets
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
int
GameSummary::points(unsigned const playerno) const
{
  return this->points(this->team(playerno));
} // int GameSummary::points(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	the player
 **
 ** @return	the points the player gets, 0 if he has not played in the game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
int
GameSummary::points(Player const& player) const
{
  return this->points(this->team(player));
} // int GameSummary::points(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	the player
 **
 ** @return	the number of points 'playerno' has made in his tricks
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
GameSummary::trick_points(unsigned const playerno) const
{
  if (playerno >= this->trick_points().size())
    return 0;
  return this->trick_points()[playerno];
} // unsigned GameSummary::trick_points(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	playerno	the player
 **
 ** @return	the number of points player has made in his tricks
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
GameSummary::trick_points(Player const& player) const
{
  if (this->party_player_has_played(::party.no(player)))
    return this->trick_points(this->game_player(::party.no(player)));
  return 0;
} // unsigned GameSummary::trick_points(Player player) const

/**
 ** -> result
 **
 ** @param	team	the team
 **
 ** @return	the points made by the team
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
GameSummary::trick_points(Team const team) const
{
  unsigned points = 0;
  for (unsigned p = 0; p < this->trick_points().size(); ++p)
    if (this->team(p) == team)
      points += this->trick_points(p);

  return points;
} // unsigned GameSummary::trick_points(Team const team) const

/**
 ** -> result
 **
 ** @param	playerno	the player number
 **
 ** @return	team of the player, NOTEAM if the player has not played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Team
GameSummary::team(unsigned const playerno) const
{
  if (playerno >= this->teams().size())
    return TEAM::NOTEAM;
  return this->teams()[playerno];
} // Team GameSummary::team(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	the player
 **
 ** @return	team of the player, NOTEAM if the player has not played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Team
GameSummary::team(Player const& player) const
{
  if (this->party_player_has_played(::party.no(player)))
    return this->team(this->game_player(::party.no(player)));
  return TEAM::NOTEAM;
} // Team GameSummary::team(Player player) const

/**
 ** -> result
 **
 ** @param	i   number of special point
 **
 ** @return	the i'th special point
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
Specialpoints const&
GameSummary::specialpoint(unsigned const i) const
{
  DEBUG_ASSERTION(i < this->specialpoints().size(),
                  "GameSummary::specialpoint(i)\n"
                  "  i = " << i << " < " << this->specialpoints().size()
                  << " = number of specialpoints");

  return this->specialpoints()[i];
} // Specialpoints GameSummary::specialpoint(unsigned i) const

/**
 ** -> result
 **
 ** @param	i   number of special point
 **
 ** @return	the i'th special point
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
int
GameSummary::bock_multiplier() const
{
  int multiplier = 1;
  for (Specialpointsvector::const_iterator
       s = this->specialpoints().begin();
       s != this->specialpoints().end();
       ++s)
    if (s->type == SPECIALPOINT::BOCK) {
      multiplier *= s->value();
    }

  return multiplier;
} // int GameSummary::bock_multiplier() const

/**
 ** evaluate the game
 **
 ** @param	game   game to evaluate
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
GameSummary::evaluate(Game const& game)
{
  // save the team and the trick points
  for (unsigned p = 0; p < game.playerno(); ++p) {
    Player const& player = game.player(p);
    if (game.no(player) == UINT_MAX) {
      // the player has not played in this game
      this->teams_.push_back(TEAM::NOTEAM);
      this->trick_points_.push_back(0);
    } else {
      this->teams_.push_back(game.team(player));
      this->trick_points_.push_back(game.points_of_player(player));
    }
  } // for (p < game.playerno())

  // save the winnerteam
  this->winnerteam_ = game.winnerteam();

  // calculate highest announcements
  vector<unsigned> re_announcement_player(ANNOUNCEMENT::NUMBER, UINT_MAX);
  vector<unsigned> contra_announcement_player(ANNOUNCEMENT::NUMBER, UINT_MAX);
  Announcement reply_announcement = ANNOUNCEMENT::NOANNOUNCEMENT;
  unsigned reply_player;
  for(unsigned p = 0; p < game.number_of_players(); p++ ) {
    Player const& player = game.player(p);
    vector<Game::AnnouncementWithTrickno> const& announcements
      = game.announcements_of_player(player);

    if (is_reply(announcements.back().announcement)) {
      reply_announcement = announcements.back().announcement;
      reply_player = p;
    }

    if (game.team(player) == TEAM::RE) {
      if (player.announcement() > this->highest_announcement_re_)
        this->highest_announcement_re_ = player.announcement();

      for (unsigned a = 0; a < announcements.size(); a++)
        re_announcement_player[announcements[a].announcement] = p;
    } // if (game.team(player) == TEAM::RE)

    if (game.team(player) == TEAM::CONTRA) {
      if( player.announcement() > highest_announcement_contra_)
        this->highest_announcement_contra_ = player.announcement();

      for (unsigned a = 0; a < announcements.size(); a++)
        contra_announcement_player[announcements[a].announcement] = p;
    } // if (game.team(player) == TEAM::RE)
  } // for (p < game.number_of_players())

  { // special points
    // Order of the points
    // * won
    // * re no 120 said
    // * contra no 120 said
    // * re no 90
    // * contra no 90
    // * no 90 reply
    // * re no 90 said
    // * contra no 90 said
    // * re no 90 said && contra > 120 points
    // * contra no 90 said && re > 120 points
    // * re no 60
    // * contra no 60
    // * no 60 reply
    // * re no 60 said
    // * contra no 60 said
    // * re no 60 said && contra > 90 points
    // * contra no 60 said && re > 90 points
    // * re no 30
    // * contra no 30
    // * no 30 reply
    // * re no 30 said
    // * contra no 30 said
    // * re no 30 said && contra > 60 points
    // * contra no 30 said && re > 60 points
    // * re no 0
    // * contra no 0
    // * no 0 reply
    // * re no 0 said
    // * contra no 0 said
    // * no 120 reply
    // * re no 0 said && contra > 30 points
    // * contra no 0 said && re > 30 points
    // * against club queen
    // * multiplicator 3
    // * specialpoints

    { // announcements

      // shortcuts
      // got no 90
#define TRICKPOINTS_NO(A) \
      if ((game.points_of_team(TEAM::RE) < A) \
          && (game.winnerteam() != TEAM::RE)) \
      this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO ## A, \
                                                   TEAM::CONTRA)); \
      if ((game.points_of_team(TEAM::CONTRA) < A) \
          && (game.winnerteam() != TEAM::CONTRA)) \
      this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO ## A, \
                                                   TEAM::RE))
      // announced no 60
#define TRICKPOINTS_NO_ANNOUNCED(A) \
      if (game.rule()(Rule::KNOCKING)) { \
        for (unsigned p = 0; p < game.number_of_players(); ++p) { \
          if (   (game.announcement_of_player(game.player(p)) \
                  >= ANNOUNCEMENT::NO ## A) \
              && (game.winnerteam() != TEAM::NOTEAM)) { \
            Specialpoints sp(SPECIALPOINT::NO ## A ## _SAID, \
                             game.team(game.player(p)), \
                             game.winnerteam()); \
            sp.player_of_no = p; \
            this->specialpoints_.push_back(sp); \
          } \
        } \
      } else { \
        if (is_real(this->highest_announcement_re()) \
            && (this->highest_announcement_re() >= ANNOUNCEMENT::NO ## A ) \
            && (game.winnerteam() != TEAM::NOTEAM)) { \
          Specialpoints sp(SPECIALPOINT::NO ## A ## _SAID, \
                           TEAM::RE, \
                           game.winnerteam()); \
          sp.player_of_no = re_announcement_player[ANNOUNCEMENT::NO ## A]; \
          this->specialpoints_.push_back(sp); \
        } \
        if (is_real(this->highest_announcement_contra()) \
            && (this->highest_announcement_contra() >= ANNOUNCEMENT::NO ## A ) \
            && (game.winnerteam() != TEAM::NOTEAM)) { \
          Specialpoints sp(SPECIALPOINT::NO ## A ## _SAID, \
                           TEAM::CONTRA, \
                           game.winnerteam()); \
          sp.player_of_no = contra_announcement_player[ANNOUNCEMENT::NO ## A]; \
          this->specialpoints_.push_back(sp); \
        } \
      }
      // got 120 (against 'no 90')
#define TRICKPOINTS_GOT_AGAINST(A, B) \
      if ((this->highest_announcement_contra() == ANNOUNCEMENT::NO ## A) \
          && (game.points_of_team(TEAM::RE) >= A + 30)) \
      this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO ## A ## _SAID_ ## B ## _GOT, \
                                                   TEAM::RE)); \
      if ((this->highest_announcement_re() == ANNOUNCEMENT::NO ## A) \
          && (game.points_of_team(TEAM::CONTRA) >= A + 30)) \
      this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO ## A ## _SAID_ ## B ## _GOT, \
                                                   TEAM::CONTRA))
      // replied on announcement
#define ANNOUNCEMENT_REPLY(A) \
      if ( (reply_announcement == ANNOUNCEMENT::NO ## A ## _REPLY) \
          && (game.winnerteam() != TEAM::NOTEAM)) { \
        Specialpoints sp( SPECIALPOINT::NO ## A ## _REPLY, \
                         game.player(reply_player).team(), \
                         game.winnerteam()); \
        sp.player_of_no = reply_player; \
        this->specialpoints_.push_back(sp); \
      } else
      // re announced no 60

#define POINTS(A, B) \
      TRICKPOINTS_NO(A); \
      TRICKPOINTS_NO_ANNOUNCED(A); \
      ANNOUNCEMENT_REPLY(A); \
      TRICKPOINTS_GOT_AGAINST(A, B);

      { // won
        if (game.winnerteam() != TEAM::NOTEAM)
          this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::WON,
                                                       game.winnerteam()));
      } // won
      if (game.rule()(Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE)) {
        // against club queens
        if (   !GAMETYPE::is_solo(this->game_type())
            && (game.winnerteam() == TEAM::CONTRA) )
          this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::CONTRA_WON,
                                                       TEAM::CONTRA));
      } // if (Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE)

      if (game.rule()(Rule::ANNOUNCEMENT_RE_DOUBLES)) {
        POINTS(90, 120);
        POINTS(60, 90);
        POINTS(30, 60);

        { // 0
          // re no 0
          if (( !game.hastrick(TEAM::RE))
              && (game.winnerteam() != TEAM::RE))
            this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO0,
                                                         TEAM::CONTRA));
          // contra no 0
          if (( !game.hastrick( TEAM::CONTRA ) )
              && (game.winnerteam() != TEAM::CONTRA))
            this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO0,
                                                         TEAM::RE));
          TRICKPOINTS_NO_ANNOUNCED(0);
          ANNOUNCEMENT_REPLY(0);
          TRICKPOINTS_GOT_AGAINST(0, 30);
        } // 0

        ANNOUNCEMENT_REPLY(120);
      } // if (game.rule()(Rule::ANNOUNCEMENT_RE_DOUBLES))

      if (game.winnerteam() != TEAM::NOTEAM) {
        if (game.rule()(Rule::KNOCKING)) {
          // no 120 announced
          for (unsigned p = 0; p < game.number_of_players(); ++p) {
            if (   (game.announcement_of_player(game.player(p))
                    >= ANNOUNCEMENT::NO120)
                && (game.winnerteam() != TEAM::NOTEAM)) {
              Specialpoints sp(SPECIALPOINT::NO120_SAID,
                               game.team(game.player(p)),
                               game.winnerteam());
              sp.player_of_no = p;
              this->specialpoints_.push_back(sp);
            }
          } // for (p)
        } else { // if !(game.rule()(Rule::KNOCKING))
          // re announced
          if (   (  (this->highest_announcement_re() >= ANNOUNCEMENT::NO120)
                  && ANNOUNCEMENT::is_real(this->highest_announcement_re()) )
              || (this->highest_announcement_re() == ANNOUNCEMENT::REPLY) ) {
            Specialpoints sp(SPECIALPOINT::NO120_SAID,
                             TEAM::RE,
                             game.winnerteam());
            sp.player_of_no
              = re_announcement_player[( (this->highest_announcement_re()
                                          == ANNOUNCEMENT::REPLY)
                                        ? ANNOUNCEMENT::REPLY
                                        : ANNOUNCEMENT::NO120) ];
            this->specialpoints_.push_back(sp);
          }
          // contra announced
          if (   (   (this->highest_announcement_contra() >= ANNOUNCEMENT::NO120)
                  && ANNOUNCEMENT::is_real(this->highest_announcement_contra()) )
              || (this->highest_announcement_contra() == ANNOUNCEMENT::REPLY) ) {
            Specialpoints sp(SPECIALPOINT::NO120_SAID,
                             TEAM::CONTRA,
                             game.winnerteam());
            sp.player_of_no
              = contra_announcement_player[( (this->highest_announcement_contra()
                                              == ANNOUNCEMENT::REPLY)
                                            ? ANNOUNCEMENT::REPLY
                                            : ANNOUNCEMENT::NO120) ];
            this->specialpoints_.push_back(sp);
          }
        } // if !(game.rule()(Rule::KNOCKING))
      } // if (game.winnerteam() != TEAM::NOTEAM)

      if (!game.rule()(Rule::ANNOUNCEMENT_RE_DOUBLES)) {
        POINTS(90, 120);
        POINTS(60, 90);
        POINTS(30, 60);

        { // 0
          // re no 0
          if (( !game.hastrick(TEAM::RE))
              && (game.winnerteam() != TEAM::RE))
            this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO0,
                                                         TEAM::CONTRA));
          // contra no 0
          if (( !game.hastrick( TEAM::CONTRA ) )
              && (game.winnerteam() != TEAM::CONTRA))
            this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::NO0,
                                                         TEAM::RE));
          TRICKPOINTS_NO_ANNOUNCED(0);
          ANNOUNCEMENT_REPLY(0);
          TRICKPOINTS_GOT_AGAINST(0, 30);
        } // 0

        ANNOUNCEMENT_REPLY(120);
      } // if (!game.rule()(Rule::ANNOUNCEMENT_RE_DOUBLES))

      if (!game.rule()(Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE)) {
        // against club queens
        if (   !GAMETYPE::is_solo(this->game_type())
            && (game.winnerteam() == TEAM::CONTRA) )
          this->specialpoints_.push_back(Specialpoints(SPECIALPOINT::CONTRA_WON,
                                                       TEAM::CONTRA));
      } // if (Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE)

#undef TRICKPOINTS_RE_NO
#undef TRICKPOINTS_CONTRA_NO
#undef TRICKPOINTS_RE_GOT
#undef TRICKPOINTS_CONTRA_GOT
#undef ANNOUNCEMENT_REPLY
#undef TRICKPOINTS_RE_NO_ANNOUNCED
#undef TRICKPOINTS_CONTRA_NO_ANNOUNCED
#undef POINTS

    } // announcements

    // multiplicator 3 for solo games
    if (   GAMETYPE::is_solo(this->game_type())
        && (   game.rule()(Rule::SOLO_ALWAYS_COUNTS_TRIPLE)
            || (this->winnerteam() == TEAM::RE) ) ) {
      Specialpoints sp(SPECIALPOINT::SOLO, this->winnerteam());
      sp.player_of_no = game.no(game.soloplayer());
      this->specialpoints_.push_back(sp);
    }

    // last collect all specialpoints of each trick
    for (unsigned t = 0; t < game.trick_current_no(); ++t) {
      Specialpointsvector const spv = game.trick(t).specialpoints();
      for (Specialpointsvector::const_iterator s = spv.begin();
           s != spv.end();
           s++)
        if (s->is_valid(this->teams()))
          this->specialpoints_.push_back(*s);
    }

    { // bock triggers
      if (game.rule()(Rule::BOCK_120)) {
        // bock after 120-120 points
        if (game.points_of_team(TEAM::RE) == game.points_of_team(TEAM::CONTRA))
          this->bock_triggers_.push_back(BOCK_TRIGGER::EQUAL_POINTS);
      } // if (game.rule()(Rule::BOCK_120))

      if (game.rule()(Rule::BOCK_SOLO_LOST)) {
        // bock after a lost solo game
        if (   GAMETYPE::is_solo(game.type())
            && (game.winnerteam() == TEAM::CONTRA))
          this->bock_triggers_.push_back(BOCK_TRIGGER::SOLO_LOST);
      } // if (game.rule()(Rule::BOCK_SOLO_LOST))

      if (game.rule()(Rule::BOCK_RE_LOST)) {
        // bock after a lost re/contra game (only re/contra is announced)
        if (   (   game.announcement_of_team(TEAM::RE)
                != ANNOUNCEMENT::NOANNOUNCEMENT)
            && (game.winnerteam() != TEAM::RE))
          this->bock_triggers_.push_back(BOCK_TRIGGER::RE_LOST);
        if (   (   game.announcement_of_team(TEAM::CONTRA)
                != ANNOUNCEMENT::NOANNOUNCEMENT)
            && (game.winnerteam() != TEAM::CONTRA))
          this->bock_triggers_.push_back(BOCK_TRIGGER::CONTRA_LOST);
      } // if (game.rule()(Rule::BOCK_RE_LOST))

      if (game.rule()(Rule::BOCK_HEART_TRICK)) {
        if (!GAMETYPE::is_solo(game.type())) {
          // bock after a real heart trick
          for (vector<Trick*>::const_iterator t = game.tricks().begin();
               t != game.tricks().end();
               ++t) {
            HandCards::const_iterator c;
            for (c = (*t)->cards().begin();
                 c != (*t)->cards().end();
                 ++c)
              if (c->tcolor() != Card::HEART)
                break;

            if (c == (*t)->cards().end())
              this->bock_triggers_.push_back(BOCK_TRIGGER::HEART_TRICK);
          } // for (t \in game.tricks())
        } // if (!GAMETYPE::is_solo(game.type()))
      } // if (game.rule()(Rule::BOCK_HEART_TRICK))

      if (game.rule()(Rule::BOCK_BLACK)) {
        // bock after a black game (a team got no trick)
        if (   (game.numberoftricks_of_team(TEAM::RE) == 0)
            || (game.numberoftricks_of_team(TEAM::CONTRA) == 0) )
          this->bock_triggers_.push_back(BOCK_TRIGGER::BLACK);
      } // if (game.rule()(Rule::BOCK_BLACK))

      if (game.bock_multiplier() != 1) {
        Specialpoints sp(SPECIALPOINT::BOCK,
                         game.bock_multiplier());
        this->specialpoints_.push_back(sp);
      } // if (game.bock_multiplier())
    } // bock triggers

    // calculating the main game points
    this->points_ = Sum_of_Specialpoints(this->specialpoints(),
                                         this->winnerteam(),
                                         game);

    DEBUG_ASSERTION((( this->winnerteam() == TEAM::RE )
                     || ( this->winnerteam() == TEAM::CONTRA )
                     || ( this->winnerteam() == TEAM::NOTEAM )),
                    "GameSummary::evaluate(Game):" << endl
                    << "  wrong team at end: " << this->winnerteam());

  } // special points

  return ;
} // void GameSummary::evaluate(Game const& game)

#ifdef OUTDATED
/**
 ** compares this game summary with the gamepoints
 ** (test for errors)
 **
 ** @param	gamepoints	gamepoints to compare
 **
 ** @return	whether the data is the same
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
GameSummary::test(GamePoints const& gamepoints) const
{
  if (this->points() != gamepoints.points()) {
    cerr << "points" << endl;
    cerr << this->points() << " != " << gamepoints.points() << endl;
    return false;
  }

  for (unsigned p = 0; p < this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME); ++p) {
    if (this->trick_points(p) != gamepoints.playerspoints(p)) {
      cerr << "playerpoins " << p << endl;
      return false;
    }
    if (this->team(p) != gamepoints.team(p)) {
      cerr << "team " << p << endl;
      return false;
    }
  }

  if (this->trick_points(TEAM::RE) != gamepoints.teampoints(TEAM::RE)) {
    cerr << "trick_points(RE)" << endl;
    return false;
  }
  if (this->trick_points(TEAM::CONTRA) != gamepoints.teampoints(TEAM::CONTRA)) {
    cerr << "trick_points(CONTRA)" << endl;
    return false;
  }

  if (this->game_type() != gamepoints.gametype()) {
    cerr << "gametype" << endl;
    return false;
  }

  if (this->winnerteam() != gamepoints.winnerteam()) {
    cerr << "winnerteam" << endl;
    return false;
  }

#if 0
  if (this->specialpoints() != gamepoints.pointlist()) {
    cerr << "pointlist" << endl;
    return false;
  }
#endif

  return true;
} // bool GameSummary::test(GamePoints const& gamepoints) const
#endif // #ifndef OUTDATED

/**
 ** writes the game summary into the stream
 **
 ** @param	ostr		output stream
 ** @param	game_summary	game summary
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, GameSummary const& game_summary)
{
  game_summary.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, GameSummary const& game_summary)
