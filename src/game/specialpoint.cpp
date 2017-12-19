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

#include "specialpoint.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game.h"

#include "../utils/string.h"

/**
 ** -> return
 **
 ** @param	sp	the specialpoint type
 **
 ** @return	whether 'sp' was got by winning
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
SPECIALPOINT::is_winning(Specialpoint const sp)
{
  switch (sp) {
  case WON:
  case NO90:
  case NO60:
  case NO30:
  case NO0:
  case CONTRA_WON:
  case NO90_SAID_120_GOT:
  case NO60_SAID_90_GOT:
  case NO30_SAID_60_GOT:
  case NO0_SAID_30_GOT:
  case SOLO:
    return true;
    break;
  default:
    return false;
    break;
  }; // switch(sp)
} // bool SPECIALPOINT::is_winning(Specialpoint const sp)

/**
 **
 ** -> return
 **
 ** @param	sp	the specialpoint type
 **
 ** @return	whether 'sp' was got by announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
bool
SPECIALPOINT::is_announcement(Specialpoint const sp)
{
  switch (sp) {
  case NO120_SAID:
  case NO90_SAID:
  case NO60_SAID:
  case NO30_SAID:
  case NO0_SAID:
  case NO120_REPLY:
  case NO90_REPLY:
  case NO60_REPLY:
  case NO30_REPLY:
  case NO0_REPLY:
    return true;
    break;
  default:
    return false;
    break;
  }; // switch(sp)
} // bool SPECIALPOINT::is_announcement(Specialpoint const sp)


/**
 ** -> result
 **
 ** @param	sp	special point
 **
 ** @result	points for this specialpoint
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
int
Value_of_Specialpoint(Specialpoint const sp)
{
  using namespace SPECIALPOINT;

  switch (sp) {
  case NOSPECIALPOINT:
    return 0;
  case CAUGHT_FOX:
  case CAUGHT_FOX_LAST_TRICK:
  case FOX_LAST_TRICK:
  case CHARLIE:
  case CAUGHT_CHARLIE:
  case DOLLE_CAUGHT_DOLLE:
  case HEART_TRICK:
  case DOPPELKOPF:
  case WON:
  case NO90:
  case NO60:
  case NO30:
  case NO0:
    return 1;
  case NO120_SAID:
    return 2;
  case NO90_SAID:
  case NO60_SAID:
  case NO30_SAID:
  case NO0_SAID:
  case NO90_SAID_120_GOT:
  case NO60_SAID_90_GOT:
  case NO30_SAID_60_GOT:
  case NO0_SAID_30_GOT:
  case NO120_REPLY:
  case NO90_REPLY:
  case NO60_REPLY:
  case NO30_REPLY:
  case NO0_REPLY:
  case CONTRA_WON:
    return 1;
  case SOLO:
    return 3;
  case BOCK:
    return 0;
  } // switch(sp)

  return 0;
} // int Value_of_Specialpoint(Specialpoint sp)

/**
 ** -> result
 **
 ** @param	type	the type of the special point
 **
 ** @return	name of the specialpoints
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
string
name(Specialpoint const type)
{
  using namespace SPECIALPOINT;

  switch(type) {
  case NOSPECIALPOINT:
    return "no specialpoint";
  case CAUGHT_FOX:
    return "caught fox";
  case CAUGHT_FOX_LAST_TRICK:
    return "caught fox last trick";
  case FOX_LAST_TRICK:
    return "fox last trick";
  case CHARLIE:
    return "charlie";
  case CAUGHT_CHARLIE:
    return "caught charlie";
  case DOLLE_CAUGHT_DOLLE:
    return "dolle caught dolle";
  case HEART_TRICK:
    return "heart trick";
  case DOPPELKOPF:
    return "Doppelkopf";
  case WON:
    return "won";
  case NO90:
    return "no 90";
  case NO60:
    return "no 60";
  case NO30:
    return "no 30";
  case NO0:
    return "no 0";
  case NO120_SAID:
    return "no 120 said";
  case NO90_SAID:
    return "no 90 said";
  case NO60_SAID:
    return "no 60 said";
  case NO30_SAID:
    return "no 30 said";
  case NO0_SAID:
    return "no 0 said";
  case NO90_SAID_120_GOT:
    return "got 120 against no 90";
  case NO60_SAID_90_GOT:
    return "got 90 against no 60";
  case NO30_SAID_60_GOT:
    return "got 60 against no 30";
  case NO0_SAID_30_GOT:
    return "got 30 against no 0";
  case NO120_REPLY:
    return "no 120 reply said";
  case NO90_REPLY:
    return "no 90 reply said";
  case NO60_REPLY:
    return "no 60 reply said";
  case NO30_REPLY:
    return "no 30 reply said";
  case NO0_REPLY:
    return "no 0 reply said";
  case CONTRA_WON:
    return "contra won";
  case SOLO:
    return "solo";
  case BOCK:
    return "bock";
  } // switch(specialpoints)

  return "";
} // string name(Specialpoint const specialpoint)

/**
 ** -> result
 **
 ** @param	name	special point name
 **
 ** @return	the special point with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
Specialpoint
specialpoint_from_name(string const& name)
throw(ReadException)
{
#ifndef OUTDATED
  // 0.7.5: no 120 -> won
  if (name == "no 120")
    return SPECIALPOINT::WON;
  if (name == "no 90 won")
    return SPECIALPOINT::NO90;
  if (name == "no 60 won")
    return SPECIALPOINT::NO60;
  if (name == "no 30 won")
    return SPECIALPOINT::NO30;
  if (name == "no 0 won")
    return SPECIALPOINT::NO0;
#endif
  for (int s = SPECIALPOINT::FIRST; s <= SPECIALPOINT::LAST; ++s)
    if (name == ::name(static_cast<Specialpoint>(s)))
      return static_cast<Specialpoint>(s);

  throw ReadException("unknown special point '" + name + "'");
  return SPECIALPOINT::NOSPECIALPOINT;
} // Specialpoint specialpoint_from_name(string name)

/**
 ** writes the specialpoint (name) in the output stream
 **
 ** @param	ostr	output stream
 ** @param	type	type to write
 **
 ** @return	output stream
 **
 ** @author	Borg Enders
 **
 ** @version	0.6.8
 **
 **/
ostream&
operator<<(ostream& ostr, Specialpoint const& type)
{
  ostr << name(type);

  return ostr;
} // ostream operator<<(ostream& ostr, Specialpoint type);

/**
 ** constructor
 **
 ** @param	type   the special point type
 ** @param	team   the team which has got the special point
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Specialpoints::Specialpoints(Specialpoint const& type,
                             Team const& team) :
  type(type),
  team(team),
  counting_team(team),
  player_get_no(UINT_MAX),
  player_of_no(UINT_MAX),
  value_(Value_of_Specialpoint(type))
{ }

/**
 ** constructor
 **
 ** @param	type            the special point type
 ** @param	team            the team which has got the special point
 ** @param	counting_team   the counting team
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Specialpoints::Specialpoints(Specialpoint const& type,
                             Team const& team,
                             Team const& counting_team) :
  type(type),
  team(team),
  counting_team(counting_team),
  player_get_no(UINT_MAX),
  player_of_no(UINT_MAX),
  value_(Value_of_Specialpoint(type))
{ }

/**
 ** constructor
 **
 ** @param	type    the special point type
 ** @param	value   the value of the special point (used for bock rounds)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Specialpoints::Specialpoints(Specialpoint const& type,
                             int const value) :
  type(type),
  team(TEAM::NOTEAM),
  counting_team(TEAM::NOTEAM),
  player_get_no(UINT_MAX),
  player_of_no(UINT_MAX),
  value_(value)
{ }

/**
 ** Constructor
 **
 ** @param	istr	input stream to read the special points from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
Specialpoints::Specialpoints(istream& istr) :
  type(SPECIALPOINT::NOSPECIALPOINT),
  team(TEAM::NOTEAM),
  counting_team(TEAM::NOTEAM),
  player_get_no(UINT_MAX),
  player_of_no(UINT_MAX),
  value_(0)
{
  Config config;
  istr >> config;

  this->type = specialpoint_from_name(config.name);
  this->value_ = Value_of_Specialpoint(this->type);
  string::size_type pos_begin = 0;
  while (   isspace(config.value[pos_begin])
         && (pos_begin < config.value.size()))
    pos_begin++;
  string::size_type pos_end = config.value.find(',');
  if (pos_end == string::npos) {
    return ;
  }
  this->team = TEAM::from_name(string(config.value,
                                      pos_begin, pos_end - pos_begin));
  pos_begin = pos_end + 1;
  while (   isspace(config.value[pos_begin])
         && (pos_begin < config.value.size()))
    pos_begin++;
  pos_end = config.value.find(',', pos_begin);
  if (pos_end == string::npos) {
    return ;
  }
  this->counting_team = TEAM::from_name(string(config.value,
                                               pos_begin, pos_end - pos_begin));
  pos_begin = pos_end + 1;
  while (   isspace(config.value[pos_begin])
         && (pos_begin < config.value.size()))
    pos_begin++;
  pos_end = config.value.find(',', pos_begin);
  if (pos_end == string::npos) {
    return ;
  }
  // explicit cast because of problems with MinGW g++-3.2
  this->player_get_no = Unsigned(string(config.value,
                                        pos_begin, pos_end - pos_begin)
                                ).operator unsigned const&();
  pos_begin = pos_end + 1;
  while (   isspace(config.value[pos_begin])
         && (pos_begin < config.value.size()))
    pos_begin++;
  pos_end = config.value.find(',', pos_begin);
  if (pos_end == string::npos) {
    return ;
  }
  // explicit cast because of problems with MinGW g++-3.2
  this->player_of_no = Unsigned(string(config.value,
                                       pos_begin, pos_end - pos_begin)
                               ).operator unsigned const&();
  pos_begin = pos_end + 1;
  while (   isspace(config.value[pos_begin])
         && (pos_begin < config.value.size()))
    pos_begin++;
  pos_end = config.value.size();
  if (pos_end == string::npos) {
    return ;
  }

  DK::Utils::String::from_string(string(config.value,
                                        pos_begin, pos_end - pos_begin),
                                 this->value_);
  DEBUG_ASSERTION((this->type == SPECIALPOINT::BOCK)
                  || (this->value() == Value_of_Specialpoint(this->type)),
                  "Specialpoints::Specialpoints(istream& istr)\n"
                  "  type = " << this->type << " != BOCK and "
                  "value = " << this->value() << " != "
                  << Value_of_Specialpoint(this->type)
                  << " = value of the type");


  return ;
} // Specialpoints::Specialpoints(istream& istr)

/**
 ** writes the specialpoint class in the output stream
 **
 ** @param	ostr		output stream
 ** @param	specialpoint	specialpoint to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
ostream&
operator<<(ostream& ostr, Specialpoints const& specialpoint)
{
  ostr << specialpoint.type << " = "
    << specialpoint.team << ", "
    << specialpoint.counting_team << ", "
    << Unsigned(specialpoint.player_get_no) << ", "
    << Unsigned(specialpoint.player_of_no) << ", "
    << specialpoint.value()
    << '\n';

  return ostr;
} // ostream operator<<(ostream& ostr, Specialpoints specialpoint);

/**
 ** writes the specialpoint vector in the output stream
 **
 ** @param	ostr			output stream
 ** @param	specialpointsvector	specialpoints to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
ostream&
operator<<(ostream& ostr, Specialpointsvector const& specialpointsvector)
{
  for (Specialpointsvector::const_iterator sp = specialpointsvector.begin();
       sp != specialpointsvector.end();
       ++sp)
    ostr << *sp;

  return ostr;
} // ostream operator<<(ostream& ostr, Specialpointsvector specialpointsvector);

/**
 ** -> result
 **
 ** @param      team	teamvector
 **
 ** @return     whether the specialpoint is valid at the end of the game
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.6.1
 **/
bool
Specialpoints::is_valid(vector<Team> const team) const
{
  switch(type) {
  case SPECIALPOINT::CAUGHT_FOX:
  case SPECIALPOINT::CAUGHT_FOX_LAST_TRICK:
  case SPECIALPOINT::CAUGHT_CHARLIE:
  case SPECIALPOINT::DOLLE_CAUGHT_DOLLE:
    return (team[this->player_of_no] != team[this->player_get_no]);
  case SPECIALPOINT::FOX_LAST_TRICK:
  case SPECIALPOINT::CHARLIE:
  case SPECIALPOINT::HEART_TRICK:
  case SPECIALPOINT::DOPPELKOPF:
    return true;
  case SPECIALPOINT::NOSPECIALPOINT:
  case SPECIALPOINT::WON:
  case SPECIALPOINT::NO90:
  case SPECIALPOINT::NO60:
  case SPECIALPOINT::NO30:
  case SPECIALPOINT::NO0:
  case SPECIALPOINT::NO120_SAID:
  case SPECIALPOINT::NO90_SAID:
  case SPECIALPOINT::NO60_SAID:
  case SPECIALPOINT::NO30_SAID:
  case SPECIALPOINT::NO0_SAID:
  case SPECIALPOINT::NO90_SAID_120_GOT:
  case SPECIALPOINT::NO60_SAID_90_GOT:
  case SPECIALPOINT::NO30_SAID_60_GOT:
  case SPECIALPOINT::NO0_SAID_30_GOT:
  case SPECIALPOINT::NO120_REPLY:
  case SPECIALPOINT::NO90_REPLY:
  case SPECIALPOINT::NO60_REPLY:
  case SPECIALPOINT::NO30_REPLY:
  case SPECIALPOINT::NO0_REPLY:
  case SPECIALPOINT::CONTRA_WON:
  case SPECIALPOINT::SOLO:
  case SPECIALPOINT::BOCK:
    return true;
  } // switch(specialpoints)
  return false;
} // bool Specialpoints::is_valid(vector<Team> const team) const

/**
 ** -> result
 **
 ** @param     spv     vector with the special points
 ** @param     winner   the winnerteam
 ** @param     spv     vector with the special points
 **
 ** @result    sum of the special points for winnerteam minus all specialpoints of the losing team
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
Sum_of_Specialpoints(Specialpointsvector const& spv,
                     Team const& winner,
                     Game const& g)
{
  int re_played=0;
  int re_ann=0;
  int re_trick=0;
  int contra_played=0;
  int contra_trick=0;
  int contra_ann=0;
  int no120s=1;

  for (unsigned int i=0; i<spv.size(); i++)  {
#ifdef BENDERS
#ifndef RELEASE
    DEBUG_PRINTING((spv[i].team!=TEAM::RE) &&
                   (spv[i].team!=TEAM::CONTRA),
                   "WARNING: Specialpoints for undetermined team");

    if (   (spv[i].team != TEAM::RE)
        && (spv[i].team != TEAM::CONTRA)
        && (spv[i].type != SPECIALPOINT::BOCK) ) {
      COUT << "Game: \n"
        << "  Current trick no: " << g.trick_current_no()
        << endl;

      COUT << "Teams: " << endl;
      for (unsigned p = 0; p < g.playerno(); p++)
        COUT << p << ": " << g.team(g.player(p)) << endl;

      COUT << "Specialpoint:\n"
        << "  Type: " << spv[i].type << '\n'
        << "  Team: " << spv[i].team << '\n'
        << "  player_get_no: " << spv[i].player_get_no << '\n'
        << "  player_of_no:  " << spv[i].player_of_no << '\n'
        << endl;
      DEBUG_ASSERTION((spv[i].team==TEAM::RE)
                      || (spv[i].team==TEAM::CONTRA),
                      "WARNING: Specialpoints for undetermined team");
    } // if (undetermined team)
#endif
#endif


    Specialpoints const& sp = spv[i];
    // splitting of points in Announcements and trick specialpoints 
    // for doubling with RE
    switch (spv[i].type) {
    case SPECIALPOINT::NOSPECIALPOINT:
      break;

    case SPECIALPOINT::CAUGHT_FOX:
    case SPECIALPOINT::CAUGHT_FOX_LAST_TRICK:
    case SPECIALPOINT::FOX_LAST_TRICK:
    case SPECIALPOINT::CHARLIE:
    case SPECIALPOINT::CAUGHT_CHARLIE:
    case SPECIALPOINT::DOLLE_CAUGHT_DOLLE:
    case SPECIALPOINT::HEART_TRICK:
    case SPECIALPOINT::DOPPELKOPF:
      if (sp.counting_team == TEAM::RE)
        re_trick += spv[i].value();
      else
        contra_trick += spv[i].value();
      break;

    case SPECIALPOINT::WON:
      if (sp.counting_team == TEAM::RE)
        re_played += spv[i].value();
      else
        contra_played += spv[i].value();
      break;
    case SPECIALPOINT::NO90:
    case SPECIALPOINT::NO60:
    case SPECIALPOINT::NO30:
    case SPECIALPOINT::NO0:
      if (sp.counting_team == TEAM::RE)
        re_played += spv[i].value();
      else
        contra_played += spv[i].value();
      break;
    case SPECIALPOINT::NO120_SAID:
      if (::party.rule()(Rule::ANNOUNCEMENT_RE_DOUBLES)) {
        no120s *= spv[i].value();
      } else {
        if (sp.counting_team == TEAM::RE)
          re_ann += spv[i].value();
        else
          contra_ann+= spv[i].value();
      }
      break;  
    case SPECIALPOINT::NO90_SAID:
    case SPECIALPOINT::NO60_SAID:
    case SPECIALPOINT::NO30_SAID:
    case SPECIALPOINT::NO0_SAID:
      if (sp.counting_team == TEAM::RE)
        re_ann += spv[i].value();
      else
        contra_ann+= spv[i].value();
      break;
    case SPECIALPOINT::NO90_SAID_120_GOT:
    case SPECIALPOINT::NO60_SAID_90_GOT:
    case SPECIALPOINT::NO30_SAID_60_GOT:
    case SPECIALPOINT::NO0_SAID_30_GOT:
      if (sp.counting_team == TEAM::RE)
        re_played += spv[i].value();
      else
        contra_played += spv[i].value();
      break;
    case SPECIALPOINT::NO120_REPLY:
    case SPECIALPOINT::NO90_REPLY:
    case SPECIALPOINT::NO60_REPLY:
    case SPECIALPOINT::NO30_REPLY:
    case SPECIALPOINT::NO0_REPLY:
      if (sp.counting_team == TEAM::RE)
        re_ann += spv[i].value();
      else
        contra_ann+= spv[i].value();
      break;
    case SPECIALPOINT::CONTRA_WON:
      if (::party.rule()(Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE))
        contra_played += spv[i].value();
      else
        contra_trick += spv[i].value();
      break;

    case SPECIALPOINT::SOLO:
      // multiplier
      break;
    case SPECIALPOINT::BOCK:
      // multiplier
      break;
    } // switch (spv[i].type)
  } // for (unsigned int i=0; i<spv.size(); i++)


  DEBUG_ASSERTION((no120s <= (g.rule()(Rule::KNOCKING) ? 16 : 4)),
                  "Sum_of_Specialpoints():\n"
                  "  'no120s' = " << no120s << " > " << (g.rule()(Rule::KNOCKING) ? 16 : 4) << "\n"
                  << spv);

  { // add the points
    int result = 0;
    switch (winner) {
    case TEAM::RE:
      result += re_played + re_ann - contra_played;
      result *= no120s;
      result += re_trick - contra_trick; 
      break;


    case TEAM::CONTRA:
      result += contra_played + contra_ann - re_played;
      result *= no120s;
      result += contra_trick - re_trick; 
      break;

    case TEAM::NOTEAM:
      result += re_played - contra_played;
      result *= no120s;
      result += re_trick - contra_trick; 
      break;

    default:
      break;
    } // switch (winner)

    // multiply with the bock value
    for (unsigned int i = 0; i < spv.size(); i++)
      if (spv[i].type == SPECIALPOINT::BOCK)
        result *= spv[i].value();

    return result;
  } // add the points
} // int Sum_of_Specialpoints(Specialpointsvector, Team winner)


/**
 ** -> result
 **
 ** @param      special_points   special point
 **
 ** @return     the translation of the specialpoint
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **/
Translator::Translation
translation(Specialpoints const& special_points)
{
  return ::translation(special_points.type);

#ifdef OUTDATED
  // 2010-03-27: outdated because of knocking rule -- so now one must know the rule for the translation :-(
  switch(special_points.type) {
  case SPECIALPOINT::NO120_SAID:
    if (special_points.team == TEAM::RE)
      return Translator::Translation(::translator,
                                     "SpecialPoint::re said");
    else if (special_points.team == TEAM::CONTRA)
      return Translator::Translation(::translator,
                                     "SpecialPoint::contra said");

  default:
    return ::translation(special_points.type);
  }; // switch(special_points.type)
#endif
} // Translator::Translation translation(Specialpoints special_points)
