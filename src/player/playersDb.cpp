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

#include "playersDb.h"
#include "../game/game.h"
#include "../game/game_summary.h"

#include <sstream>
using std::istringstream;

ostream& operator<<(ostream& ostr, TLWcount const& tlwcount)
{ return tlwcount.write(ostr); }
istream& operator>>(istream& istr, TLWcount& tlwcount)
{ return tlwcount.read(istr); }

ostream& operator<<(ostream& ostr, Ranking const& ranking)
{ return ranking.write(ostr); }
istream& operator>>(istream& istr, Ranking& ranking)
{ return ranking.read(istr); }


ostream& operator<<(ostream& ostr, PlayersDb const& players_db)
{ return players_db.write(ostr); }
istream& operator>>(istream& istr, PlayersDb& players_db)
{ return players_db.read(istr); }

/**
 **
 ** Constructor
 **
 ** @param	-
 **
 ** @return	-
 ** 
 ** @author	Borg Enders
 **
 ** @version	0.6.1
 **
 **/
PlayersDb::PlayersDb() :
rank(),
    averageGamePoints_(),
    averageGameTrickPoints_(),
    games_all_(),
    games_(GAMETYPE::LAST + 1),
    specialpoints_all_(),
    specialpoints_(SPECIALPOINT::LAST + 1)
{
}

/**
 **
 ** constructor from a stream
 **
 ** @param	istr	stream with the database info
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
PlayersDb::PlayersDb(istream& istr) :
rank(),
    averageGamePoints_(),
    averageGameTrickPoints_(),
    games_all_(),
    games_(GAMETYPE::LAST + 1),
    specialpoints_all_(),
    specialpoints_(SPECIALPOINT::LAST + 1)
{
  this->read(istr);
} // PlayersDb::PlayersDb(istream& istr)

/**
 **
 ** Constructor
 **
 ** @param	p	players db to copy
 **
 ** @return	-
 ** 
 ** @author	Borg Enders
 **
 ** @version	0.6.1
 **
 **/
PlayersDb::PlayersDb( PlayersDb const& p ) :
rank(p.rank),
    averageGamePoints_(p.averageGamePoints_),
    averageGameTrickPoints_(p.averageGameTrickPoints_),
    games_all_(p.games_all_),
    games_(p.games_),
    specialpoints_all_(p.specialpoints_all_),
    specialpoints_(p.specialpoints_)
{
}

/**
 **
 ** writes the database to 'ostr'
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
ostream&
PlayersDb::write(ostream& ostr) const
{
  ostr << setw(24) << "ranking" << " =\t"
    << this->rank << '\n';
  ostr << setw(24) << "total games" << " =\t"
    << this->games_all() << '\n';
  ostr << setw(24) << "total special points" << " =\t"
    << this->specialpoints_all() << '\n';

  ostr << setw(24) << "average game points" << " =\t"
    << this->averageGamePoints() << '\n';
  ostr << setw(24) << "average game trick points" << " =\t"
    << this->averageGameTrickPoints() << '\n';

  ostr << '\n';

  ostr << "games\n"
    << "{\n";
  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    ostr << setw(32) << ::name(static_cast<GameType>(gt)) << " =\t"
      << this->games(static_cast<GameType>(gt)) << '\n';
  ostr <<"}\n";

  ostr << '\n';

  ostr << "specialpoints\n"
    << "{\n";
  for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp)
    ostr << setw(32) << ::name(static_cast<Specialpoint>(sp)) << " =\t"
      << this->specialpoints(static_cast<Specialpoint>(sp)) << '\n';
  ostr <<"}\n";

  return ostr;
} // ostream& PlayersDb::write(ostream& ostr) const

/**
 **
 ** reads the database from 'istr'
 **
 ** @param	istr	stream with the database info
 **
 ** @return	input stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
istream&
PlayersDb::read(istream& istr)
{
  // first clear all
  this->averageGamePoints_ = 0;
  this->averageGameTrickPoints_ = 0;
  this->games_all_ = TLWcount();
  this->games_ = vector<TLWcount>(GAMETYPE::LAST + 1);
  this->specialpoints_all_ = TLWcount();
  this->specialpoints_ = vector<TLWcount>(SPECIALPOINT::LAST + 1);

  unsigned depth = 0;
  while (istr.good()) {
    Config config;
    istr >> config;
    if (istr.fail() || istr.eof())
      break;

    istringstream istr_value(config.value);
    if (!config.separator
	&& (config.name == "{")) {
      depth += 1;
    } else if (!config.separator
	       && (config.name == "}")) {
      if (depth == 0) {
	cerr << "players database: "
	  << "found a '}' without a '{' before.\n"
	  << "Finish reading the the file."
	  << endl;
	break;
      } // if (depth == 0)
      depth -= 1;
      if (depth == 0)
	break;
    } else if (config.name == "ranking")
      this->rank.read(istr_value);
    else if (config.name == "total games")
      this->games_all_.read(istr_value);
    else if (config.name == "total special points")
      this->specialpoints_all_.read(istr_value);
    else if (config.name == "average game points")
      istr_value >> this->averageGamePoints_;
    else if (config.name == "average game trick points")
      istr_value >> this->averageGameTrickPoints_;
    else if (!config.separator
	     && this->read_group(config.name, istr))
      // this complex reading is necessary to easily add groups in child
      // classes
      ;
    else {
      cerr << "Reading the player database:\n"
	<< "found following unknown line:\n"
	<< (config.separator
	    ? (config.name + " = " + config.value)
	    : config.name + config.value)
	<< '\n'
	<< "ignoring it."
	<< endl;
      exit(EXIT_FAILURE);
    }
  }; // while (istr.good())

  return istr;
} // istream& PlayersDb::read(istream& istr)

/**
 **
 ** reads the group 'group' from 'istr'
 **
 ** @param	group	name of the group to read
 ** @param	istr	stream with the group info
 **
 ** @return	whether the group 'group' is known
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
bool
PlayersDb::read_group(string const& group, istream& istr)
{
  if (group == "games")
    this->read_group_games(istr);
  else if (group == "specialpoints")
    this->read_group_specialpoints(istr);
#ifndef OUTDATED
  // 0.7.4   2006-09-14
  else if (group == "Games")
    this->read_group_games(istr);
  else if (group == "Specialpoints")
    this->read_group_specialpoints(istr);
#endif
  else
    return false;

  return true;
} // bool PlayersDb::read_group(string const& group, istream& istr)

/**
 **
 ** reads the games group from 'istr'
 **
 ** @param	istr	stream with the games info
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
void
PlayersDb::read_group_games(istream& istr)
{
  unsigned depth = 0;

  // load the configuration
  while (istr.good()) {
    Config config;
    istr >> config;

    if (config.separator) {
      int gt;
      for (gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
	if (config.name == ::name(static_cast<GameType>(gt))) {
	  istringstream istr_value(config.value);
	  istr_value >> this->games_[gt];
	  break;
	}
      if (gt > GAMETYPE::LAST)
	cerr << "players database: group 'games': "
	  << "unknown type '" << config.name << "'."
	  << endl;
    } else { // if (config.separator)
      if(config.name == "{") {
	depth += 1;
      } else if(config.name == "}") {
	if (depth == 0) {
	  cerr << "players database: group 'games': "
	    << "found a '}' without a '{' before.\n"
	    << "Finish reading the the file."
	    << endl;
	  break;
	} // if (depth == 0)
	depth -= 1;
	if (depth == 0)
	  break;
      } else if(config.name == "") {
	cerr << "players database: group 'games': "
	  << "Ignoring line \'" << config.value << "\'.\n";
      } else {
	cerr << "players database: group 'games': "
	  << "type '" << config.name << "' unknown.\n"
	  << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator
  } // while (istr.good())

  return ;
} // void PlayersDb::read_group_games(istream& istr)

/**
 **
 ** reads the specialpoints group from 'istr'
 **
 ** @param	istr	stream with the specialpoints info
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
void
PlayersDb::read_group_specialpoints(istream& istr)
{
  unsigned depth = 0;

  // load the configuration
  while (istr.good()) {
    Config config;
    istr >> config;

    if (config.separator) {
      int sp;
      for (sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp)
	if (config.name == ::name(static_cast<Specialpoint>(sp))) {
	  istringstream istr_value(config.value);
	  istr_value >> this->specialpoints_[sp];
	  break;
	}
      if (sp > SPECIALPOINT::LAST)
	cerr << "players database: group 'specialpoints': "
	  << "unknown type '" << config.name << "'."
	  << endl;
    } else { // if (config.separator)
      if(config.name == "{") {
	depth += 1;
      } else if(config.name == "}") {
	if (depth == 0) {
	  cerr << "players database: group 'specialpoints': "
	    << "found a '}' without a '{' before.\n"
	    << "Finish reading the the file."
	    << endl;
	  break;
	} // if (depth == 0)
	depth -= 1;
	if (depth == 0)
	  break;
      } else if(config.name == "") {
	cerr << "players database: group 'specialpoints': "
	  << "Ignoring line \'" << config.value << "\'.\n";
      } else {
	cerr << "players database: group 'specialpoints': "
	  << "type '" << config.name << "' unknown.\n"
	  << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator
  } // while (istr.good())

  return ;
} // void PlayersDb::read_group_specialpoints(istream& istr)

/**
 **
 ** adds the game to the db
 **
 ** @param	p	player
 ** @param	g	gamepoints
 **
 ** @return	-
 ** 
 ** @author	Borg Enders
 **
 ** @version	0.6.1
 **
 **/
void
PlayersDb::evaluate( const Player& p, GameSummary const& g )
{
  if( g.winnerteam() == p.team() )
  {

    games_[g.game_type()].hasWon();
    rank.add( Ranking( 1.0 ) );
    averageGamePoints_ = (( averageGamePoints_ * games_all_.total() ) + g.points() )
      / ( games_all_.total() + 1 );
    games_all_.hasWon();
  } else
  {
    games_[g.game_type()].hasLost();
    rank.add( Ranking( -1.0 ) );
    games_all_.hasLost();
  }

  averageGameTrickPoints_ = 
    ( ( averageGameTrickPoints_ * games_all_.total() ) + g.points( p.game().no(p) ) ) 
    / (games_all_.total()+1);

  Specialpointsvector spv = g.specialpoints();  

  for( unsigned i = 0; i < spv.size(); i++ ) {
    if ((spv[i].player_get_no == p.game().no(p) )
	|| (spv[i].player_get_no == UINT_MAX) )
      if( spv[i].counting_team == p.team() ) {
	specialpoints_[ spv[i].type ].hasWon();
	specialpoints_all_.hasWon();
      }
    if ((spv[i].player_of_no == p.game().no(p) )
	|| (spv[i].player_of_no == UINT_MAX) )
      if( spv[i].counting_team != p.team() ) {
	specialpoints_[ spv[i].type ].hasLost();
	specialpoints_all_.hasLost();
      }
  }

  return ;
} // virtual void PlayerDb::evaluate( const Player& p, const GamePoints& g )

/**
 **
 ** clears the database
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
void
PlayersDb::clear()
{
  this->rank.clear();

  for (std::vector<TLWcount>::iterator g = this->games_.begin();
       g != this->games_.end();
       ++g)
    g->clear();
  for (std::vector<TLWcount>::iterator sp = this->specialpoints_.begin();
       sp != this->specialpoints_.end();
       ++sp)
    sp->clear();

  this->averageGamePoints_ = 0;
  this->averageGameTrickPoints_ = 0;

  this->games_all_.clear();
  this->specialpoints_all_.clear();

  return ;
} // void PlayersDb::clear()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'marriage'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_marriage() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_marriage(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_marriage() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'poverty'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_poverty() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_poverty(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_poverty() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_solo() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_solo(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_solo() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'color solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_solo_color() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_color_solo(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_solo_color() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'picture solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_solo_picture() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_picture_solo(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_solo_picture() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'single picture solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_solo_picture_single() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_single_picture_solo(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_solo_picture_single() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the games group 'double picture solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::games_group_solo_picture_double() const
{
  TLWcount tlwcount;

  for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt)
    if (GAMETYPE::is_double_picture_solo(static_cast<GameType>(gt)))
      tlwcount += this->games(static_cast<GameType>(gt));

  return tlwcount;
} // TLWcount PlayersDb::games_group_solo_picture_double() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the specialpoints group 'winning'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::specialpoints_group_winning() const
{
  TLWcount tlwcount;

  for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp)
    if (SPECIALPOINT::is_winning(static_cast<Specialpoint>(sp)))
      tlwcount += this->specialpoints(static_cast<Specialpoint>(sp));

  return tlwcount;
} // TLWcount PlayersDb::specialpoints_group_winning() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a TLW for the specialpoints group 'announcement'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
TLWcount
PlayersDb::specialpoints_group_announcement() const
{
  TLWcount tlwcount;

  for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp)
    if (SPECIALPOINT::is_announcement(static_cast<Specialpoint>(sp)))
      tlwcount += this->specialpoints(static_cast<Specialpoint>(sp));

  return tlwcount;
} // TLWcount PlayersDb::specialpoints_group_announcement() const
