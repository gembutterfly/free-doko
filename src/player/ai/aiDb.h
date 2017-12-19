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

#ifndef CLASS_AIDB
#define CLASS_AIDB

#include "../playersDb.h"
#include "../aiconfig.h"

class AiDb : public PlayersDb {
  public:
    AiDb() :
      PlayersDb(),
    heuristics_(Aiconfig::HEURISTIC_LAST + 1 )
    { } 

    AiDb(istream& istr) :
      PlayersDb(),
    heuristics_(Aiconfig::HEURISTIC_LAST + 1)
    {
      this->read(istr);
    }

    AiDb( PlayersDb const& players_db ) :
      PlayersDb( players_db ),
    heuristics_(Aiconfig::HEURISTIC_LAST + 1 )
    {}

    AiDb( AiDb const& aidb ) :
      PlayersDb( aidb ),
    heuristics_( aidb.heuristics_ )
    {}

    ostream& write(ostream& ostr) const;
  private:
    bool read_group(string const& name, istream& istr);
    void read_group_heuristics(istream& istr);

  public:
    unsigned heuristics_group_general() const;
    unsigned heuristics_group_poverty() const;
    unsigned heuristics_group_solo() const;
    unsigned heuristics_group_solo_color() const;
    unsigned heuristics_group_solo_picture() const;
    unsigned heuristics_group_solo_meatless() const;

    /// how often each statistic was used
    unsigned int heuristic( Aiconfig::Heuristic const heuristic) const
    {
      // *** assert( i >= 0 && (unsigned int)i < heuristics_.size() );
      return heuristics_[ heuristic ];	
    }

    void heuristicInc(Aiconfig::Heuristic const heuristic)
    {
      heuristics_[ heuristic ]++;
    }

    virtual void clear();

  private:
    // number of usage for each statistic
    std::vector< unsigned int > heuristics_;	
}; // class AiDb : public PlayersDb

#endif // #ifndef CLASS_AIDB
