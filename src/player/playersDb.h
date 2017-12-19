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



#ifndef CLASS_PLAYERSDB_HEADER
#define CLASS_PLAYERSDB_HEADER

#include "../game/specialpoint.h"
#include "player.h"
class GameSummary;

/// [T]otal[L]ost[W]on count
class TLWcount
{
  public:
    TLWcount(): total_(0), lost_(0), won_(0){};

    ostream& write(ostream& ostr) const
    {
      ostr << this->total() << '\t'
	<< this->won() << '\t'
	<< this->lost();
      return ostr;
    }
    istream& read(istream& istr)
    {
      istr >> this->total_
	>> this->won_
	>> this->lost_;
      return istr;
    }

    void hasLost() { total_++; lost_++; };
    void hasWon()  { total_++; won_++; };

    unsigned long long total() const { return total_; }
    unsigned long long lost() const { return lost_; }
    unsigned long long won() const { return won_; }

    void clear()
    {
      this->total_ = 0;
      this->lost_ = 0;
      this->won_ = 0;

      return ;
    }

    TLWcount& operator+=(TLWcount const& tlwcount)
    {
      this->total_ += tlwcount.total();
      this->lost_ += tlwcount.lost();
      this->won_ += tlwcount.won();

      return *this;
    }

  private:
    unsigned long long total_;
    unsigned long long lost_;
    unsigned long long won_;	
}; // class TLWcount

ostream& operator<<(ostream& ostr, TLWcount const& tlwcount);
istream& operator>>(istream& istr, TLWcount& tlwcount);


class Ranking
{
  public:
    Ranking( double const d = 0 ) : value_( d ) {};

    ostream& write(ostream& ostr) const
    { 
      ostr << this->value();
      return ostr;
    }
    istream& read(istream& istr)
    {
      istr >> this->value_;
      return istr;
    }

    void add( Ranking const r )
    {
      this->value_ = ( 0.9 * this->value_ + r.value_ ) / 1.9;
    }

    double value() const { return value_; };

    Ranking& operator=( Ranking const r )
    {
      this->value_ = r.value_;
      return *this;
    }

    Ranking& operator+=( Ranking const& b )
    {
      this->add( b );
      return *this;
    };

    void clear()
    {
      this->value_ = 0;
      return ;
    }

  private:
    double value_;
}; // class Ranking

ostream& operator<<(ostream& ostr, Ranking const& ranking);
istream& operator>>(istream& istr, Ranking& ranking);


class PlayersDb
{
  public:
    PlayersDb();
    PlayersDb(istream& istr);
    PlayersDb( PlayersDb const& p );

    virtual ~PlayersDb() {}

    virtual ostream& write(ostream& ostr) const;
    istream& read(istream& istr);
  protected:
    virtual bool read_group(string const& name, istream& istr);
  private:
    void read_group_games(istream& istr);
    void read_group_specialpoints(istream& istr);
  public:

    virtual void evaluate( const Player& p, GameSummary const& game_summary );

    /// number of each played game  lost and won
    TLWcount const& games( GAMETYPE::GameType const i ) const
    {
      DEBUG_ASSERT( i >= 0 && static_cast<size_t>(i) < games_.size() );
      return games_[i];
    }	

    /// number of special points made or given away
    TLWcount const& specialpoints( SPECIALPOINT::Specialpoint const i ) const
    {
      DEBUG_ASSERT( i >= 0 && static_cast<size_t>(i) < specialpoints_.size() );
      return specialpoints_[i];
    }	

    TLWcount const& games_all() const
    { return this->games_all_; }
    TLWcount games_group_marriage() const;
    TLWcount games_group_poverty() const;
    TLWcount games_group_solo() const;
    TLWcount games_group_solo_color() const;
    TLWcount games_group_solo_picture() const;
    TLWcount games_group_solo_picture_single() const;
    TLWcount games_group_solo_picture_double() const;

    TLWcount const& specialpoints_all() const
    { return this->specialpoints_all_; }
    TLWcount specialpoints_group_winning() const;
    TLWcount specialpoints_group_announcement() const;

    /// average points made with each game
    double averageGamePoints() const
    { return averageGamePoints_; }

    /// average point made with trikcs in each game
    double averageGameTrickPoints() const
    { return averageGameTrickPoints_; }

    virtual void clear();

    Ranking rank;
  private:

    double averageGamePoints_;
    double averageGameTrickPoints_;

    TLWcount games_all_;
    std::vector<TLWcount> games_; // a map would be better

    TLWcount specialpoints_all_;
    std::vector<TLWcount> specialpoints_; // a map would be better
}; // class PlayersDb

ostream& operator<<(ostream& ostr, PlayersDb const& players_db);
istream& operator>>(istream& istr, PlayersDb& players_db);

#endif // #ifndef CLASS_PLAYERSDB_HEADER
