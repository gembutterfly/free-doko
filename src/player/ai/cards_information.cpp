/*********************************************************************
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

#include "cards_information.h"
#include "cards_information.of_player.h"
#include "cards_information.heuristics.h"

#include "ai.h"

#include "../../card/trick.h"
#include "../../party/party.h"
#include "../../party/rule.h"

// whether to update only when a hand is requested
#define UPDATE_ON_DEMAND
#ifdef NO_UPDATE_ON_DEMAND
#undef UPDATE_ON_DEMAND
#endif
#ifdef CHECK_RUNTIME
#include "../../runtime.h"
#endif

// whether to make self checks
// local macro
#ifndef DEBUG_NO
#ifndef RELEASE
#define SELF_CHECK
#endif
#endif

/*
 * CardsInformation contains the information a player has of the distribution
 * of the cards in a game. The player does not necessarily be an active player,
 * The 'user' of this class has to call the methods corresponding to the
 * gameplay.
 * Information can be contained from the methods 'possible_hand(Player)',
 * 'estimated_hand(Player) and with 'of_player(Player)' specific information
 * about a player.
 *
 * According to the number of remembered tricks the first played cards are
 * unknown (and with it that a player does not have the color -- 
 * nearly just as if the first tricks did not exists at all.
 * In virtual games there is no additional information lost when calculating
 * one trick further.
 *
 * The idea is to take all information from the gameplay to tell which cards
 * - are played
 * - a specific player must have
 * - a specific player cannot have
 * Merging the informations between the players (see CardsInformation::OfPlayer)
 * more data can be created (if par example three players do not have spade,
 * then the last one must have all remaining spade cards).
 *
 * The collected informations are
 * - the hand of the ai is set
 *   the other players cannot have the cards
 *   (in virtual games a bit more complicated, since the hand can contain more
 *    cards than the ai has to play)
 * - a card is played
 *   the card does not longer remain, it is marked as played by the player
 * - a player has not served a color
 *   the player has no more card of the color
 * - announcement re (normal game)
 *   the player has at least one club queen (can be already played)
 * - announcement contra (normal game)
 *   the player has no club queen
 * - marriage
 *   the player has both club queens
 * - poverty
 *   the poverty player has as many trumps as he has gotten back
 * - announcement swines / hyperswines / genscher
 *   the player has both trump aces / nines / kings
 *
 * When more information was collected the information about a player is
 * updated as good as I could program it (see 'OfPlayer').
 *
 * 'CardsInformation' gets noted about changed cards ('queue_update()')
 * Then the 'of_player's are informed about the changes ('do_update()') till no
 * changed card remains (the 'of_player's can add cards again).
 *
 */

/*
 * Implemention remark
 * the class cannot be an 'OS' because then the functions are not called in
 * virtual games
 */

/**
 ** constructor
 **
 ** @param     player			corresponding player
 **
 ** @return    -
 **/
CardsInformation::CardsInformation(Ai const& player) :
  player_(&player)
{ this->reset(); }

/**
 ** copy constructor
 **
 ** @param     cards_information	object to be copied
 **
 ** @return    -
 **/
CardsInformation::CardsInformation(CardsInformation const& cards_information) :
  player_(cards_information.player_),
  of_player_(cards_information.of_player_),
  color_runs_(cards_information.color_runs_),
  played_(cards_information.played_),
  tcolor_played_(cards_information.tcolor_played_),
  in_recalcing(cards_information.in_recalcing),
  in_update(cards_information.in_update),
  possible_hands_(cards_information.possible_hands_),
  estimated_hands_(cards_information.estimated_hands_),
  auto_update_(cards_information.auto_update_),
  cards_to_update(cards_information.cards_to_update),
  hands_outdated(cards_information.hands_outdated)
{
  for (auto& p : this->of_player_)
    p.set_cards_information(*this);
} // CardsInformation::CardsInformation(CardsInformation cards_information)

/**
 ** copy operator
 **
 ** @param     cards_information	object to be copied
 **
 ** @return    object
 **/
CardsInformation&
CardsInformation::operator=(CardsInformation const& cards_information)
{
  DEBUG_ASSERTION((!this->in_recalcing
                   && !cards_information.in_recalcing),
                  "CardsInformation::operator=(CardInformation)\n"
                  "  'this' or 'cards_information' is in recalcing");
  DEBUG_ASSERTION((!this->in_update
                   && !cards_information.in_update),
                  "CardsInformation::operator=(CardInformation)\n"
                  "  'this' or 'cards_information' is in update");


  this->player_ = cards_information.player_;
  this->of_player_ = cards_information.of_player_;
  this->color_runs_ = cards_information.color_runs_;
  for (auto& p : this->of_player_)
    p.set_cards_information(*this);

  this->played_ = cards_information.played_;
  this->tcolor_played_ = cards_information.tcolor_played_;
  this->possible_hands_ = cards_information.possible_hands_;
  this->estimated_hands_ = cards_information.estimated_hands_;
  this->auto_update_ = cards_information.auto_update_;
  this->cards_to_update = cards_information.cards_to_update;
  this->hands_outdated = cards_information.hands_outdated;

#ifdef SELF_CHECK
  this->self_check();
#endif

  return *this;
} // CardsInformation& CardsInformation::operator=(CardsInformation cards_information)

/**
 ** destructor
 **/
CardsInformation::~CardsInformation()
{ }

/**
 ** writes 'cards_information' in 'ostr'
 **
 ** @param     ostr			output stream
 ** @param     cards_information	object to write into 'ostr'
 **
 ** @return    the output stream
 **/
ostream&
operator<<(ostream& ostr, CardsInformation const& cards_information)
{
  cards_information.write_summary(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, CardsInformation cards_information)

/**
 ** writes 'cards_information' in 'ostr'
 **
 ** @param     ostr		output stream
 **
 ** @return    -
 **/
void
CardsInformation::write(ostream& ostr) const
{
  ostr << "cards information:\n"
    << "{\n";

  ostr << "played:\n"
    << "{\n"
    << this->played_
    << "}\n"
    << "tcolor played:\n"
    << "{\n"
    << this->tcolor_played_
    << "}\n"
    << '\n';

  for (auto const& p : this->of_player_)
    ostr << p << '\n';

  ostr << "}\n";
} // void CardsInformation::write(ostream& ostr) const


/**
 ** writes the summary of 'cards_information' in 'ostr'
 **
 ** @param     ostr		output stream
 **
 ** @return    -
 **/
void
CardsInformation::write_summary(ostream& ostr) const
{
  ostr << "cards information summary:\n"
    << "  {\n";
  ostr << setw(19) << "|";
  for (unsigned p = 0; p < this->game().playerno(); ++p)
    ostr << " " << p
      << (p == this->game().no(this->player()) ? '*' : ' ')
      << " "
      << this->game().player(p).name() << setw(10 - this->game().player(p).name().length()) << "|";
  ostr << '\n';
  ostr << "-------------+----+-------------+-------------+-------------+-------------+\n";
  { // tcolors
    vector<Card::TColor> tcolors;
    tcolors.push_back(Card::TRUMP);
    for (auto c : this->game().rule().card_colors())
      tcolors.push_back(c);
    for (auto c : tcolors) {
      if (this->remaining(c)) {
        ostr << setw(13) << c << ":" << setw(3) << this->remaining(c) << " |";
        for (unsigned p = 0; p < this->game().playerno(); ++p) {
          if (this->of_player(p).can_have(c) == 0) {
            ostr << setw(14) << "|";
          } else {
            if (this->of_player(p).must_have(c))
              ostr << setw(2) << this->of_player(p).must_have(c);
            else
              ostr << " -";
            ostr << " ";
            if (  this->of_player(p).can_have(c)
                > this->of_player(p).must_have(c)) {
              ostr << setw(2) << (  this->of_player(p).can_have(c)
                                  - this->of_player(p).must_have(c))
                << "  "
                << setw(5) << ""
                << " |";
            } else {
              ostr << " -" << setw(9) << "|";
            }
          }

        } // for (p)
        ostr << '\n';
      } // if (this->remaining(c))
    } // for (c : card_colors)
  } // tcolors
  ostr << "-------------+----+-------------+-------------+-------------+-------------+\n";
  vector<Card> cards = this->game().rule().cards();
  cards.push_back(Card::UNKNOWN);
  for (auto c : cards) {
    if (this->remaining(c)) {
      ostr << setw(13) << c << ":" << setw(3) << this->remaining(c) << " |";
      for (unsigned p = 0; p < this->game().playerno(); ++p) {
        ostr << " ";
        if (this->of_player(p).can_have(c) == 0) {
          ostr << setw(13) << "|";
        } else {
          if (this->of_player(p).must_have(c))
            ostr << this->of_player(p).must_have(c);
          else
            ostr << "-";
          ostr << "  ";
          if (  this->of_player(p).can_have(c)
              > this->of_player(p).must_have(c)) {
            ostr << (  this->of_player(p).can_have(c)
                     - this->of_player(p).must_have(c))
              << "  "
              << setw(5) << this->of_player(p).weighting(c)
              << " |";
          } else {
            ostr << "-" << setw(9) << "|";
          }
        }

      } // for (p)
      ostr << '\n';
    } // if (this->remaining(c))
  } // for (c : cards)
  { // sum
    ostr << "-------------+----+-------------+-------------+-------------+-------------+\n";
    ostr << setw(14) << "sum:" << setw(3) << this->remaining_cards_no() << " |";
    for (unsigned p = 0; p < this->game().playerno(); ++p) {
      if (this->of_player(p).can_have_.cards_no() == 0) {
        ostr << setw(13) << "|";
      } else {
        ostr << setw(2);
        if (this->of_player(p).must_have_.cards_no())
          ostr << this->of_player(p).must_have_.cards_no();
        else
          ostr << "-";
        ostr << " ";
        ostr << setw(2);
        if (  this->of_player(p).can_have_.cards_no()
            > this->of_player(p).must_have_.cards_no()) {
          ostr << (  this->of_player(p).can_have_.cards_no()
                   - this->of_player(p).must_have_.cards_no());
        } else {
          ostr << "-";
        }
        ostr << " / " << setw(2) << this->cardno_to_play(this->game().player(p))
          << setw(4) << "|";
      }
    } // for (p)
    ostr << '\n';
  } // sum
  ostr << "  must have     can have  /  cardno to play\n";
  ostr << "  }\n";
} // void CardsInformation::write_summary(ostream& ostr) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the cards information are virtual
 **/
bool
CardsInformation::is_virtual() const
{
  return (   this->game().isvirtual()
          || (this != &this->player().cards_information())
          || (this->player() != this->game().player(this->game().no(this->player())))
         );
} // bool CardsInformation::is_virtual() const

/**
 ** resets all information
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::reset()
{
  this->played_.clear();
  this->tcolor_played_.clear();

  this->of_player_.clear();
  for (unsigned p = 0; p < this->game().playerno(); ++p)
    this->of_player_.push_back(OfPlayer(*this, p));

  this->color_runs_ = vector<unsigned>(Card::MAXCARDCOLOR+1, 0);

  this->possible_hands_.clear();
  this->possible_hands_.resize(this->game().playerno());
  this->estimated_hands_.clear();
  this->estimated_hands_.resize(this->game().playerno());

  this->hands_outdated = true;
} // void CardsInformation::reset()

/**
 ** recalcs all information
 ** takes 'remembertricksno' into account
 ** does remember all color runs and all 'important' cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::recalc()
{
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  this->in_recalcing = true;

  this->reset();

  if (::game_status >= GAMESTATUS::GAME_PLAY) {
    // the first trick to take into account
    auto const trick_begin
      = ((static_cast<int>(::party.game().trick_current_no())
          <= this->player().value(Aiconfig::REMEMBERTRICKS))
         ? 0
         : (::party.game().trick_current_no()
            - static_cast<unsigned>(this->player().value(Aiconfig::REMEMBERTRICKS))));

    { // remember own cards and important cards
      vector<Card> important_cards;
      switch (this->game().type()) {
      case GAMETYPE::THROWN_NINES:
      case GAMETYPE::THROWN_KINGS:
      case GAMETYPE::THROWN_NINES_AND_KINGS:
      case GAMETYPE::THROWN_RICHNESS:
      case GAMETYPE::FOX_HIGHEST_TRUMP:
        break;
      case GAMETYPE::NORMAL:
      case GAMETYPE::POVERTY:
      case GAMETYPE::GENSCHER:
      case GAMETYPE::MARRIAGE:
      case GAMETYPE::MARRIAGE_SOLO:
        // remember fox, club queens and special cards
        important_cards.push_back(Card::FOX);
      case GAMETYPE::MARRIAGE_SILENT:
        important_cards.push_back(Card::CLUB_QUEEN);
      case GAMETYPE::SOLO_CLUB:
      case GAMETYPE::SOLO_HEART:
      case GAMETYPE::SOLO_SPADE:
      case GAMETYPE::SOLO_DIAMOND:
        // remember special cards
        if (this->game().rule()(Rule::DOLLEN))
          important_cards.push_back(this->game().dolle());
        if (this->game().swines_announced())
          important_cards.push_back(this->game().swine());
        if (this->game().hyperswines_announced())
          important_cards.push_back(this->game().hyperswine());
        break;

      case GAMETYPE::SOLO_JACK:
      case GAMETYPE::SOLO_QUEEN:
      case GAMETYPE::SOLO_KING:
      case GAMETYPE::SOLO_QUEEN_JACK:
      case GAMETYPE::SOLO_KING_JACK:
      case GAMETYPE::SOLO_KING_QUEEN:
      case GAMETYPE::SOLO_KOEHLER:
        // remember trumps and aces
        important_cards = this->game().trumps();

      case GAMETYPE::SOLO_MEATLESS:
        // remember aces
        for (auto c : this->game().rule().card_colors())
          important_cards.push_back(Card(c, Card::ACE));
        break;
      } // switch (this->game().type())

      { // mark the played cards
        for (unsigned t = 0; t < this->game().trick_current_no(); ++t) {
          auto const& trick = this->game().trick(t);
          auto trick2 = Trick(trick.startplayer());
          for (unsigned c = 0; c < trick.actcardno(); ++c) {
            auto const& card = trick.card(c);
            trick2 += card;
            if (   (t >= trick_begin)
                || (c == trick.cardno_of_player(this->player()))
                || (std::find(important_cards.begin(), important_cards.end(), card)
                    != important_cards.end()) )
              this->card_played(card, trick2);
            else
              this->card_played(HandCard(trick.player_of_card(c).hand(),
                                         Card::UNKNOWN), 
                                trick2);
          } // for (c < trick.actcardno())
          this->color_runs_[trick.startcard().tcolor()] += 1;

          // check for colors the players do not have
          auto const tcolor = trick2.startcard().tcolor();
          if (tcolor != Card::UNKNOWNCARDCOLOR) {
            for (unsigned c = 1; c < trick2.actcardno(); ++c) {
              if (   (trick.card(c).tcolor() != tcolor)
                  && (trick.card(c).tcolor() != Card::UNKNOWNCARDCOLOR) )
                this->of_player(trick2.card(c).player()
                               ).add_cannot_have(tcolor);
            } // for (c < trick.actcardno())
          } // if (tcolor != Card::UNKNOWNCARDCOLOR)
        } // for (t < this->game().trick_current_no())
        if (   !this->game().trick_current().isfull()
            && !this->game().trick_current().isempty())
          this->color_runs_[this->game().trick_current().startcard().tcolor()]
            -= 1;
      } // mark the played cards
    } // remember own cards and important cards

    { // add announcements info
      for (auto& p : this->of_player_) {
        if (this->game().announcement_of_player(p.player()))
          this->announcement_made(this->game().announcement_of_player(p.player()), p.player());
      } // for (p \in this->of_player_)
    } // add announcements info
  } // if (::game_status >= GAMESTATUS::GAME_PLAY)

  { // add marriage info
    if (GAMETYPE::is_marriage(this->game().type())) {
      auto n = (this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                    - this->played(Card::CLUB_QUEEN));
      for (auto& p : this->of_player_) {
        if (p.player() == this->game().soloplayer()) {
          if (n >= p.played(Card::UNKNOWN))
            p.add_must_have(Card::CLUB_QUEEN,
                            n - p.played(Card::UNKNOWN));
        } else {
          p.add_cannot_have(Card::CLUB_QUEEN);
        }
      }
    } // if (is marriage)
  } // add marriage info
  { // add poverty info
    if (   (::game_status >= GAMESTATUS::GAME_START)
        && (this->game().type() == GAMETYPE::POVERTY) ) {
      auto& of_player = this->of_player(this->game().soloplayer());
      auto n = (this->game().poverty_trumpno_returned()
                    - of_player.played(Card::TRUMP));
      of_player.add_can_have(Card::TRUMP, n);
      if (of_player.played(Card::UNKNOWN) < n)
        of_player.add_must_have(Card::TRUMP,
                                n - of_player.played(Card::UNKNOWN));
    } // if (this->game().type() == GAMETYPE::POVERTY)
  } // add poverty info
  { // add swines info
    if (this->game().swines_owner()) {
      for (auto& p : this->of_player_) {
        if (p.player() != *this->game().swines_owner())
          p.add_cannot_have(this->game().swine());
      }

      if (this->game().hyperswines_owner()) {
        for (auto& p : this->of_player_) {
          if (p.player() != *this->game().hyperswines_owner())
            p.add_cannot_have(this->game().hyperswine());
        }
      } // if (this->game().hyperswines_owner())
    } // if (this->game().swines_owner())
  } // add swines info
  { // add genscher info
    if (this->game().type() == GAMETYPE::GENSCHER) {
      auto const genscher_card = Card(this->game().trumpcolor(), Card::KING);
      for (auto& p : this->of_player_) {
        if (p.player() != this->game().soloplayer())
          p.add_cannot_have(genscher_card);
      }
    }
  } // add genscher info

  if (this->player_)
    this->set_hand(this->player(), this->player().hand());

  this->in_recalcing = false;

  if (   this->player().value(Aiconfig::HANDS_KNOWN)
      && (::game_status >= GAMESTATUS::GAME_RESERVATION)
      && (::game_status != GAMESTATUS::GAME_REDISTRIBUTE) ) {
    for (auto p = this->game().players_begin();
         p != this->game().players_end();
         ++p) {
      this->set_hand(**p, (*p)->hand());
    }
  }

  // this must be called here because it is skipped being in recalcing
  this->update_remaining_cards();

  this->queue_update_all();

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::recalc()

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   the corresponding player
 **/
Ai const&
CardsInformation::player() const
{ return *this->player_; }

/**
 ** -> result
 **
 ** @param    -
 **
 ** @return   whether to auto update
 **/
bool
CardsInformation::auto_update() const
{ return this->auto_update_; }

/**
 ** sets the auto update
 **
 ** @param    value   new value
 **
 ** @return   -
 **/
void
CardsInformation::set_auto_update(bool const value)
{ this->auto_update_ = value; }

/**
 ** -> result
 **
 ** @param     player	the player whose information is asked for
 **
 ** @return    the information of 'player'
 **/
CardsInformation::OfPlayer const&
CardsInformation::of_player(Player const& player) const
{
  return this->of_player(this->game().no(player));
} // CardsInformation::OfPlayer CardsInformation::of_player(Player player) const

/**
 ** -> result
 **
 ** @param     playerno	the player whose information is asked for
 **
 ** @return    the information of player 'playerno'
 **/
CardsInformation::OfPlayer const&
CardsInformation::of_player(unsigned const playerno) const
{
  return this->of_player_[playerno];
} // CardsInformation::OfPlayer CardsInformation::of_player(unsigned playerno) const

/**
 ** -> result
 **
 ** @param     player	the player whose information is asked for
 **
 ** @return    the information of 'player'
 **/
CardsInformation::OfPlayer&
CardsInformation::of_player(Player const& player)
{
  return this->of_player(this->game().no(player));
} // CardsInformation::OfPlayer& CardsInformation::of_player(Player player)

/**
 ** -> result
 **
 ** @param     playerno	the player whose information is asked for
 **
 ** @return    the information of player 'playerno'
 **/
CardsInformation::OfPlayer&
CardsInformation::of_player(unsigned const playerno)
{
  return this->of_player_[playerno];
} // CardsInformation::OfPlayer& CardsInformation::of_player(unsigned playerno)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many of the players still have unknown cards
 **/
unsigned
CardsInformation::remaining_unknown_players() const
{
  unsigned n = 0;
  for (auto const& p : this->of_player_)
    if (!p.all_known())
      n += 1;
  return n;
} // unsigned CardsInformation::remaining_unknown_players() const

/**
 ** -> result
 **
 ** @param     tcolor   trump color
 **
 ** @return    the number of runs of tcolor (without the current trick)
 **/
unsigned
CardsInformation::color_runs(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::color_runs(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");
  return this->color_runs_[tcolor];
} // CardsInformation::OfPlayer CardsInformation::color_runs(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the number of runs of trump (without the current trick)
 **/
unsigned
CardsInformation::trump_runs() const
{
  unsigned runs = 0;
  for (unsigned t = 0; t < this->game().trick_current_no(); ++t)
    if (this->game().trick(t).startcard().istrump())
      runs += 1;

  return runs;
} // CardsInformation::OfPlayer CardsInformation::trump_runs() const

/**
 ** -> result
 **
 ** @param     player	the player whose hand is asked for
 **
 ** @return    hand with all cards the player can have
 **/
Hand const&
CardsInformation::possible_hand(Player const& player) const
{
  DEBUG_ASSERTION((&this->game() == &player.game()),
                  "CardsInformation::possible_hand(player):\n"
                  "  the game of the player is not the game of the player");

  return this->possible_hand(this->game().no(player));
} // Hand CardsInformation::possible_hand(Player player) const

/**
 ** -> result
 **
 ** @param     playerno   the number of the player
 **
 ** @return    hand with all cards the player can have
 **/
Hand const&
CardsInformation::possible_hand(unsigned const playerno) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  if (this->hands_outdated)
    this->recalc_hands();
  return this->possible_hands_[playerno];
} // Hand CardsInformation::possible_hand(unsigned playerno) const

/**
 ** -> result
 **
 ** @param     player	the player whose hand is asked for
 **
 ** @return    the estimated hand for the player
 **/
Hand const&
CardsInformation::estimated_hand(Player const& player) const
{
  DEBUG_ASSERTION((&this->game() == &player.game()),
                  "CardsInformation::estimated_hand(player):\n"
                  "  the game of the player is not the game of the player");

  return this->estimated_hand(this->game().no(player));
} // Hand CardsInformation::estimated_hand(Player player) const

/**
 ** -> result
 **
 ** @param     playerno   the number of the player
 **
 ** @return    hand with all cards the player can have
 **/
Hand const&
CardsInformation::estimated_hand(unsigned const playerno) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  if (this->hands_outdated)
    this->recalc_hands();
  return this->estimated_hands_[playerno];
} // Hand CardsInformation::estimated_hand(unsigned playerno) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the highest remaining trump in the game
 **/
Card
CardsInformation::highest_remaining_trump() const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  HandCard card;
  for (auto p = this->game().players_begin();
       p != this->game().players_end();
       ++p) {
    // note: this->possible_hand() is a temporary hand,
    // so 'c' needs another hand
    auto const c
      = HandCard((*p)->hand(),
                 this->possible_hand(**p).highest_card(Card::TRUMP));
    if (   !c.is_empty()
        && (   card.is_empty()
            || card.less(c)
           )
       )
      card = c;
  } // for (p)

  return card;
} // Card CardsInformation::highest_remaining_trump() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the highest remaining trump of the other players in the game
 **/
Card
CardsInformation::highest_remaining_trump_of_others() const
{
  return this->highest_remaining_card_of_others(Card::TRUMP);
} // Card CardsInformation::highest_remaining_trump_of_others() const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor
 **
 ** @return    the highest remaining card of tcolor of the other players in the game
 **/
Card
CardsInformation::highest_remaining_card_of_others(Card::TColor const&
                                                   tcolor) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  Card card;
  for (auto p = this->game().players_begin();
       p != this->game().players_end();
       ++p) {
    if (*p == &this->player())
      continue;
    // note: this->possible_hand() is a temporary hand,
    // so 'c' needs another hand
    auto const c
      = HandCard((*p)->hand(),
                 this->possible_hand(**p).highest_card(tcolor));
    if (   !c.is_empty()
        && (   card.is_empty()
            || card.less(c)
           )
       )
      card = c;
  } // for (p)

  return card;
} // Card CardsInformation::highest_remaining_card_of_others(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     card   card to compare with
 **
 ** @return    whether there exists a higher card than 'card' on the other hands
 **/
bool
CardsInformation::higher_card_exists(HandCard const& card) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  for (auto p = this->game().players_begin();
       p != this->game().players_end();
       ++p) {
    if (*p == &card.player())
      continue;
    if (this->possible_hand(**p).higher_card_exists(card))
      return true;
  } // for (p)

  return false;
} // bool CardsInformation::higher_card_exists(HandCard card) const

/**
 ** -> result
 **
 ** @param     card   card to compare with
 **
 ** @return    number of higher cards then 'card'
 **/
unsigned
CardsInformation::higher_cards_no_of_others(HandCard const& card) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  unsigned n = 0;
  for (auto const& c : this->game().rule().cards()) {
    if (card.less(c))
      n += this->remaining_others(c);
  }

  return n;
} // unsigned CardsInformation::higher_cards_no_of_others(HandCard card) const


/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding game
 **/
Game const&
CardsInformation::game() const
{
  return this->player().game();
}

/**
 ** sets (changes) the player
 **
 ** @param     player	new player
 **
 ** @return    -
 **/
void
CardsInformation::set_player(Ai const& player)
{
  this->player_ = &player;
} // void CardsInformation::set_player(Ai player)

/**
 ** sets the hand of the player
 **
 ** @param     player	player whose hand is set
 ** @param     hand	the hand of the player
 **
 ** @return    -
 **/
void
CardsInformation::set_hand(Player const& player, Hand const& hand)
{
  if (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
    return ;

#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  this->of_player(player).add_can_only_have(hand.cards());

  if (   !this->in_recalcing
      && this->auto_update())
    this->do_update();
} // void CardsInformation::set_hand(Player player, Hand hand)

/**
 ** queue to update the information of the cards
 **
 ** @param     cards   cards to update the information of
 **
 ** @return    -
 **/
void
CardsInformation::queue_update(vector<Card> const& cards)
{
  for (auto const& c : cards)
    this->queue_update(c);
} // void CardsInformation::queue_update(vector<Card> cards)

/**
 ** queue to update the information of the card
 **
 ** @param     card	card to update the information of
 **
 ** @return    -
 **/
void
CardsInformation::queue_update(Card const& card)
{
  this->cards_to_update.insert(card);
} // void CardsInformation::queue_update(Card card)

/**
 ** queue to update the information of the cards of tcolor
 **
 ** @param     tcolor   tcolor of the cards to update the information of
 **
 ** @return    -
 **/
void
CardsInformation::queue_update(Card::TColor const& tcolor)
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::queue_update(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");
  this->queue_update(this->game().cards(tcolor));
} // void CardsInformation::queue_update(Card::TColor tcolor)

/**
 ** do the update of all cards in 'cards_to_update'
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::do_update() const
{
  if (this->in_update)
    return ;
  if (this->cards_to_update.empty())
    return ;

  this->in_update = true;

  while (!this->cards_to_update.empty()) {
    while (!this->cards_to_update.empty()) {
      auto const card = *this->cards_to_update.begin();
      this->cards_to_update.erase(this->cards_to_update.begin());
      if (card.is_unknown())
        continue;
      for (auto& p : this->of_player_)
        p.update_information(card);
    } // while (!this->cards_to_update().empty())

    for (auto& p : this->of_player_) {
      p.check_can_is_must();
      p.check_must_is_can();
      p.update_tcolor_information();
    } // for (p : this->of_player_)
  } // while (!this->cards_to_update().empty())

#ifdef DKNOF
  const_cast<CardsInformation*>(this)->check_joined_hands();
#endif
  if (!this->cards_to_update.empty())
    this->do_update();

#ifdef SELF_CHECK
  this->self_check();
#endif
  this->in_update = false;

#ifndef UPDATE_ON_DEMAND
  // ToDo: recalc the weighting for all cards
  this->recalc_hands();
#else
  this->hands_outdated = true;
#endif
} // void CardsInformation::do_update() const

/**
 ** updates the information of all cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::queue_update_all()
{
  for (auto const& c : this->game().rule().cards())
    this->queue_update(c);
} // void CardsInformation::queue_update_all()

/**
 ** check joined hands
 ** Check whether two players can have so many cards between them as they still have to play. Then the other players cannot have the cards.
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::check_joined_hands()
{
  if (this->remaining_unknown_players() <= 2)
    return;

  for (auto p1 = this->of_player_.begin();
       p1 != this->of_player_.end();
       ++p1) {
    if (p1->all_known())
      continue;
    for (auto p2 = p1 + 1;
         p2 != this->of_player_.end();
         ++p2) {
      if (p2->all_known())
        continue;
      if (   (p1->can_have_.cards_no()
              > this->cardno_to_play(p1->player())
              + this->cardno_to_play(p2->player()))
          || (p2->can_have_.cards_no()
              > this->cardno_to_play(p1->player())
              + this->cardno_to_play(p2->player())))
        continue;

      // check the joined cards
      auto const can_have = this->joined_can_have(*p1, *p2);
      if (can_have.cards_no()
          < this->cardno_to_play(p1->player())
          + this->cardno_to_play(p2->player())) {
        if (this->game().isvirtual())
          DEBUG_THROW(InvalidGameException, InvalidGameException());
        else
          DEBUG_ASSERTION(false, "throwing in real game");
      }

      if (can_have.cards_no() 
          == this->cardno_to_play(p1->player())
          + this->cardno_to_play(p2->player())) {
        for (auto p = this->of_player_.begin();
             p != this->of_player_.end();
             ++p) {
          if ((p == p1) || (p == p2))
            continue;
          if (p->all_known())
            continue;
          // all cards of can_have must be shared between p1 and p2
          for (auto const& c : can_have)
            p->add_can_have(c.first,
                            this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                            - this->played(c.first) - c.second);
        } // for (p)
        if (   !this->in_recalcing
            && this->auto_update())
          this->do_update();

      } // if (cards shared between p1 and p2)
    } // for (p2)
  } // for (p1)
} // void CardsInformation::check_joined_hands()

/**
 ** updates 'can have' according to 'remaining cards'
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::update_remaining_cards()
{
  for (auto& p : this->of_player_)
    p.update_remaining_cards();
} // void CardsInformation::update_remaining_cards()

/**
 ** recalcs the (possible and estimated) hands
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::recalc_hands() const
{
  this->recalc_possible_hands();
  this->recalc_estimated_hands();
  this->hands_outdated = false;
} // void CardsInformation::recalc_hands()

/**
 ** updates the possible hands
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::recalc_possible_hands() const
{
  for (auto const& p : this->of_player_)
    this->possible_hands_[p.playerno()] = p.possible_hand();
} // void CardsInformation::recalc_possible_hands()

/**
 ** updates the estimated hands
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::recalc_estimated_hands() const
{
#ifdef CHECK_RUNTIME
  auto const ssp = (   (this == &this->player().cards_information())
                    ? ::runtime["ai recalc estimated hands"].start_stop_proxy()
                    : nullptr);
#endif
  // overview
  // 1) estimated for each player the hand, according to the weighting
  // 2) make sure, that all cards are distributed
  // 3a) check that all pairs of players have enough cards between them
  // 3b) check that no player has more cards the other players do not have,
  //     than he can have
  // 4) check for full estimated hands
  // 5) check, that the estimated hands do not differ if the hands are set
  //    (this uses recursion till depth 1)
  // If no valid estimation is found, the possible hands are used.

  try {
    if (this->player().value(Aiconfig::HANDS_KNOWN)) {
      for (auto const& p : this->of_player_)
        this->estimated_hands_[p.playerno()] = p.player().hand();
      return ;
    } // if (this->player().value(Aiconfig::HANDS_KNOWN))

    if (!this->player().value(Aiconfig::ESTIMATE_HANDS)) {
      this->estimated_hands_ = this->possible_hands_;
      return ;
    } // if (!this->player().value(Aiconfig::ESTIMATE_HANDS))

    { // 1) estimated for each player the hand, according to the weighting
      for (auto const& p : this->of_player_) 
        this->estimated_hands_[p.playerno()] = p.estimated_hand();
    } // 1) estimated for each player the hand, according to the weighting

    { // 2) make sure, that all cards are distributed
      for (auto card = this->game().rule().cards().begin();
           card != this->game().rule().cards().end();
           ++card) {
        auto sum = this->played(*card);
        for (auto p = this->game().players_begin();
             p != this->game().players_end();
             ++p)
          sum += this->estimated_hands_[this->game().no(**p)].numberof(*card);

        for ( ;sum < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS); ++sum) {
          // maximal weighting
          int max_weighting = INT_MIN;
          // player with the maximum weighting
          Player* max_player = nullptr;
          for (auto p = this->game().players_begin();
               p != this->game().players_end();
               ++p) {
            if (this->estimated_hands_[this->game().no(**p)].numberof(*card)
                < this->of_player(**p).can_have(*card)) {
              if (   (this->of_player(**p).weighting(*card) > max_weighting)
                  && (this->cardno_to_play(**p)
                      > this->estimated_hands_[this->game().no(**p)].numberof(*card)) ) {
                max_weighting = this->of_player(**p).weighting(*card);
                max_player = *p;
              }
            } // if (player can have another card)
          } // for (p \in game.players)

          // This can happen when tricks are forgotten.
          // P.e. if only one player still has 'card' and still has one card to play
          // he cannot have both forgotten club aces.
          if (!max_player)
            break;

          this->estimated_hands_[this->game().no(*max_player)].add(*card);
          // ToDo: check, how many times this loop is used more than ones
        } // for (sum < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))

      } // for (card \in rule.cards())
    } // 2) make sure, that all cards are distributed

    { // 3a) check that no player has more cards the other players do not have, than he can have
      for (auto player = this->game().players_begin();
           player != this->game().players_end();
           ++player) {
        if (this->of_player(**player).all_known())
          continue;
        unsigned sum = 0;
        for (auto const& card : this->game().rule().cards()) {
          int s = this->remaining(card);
          for (auto p = this->game().players_begin();
               p != this->game().players_end();
               ++p)
            if (**p != **player)
              s -= this->estimated_hands_[this->game().no(**p)].numberof(card);
          if (s > 0)
            sum += s;
        } // for (card)

        // ToDo: add cards to the other players
        if (sum > this->cardno_to_play(**player)) {
#ifdef WORKAROUND
          DEBUG_THROW(InvalidGameException, InvalidGameException());
#else
          // add cards to the other players
#endif
        }
      } // for (player)
    } // 3a) check that no player has more cards the other players do not have, than he can have
    { // 3b) check that all pairs of players have enough cards between them
      for (auto p1 = this->game().players_begin();
           p1 != this->game().players_end();
           ++p1) {
        if (this->of_player(**p1).all_known())
          continue;
        for (vector<Player*>::const_iterator p2 = p1 + 1;
             p2 != this->game().players_end();
             ++p2) {
          if (this->of_player(**p2).all_known())
            continue;
          unsigned sum = 0;
          for (auto const& card : this->game().rule().cards()) {
            sum += min(this->estimated_hands_[this->game().no(**p1)].numberof(card)
                       + this->estimated_hands_[this->game().no(**p2)].numberof(card),
                       this->remaining(card));
          } // for (card)
          if (sum < this->cardno_to_play(**p1) + this->cardno_to_play(**p2)) {
            // ToDo: add cards until it matches
            // for that, we also need the WeightedCardList for the estimated hand of the OfPlayer
#ifdef WORKAROUND
            DEBUG_THROW(InvalidGameException, InvalidGameException());
#endif
          }
        } // for (p2)
      } // for (p1)

    } // 3b) check that all pairs of players have enough cards between them

    { // 4) check for full estimated hands
      // if all cards of a player are estimated,
      // the other players cannot have the cards

      // the number of known distributed cards
      map<Card, unsigned> known_cards; // Replace with a vector
      for (auto p = this->game().players_begin();
           p != this->game().players_end();
           ++p) {
        auto const& hand = this->estimated_hands_[this->game().no(**p)];
        if (hand.cardsnumber() == this->cardno_to_play(**p)) {
          for (auto const& c : hand.cards())
            known_cards[c] += 1;
        } // if (full hand estimated
      } // for (p \in this->game().players)

      // whether the estimated hands will change
      bool changed = false;
      // cards information which takes the full estimated hands into account
      CardsInformation cards_information(*this);
      cards_information.set_auto_update(false);

      // check, that a card is not in two full estimated hands
      for (auto const& c : known_cards) {
        // ToDo: move in upper loop
        auto const& card = c.first;
        // number of known distributed cards of 'c'
        auto const n = (this->played(card)
                            + c.second);
        if (n > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
          changed = true;
          // decrease the weighting for the card
          for (auto p = this->game().players_begin();
               p != this->game().players_end();
               ++p)
            cards_information.of_player(**p).cards_weighting_[card] -= 10; // *Value*
        } // if (n > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
      } // for (c : known_cards)

      if (!changed) {
        // remove the known cards in the new cards information
        for (auto p = this->game().players_begin();
             p != this->game().players_end();
             ++p) {
          auto const& hand = this->estimated_hands_[this->game().no(**p)];
          if (hand.cardsnumber() != this->cardno_to_play(**p)) {
            // ToDo: replace by 'set_hand()' ?
            for (auto const& c : known_cards) {
              Card const& card = c.first;
              // number of known distributed cards of 'c'
              unsigned const n = (this->played(card)
                                  + c.second);
              cards_information.of_player(**p).add_can_have(card, this->game().rule()(Rule::NUMBER_OF_SAME_CARDS) - n + this->of_player(**p).must_have(card));
              if (hand.numberof(card) + n > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
                changed = true;
            } // for (c \in known_cards)
          } // if (full hand not known)
        } // if (!changed)
      } // for (p \in this->game().players)

      if (changed) {
        // recursion

        // only take one recursion depth
        if (this != &this->player().cards_information()) {
          DEBUG_THROW(InvalidGameException, InvalidGameException());
        }

        // use the estimated hands from the modified cards information
        cards_information.do_update();
        for (auto p = this->game().players_begin();
             p != this->game().players_end();
             ++p) {
          this->estimated_hands_[this->game().no(**p)]
            = cards_information.estimated_hand(**p);
        } // for (p \in this->game().players)
        return ;
      } // if (changed)

    } // 4) check for full estimated hands

    { // 5) check, that the estimated hands do not differ if the hands are set
      CardsInformation cards_information(*this);
      cards_information.set_auto_update(false);
      for (auto p = this->game().players_begin();
           p != this->game().players_end();
           ++p) {
        cards_information.set_hand(**p, this->estimated_hands_[this->game().no(**p)]);
      }
      // estimate the hand (only in first recursion depth)
      /// @todo   if the do_update throws an exception, normalize the weightings
      if (this == &this->player().cards_information())
        cards_information.do_update();

      // whether the estimated hands differ
      bool differ = false;
      for (auto p = this->game().players_begin();
           p != this->game().players_end();
           ++p) {
        if (cards_information.of_player(**p).estimated_hand()
            != this->estimated_hands_[this->game().no(**p)]) {
          differ = true;
          break;
        }
      }

      if (differ) {
        // recursion

        // only take one recursion depth
        if (this != &this->player().cards_information())
          DEBUG_THROW(InvalidGameException, InvalidGameException());

        // use the estimated hands from the modified cards information
        cards_information.do_update();
        for (auto p = this->game().players_begin();
             p != this->game().players_end();
             ++p) {
          this->estimated_hands_[this->game().no(**p)]
            = cards_information.estimated_hand(**p);
        } // for (p \in this->game().players)
      } // if (differ)

    } // 5) check, that the estimated hands do not differ if the hands are set

  } catch (InvalidGameException const& exception) {
    // could not create estimated hands -- do no estimation
    this->estimated_hands_ = this->possible_hands_;
  } // try
} // void CardsInformation::recalc_estimated_hands()

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
CardsInformation::recalc_weightings() const
{
#ifdef CHECK_RUNTIME
  auto const ssp = (  (this == &this->player().cards_information())
                    ? ::runtime["ai recalc weightings"].start_stop_proxy()
                    : nullptr);
#endif
  // overview
  // * Rebuild a new cards information for the start of the game with 'must have' and 'cannot have' from the gameplay.
  // * Replay the game and update the weighting (using the information of further played cards)

  return ;

  // create a virtual game

  // virtual players
  vector<Player*> player_virt;
  {
    // Create new players.
    // The hand is set during the recursion.
    for (auto player = this->game().players_begin();
         player != this->game().players_end();
         player++) {
      if (**player == this->player())
        player_virt.push_back(this->player().Ai::clone());
      else
        player_virt.push_back(this->player().Player::clone());
      player_virt.back()->set_name((*player)->name());
      player_virt.back()->set_team(this->player().team_information().guessed_team(player_virt.size() - 1));
    } // for (player)

  } // create a virtual game
  Game virt_game(this->game(), player_virt);
  virt_game.reset_to_first_trick();

  // build a new cards information for the start of the game
  CardsInformation cards_information(*player_virt[this->game().no(this->player())]);

  // replay the game (with announcements!)
  // for each game action do game action

  // copy the cards information
} // void CardsInformation::recalc_weightings() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the number of forgotten tricks
 **/
unsigned
CardsInformation::forgotten_tricks_no() const
{
  if (this->game().trick_current_no() == UINT_MAX)
    return 0;
  return max(this->played(Card::UNKNOWN) / this->game().playerno(),
             ( (this->player().value(Aiconfig::REMEMBERTRICKS)
                < static_cast<int>(::party.game().real_trick_current_no()))
              ? (::party.game().real_trick_current_no()
                 - this->player().value(Aiconfig::REMEMBERTRICKS))
              : 0
             ));
} // unsigned CardsInformation::forgotten_tricks_no() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the number of forgotten (unknown) cards
 **/
unsigned
CardsInformation::forgotten_cards_no() const
{
  return this->played(Card::UNKNOWN);
} // unsigned CardsInformation::forgotten_cards_no() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many cards have been played in total in the game
 **/
unsigned
CardsInformation::played_cards_no() const
{
  return this->played_.cards_no();
} // unsigned CardsInformation::played_cards_no() const

/**
 ** -> result
 **
 ** @param     card	card to look at
 **
 ** @return    how many of 'card' have been played
 **/
unsigned
CardsInformation::played(Card const& card) const
{
  return (this->played_)[card];
} // unsigned CardsInformation::played(Card card) const

/**
 ** -> result
 **
 ** @param     color   color of the card
 ** @param     value   color of the card
 **
 ** @return    how many of 'Card(color, value)' have been played
 **/
unsigned
CardsInformation::played(Card::Color const& color,
                         Card::Value const& value) const
{
  return this->played(Card(color, value));
} // unsigned CardsInformation::played(Card::Color color, Card::Value value) const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor to look at
 **
 ** @return    how many cards of 'tcolor' have been played
 **/
unsigned
CardsInformation::played(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::played(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");
  if (tcolor == Card::TRUMP) {
    unsigned n = 0;
    for (auto const& c : this->game().rule().cards())
      if (c.istrump(this->game()))
        n += this->played(c);
    return n;
  } else { // if !(tcolor == Card::TRUMP)
    unsigned n = 0;
    for (auto const& v : this->game().rule().card_values())
      if (!Card(tcolor, v).istrump(this->game()))
        n += this->played(Card(tcolor, v));
    return n;
  } // if !(tcolor == Card::TRUMP)
} // unsigned CardsInformation::played(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     player   player
 **
 ** @return    how many cards the player still has to play
 **/
unsigned
CardsInformation::cardno_to_play(Player const& player) const
{
  if (this->game().tricks().empty())
    return this->game().trickno();

  return (this->game().trickno() - this->game().real_trick_current_no()
          - (this->game().trick_current().has_played(player) ? 1 : 0));
} // unsigned CardsInformation::cardno_to_play(Player player) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many cards are still to be played
 **/
unsigned
CardsInformation::remaining_cards_no() const
{
  return (this->game().trickno() * this->game().playerno()
          - this->played_cards_no());
} // unsigned CardsInformation::remaining_cards_no() const

/**
 ** -> result
 **
 ** @param     card   card to check
 **
 ** @return    how many of 'card' are still to be played
 **/
unsigned
CardsInformation::remaining(Card const& card) const
{
  return (this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
          - this->played(card));
} // unsigned CardsInformation::remaining(Card card) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many trumps are still in the game
 **/
unsigned
CardsInformation::remaining_trumps() const
{
  return this->remaining(Card::TRUMP);
} // unsigned CardsInformation::remaining_trumps() const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor to check
 **
 ** @return    how many cards of 'tcolor' are still to be played
 **/
unsigned
CardsInformation::remaining(Card::TColor const& tcolor) const
{
  return ( (this->game().different_cards_no(tcolor)
            * this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
          - this->played(tcolor));
} // unsigned CardsInformation::remaining(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many cards the other players do have
 **/
unsigned
CardsInformation::remaining_others_cards_no() const
{
  if (this->cardno_to_play(this->player())
      == this->player().hand().cardsnumber())
    return (this->remaining_cards_no()
            - this->player().hand().cardsnumber());
  else
    return this->remaining_cards_no();
} // unsigned CardsInformation::remaining_others_cards_no() const

/**
 ** -> result
 **
 ** @param     card   the card
 **
 ** @return    how many cards 'card' are played or known
 **/
unsigned
CardsInformation::known(Card const& card) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  unsigned result = this->played(card);
  for (auto const& p : this->of_player_)
    result += p.must_have(card);

  return result;
} // unsigned CardsInformation::known(Card card) const

/**
 ** -> result
 **
 ** @param     card   the card
 **
 ** @return    how many cards 'card' are remaining and unknown
 **/
unsigned
CardsInformation::remaining_unknown(Card const& card) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  return (this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
          - this->known(card));
} // unsigned CardsInformation::remaining_unknown(Card card) const

/**
 ** -> result
 **
 ** @param     card   the card
 **
 ** @return    the joined "can have" information of both players
 **/
CardCounter
CardsInformation::joined_can_have(OfPlayer const& p1, OfPlayer const& p2) const
{
  auto can_have = p1.can_have_;
  for (auto const& c : p2.can_have_) {
    auto const& card = c.first;
    can_have.add(card, c.second);
    can_have.max_set(card,
                     this->game().rule()(Rule::NUMBER_OF_SAME_CARDS) 
                     - this->played(card));
    // ToDo: check must have of other players
  } // for (c)

  return can_have;
} // CardCounter CardsInformation::joined_can_have(OfPlayer p1, OfPlayer p2) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many trumps the other players do still have
 **/
unsigned
CardsInformation::remaining_trumps_others() const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  return remaining_others(Card::TRUMP);
} // unsigned CardsInformation::remaining_trumps_others() const

/**
 ** -> result
 **
 ** @param     card   card to check
 **
 ** @return    how many of 'card' the other players do have
 **/
unsigned
CardsInformation::remaining_others(Card const& card) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  if (this->cardno_to_play(this->player())
      == this->player().hand().cardsnumber())
    return (this->remaining(card)
            - this->player().hand().numberof(card));
  else
    return this->remaining(card);
} // unsigned CardsInformation::remaining_others(Card card) const

/**
 ** -> result
 **
 ** @param     color   color of the card to check
 ** @param     value   value of the card to check
 **
 ** @return    how many of Card(color, value) the other players do have
 **/
unsigned
CardsInformation::remaining_others(Card::Color const& color,
                                   Card::Value const& value) const
{
  return this->remaining_others(Card(color, value));
} // unsigned CardsInformation::remaining_others(Card::Color color, Card::Value value) const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor to check
 **
 ** @return    how many cards of 'tcolor' the other players do have
 **/
unsigned
CardsInformation::remaining_others(Card::TColor const& tcolor) const
{
#ifdef UPDATE_ON_DEMAND
  this->do_update();
#endif

  if (this->cardno_to_play(this->player())
      == this->player().hand().cardsnumber())
    return (this->remaining(tcolor)
            - this->player().hand().numberof(tcolor));
  else
    return this->remaining(tcolor);
} // unsigned CardsInformation::remaining_others(Card::TColor tcolor) const

/**
 ** the game starts
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardsInformation::game_start()
{
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  // reset (needed in poverty: swines owner can change)
  this->reset();

  if (this->player_)
    this->set_hand(this->player(), this->player().hand());

  if (this->game().type() == GAMETYPE::POVERTY) {
    if (this->player() == this->game().soloplayer()) {
      // the soloplayer knows his shifted cards
      auto known_cards = this->game().poverty_cards();
      for (auto const& c : this->game().poverty_cards_returned()) {
        if (std::find(known_cards.begin(), known_cards.end(), c)
            != known_cards.end())
          known_cards.erase(std::find(known_cards.begin(), known_cards.end(),
                                      c));
      }
      this->of_player(this->game().poverty_partner()).add_must_have(known_cards);
    } else if (this->player() == this->game().poverty_partner()) {
      // the poverty partner knows the returned cards
      auto& of_player = this->of_player(this->game().soloplayer());
      of_player.add_must_have(this->game().poverty_cards_returned());
      // the poverty player cannot have any other card
      for (auto const& c : this->game().trumps())
        of_player.add_can_have(c, of_player.must_have(c));

    } else {
      // the other players know the returned trumps
      this->of_player(this->game().soloplayer()).add_must_exactly_have(Card::TRUMP, this->game().poverty_trumpno_returned());
    }
  } // if (this->game().type() == GAMETYPE::POVERTY)

  // Take care of swines when the game is started,
  // else I get problems with shifted swines.
  if (this->game().swines_announced())
    this->swines_announced(*this->game().swines_owner());
  if (this->game().hyperswines_announced())
    this->hyperswines_announced(*this->game().hyperswines_owner());

  for (auto& p : this->of_player_)
    p.game_start();

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::game_start()

/**
 ** the trick is opened
 **
 ** @param      trick	opened trick
 **
 ** @return     -
 **/
void
CardsInformation::trick_open(Trick const& trick)
{ 
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  // In a virtual game do *not* recalc!
  // The problem is, that additional information get lost
  // (that is the hand of the player that has started the virtual game),
  // so there is a discrepance between the hand of the player and the cards
  // he can have.
  if ( (static_cast<int>(this->game().trick_current_no())
        > this->player().value(Aiconfig::REMEMBERTRICKS))
      && !this->game().isvirtual() )
    this->recalc();
} // void CardsInformation::trick_open(Trick const& trick)

/**
 ** the trick is full
 ** update the color runs
 **
 ** @param      trick	full trick
 **
 ** @return     -
 **/
void
CardsInformation::trick_full(Trick const& trick)
{
  this->color_runs_[trick.startcard().tcolor()] += 1;
} // void CardsInformation::trick_full(Trick trick)

/**
 ** 'card' has been played - update the information
 **
 ** @param     card	played card
 **
 ** @return    -
 **/
void
CardsInformation::card_played(HandCard const& card)
{
  this->card_played(card, card.game().trick_current());
} //  void CardsInformation::card_played(HandCard card)

/**
 ** 'card' has been played - update the information
 **
 ** @param     card	played card
 ** @param     trick   current trick
 **
 ** @return    -
 **/
void
CardsInformation::card_played(HandCard const& card, Trick const& trick)
{
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  this->played_.inc(card);
  this->tcolor_played_.inc(card.tcolor());
  DEBUG_ASSERTION(card.is_unknown()
                  || (this->played(card)
                      <= this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)),
                  "CardsInformation::card_played(" << card << ")\n"
                  "  card is played more often than it is in the game:\n"
                  << "  " << this->played(card) << " > "
                  << this->game().rule()(Rule::NUMBER_OF_SAME_CARDS));

  this->of_player(card.player()).card_played(card, trick);

  if (card.is_unknown()) {
    return ;
  }

  if (this->in_recalcing) {
    return ;
  }

  if (card.tcolor() != this->game().trick_current().startcard().tcolor()) {
    this->of_player(card.player()).add_cannot_have(this->game().trick_current().startcard().tcolor());
  }

  this->queue_update(card);
  this->queue_update(card.tcolor());

  if (this->auto_update())
    this->do_update();

  this->of_player(card.player()).weight_played_card(card, trick);
} // void CardsInformation::card_played(HandCard card, Trick trick)

/**
 ** an announcement has been made - update the information
 **
 ** @param     announcement	made announcement
 ** @param     player		player who has made the announcement
 **
 ** @return    -
 **/
void
CardsInformation::announcement_made(Announcement const& announcement,
                                    Player const& player)
{
  // in a virtual game the distribution of the remaining club queens does not
  // depend on the team, so we can get no more information
  if (this->game().isvirtual())
    return ;

  if (this->game().type() != GAMETYPE::NORMAL)
    return ;

  if (this->game().rule()(Rule::KNOCKING))
    return ;

#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  if (this->game().teaminfo(player) == TEAM::RE) {
    if ( (this->of_player(player).played(Card::CLUB_QUEEN) == 0)
        && (this->forgotten_tricks_no() == 0) ) {
      this->of_player(player).add_must_have(Card::CLUB_QUEEN);
    }
  } else if (this->game().teaminfo(player) == TEAM::CONTRA) {
    this->of_player(player).add_cannot_have(Card::CLUB_QUEEN);
  } else {
    DEBUG_ASSERTION(false,
                    "CardsInformation::announcement_made("
                    << announcement << ", " << this->game().no(player) << "):\n"
                    "  team of the player not known in the game: "
                    << this->game().teaminfo(player));
  }

  this->queue_update(Card::CLUB_QUEEN);

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::announcement_made(Announcement announcement, Player player)

/**
 ** a player has announced swines - update the information
 **
 ** @param     player	player who has swines
 **
 ** @return    -
 **/
void
CardsInformation::swines_announced(Player const& player)
{
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  if (::game_status <= GAMESTATUS::GAME_START) {
    // this is needed in the case that the swines are shifted or when 'game_start' is not called in a color solo
    this->recalc();
  } // if (::game_status <= GAMESTATUS::GAME_START)

  Card const swine(this->game().swine());

  if (this->game().rule()(Rule::SWINE_ONLY_SECOND))
    this->of_player(player).add_must_have(swine, 1);
  else
    this->of_player(player).add_must_have(swine, 2);

  for (auto& p : this->of_player_)
    if (p.player() != player)
      p.add_cannot_have(swine);

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::swines_announced(Player player)

/**
 ** a player has announced hyperswines - update the information
 **
 ** @param     player	player who has hyperswines
 **
 ** @return    -
 **/
void
CardsInformation::hyperswines_announced(Player const& player)
{
  // Wait till the game is started, else I get problems with shifted swines.
  if (::game_status <= GAMESTATUS::GAME_RESERVATION)
    return ;

#ifdef CHECK_RUNTIME
  auto x = ::runtime["ai cards information"].start_stop_proxy();
#endif

  if (::game_status <= GAMESTATUS::GAME_START) {
    // this is needed in the case that the swines are shifted or when 'game_start' is not called in a color solo
    this->recalc();
  } // if (::game_status <= GAMESTATUS::GAME_START)

  auto const hyperswine = this->game().hyperswine();
  this->of_player(player).add_must_have(hyperswine, 2);
  for (auto& p : this->of_player_)
    if (p.player() != player)
      p.add_cannot_have(hyperswine);

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::hyperswines_announced(Player player)

/**
 ** player 'genscher' has a genscher
 **
 ** @param     genscher	player with the genscher
 ** @param     partner		selected partner
 **
 ** @return    -
 **/
void
CardsInformation::genscher(Player const& genscher, Player const& partner)
{
#ifdef CHECK_RUNTIME
  auto const ssp = ::runtime["ai cards information"].start_stop_proxy();
#endif

  auto const genscher_card = Card(this->game().trumpcolor(), Card::KING);
  this->of_player(genscher).add_must_have(genscher_card, 2);
  for (auto& p : this->of_player_)
    if (p.player() != genscher)
      p.add_cannot_have(genscher_card);

  if (this->auto_update())
    this->do_update();
} // void CardsInformation::genscher(Player genscher, Player partner)

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param     -
 **
 ** @return    whether the self-check was successful (no error)
 **/
bool
CardsInformation::self_check() const
{
  if (this->in_recalcing)
    return true;
  if (!this->cards_to_update.empty())
    return false;

#ifdef WORKAROUND
  // We get problems with 'forgotten cards no'
  // because the last trick is closed but no further cards are forgotten.
  if (this->game().finished())
    return true;
#endif

  { // the number of played cards must not be
    // greater than the number of played cards in the game and
    // less than the number of played cards in the game
    //      minus the number of forgotten cards
    if (this->played_.cards_no() != this->game().played_cards_no()) {
      DEBUG_ASSERTION(false,
                      "CardsInformation::self_check():\n"
                      "  played cards no "
                      << " = " << this->played_.cards_no()
                      << " != " << this->game().played_cards_no()
                      << " = game.played_cards_no()\n"
                      << "  trick = " << this->game().real_trick_current_no());
      return false;
    }
  } // the number of played cards
  { // the number of played tcolors must not be
    // greater than the number of played tcolors in the game and
    // less than the number of played tcolors in the game
    //      minus the number of forgotten tcolors
    if (this->tcolor_played_.cards_no() != this->game().played_cards_no()) {
      DEBUG_ASSERTION(false,
                      "CardsInformation::self_check():\n"
                      "  played tcolor no + forgotten tcolor no"
                      << " = " << this->tcolor_played_.cards_no()
                      << " != " << this->game().played_cards_no()
                      << " = game.played_cards_no()\n"
                      << "  trick = " << this->game().real_trick_current_no() << '\n'
                      << this->tcolor_played_
                     );
      return false;
    }
  } // the number of played tcolors

  // the number of played cards in the game
  for (auto const& p : this->played_) {
    { // every card is at max played as many times as it is in the game
      if (   !p.first.is_unknown()
          && (p.second > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) ) {
        DEBUG_ASSERTION(p.first.is_unknown(),
                        "CardsInformation::self_check():\n"
                        "  card '" << p.first << "' has been played "
                        << p.second << " times.\n"
                       );
        return false;
      }
    } // every card is at max played as many times as it is in the game
    { // the number of played in total is the sum of played over the players
      unsigned n = 0;
      for (auto f : this->of_player_)
        n += f.played(p.first);

      if (p.second != n) {
        DEBUG_ASSERTION(false,
                        "CardsInformation::self_check():\n"
                        "  card '" << p.first << "' has been played "
                        << p.second << " times, but the players say "
                        << n << " times.\n"
                       );
        return false;
      }
    } // the number of played in total is the sum of played over the players
  } // for (p : this->played_)

  { // the number of played cards of a tcolors does not exceed the number in the game
    for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
      DEBUG_ASSERTION(this->tcolor_played_[static_cast<Card::TColor>(c)]
                      <= this->game().cards_no(static_cast<Card::TColor>(c)),
                      "CardsInformation::self_check():\n"
                      "  tcolor '" << static_cast<Card::TColor>(c) << "' has been played"
                      << this->game().cards_no(static_cast<Card::TColor>(c)) << "times.\n"
                     );
  } // the number of played cards of a tcolors does not exceed the number in the game

  { // every card is either played or a player can have it or it is forgotten
    auto const& cards = this->game().rule().cards();

    for (auto const& c : cards) {
      auto n = this->played(c);
      for (auto const& f : this->of_player_)
        n += f.can_have(c);

      n += this->forgotten_cards_no();

#ifdef DEBUG_ASSERT
      if (n < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
        if (   (::game_status & GAMESTATUS::GAME)
            && (this->game().trick_current_no() != UINT_MAX)) {
          cerr << "current trick\n";
          cerr << this->game().trick_current() << endl;
          cerr << *this << endl;
        }
      }
#endif
      DEBUG_ASSERTION((n >= this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)),
                      "CardsInformation::self_check():\n"
                      "  card '" << c << "' is only distributed "
                      << n << " times, must be "
                      << this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                      << " times.\n"
                      << "  played: " << this->played(c)
                      << ", sum can_have = " << (n - this->played(c)));
    } // for (c \in cards)
  } // every card is either played or a player can have it

  // ToDo: check there are at most as many cards missing as are forgotten

  { // the information of the single players is valid
    for (auto const& f : this->of_player_)
      if (!f.self_check())
        return false;
  } // the information of the single players is valid

  return true;
} // bool CardsInformation::self_check() const

/**
 ** -> result
 **
 ** @param     cards_information_a	first object
 ** @param     cards_information_b	second object
 **
 ** @return    whether the two objects are equal (the player may differ)
 **/
bool
operator==(CardsInformation const& cards_information_a,
           CardsInformation const& cards_information_b)
{
  return ( (   &cards_information_a.player_
            == &cards_information_b.player_)
          && (   cards_information_a.of_player_
              == cards_information_b.of_player_) );
} // bool operator==(CardsInformation cards_information_a, CardsInformation cards_information_b)

/**
 ** -> result
 **
 ** @param     cards_information_a	first object
 ** @param     cards_information_b	second object
 **
 ** @return    whether the two objects are different
 **/
bool
operator!=(CardsInformation const& cards_information_a,
           CardsInformation const& cards_information_b)
{
  return !(cards_information_a == cards_information_b);
} // bool operator!=(CardsInformation cards_information_a, CardsInformation cards_information_b)

#ifdef SELF_CHECK
#undef SELF_CHECK
#endif
