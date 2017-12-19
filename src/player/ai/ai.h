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

#ifndef CLASS_AI_HEADER
#define CLASS_AI_HEADER

#include "aiDb.h"
#include "VirtualGamesInterface.h"
#include "HeuristicInterface.h"

#include "../player.h"
#include "../aiconfig.h"
#include "../../game/game.h"
#include "../../game/exception.h"
#include "../../card/cardvector.h"

#include "../../basistypes.h"

class VirtualGamesAi;

class Ai: public Player, public Aiconfig {
  friend class WVirtualGames;
  friend class Gametree;
  friend class Heuristics;
  friend class TrickWeighting;

  friend class VirtualGamesAi;
  friend class HeuristicAi;
  public:

  Ai();
  Ai(Aiconfig const& aiconfig);
  Ai(Player const& player);
  Ai(Player const& player, Aiconfig const& aiconfig);
  Ai(istream& istr);
  Ai(Ai const&);
  Ai& operator=(Ai const&);
  // clone the player
  virtual Ai* clone() const;

  virtual ~Ai();

  virtual operator HeuristicInterface const&() const;
  virtual operator VirtualGamesInterface const&() const;

  // write the ai in the stream
  virtual ostream& write(ostream& ostr) const;

  // read the config 'config' from 'istr'
  virtual bool read(Config const& config, istream& istr);

  // compares the ai
  virtual bool isequal(Ai const& ai) const;

  AiType const& aitype() const;
  unsigned const future_limit() const;
  Rating::Type const& rating() const;

  // set the cards information
  void set_cards_information(CardsInformation const& cards_information);

  // set the game
  virtual void set_game(Game& game);
  virtual Hand const& set_hand(Hand const& hand);

  // h hands of players 0 till number_of_players number of player for Weighting
  virtual void new_game(Game& game);
  virtual Reservation const& reservation_get( bool const isDuty = false );
  virtual void rule_changed(int const type, void const* const old_value);
  virtual void game_open(Game& game);
  virtual void game_start();
  virtual void game_close(Game const& game);
  virtual HandCard card_get();
  virtual HandCard card_suggestion();
  virtual void card_played(HandCard const& card);
  virtual void check_swines_announcement_at_game_start();
  virtual void swines_announced(Player const& player);
  virtual void hyperswines_announced(Player const& player);
  virtual void marriage(Player const& bridegroom, Player const& bride);
  virtual void genscher(Player const& genscher, Player const& partner);
  virtual Card nextcard(const Trick& trick);

  // the teaminfo of 'player'
  Team const& teaminfo(Player const& player) const; 
  // update the teaminfo
  // (compare with the teaminfo of the game and check the current trick
  virtual void teaminfo_update();
  // the partner of the player
  Player const* partner(Player const& player) const;
  // the points the team has made
  unsigned points_of_team() const;

  // set the teams
  virtual void set_teams(vector<Team> const& teams);

  // the trick is opened
  virtual void trick_open(Trick const& trick);
  // the trick is full
  virtual void trick_full(Trick const& trick);

  Announcement announcement_request() const;
  void announcement_made(Announcement const& announcement,
			 Player const& player);

  bool colorjabbed(const Card::Color c) const;

  unsigned color_runs(Card::Color const color) const;
  unsigned color_runs(Trick const& trick) const;

  bool jabbedbyownteam(const Card::Color c) const;

  bool jabbedbyotherteam(const Card::Color c) const;


  bool next_low() const;

  void check_low_high(HandCard const& card);

  bool single_ace() const;

  protected:
  unsigned const& last_trick_to_calculate() const;
  void set_last_trick_to_calculate(unsigned const last_trick_to_calculate);

  // 'player' shifts cards
  virtual HandCards poverty_shift();
  // returns whether 'player' accepts the poverty
  virtual bool poverty_take_accept(unsigned const cardno);
  // the player changes the cards of the poverty
  virtual HandCards poverty_cards_change(HandCards const& cards);
  // the poverty player gets 'cards'
  virtual void poverty_cards_get_back(HandCards const& cards);

  public:
  Card trump_card_limit() const;
  Card lowest_trump_card_limit() const;

  private:

  MarriageSelector get_Marriage() const;

  public:
  virtual Player const* genscher_partner();

  private:

  int calculate_meatless_color( Card::Color const co );

  Card nextcard_meatless(const Trick& t);

  int checksolo(GameType gametype) const;
  int checksolo_single_picture(GameType gametype) const;
  int checksolo_double_picture(GameType gametype) const;
  int checksolo_koehler(GameType gametype) const;
  int checksolo_color(GameType gametype) const; 
  int checksolo_meatless(GameType gametype) const;

  PRIV_VAR_P_RW(TeamInformation, team_information);
  PRIV_VAR_P_RW(CardsInformation, cards_information);
  // Team of all players known by this player
  vector<Team> teaminfo_p;
  vector<Trick> trick_p; // last played tricks
  unsigned trickno_p;  
  vector<vector<bool> > colorjabbed_p; // remembering for each player if he has jabbed this color
  bool low_p; // next card should be low or high trump
  Card::Color meatlesscolor_p;
  vector<bool> trumpless_; // players has no trumps
  Card::Color firstMarriageColor_;

  unsigned last_trick_to_calculate_p;

#ifdef WORKAROUND
  public:
#endif
  Aiconfig::Heuristic lastHeuristic_;
#ifdef WORKAROUND
  private:
#endif

  AiDb* db(){ return dynamic_cast<AiDb*>(db_); }
  AiDb const* db() const { return dynamic_cast<AiDb const*>(this->db_); }

  public:
  Hand const& handofplayer( Player const& player ) const;
  Team teamofplayer( Player const& player ) const; 

  protected:
  VirtualGamesInterface* vgi_;
  HeuristicInterface* hi_;

  bool silentMarriage_;
}; // class Ai

class VirtualGamesAi: public VirtualGamesInterface {
  public:	
    VirtualGamesAi( Ai const& a ): VirtualGamesInterface(), ai_( &a ){}
    ~VirtualGamesAi() {}

    CardsInformation const& cards_information() const
    { return this->ai_->cards_information(); }
    TeamInformation const& team_information() const
    { return this->ai_->team_information(); }
    
    Player const& player() const
    { return *this->ai_; }
    Hand const& handofplayer( Player const& player ) const 
    { return this->ai_->handofplayer( player ); }
    Team teamofplayer( Player const& player ) const 
    { return this->ai_->teamofplayer( player ); }
 
    // create a virtual game with virtual players (uses cards information and team information)
    Game* create_virtual_game() const;

    Hand const& hand() const
    { return this->ai_->hand(); }
    Team team() const 
    { return this->ai_->team(); }
    Game const& game() const
    { return this->ai_->game(); }
    unsigned const last_trick_to_calculate() const 
    { return this->ai_->last_trick_to_calculate(); }

    bool value(const Aiconfig::TypeBool& type) const
    { return this->ai_->value( type ); }
    int value(const Aiconfig::TypeInt& type) const 
    { return this->ai_->value( type ); }
    Card value(const Aiconfig::TypeCard& type) const
    { return this->ai_->value( type ); }

    unsigned color_runs(const Card::TColor c) const 
    { return this->ai_->color_runs( c ); }
    bool colorjabbed(const Card::TColor c) const 
    { return this->ai_->colorjabbed( c ); }

    bool jabbedbyownteam(const Card::TColor c) const
    { return this->ai_->jabbedbyownteam( c ); }
    bool jabbedbyotherteam(const Card::TColor c) const
    { return this->ai_->jabbedbyotherteam( c ); }
    
    Card trump_card_limit() const 
    { return this->ai_->trump_card_limit(); }
  private:
    Ai const* ai_;	
}; // class VirtualGamesAi : public VirtualGameInterface

class HeuristicAi: public HeuristicInterface {
  public:	
    HeuristicAi( Ai const& a ): HeuristicInterface(), ai_(&a ) { };
    ~HeuristicAi() { }

    CardsInformation const& cards_information() const
    { return this->ai_->cards_information(); }
    TeamInformation const& team_information() const
    { return this->ai_->team_information(); }

 
    Player const& player() const
    { return *this->ai_; }
    Hand const& handofplayer( Player const& player ) const 
    { return this->ai_->handofplayer( player ); }
    Team teamofplayer( Player const& player ) const 
    { return this->ai_->teamofplayer( player ); }

    Hand const& hand() const
    { return this->ai_->hand(); }
    Team team() const 
    { return this->ai_->team(); }
    unsigned no() const 
    { return this->game().no(*this->ai_); }
    Game const& game() const
    { return this->ai_->game(); }
    unsigned const last_trick_to_calculate() const 
    { return this->ai_->last_trick_to_calculate(); }

    bool value(const Aiconfig::TypeBool& type) const
    { return this->ai_->value( type ); }
    int value(const Aiconfig::TypeInt& type) const 
    { return this->ai_->value( type ); }
    Card value(const Aiconfig::TypeCard& type) const
    { return this->ai_->value( type ); }

    bool colorjabbed(const Card::TColor c) const 
    { return this->ai_->colorjabbed( c ); }

    unsigned color_runs(const Card::TColor c) const 
    { return this->ai_->color_runs( c ); }

    bool jabbedbyownteam(const Card::TColor c) const
    { return this->ai_->jabbedbyownteam( c ); }

    bool jabbedbyotherteam(const Card::TColor c) const
    { return this->ai_->jabbedbyotherteam( c ); }


    bool next_low() const
    { return this->ai_->next_low() ; }

    Card trump_card_limit() const 
    { return this->ai_->trump_card_limit(); }

    Card lowest_trump_card_limit() const 
    { return this->ai_->lowest_trump_card_limit(); }

  private:
    Ai const* ai_;	
  private:
    HeuristicAi();
    HeuristicAi(HeuristicAi const&);
    HeuristicAi& operator=(HeuristicAi const&);
}; // class HeuristicAi

#endif // #ifndef CLASS_AI_HEADER
