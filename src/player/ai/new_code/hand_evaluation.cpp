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
 ********************************************************************/

#include "constants.h"

#include "hand_evaluation.h"

#include "ai.h"
#include "HeuristicInterface.h"
#include "heuristics.h"
#include "heuristics_utils.h"

#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"


#ifdef BENDERS
#define ANNOUNCE_DEBUG
#endif

// #define ESSENER_DEBUG

using namespace HEURISTICS_UTILS;


////////////////////////////////////////////////////////////////
// calculate the value of a hand for the different solo types //
////////////////////////////////////////////////////////////////

///////////////////////
// support functions //
///////////////////////

/**
 ** -> result
 **
 ** @param     hand    the hand to check
 ** @param     t       the game type
 **
 ** @return    number of single plain suit tens of the given hand
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
unsigned
number_of_single_tens(Hand const& hand, GameType t)
{
  bool const dollen = hand.game().rule()(Rule::DOLLEN);

  unsigned result=0;
  if (hand.numberof(Card::DIAMOND)<=2 && hand.numberof(Card::DIAMOND,Card::ACE)==0
      && !Card(Card::DIAMOND,Card::ACE).istrump(t, dollen))
    result+=hand.numberof(Card::DIAMOND,Card::TEN);
  if (hand.numberof(Card::HEART)<=2 && hand.numberof(Card::HEART,Card::ACE)==0
      && !Card(Card::HEART,Card::ACE).istrump(t, dollen))
    result+=hand.numberof(Card::HEART,Card::TEN);
  if (hand.numberof(Card::SPADE)<=2 && hand.numberof(Card::SPADE,Card::ACE)==0
      && !Card(Card::SPADE,Card::ACE).istrump(t, dollen))
    result+=hand.numberof(Card::SPADE,Card::TEN);
  if (hand.numberof(Card::CLUB)<=2 && hand.numberof(Card::CLUB,Card::ACE)==0
      && !Card(Card::CLUB,Card::ACE).istrump(t, dollen))
    result+=hand.numberof(Card::CLUB,Card::TEN);


  return result;
} // unsigned number_of_single_tens(Hand const& hand, GameType t)


/**
 ** -> result
 **
 ** @param     hand    the hand to check
 ** @param     t       the game type
 **
 ** @return    number of void suits of the given hand
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
unsigned
number_of_voids(Hand const& hand, GameType t)
{
  bool const dollen = hand.game().rule()(Rule::DOLLEN);

  unsigned result=0;
  if (hand.numberof(Card::DIAMOND, t, dollen)==0)
  {
    result++;
  }
  if (hand.numberof(Card::HEART, t, dollen)==0)
  {
    result++;
  }
  if (hand.numberof(Card::SPADE, t, dollen)==0)
  {
    result++;
  }
  if (hand.numberof(Card::CLUB, t, dollen)==0)
  {
    result++;
  }

  return result;
} // unsigned number_of_voids(Hand const& hand, GameType t)


#ifdef OUTDATED

/**********************************************************************
 **
 **   bool Ai::single_ace()
 **
 **    Parameters: none
 *
 **    Result: number of single tens on hand
 **
 **    Version: Alpha
 **
 **    Description:
 **
 ** @todo	'Card::inttovalue()' in the Card-constructors
 **
 **********************************************************************/
/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the player has a single ace of a color
 **            and else only tens of the color
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
bool
Ai::single_ace(Hand const& hand) const
{
  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().begin();
       ++c) {
    if (*c == hand.game().trumpcolor())
      continue;

    if (   (this->hand.numberof(*c, Card::ACE) == 1)
        && (this->hand.numberof(*c, Card::ACE)
            + this->hand.numberof(*c, Card::TEN)
            == this-> hand.numberof(*c)))
      return true;
  } // for (c \in rule->card_colors)

  return false;
} // bool Ai::single_ace() const

#endif


/////////////////////
// solo evaluation //
/////////////////////

/**
 ** calculates the hand value for single picture solos
 **
 ** @param	hand     the hand to check
 ** @param	c        trump card for the solo
 ** @param	t        the type of the solo
 **
 ** @return	hand value for the solo to check
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 **
 **/
int
checksolo(Hand const& hand, Card::Value c, GameType t)
{
  int res=0;
  int countTrumps = 0;
  int clubs = 0;
  int spades = 0;

  for (unsigned i=0; i<hand.cardsnumber();i++)
  {
    if (hand.card(i).value()==c)
    {
      countTrumps++;
      if (hand.card(i).color()== Card::CLUB)
      {
        res+=4;
        clubs++;
      }

      if (hand.card(i).color()==Card::SPADE)
      {
        res+=3;
        spades++;
      }


      if (hand.card(i).color()==Card::HEART)
      {
        res+=2;
      }


      if (hand.card(i).color()==Card::DIAMOND)
      {
        res+=1;
      }

      continue;
    }

    if (hand.card(i).value()==Card::ACE)
    {
      res+=1;
      continue;
    }
    res -= 1;
  }

  if ( clubs + spades >= 3 )
    res += 4;

  if (countTrumps > 4) res+= 9 + countTrumps - hand.cardsnumber();

  res+=number_of_voids(hand, t)*2;

  res-=number_of_single_tens(hand, t)*4;
  // color without aces
  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().end();
       ++c)
    if ( hand.numberof(*c,t,hand.game().rule()(Rule::DOLLEN) ) == 0
        && hand.numberof(*c, Card::ACE) == 0 )
      res-= 4;

  return res;
} // int checksolo(Hand const& hand, Card::Value c, GameType t)


/**
 ** calculates the hand value for double picture solos
 **
 ** @param	hand     the hand to check
 ** @param	c1       higher trump card for the solo
 ** @param	c2       lower trump card for the solo
 ** @param	t        the type of the solo
 **
 ** @return	hand value for the solo to check
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 **
 **/
int
checksolo(Hand const& hand, Card::Value c1, Card::Value c2, GameType t)
{
  int res=0;
  for (unsigned i=0; i<hand.cardsnumber();i++)
  {
    if (hand.card(i).value()==c1)
    {
      if (hand.card(i).color()==Card::CLUB)
        res+=8;

      if (hand.card(i).color()==Card::SPADE)
        res+=7;

      if (hand.card(i).color()==Card::HEART)
        res+=6;

      if (hand.card(i).color()==Card::DIAMOND)
        res+=5;
      continue;
    }

    if (hand.card(i).value()==c2)
    {
      if (hand.card(i).color()==Card::CLUB)
        res+=4;

      if (hand.card(i).color()==Card::SPADE)
        res+=3;

      if (hand.card(i).color()==Card::HEART)
        res+=2;

      if (hand.card(i).color()==Card::DIAMOND)
        res+=1;
      continue;
    }

    if (hand.card(i).value()==Card::ACE)
    {
      res+=1;
      continue;
    }

    res-=2;
  }

  res+=number_of_voids(hand, t)*6;
  res-=number_of_single_tens(hand, t)*8;

  // color without aces
  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().end();
       ++c)
    if (   hand.numberof(*c, t, hand.game().rule()(Rule::DOLLEN) == 0)
        && hand.numberof(*c, Card::ACE) == 0
       )
      res-= 7;

  return res;
} // int checksolo(Hand const& hand, Card::Value c1, Card::Value c2, GameType t)


/**
 ** calculates the hand value for triple picture solos
 **
 ** @param	hand     the hand to check
 ** @param	c1       highest trump card for the solo
 ** @param	c2       middle trump card for the solo
 ** @param	c3       lowest trump card for the solo
 ** @param	t        the type of the solo
 **
 ** @return	hand value for the solo to check
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 **
 **/
int
checksolo(Hand const& hand, Card::Value c1, Card::Value c2, Card::Value c3, GameType t)
{
  int res=0;
  for (unsigned i=0; i<hand.cardsnumber();i++)
  {

    if (hand.card(i).value()==c1)
    {
      if (hand.card(i).color()==Card::CLUB)
        res+=12;

      if (hand.card(i).color()==Card::SPADE)
        res+=11;

      if (hand.card(i).color()==Card::HEART)
        res+=10;

      if (hand.card(i).color()==Card::DIAMOND)
        res+=9;
      continue;
    }

    if (hand.card(i).value()==c2)
    {
      if (hand.card(i).color()==Card::CLUB)
        res+=8;

      if (hand.card(i).color()==Card::SPADE)
        res+=7;

      if (hand.card(i).color()==Card::HEART)
        res+=6;

      if (hand.card(i).color()==Card::DIAMOND)
        res+=5;
      continue;
    }

    if (hand.card(i).value()==c3)
    {
      if (hand.card(i).color()==Card::CLUB)
        res+=4;

      if (hand.card(i).color()==Card::SPADE)
        res+=3;

      if (hand.card(i).color()==Card::HEART)
        res+=2;

      if (hand.card(i).color()==Card::DIAMOND)
        res+=1;
      continue;
    }

    if (hand.card(i).value()==Card::ACE)
    {
      res+=1;
      continue;
    }

    res -= 3;
  }

  res+=number_of_voids(hand, t)*10;

  res-=number_of_single_tens(hand, t)*12;

  if (t==GAMETYPE::SOLO_KOEHLER)
    for (vector<Card::Color>::const_iterator
         c = hand.game().rule().card_colors().begin();
         c != hand.game().rule().card_colors().end();
         ++c)
      if (hand.numberof(*c, t, hand.game().rule()(Rule::DOLLEN)) == 0)
        res-=5;

  return res;
} // int checksolo(Hand const& hand, Card::Value c1, Card::Value c2, Card::Value c3, GameType t)


/**
 ** calculates the hand value for color solos
 **
 ** @param	hand     the hand to check
 ** @param	c        trump color for the solo
 ** @param	t        the type of the solo
 **
 ** @return	hand value for the solo to check
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 **
 **/
int
checksolo(Hand const& hand, Card::Color c, GameType t)
{
  int res=0;
  unsigned trumps = hand.cardsnumber();

  if ((hand.numberof(Card(c,Card::ACE))==2) && party.rule()(Rule::SWINES_IN_SOLO))
  {
    res+=9;
  }

  if( c == Card::HEART && hand.game().rule()(Rule::DOLLEN) )
  {
    res += 2;
  }

  for (unsigned i=0; i<hand.cardsnumber();i++)
  {
    if (hand.card(i).isdolle())
    {

      res+=4;
      if( !party.rule()(Rule::SWINES_IN_SOLO) ) res += 2;
      continue;
    }

    if (hand.card(i).value()==Card::QUEEN)
    {
      res+=3;

      continue;

    }

    if (hand.card(i).value()==Card::JACK)
    {
      res+=2;

      continue;

    }

    if( hand.card(i).value()==Card::ACE && hand.card(i).color()!=c )
    {
      if (hand.card(i).color()==Card::HEART && hand.game().rule()(Rule::DOLLEN) )
      {
        if (hand.numberof(hand.card(i).color())<2)
        {
          res+=1;
          trumps--;
          continue;
        }

      } else
        if (hand.numberof(hand.card(i).color())<3)
        {
          res+=1;
          trumps--;
          continue;
        }
    }

    if( hand.card(i).color() != c )
    {
      trumps--;
      res -= 1;
    }

  }


  res+=number_of_voids(hand, t)*2;


  if ( c!=Card::DIAMOND && hand.numberof(c)==0) res-=3;
  if ( c==Card::DIAMOND && hand.numberof(c,GAMETYPE::SOLO_SPADE,hand.game().rule()(Rule::DOLLEN))==0) res-=3;


  if( trumps == hand.cardsnumber() )   res += 5;
  if( trumps == hand.cardsnumber()-1 ) res += 4;
  if( trumps == hand.cardsnumber()-2 ) res += 2;


  res-=number_of_single_tens(hand, t)*4;

  return res;
} // int checksolo(Hand const& hand, Card::Color c, GameType t)


/**
 ** calculates the hand value for a meatless solo
 **
 ** @param	hand     the hand to check
 **
 ** @return	hand value for a meatless solo
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 **
 **/
int
checksolo(Hand const& hand)
{
  int res=0;

  Card::Color co=Card::DIAMOND;
  res+=hand.numberof(co)*hand.numberof(Card(co,Card::ACE))+
    hand.numberof(co)*hand.numberof(Card(co,Card::TEN));

  co=Card::HEART;
  res+=hand.numberof(co)*hand.numberof(Card(co,Card::ACE))+
    hand.numberof(co)*hand.numberof(Card(co,Card::TEN));

  co=Card::SPADE;
  res+=hand.numberof(co)*hand.numberof(Card(co,Card::ACE))+
    hand.numberof(co)*hand.numberof(Card(co,Card::TEN));

  co=Card::CLUB;
  res+=hand.numberof(co)*hand.numberof(Card(co,Card::ACE))+
    hand.numberof(co)*hand.numberof(Card(co,Card::TEN));

  res-=number_of_single_tens(hand, GAMETYPE::SOLO_MEATLESS)*5;

  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().end();
       ++c)
    if ( hand.numberof(*c,GAMETYPE::SOLO_MEATLESS,hand.game().rule()(Rule::DOLLEN) ) == 0)
      res-=2*(2 - hand.numberof(*c, Card::ACE) );


  return res;
} // int checksolo(Hand const& hand)


/**
 ** finds a suitable marriage selector for the given hand
 **
 ** @param	hand     the hand to check
 **
 ** @return	tricktype which determines teams for a marriage
 **
 ** @version	0.7.6
 **
 ** @author	Diether Knof
 ** @author     Borg Enders
 ** @author     Tassilo Kirsch
 **
 **/
MarriageSelector
get_Marriage(Hand const& hand)
{
  if (hand.game().rule()(Rule::MARRIAGE_FIRST_FOREIGN))
    return MARRIAGE_SELECTOR::FIRST_FOREIGN;

  bool is_leading = hand.game().startplayer() == hand.player();

  if (   hand.game().rule()(Rule::MARRIAGE_FIRST_TRUMP)
      && (   (   !hand.game().rule()(Rule::MARRIAGE_FIRST_COLOR)
             && !hand.game().rule()(Rule::MARRIAGE_FIRST_SINGLE_COLOR)
             )
          || (   (hand.numberoftrumps() < 6)
              && (hand.numberoftrumpaces() == 1)
              && (   is_leading
                  || (hand.game().rule()(Rule::MARRIAGE_DETERMINATION) >= 4)
                 )
             )
         )
     )
    return MARRIAGE_SELECTOR::FIRST_TRUMP;

  if (hand.game().rule()(Rule::MARRIAGE_FIRST_SINGLE_COLOR)) {

    // check if we have a very weak plain suit

    if (   (hand.numberof(Card::CLUB) > 4)
        || (   (hand.numberof(Card::CLUB) < 2)
            && (hand.numberof(Card::CLUB,Card::TEN) == 1)
            && is_leading
           )
       )
      return MARRIAGE_SELECTOR::FIRST_CLUB;

    if (  (hand.numberof(Card::SPADE) > 4)
        || (   (hand.numberof(Card::SPADE) < 2)
            && (hand.numberof(Card::SPADE,Card::TEN) == 1)
            && is_leading
           )
       )
      return MARRIAGE_SELECTOR::FIRST_SPADE;

    if (Card::DOLLE.istrump(hand.game())) {
      if (   (hand.numberof(Card::HEART) > 3)
          && hand.numberof(Card::HEART, Card::ACE)
         )
        return MARRIAGE_SELECTOR::FIRST_HEART;
    } else {
      if (   (hand.numberof(Card::HEART) > 4)
          || (   (hand.numberof(Card::HEART) < 2)
              && (hand.numberof(Card::HEART,Card::TEN) == 1)
              && is_leading
             )
         )
        return MARRIAGE_SELECTOR::FIRST_HEART;
    }

    if (   (hand.numberof(Card::CLUB) == 1)
        && (hand.numberof(Card::CLUB,Card::ACE) != 1)
        && is_leading
       )
      return MARRIAGE_SELECTOR::FIRST_CLUB;

    if (   (hand.numberof(Card::SPADE) == 1)
        && (hand.numberof(Card::SPADE,Card::ACE) != 1)
        && is_leading
       )
      return MARRIAGE_SELECTOR::FIRST_SPADE;

    if (   (hand.numberof(Card::HEART) == 1)
        && (hand.numberof(Card::HEART,Card::ACE) != 1)
        && is_leading
       )
      return MARRIAGE_SELECTOR::FIRST_HEART;

    if (   !hand.game().rule()(Rule::MARRIAGE_FIRST_COLOR)
        && !hand.game().rule()(Rule::MARRIAGE_FIRST_TRUMP)
       )
    {
      // first choice: longest suit without ace
      unsigned max_len = 0;
      Card::Color col = Card::NOCARDCOLOR;

      for (vector<Card::Color>::const_iterator
           c = hand.game().rule().card_colors().begin();
           c != hand.game().rule().card_colors().end();
           ++c)
      {
        if (*c == Card::DIAMOND)
          continue;
        unsigned len = hand.numberof(*c);
        if (   (len > max_len)
            && !hand.contains(Card(*c, Card::ACE))
           )
        {
          max_len = len;
          col = *c;
        }
      }

      if (col == Card::NOCARDCOLOR) {
        // second choice: longest suit with a possible loser
        for (vector<Card::Color>::const_iterator
             c = hand.game().rule().card_colors().begin();
             c != hand.game().rule().card_colors().end();
             ++c)
        {
          if (*c == Card::DIAMOND)
            continue;
          unsigned len = hand.numberof(*c);
          if (len <= max_len)
            continue;
          Card::Value val = hand.lowest_value(*c);
          if ((val != Card::NOCARDVALUE) && (val != Card::ACE)) {
            unsigned aces = hand.numberof(Card(*c, Card::ACE));
            if (   (aces < 2)
                || (val < Card::KING)
                || (   (val != Card::TEN)
                    && ((*c != Card::HEART) || Card::DOLLE.istrump(hand.game()))
                   )
               )
            {
              max_len = len;
              col = *c;
            }
          }
        }
      }

      if (col == Card::NOCARDCOLOR) {
        // last choice: longest suit if we're leading, void suit otherwise
        for (vector<Card::Color>::const_iterator
             c = hand.game().rule().card_colors().begin();
             c != hand.game().rule().card_colors().end();
             ++c)
        {
          if (*c == Card::DIAMOND)
            continue;
          unsigned len = hand.numberof(*c);
          if (is_leading) {
            if (len > max_len) {
              max_len = len;
              col = *c;
            }
          } else if (!len) {
            col = *c;
            break;
          }
        }
      }

      switch (col) {
      case Card::NOCARDCOLOR:
      case Card::CLUB:
        return MARRIAGE_SELECTOR::FIRST_CLUB;
      case Card::SPADE:
        return MARRIAGE_SELECTOR::FIRST_SPADE;
      case Card::HEART:
        return MARRIAGE_SELECTOR::FIRST_HEART;
      }
    } // single color is the only possible selector

  } //if (hand.game().rule()(Rule::MARRIAGE_FIRST_SINGLE_COLOR))

  if (   hand.game().rule()(Rule::MARRIAGE_FIRST_COLOR)
      && (   ((hand.numberoftrumps() >= 8) && !is_leading)
          || !hand.game().rule()(Rule::MARRIAGE_FIRST_TRUMP)
         )
     )
    return MARRIAGE_SELECTOR::FIRST_COLOR;

  if (   hand.game().rule()(Rule::MARRIAGE_FIRST_TRUMP)
      && hand.hastrump()
     )
    return MARRIAGE_SELECTOR::FIRST_TRUMP;

  // this is the default if no other selector is possible
  return MARRIAGE_SELECTOR::FIRST_COLOR;

} // MarriageSelector get_Marriage(Hand const& hand)



////////////////////////////////////////////
// evaluate hand values for announcements //
////////////////////////////////////////////


//////////////////////////////////////////////////
// hand value according to the 'Essener System' //
//////////////////////////////////////////////////

class RuffValues : public vector<Card::Value> {
  public:
    RuffValues();
    void add(Card::Value value);
    void add(unsigned num, Card::Value value);
    void del(Card::Value value);
    unsigned pop();
    unsigned num(Card::Value value);
};

RuffValues::RuffValues() : vector<Card::Value>()
{ return; }

void
RuffValues::add(Card::Value value)
{
  this->add(1, value);
  return;
}

void
RuffValues::add(unsigned num, Card::Value value)
{
  vector<Card::Value>::iterator v;
  for ( v = this->begin(); (v != this->end()) && ((*v == Card::QUEEN) || ((value != Card::QUEEN) && (value > *v))); ++v)
  {};
  this->insert(v, num, value);
  return;
}

void
RuffValues::del(Card::Value value)
{
  // we can't use a simple values.remove(), because we may have multiple elements with the same value and
  // are only allowed to remove one of them
  for (vector<Card::Value>::iterator v = this->begin(); v != this->end(); ++v) {
    if (*v == value) {
      this->erase(v);
      break;
    }
  }
  return;
}

unsigned
RuffValues::pop()
{
  unsigned result = this->back() * 1000;
  this->pop_back();
  return result;
}

unsigned
RuffValues::num(Card::Value value)
{
  unsigned result = 0;
  for (vector<Card::Value>::const_iterator v = this->begin(); v != this->end(); ++v) {
    if (*v == value) {
      result++;
    }
  }
  return result;
}

void
addvalue(Card::TColor color, char section,
         unsigned playerpoints, unsigned otherpoints,
         unsigned const p_mul, unsigned const p_div,
         unsigned const p_mul_announced, unsigned const p_div_announced,
         unsigned& value, unsigned& value_announced)
{
  unsigned result = ((playerpoints + otherpoints) * p_mul) / p_div;
  unsigned result_announced = ((playerpoints + otherpoints) * p_mul_announced) / p_div_announced;

#ifdef ESSENER_DEBUG

  char c;
  switch (color) {
  case Card::DIAMOND:
    c = 'D';
    break;
  case Card::HEART:
    c = 'H';
    break;
  case Card::SPADE:
    c = 'S';
    break;
  case Card::CLUB:
    c = 'C';
    break;
  case Card::TRUMP:
    c = 'T';
    break;
  case Card::NOCARDCOLOR:
    c = 'L';
    break;
  }
  cout << c << section << ": "
       << setw(5) << right << result
       << " = (" << playerpoints << " + " << otherpoints
       << ") * (" << p_mul << "/" << p_div << ")";
#endif

  if (result_announced > result) {
#ifdef ESSENER_DEBUG

    cout << " [+ " << result_announced - result
         << " (" << p_mul_announced << "/" << p_div_announced << ")]";
#endif
    value_announced += result_announced - result;
  }

#ifdef ESSENER_DEBUG

  cout << endl;
#endif

  value += result;
  return;
} // addvalue


/**
 ** Predicts the trick points a player will win in a game using the Essener System
 ** with a few additions to incorporate trump length. To avoid floating point math,
 ** factored unsigned ints with a factor of 1000 are used for value and value_announced.
 **
 ** @param    hi                heuristic interface
 ** @param    &value            points predicted by the Essener System
 ** @param    &value_announced  additional points if the player's team is known to all (usually due to an announcement)
 **
 ** @return   -
 **
 ** @author   Tassilo Kirsch
 **
 ** @version  0.7.6
 **/
void
CalcHandValueEssener(HeuristicInterface const& hi, unsigned& value, unsigned& value_announced)
{
  Game const& g = hi.game();

#ifdef ESSENER_DEBUG
  if (!g.isvirtual())
    cout << "Player #" << hi.no() << " hand value according to the Essener System:" << endl;
#endif

  // to avoid floating point math, we use factored unsigned values, factor 1000 should provide sufficient precision

  // the estimated hand value
  value = 0;
  // additional points if announcement is made
  value_announced = 0;


  // *** 0 ***
  // check if we can use the Essener System

  // valid game types: normal games (including all types of marriages) and color soli
  if (!(GAMETYPE::is_normal(g.type()) || GAMETYPE::is_marriage(g.type()) || GAMETYPE::is_color_solo(g.type())))
    return;

  // not useful before the marriage is determined
  if (   (g.marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)
      && (g.marriage_selector() != MARRIAGE_SELECTOR::SILENT) )
    return;

  // only useful for the first couple of tricks
  if (!::in_running_game() || (hi.hand().cardsnumber() < 8))
    return;

  // the player
  Player const& player = g.player( hi.no() );

  // the card to play in the current trick
  HandCard card_to_play(hi.hand(), Card::EMPTY);

  // trumps which can be used to ruff tricks
  RuffValues ruff_values;

  unsigned num_suit [] = {0,0,0,0}; // number of cards per suit, indices correspond to Card::TColor
  bool chicane[] = {true,true,true,true}; // if the suit was void in the complete hand
  unsigned num_aces [] = {0,0,0,0}; // number of aces in suit
  unsigned num_trumps = 0;          // number of trumps
  unsigned num_trumps_partner = 4;  // conservative guess for the partner's number of trumps
  unsigned num_dollen = 0;          // number of Heart-10s
  unsigned num_alte = 0;            // number of Club-Queens
  unsigned num_blaue = 0;           // number of Spade-Queens

  unsigned points_suit [] = {0,0,0,0};        // points on hand per suit
  unsigned points_played_suit [] = {0,0,0,0}; // points already played per suit
  unsigned cards_played_suit [] = {0,0,0,0};  // cards already played per suit
  unsigned runs_suit [] = {0,0,0,0};          // tricks already played per suit

  unsigned points_player  = 0; // points made by the player
  unsigned points_partner = 0; // points made by the partner
  unsigned points_oppteam = 0; // points made by the opposite team


  // *** 1 ***
  // We need to know if we are in (or can get into) first position for the nexr trick.
  // For this we may have to "play" the current trick (which will also improve further estimations).

  // are we in first position?
  bool canstart = g.startplayer() == player;

  if (::in_running_game()) {

    // play current trick
    Trick t(g.trick_current());
    Card::TColor startcolor = (t.isempty()) ? Card::NOCARDCOLOR : t.startcard().color();

    // do we already know our partner (and does he sit to our right)?
    Player const* partner = hi.team_information().guessed_partner();
    bool partner_sits_right = partner && (*partner == g.player_previous(player));

    // check if we can guarantee to win a trump trick started by our partner
    // for this we need to have the nut trump, i.e. a trump that's higher than all cards from the opposite team
    Card highest_opp = Card::EMPTY;
    for (vector<Player*>::const_iterator p = g.players_begin(); p != g.players_end(); ++p) {
      if ((*p == &player) || (*p == partner))
        continue;
      HandCard const c = HandCard((*p)->hand(), hi.cards_information().possible_hand(**p).highest_card());
      if ( !(c.is_empty() || ((c == Card::CLUB_QUEEN) && player.isre())) && (highest_opp.is_empty() || highest_opp.less(c)) )
        highest_opp = c;
    } // for (p)
    bool nuttrump = highest_opp.is_empty() || highest_opp.less(hi.hand().highest_card());


    if (t.isempty())
      // our aces are playable if we're in first position or our partner is in first position and can
      // play plain suit or trump towards us without interception
      canstart = (t.cardno_of_player( player ) == 0) ||
                 ((hi.teamofplayer(t.startplayer()) == hi.team()) && (partner_sits_right || nuttrump));
    else if (!t.has_played(player) && (t.actplayer() != player)) {
        // we haven't played yet and we're not the next player
        // do not estimate points for the current trick but check if we can get it
        canstart = (t.startcard().istrump() || !hi.hand().existstcolor(startcolor))
                   && cardWillWinTheTrick(t, hi, hi.hand().highest_trump());

    } else {
      if (!t.has_played(player) && t.islastcard()) {

        // already to own team?
        if ( (hi.teamofplayer(t.winnerplayer()) == hi.team()) && (partner_sits_right || nuttrump) ) {
          card_to_play = Heuristics::choose_pfund( t, hi );
        } else {
          // see if we can get the trick
          HandCards const ha = hi.hand().validcards( t );
          HandCard const trumpace = HandCard(hi.hand(), g.trumpcolor(), Card::ACE);
          HandCard best_card(hi.hand(), Card::EMPTY);

          for( unsigned i = 0; i < ha.cardsnumber(); i++ ) {
            HandCard const& card = ha.card(i);
            if (!t.is_jabbed(card))
              continue;
            if (best_card.is_empty())
              best_card = card;
            else {
              if (trumpace.less(best_card)) {
                if (card.less(best_card))
                  best_card = card;
              } else if (!trumpace.less(card) && best_card.less(card))
                best_card = card;
            }
            if (best_card.value() == Card::ACE)
              break;
          }
          if (best_card.is_empty()) {
            // we cannot get the trick, therefore
            // @ToDo use something like "optimal_throw_card" instead
            card_to_play = lowest_card(t, hi.hand());
          } else {
            card_to_play = best_card;
          }
        }
        // now "play" the card
        if (!card_to_play.is_empty())
          t += card_to_play;

      } // if (!t.has_played(player) && t.islastcard())

      if (!t.has_played(player) && (t.actplayer() == player)) {
        // we're at position 2 or 3, we won't treat this trick as already played,
        // but we have to check if we can get it to play our aces
        if (t.startcard().istrump() || (hi.hand().numberof(startcolor, g) == 0)) {
          // trump trick or ruffing, try highest trump
          card_to_play = hi.hand().highest_trump();
          if (!card_to_play.is_empty())
            t += card_to_play;
          if (!t.startcard().istrump())
            // we're ruffing, don't count trick points
            card_to_play = Card::EMPTY;
        } else {
          // we have to (and can) follow suit
          t += hi.hand().highest_card(startcolor);
        }
      }

      // this is preliminary if the trick isn't full
      canstart = (t.winnerplayer() == player) ||
                 ((hi.teamofplayer(t.winnerplayer()) == hi.team()) && (partner_sits_right || nuttrump));

      if (!t.isfull()) {
        if (canstart) {
          if ( t.startcard().istrump() || (hi.color_runs(startcolor) > 0) ) {
            // treat 2nd(or higher) color runs as if they were trump tricks
            canstart = !oppositeTeamCanWinTrick( t, hi );
            if (!canstart) {
              // reset card_to_play
              card_to_play != Card::EMPTY;
            }
          } else {
            // assume that we'll get this first run if we've ruffed or played the first ace
            canstart = t.winnercard().istrump() || (t.winnercard().value() == Card::ACE);
          }
        }
      }
    } // if (t.isempty())

    // *** 2 ***
    // analyse already played tricks
    unsigned played = g.real_trick_current_no();

    for (unsigned i = 0; i <= played; i++) {
      Trick const tr = ((i == played) && (t.isfull() || !card_to_play.is_empty())) ? t : g.trick(i);
      if (!tr.isempty()) {
        if (tr.isfull() || ((i == played) && !card_to_play.is_empty())) {
          // this way we can include the current trick if it has already been played
          if (tr.winnerplayer() == player)
            points_player += tr.points();
          else if (hi.teamofplayer(tr.winnerplayer()) == hi.team())
            points_partner += tr.points();
          else
            points_oppteam += tr.points();
        }

        if (tr.startcard().istrump()) {
          if (num_trumps_partner)
            num_trumps_partner--;
        } else {
          if (tr.has_played(player))
            runs_suit[tr.startcard().color()]++;
        }

        for (unsigned c = 0; c < tr.actcardno(); ++c) {
          HandCard const& card = tr.card(c);
          if (!card.istrump() && ((card.color() != tr.startcard().color()) || tr.has_played(player))) {
            points_played_suit[card.color()] += 1000 * card.value();
            cards_played_suit[card.color()]++;
          }
          if ( (card.player() != player) && (card.tcolor() != tr.startcard().tcolor()) ) {
            if (hi.teamofplayer(card.player()) == hi.team()) {
              if (card.istrump() && num_trumps_partner)
                 num_trumps_partner--;
              // we do not check if our partner just ruffed a color, because that's our partner's strength,
              // and not our own.
            } else if (!tr.startcard().istrump() && (c > tr.cardno_of_player(player))) {
              // opponent sitting behind us didn't need to follow suit, don't estimate points for 2nd run ruffing
              runs_suit[card.color()] = 2;
            }
          }
        } // for (unsigned c = 0; c < tr.actcardno(); ++c)
      } // if (!tr.isempty())
    } // for (unsigned i = 0; i < played; i++)
  } // if (::in_running_game())


  // *** 3 ***
  // iterate hand's cards only once to calc all neccessary values (instead of using numberof methods)
  for( unsigned i = 0; i < hi.hand().cardsnumber_all(); i++ )
  {
    HandCard const& c = hi.hand().card_all(i);
    if (hi.hand().played(i)) {
      if (!c.istrump())
        chicane[c.color()] = false;
      continue;
    }
    if (c == card_to_play)
      // ignore the card we've estimated for the current trick
      continue;

    if (c.istrump()) {
      num_trumps++;
      if (c.isdolle())
        num_dollen++;
      else if (c == Card::CLUB_QUEEN)
        num_alte++;
      else if ((c.value() == Card::QUEEN) && (c.color() == Card::SPADE))
        num_blaue++;
      else
        ruff_values.add(c.value()); // we will add the spade queens later
    } else {
      chicane[c.color()] = false;
      num_suit[c.color()]++;
      points_suit[c.color()] += 1000 * c.value();
      if (c.value() == Card::ACE)
        num_aces[c.color()]++;
    }
  }

  // *** 4 ***
  // calc estimated points for highest trumps

  bool withdolle = hi.hand().numberofall(Card::DOLLE) > 0;

  // add heart tens
  for (; num_dollen > 0; num_dollen--) {
    addvalue(Card::TRUMP, 'T', 10000, 15000, 1, 1, 1, 1, value, value_announced);
  }

  // add club queen + spade queen combos
  while (num_alte && num_blaue) {
    if (withdolle)
      addvalue(Card::TRUMP, 'C', 3000, 14500, 1, 1, 1, 1, value, value_announced);
    else
      addvalue(Card::TRUMP, 'C', 3000, 19500, 3, 4, 3, 4, value, value_announced);
    num_blaue--;
    num_alte--;
  }
  // remaining spade queens can be used to ruff tricks
  ruff_values.add(num_blaue, Card::QUEEN);

  // add club queens
  for (; num_alte > 0; num_alte--) {
    if (withdolle)
      addvalue(Card::TRUMP, 'Q', 3000, 9500, 1, 1, 1, 1, value, value_announced);
    else
      addvalue(Card::TRUMP, 'Q', 3000, 14500, 3, 4, 3, 4, value, value_announced);
  }

  // *** 5 ***
  // calc estimated points for plain suits

  // from now on there'll be lots of () - we want to make shure to multiply first, then divide (factored ints, remember?)

  // to simplify following loops,
  // allows to treat black colors first, heart last (diamond can be treated as a black color in non-trump color soli)
  Card::Color suits[3];
  unsigned i = 0;
  if (g.trumpcolor() != Card::CLUB)
    suits[i++] = Card::CLUB;
  if (g.trumpcolor() != Card::SPADE)
    suits[i++] = Card::SPADE;
  if (g.trumpcolor() != Card::DIAMOND)
    suits[i++] = Card::DIAMOND;
  if (g.trumpcolor() != Card::HEART)
    suits[i++] = Card::HEART;


  // trick points from cards of one suit
  unsigned suitpoints;

  // start with the best: chicane - ruffing the first run
  for (i = 0; !ruff_values.empty() && (i < 3); i++) {
    Card::Color suit = suits[i];
    unsigned remaining = ((suit == Card::HEART) ? 6 : 8) - cards_played_suit[suit];
    if ((num_suit[suit] + runs_suit[suit] == 0) && (remaining >= 3)) {
      if (suit == Card::HEART) {
        suitpoints =  11000 + ((2 * (19000 - points_played_suit[suit])) / (5 - cards_played_suit[suit]));
        addvalue(suit, '1', ruff_values.pop(), suitpoints, 3, 4, 3, 4, value, value_announced);
      } else {
        suitpoints =  11000 + ((2 * (39000 - points_played_suit[suit])) / (7 - cards_played_suit[suit]));
        addvalue(suit, '1', ruff_values.pop(), suitpoints, 9, 10, 9, 10, value, value_announced);
      }
      num_trumps--; // trump was used to ruff, thus can't be used to generate trick through trump length
      runs_suit[suit]++;
      points_played_suit[suit] += suitpoints;
      cards_played_suit[suit] += 3;
    }
  }; // for (i = 0; !ruff_values.empty() && (i < 3); i++)



  // continue with chicane - ruffing the second run
  for (i = 0; !ruff_values.empty() && (i < 3); i++) {
    Card::Color suit = suits[i];
    unsigned remaining = 8 - cards_played_suit[suit];
    if ((suit != Card::HEART) && chicane[suit] && (runs_suit[suit] == 1) && (remaining >= 3)) {
      suitpoints =  (3 * (50000 - points_played_suit[suit])) / remaining;
      addvalue(suit, '2', ruff_values.pop(), suitpoints, 2, 3, 2, 3, value, value_announced);
      num_trumps--; // trump was used to ruff, thus can't be used to generate trick through trump length
    }
  }; // for (i = 0; !ruff_values.empty() && (i < 3); i++)


  // now for our aces - we won't award points for aces in 2nd runs and we only award points for single aces if we're in first position
  for (i = 0; i < 3; i++) {
    Card::Color suit = suits[i];
    unsigned remaining = ((suit == Card::HEART) ? 6 : 8) - cards_played_suit[suit] - num_suit[suit];
    if ( (remaining >= 3) && ((runs_suit[suit] == 0) && (num_aces[suit] > 0)) && (canstart || (num_aces[suit] == 2)) ) {
      // (11 + (3 / Anzahl der fehlenden Karten der jeweiligen Farbe) * (Gesamtaugen der Farbe - eigene Augen der Farbe)) * Wahrscheinlichkeit (bei Ansage)
      suitpoints = ((3 * (((suit == Card::HEART) ? 30000 : 50000) - points_played_suit[suit] - points_suit[suit])) / remaining);
      switch (remaining) {
      case 7:
        addvalue(suit, 'A', 11000, suitpoints, 9, 10, 9, 10, value, value_announced);
        break;
      case 6:
        addvalue(suit, 'A', 11000, suitpoints, 3, 4, 9, 10, value, value_announced);
        break;
      case 5:
        addvalue(suit, 'A', 11000, suitpoints, 2, 3, 3, 4, value, value_announced);
        break;
      case 4:
        addvalue(suit, 'A', 11000, suitpoints, 1, 2, 2, 3, value, value_announced);
        break;
      case 3:
        addvalue(suit, 'A', 11000, suitpoints, 1, 4, 1, 2, value, value_announced);
        break;
      default:
        break;
      }
      runs_suit[suit]++;
      points_suit[suit] -= 11000;
      points_played_suit[suit] += suitpoints + 11000;
      cards_played_suit[suit] += 4;
    } // ( (remaining >= 3) && ...
  }; // for (i = 0; i < 3; i++)

  // now for ruffing the second run with a singleton
  for (i = 0; !ruff_values.empty() && (i < 3); i++) {
    Card::Color suit = suits[i];
    if ( (suit != Card::HEART) && !chicane[suit] && ((num_suit[suit] + runs_suit[suit]) == 1) ) {
      if (runs_suit[suit] == 0) {
        if (cards_played_suit[suit] > 1)
          continue;
        // first run hasn't happened yet, estimate points
        points_played_suit[suit] += points_suit[suit] + ((3 * (50000 - points_suit[suit] - points_played_suit[suit])) / (7 - cards_played_suit[suit]));
        cards_played_suit[suit] += 4;
      }
      unsigned remaining = 8 - cards_played_suit[suit];
      if (remaining >= 3) {
        suitpoints =  (3 * (50000 - points_played_suit[suit])) / remaining;
        addvalue(suit, '2', ruff_values.pop(), suitpoints, 1, 2, 1, 2, value, value_announced);
        num_trumps--; // trump was used to ruff, thus can't be used to generate trick through trump length
      }
    }
  };


  // *** 6 ***
  // Addition to the Essener System: trump length (queens and general)

  // strength in weak queens
  if (ruff_values.num(Card::QUEEN) >= 4) {
    // we estimate one additional minimum value trick (Queen + 3 Jacks): 9
    addvalue(Card::NOCARDCOLOR, 'Q', 3000, 6000, 1, 1, 1, 1, value, value_announced);
  };


  // finally, trump length

  if (!ruff_values.empty() &&
      (num_trumps > (hi.cards_information().remaining_trumps_others() - num_trumps_partner) / 2))
  {
    // one additional minimum value trick (ruff card + ten (partner) + king + nine)
    addvalue(Card::NOCARDCOLOR, 'T', ruff_values.pop(), 14000, 1, 1, 1, 1, value, value_announced);
  };

#ifdef ESSENER_DEBUG
  if (!g.isvirtual()) {
    cout << "TOT " << value / 1000 << "." << value % 1000;
    if (value_announced > 0) {
      cout << " (" << (value + value_announced) / 1000 << "." << (value + value_announced) % 1000 << ")";
    }
    cout << " + " << points_player << " + " << points_partner << " - " << points_oppteam << endl;
  }
#endif

  return;

} // CalcHandValueEssener(...)


//////////////////////////
// freedoko hand values //
//////////////////////////

/**
 ** value of cards for announcement
 **
 ** @param        hi        heuristic interface
 ** @param        g         current game
 **
 ** @return       the value of the hand
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
int
CalcHandValue(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  calc hand value  ?
  // @heuristic::idea calculates the value of a hand for decision making of announcements, soli decisions,...

  Game const& g = hi.game();

#ifdef ESSENER_DEBUG
  if (!g.isvirtual()) {
    unsigned v, va;
    CalcHandValueEssener(hi, v, va);
  }
#endif

  int value = 0;
  unsigned i;

  unsigned v_queen = 0;
  unsigned v_jack = 0;
  unsigned v_highest = 0;
  unsigned v_king = 0;
  unsigned v_ace = 0;
  unsigned v_fehl = 0;

  if( g.type() == GAMETYPE::NORMAL )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2;
    // Todo: if aggressive
    v_highest += 1;
    if (hi.team() == TEAM::RE)
    {
      value = 1;
    } else
    {
      value = 0;
    }
    // Todo: if aggressive
    //  	value += 1;
  }


  if( g.type() == GAMETYPE::POVERTY )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 0; v_fehl = 2;
    value = -3;
  }

  if( g.type() == GAMETYPE::MARRIAGE )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2; }

  if( g.type() == GAMETYPE::SOLO_JACK )
  { v_queen = 0; v_jack = 3; v_highest = 0; v_king = 0; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if (g.type()==GAMETYPE::SOLO_QUEEN)
  { v_queen = 3; v_jack = 0; v_highest = 0; v_king = 0; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING )
  { v_queen = 0; v_jack = 0; v_highest = 0; v_king = 3; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if( g.type() == GAMETYPE::SOLO_QUEEN_JACK )
  { v_queen = 3; v_jack = 2; v_highest = 0; v_king = 0; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING_JACK )
  { v_queen = 0; v_jack = 2; v_highest = 0; v_king = 3; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING_QUEEN )
  { v_queen = 2; v_jack = 0; v_highest = 0; v_king = 3; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KOEHLER )
  { v_queen = 2; v_jack = 1; v_highest = 0; v_king = 3; v_ace = 0; v_fehl = 0;
    if (hi.team() == TEAM::RE)
    {
      value = -5;
    } else
    {
      value = -2;
    }
  }

  if(    g.type()==GAMETYPE::SOLO_CLUB
     || g.type()==GAMETYPE::SOLO_HEART
     || g.type()==GAMETYPE::SOLO_SPADE
     || g.type()==GAMETYPE::SOLO_DIAMOND )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2;
    if (hi.team() == TEAM::RE)
    {
      value = -7;
    } else
    {
      value = 0;
    }
  }


  if( g.type() == GAMETYPE::SOLO_MEATLESS )
  { v_queen = 0; v_jack = 0; v_highest = 0; v_king = 0; v_ace = 4; v_fehl = 1; // missing aces
    value = -2;
  }

  Player const& player = hi.game().player( hi.no() );

  for( i = 0; i < hi.hand().cardsnumber(); i++ )
  {
    if(   hi.hand().card(i).isdolle()
       || hi.hand().card(i).isswine()
       || hi.hand().card(i).possible_swine()
       || hi.hand().card(i).ishyperswine()
       || hi.hand().card(i).possible_hyperswine()
      )
      value += v_highest;
    else if( hi.hand().card(i).value()==Card::QUEEN
            || hi.hand().card(i).possible_genscher()  )
      value += v_queen;
    else if( hi.hand().card(i).value()==Card::JACK )
      value += v_jack;
    else if( hi.hand().card(i).value()==Card::KING )
      value += v_king;
    else if(    hi.hand().card(i).value()==Card::ACE
            && !hi.hand().card(i).istrump()
            && !hi.jabbedbyotherteam(hi.hand().card(i).color()))
    {
      value += v_ace;
      if (::in_running_game()
          && (hi.game().trick_current().cardno_of_player( player ) == 0 ) )
      {
        value += 1;
      }
    }
  }

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    Card::Color const color = *c;
    if (   !Card( color, Card::ACE ).istrump( hi.game() )
        && !hi.hand().existstcolor( color )
        && !hi.jabbedbyotherteam(color)
       )
      if( ::in_running_game() )
      {
        if(     hi.game().trick_current().actcardno()>0
           && hi.color_runs( color ) == 0
           && (hi.game().trick_current().startcard().tcolor()
               != color) )
          value += v_fehl;
      } else {
        value += v_fehl;
      }
  }

  if( g.type() == GAMETYPE::SOLO_MEATLESS ) {
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c)
    {
      if( hi.hand().existstcolor( *c ) )
        value -=  v_fehl
          * ( 2 - hi.hand().numberof( *c, Card::ACE ) );
    }
    if( hi.no() == hi.game().soloplayer().no() )
    {
      unsigned longColor = 0;
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
      {
        if(   hi.hand().numberof(*c,Card::ACE)  == 2
           && hi.hand().numberof(*c ) > longColor )
          longColor = hi.hand().numberof(*c );
      }
      value += longColor;
    }
  } else  {

    for( vector<Card::Color>::const_iterator
        c = hi.game().rule().card_colors().begin();
        c != hi.game().rule().card_colors().end();
        ++c )
    {
      Card::Color color = *c;

      if( color == hi.game().trumpcolor() )
        continue;
      // including this trick the player does not have the color
      if (    !hi.hand().existstcolor( color )
          && !(    ::in_running_game()
               && ( hi.game().trick_current().actcardno()
                   > hi.game().trick_current().cardno_of_player( player ) )
               && ( hi.game().trick_current().card_of_player( player ).tcolor()
                   == color )
              )
         )
        continue;

      if (  ( hi.hand().numberof( color, Card::ACE ) == 0 )
          && !hi.jabbedbyownteam(color) )
        value -= v_ace;

      if( (   hi.hand().numberof( color )
           + hi.color_runs( color )
           + (   ::in_running_game()
              && hi.game().trick_current().actcardno() > 0
              && ( hi.game().trick_current().startcard().color()
                  == color
                 ) ? 1 : 0 )
           > ( hi.game().different_cards_no( color ) - 1
              - ( hi.game().rule()(Rule::WITH_NINES) ? 0 : 1 ) )
          ) && !hi.jabbedbyownteam(color) )
        value -= v_fehl;

      if ( (   hi.hand().numberof( color )
            + hi.color_runs( color )
            + ( (   ::in_running_game()
                 && hi.game().trick_current().actcardno()>0
                 && ( hi.game().trick_current().startcard().color()
                     == color)
                ) ? 1 : 0)
            >= 3)
          && ( hi.hand().numberof( Card( color, Card::ACE ) ) < 2 )
          && !hi.jabbedbyownteam(color) )
        value -= v_ace;

    } // (for color)
  } // if (...)

  if( g.type() == GAMETYPE::SOLO_KOEHLER ) {
    for( vector<Card::Color>::const_iterator
        c = hi.game().rule().card_colors().begin();
        c != hi.game().rule().card_colors().end();
        ++c )
    {
      if( hi.hand().existstcolor(*c)
         && !hi.jabbedbyownteam(*c) )
        value-=5;
    }
  } // if( g.type() == GAMETYPE::SOLO_KOEHLER )


  if(   (   hi.game().swines_owner() != NULL
         && hi.game().swines_owner()->no() != hi.no() )
     || hi.game().swines_announcement_valid( hi.game().player( hi.no() ) )
    )
  {
    Player const& swines_player = ( hi.game().swines_owner()
                                   ? *hi.game().swines_owner()
                                   : hi.game().player( hi.no() ) );
    if(   hi.teamofplayer(swines_player) == hi.team()
       || hi.game().swines_announcement_valid( swines_player )
      )
    {
      if(swines_player.no() != hi.no() )
      {
        value += v_highest;
      } else
      {
        value += 1;
      }
    } else
    {
      value -= 2* v_highest;
    }
  }


  if(  (   hi.game().hyperswines_owner() != NULL
        && hi.game().hyperswines_owner()->no() != hi.no()
       )
     || hi.game().hyperswines_announcement_valid( hi.game().player( hi.no() ) )
    )
  {
    Player const& hyperswines_player = ( hi.game().hyperswines_owner()
                                        ? *hi.game().hyperswines_owner()
                                        : hi.game().player( hi.no() ) );
    if(   hi.teamofplayer(hyperswines_player) == hi.team()
       || hi.game().hyperswines_announcement_valid( hyperswines_player )
      )
    {
      if(hyperswines_player.no() != hi.no() )
      {
        value += v_highest;
      } else
      {
        value += 1;
      }
    } else
    {
      value -= 2* v_highest;
    }
  }

  if (hi.hand().numberoftrumps()
      >= ( (hi.cards_information().remaining_trumps_others() / 3)
          + round(  (::in_running_game()
                     ? hi.game().tricks_remaining_no()
                     : hi.game().trickno()
                    )/ 3)
         ) )
    value += 2;

  if (hi.hand().numberoftrumps() >= 10 )
    value += 1;
  if (hi.hand().numberoftrumps() >= hi.game().trickno() - 1 )
    value += 1;
  if (hi.hand().numberoftrumps() >= hi.game().trickno() )
    value += 1;
  if (hi.hand().numberoftrumps() == hi.hand().cardsnumber() )
    value += 1;

  if (    (::game_status > GAMESTATUS::GAME_START)
      && (hi.game().type() == GAMETYPE::POVERTY)
      && (hi.game().poverty_partner().no() == hi.no()) )
    value -= 2;

  if ( (   g.type() == GAMETYPE::NORMAL
        || g.type() == GAMETYPE::POVERTY )
      && hi.team() == TEAM::RE )
  {
    if( hi.hand().numberof( Card( Card::SPADE, Card::QUEEN ) ) == 2 )
      value += 2;
    if( hi.hand().numberof( Card( Card::HEART, Card::QUEEN ) ) == 2 )
      value += 1;
  }

#ifdef ESSENER_DEBUG
  if( !g.isvirtual() ) {
    unsigned own, opp;
    estimate_team_points(hi, own, opp);
    cout << "Handvalue: " << value
         << "  Points to win: " << hi.game().points_needed_to_win(hi.team())
         << " (" << own << ":" << opp << ")"
         << endl;
  }
#endif

  return value;
} // CalcHandValue(HeuristicInterface hi)



///////////////////////////////////////
// test for individual announcements //
///////////////////////////////////////

/**
 ** checks if an announcement should be made
 **
 ** @param      hi    heuristic interface
 **
 ** @return     returns true if an announcement should be made
 **
 ** @author     Borg Enders
 **
 ** @version    0.7.6
 **/
bool
make_announcement(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  make announcement  ?
  // @heuristic::idea announcement decision bases on Heuristics::CalcHandValue

  Game const& g = hi.game();
  int value=0;

  value= CalcHandValue(hi);

  value += calcPointsOfOwnTeam( hi, g ) / 25;


  // (trumps - no trumps) = trumps - (cards on hand - trumps ) = 2* trumps - cards on hand
  if (   ( 2*hi.hand().numberoftrumps() > hi.hand().cardsnumber() )  ) // ToDo: only for aggressive AI?
    value += (2*hi.hand().numberoftrumps() - hi.hand().cardsnumber() ) /5;

  if (   (hi.game().type() == GAMETYPE::MARRIAGE)
      && (hi.game().marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET)
      && ( hi.team() == TEAM::RE )
      && (hi.game().soloplayer().no() != hi.no() )
     )
     value += 6; // 2do should be 3*numberofqueens still in game

#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "RE/CO: " << hi.no() << "\t" << value << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
#endif

  if( (    GAMETYPE::is_solo( hi.game().type() )
       ||  (hi.game().marriage_selector() == MARRIAGE_SELECTOR::SILENT) )
     && !g.last_chance_to_announce(ANNOUNCEMENT::NO120, g.player(hi.no()) ) )
    return false;

  return ( value >=  (static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMIT))
                      + static_cast<int>(hi.value(Aiconfig::ANNOUNCECONFIG))));
} // bool make_announcement(HeuristicInterface hi)


/**
 ** checks if No90 should be said
 **
 ** @param      hi    heuristic interface
 **
 ** @return     true if hi should say No90
 **
 ** @author     Borg Enders
 **
 ** @version    0.7.6
 **/
bool
say_no90(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 90  ?
  // @heuristic::idea decision to make the annoncement no 90 depending on Heuristics::CalcHandValue
  Game const& g = hi.game();
  int value = 0;

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;

  value = CalcHandValue(hi)
    - max( 0, (int)g.tricks_remaining_no() - 8)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));




  if(    !g.hastrick(::opposite(hi.team())) )
    value += 2 * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));


  int own_p = calcPointsOfOwnTeam( hi, g );
  int opp_p = calcPointsOfOppositeTeam( hi, g );

  if( own_p > 150 )
    return true;


  opp_p+= 4 * g.tricks_remaining_no(); //previous: 5
  own_p+= 7 * g.tricks_remaining_no(); // previous: 6

  own_p += (int)(1.9 * hi.hand().numberoftrumps()
                 * hi.hand().numberofswines());
  own_p += (int)(1.4 * hi.hand().numberoftrumps()
                 * hi.hand().numberofhyperswines());


  if( GAMETYPE::is_solo( hi.game().type() ) )
  {
    own_p += 20 * hi.hand().numberoftrumps();
    opp_p += 20 * hi.cards_information().remaining_trumps_others();
  }

#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "NO90: " << hi.no() << "\t" << value << " + "
      << 2 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << 2 * (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
  cout << "\t" << own_p << "\t" << 1.5 * opp_p << std::endl;
#endif
  return (   (    value + 2 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC )
              >     (int)hi.value(Aiconfig::ANNOUNCELIMIT)
              + 2*(int)hi.value(Aiconfig::ANNOUNCECONFIG)
             )

          && own_p > 1.5 * opp_p  // previous: 2
          && opp_p < 90
          && (   opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO90,
                                           g.player(hi.no()))
             )
         );
} // bool say_no90(HeuristicInterface hi)


/**
 ** checks if No60 should be said
 **
 ** @param      hi    heuristic interface
 **
 ** @return     true if hi should say No60
 **
 ** @author     Borg Enders
 **
 ** @version    0.7.6
 **/
bool
say_no60(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 60  ?
  // @heuristic::idea decision to make the annoncement no 60 depending on Heuristics::CalcHandValue

  Game const& g = hi.game();
  int value = 0;

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;

  value = CalcHandValue(hi)
    - max( 0, (int)g.tricks_remaining_no() - 5)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  int own_p = calcPointsOfOwnTeam( hi, g );
  int opp_p = calcPointsOfOppositeTeam( hi, g );

  if( own_p > 180 )
    return true;


  opp_p+= 4 * g.tricks_remaining_no(); //previous: 5
  own_p+= 6 * g.tricks_remaining_no(); // previous: 5

  own_p += 3 * hi.hand().numberoftrumps() * hi.hand().numberofswines();
  own_p += 3 * hi.hand().numberoftrumps() * hi.hand().numberofhyperswines();

#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "NO60: " << hi.no() << "\t" << value << " + "
      << 4 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << 2 * (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
#endif
  return (   (  value + 4 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC)
              >   (int)hi.value(Aiconfig::ANNOUNCELIMIT)
              + 2 * (int)hi.value(Aiconfig::ANNOUNCECONFIG)
             )
          && own_p > 2 * opp_p  // previous: 3
          && opp_p < 60
          && (   opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO60,
                                           g.player(hi.no()))
             )
         );
} // bool say_no60( HeuristicInterface hi)


/**
 ** checks if No30 should be said
 **
 ** @param      hi    heuristic interface
 **
 ** @return     true if hi should say No30
 **
 ** @author     Borg Enders
 **
 ** @version    0.7.6
 **/
bool
say_no30( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 30  ?
  // @heuristic::idea decision to make the annoncement no 30 depending on Heuristics::CalcHandValue
  Game const& g = hi.game();
  int value = 0;

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;

  value = CalcHandValue(hi)
    - max( 0, (int)g.tricks_remaining_no() - 3)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  int own_p = calcPointsOfOwnTeam( hi, g );
  int opp_p = calcPointsOfOppositeTeam( hi, g );

  if( own_p > 210 )
    return true;


  opp_p+= 4 * g.tricks_remaining_no();
  own_p+= 5 * g.tricks_remaining_no();


  return (  (   value + 6 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC)
             >   (int)hi.value(Aiconfig::ANNOUNCELIMIT)
             + (int)hi.value(Aiconfig::ANNOUNCECONFIG)
            )
          && own_p > 7 * opp_p
          && opp_p < 30
          && (    opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO30,
                                           g.player(hi.no()))
             )
         );
} // bool say_no30( HeuristicInterface hi)


/**
 ** test whether a 'no 0' could be announced
 ** checks, that the other team does not have a trick
 ** and that the hand value is big enoug.
 **
 ** @param     hi      heuristic interface
 **
 ** @return    whether to announce 'no 0'
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
bool
say_no0(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 0  ?
  // @heuristic::idea decision to make the annoncement no 0 depending on Heuristics::CalcHandValue
  Game const& game = hi.game();

  if( game.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT )
    return false;

  // make sure, that the other team does not have a trick
  for (unsigned p = 0; p < game.playerno(); ++p)
    if (   (hi.teamofplayer(game.player(p)) != hi.team())
        && game.hastrick(game.player(p)))
      return false;

  // as long as the current trick is not closed, it has to be taken into account
  if (   game.trick_current().isfull()
      && (hi.teamofplayer(game.trick_current().winnerplayer())
          != hi.team()) )
    return false;

  // wait for the last possible moment
  if (!game.last_chance_to_announce(ANNOUNCEMENT::NO0,
                                    game.player(hi.no())))
    return false;

  // ToDo: check that no opposite player has the highest trump card
  // Test: force a player by 'ANNOUNCELIMITDEC' high

  int const value = CalcHandValue(hi)
    - max( 0, (int)game.tricks_remaining_no() )
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  return (value + 4 * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC))
          > static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMIT)
                             + 3 * hi.value(Aiconfig::ANNOUNCECONFIG))
         );
} // bool say_no0(HeuristicInterface hi)


/**
 ** checks if an announcement should be replied with an other announcement
 **
 ** @param     hi    heuristic interface
 **
 ** @return    Announcement::Reply if wanted
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Announcement
make_reply(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  make reply  ?
  // @heuristic::idea announcement reply decision bases on Heuristics::CalcHandValue

  Game const& g = hi.game();

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT
     && hi.hand().numberoftrumps() > hi.game().tricks_remaining_no() / 2 )
  {
    int value = 0;

    value = CalcHandValue(hi);

    for( int a = (int)ANNOUNCEMENT::NO90;
        a <= g.announcement_of_team( opposite( hi.team() ) ).announcement;
        a++ )
      value += (int)hi.value(Aiconfig::ANNOUNCECONFIGREPLY);


    for( unsigned i = 0; i < hi.hand().cardsnumber(); i++ )
    {
      if(   hi.hand().card(i).isdolle()
         || hi.hand().card(i).isswine()
         || hi.hand().card(i).possible_swine()
         || hi.hand().card(i).ishyperswine()
         || hi.hand().card(i).possible_hyperswine() )
        value += (int)hi.value(Aiconfig::ANNOUNCECONFIGREPLY);
    } // for (i < hi.hand().cardsnumber())

#ifdef ANNOUNCE_DEBUG
    if( !g.isvirtual() )
      cout << "REPLY: " << hi.no() << "\t" << value << "\t"
        << (int)hi.value(Aiconfig::ANNOUNCELIMITREPLY) << std::endl;
#endif
    if( value >=  (int)hi.value(Aiconfig::ANNOUNCELIMITREPLY) )
      return ANNOUNCEMENT::REPLY;
  } // if (g.announcement_of_team (opposite) != ANNOUNCEMENT::NOANNOUNCEMENT

  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement make_reply(HeuristicInterface const& hi)



//////////////////////////
// class HandEvaluation //
//////////////////////////


/**
 ** evaluates the hand of the given ai to determine if a reservation should be made
 **
 ** @param     ai            the ai to check
 ** @param     is_duty_solo  whether the player must play a duty solo
 **
 ** @result    the reservation of the ai
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Reservation
HandEvaluation::evaluate_reservation(Ai const& ai, bool const is_duty_solo)
{
  using namespace GAMETYPE;

  Reservation res = ai.reservation_get_default();
  Reservation best_res = Reservation();
  int solo_max  = 0;
  int solo_best = INT_MIN;

  // always announce swines/hyperswines
  // -- the game will reject it if the announcement is not valid
  best_res.swines
    = ai.game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN);
  best_res.hyperswines
    = ai.game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN);

  // check, whether to announce 'richness'
  if (res.game_type == THROWN_RICHNESS) {
    if (make_announcement(ai))
      res.game_type = NORMAL;
  } // if (res.game_type == GAMETYPE::THROWN_RICHNESS)

  // implementation_note:
  // * as long as GameType::LAST isn't INT_MAX, this is a safe int-to-enum cast,
  //   the worst thing that could happen is poor perfomance if GameType has huge gaps
  // * no assumption is made about the way the solo game types are organized within GameType
  DEBUG_ASSERTION((LAST < INT_MAX),
                  "HandEvaluation::reservation_get()\n"
                  "  infinite loop because GAMETYPE::GameType::LAST is INT_MAX");

  for (int i = FIRST; i <= LAST; ++i) {

    GameType solotype = static_cast<GameType>(i);
    int solovalue = -1;
    int configvalue = 0;

    switch (solotype) {
    case SOLO_MEATLESS:
      if (party.rule()(Rule::SOLO_MEATLESS)) {
        solovalue = checksolo(ai.hand());
        configvalue = ai.value(Aiconfig::MEATLESS);
      }
      break;
    case SOLO_JACK:
      if (party.rule()(Rule::SOLO_JACK)) {
        solovalue = checksolo(ai.hand(), Card::JACK, solotype);
        configvalue = ai.value(Aiconfig::SINGLESOLO);
      }
      break;
    case SOLO_QUEEN:
      if (party.rule()(Rule::SOLO_QUEEN)) {
        solovalue = checksolo(ai.hand(), Card::QUEEN, solotype);
        configvalue = ai.value(Aiconfig::SINGLESOLO);
      }
      break;
    case SOLO_KING:
      if (party.rule()(Rule::SOLO_KING)) {
        solovalue = checksolo(ai.hand(), Card::KING, solotype);
        configvalue = ai.value(Aiconfig::SINGLESOLO);
      }
      break;
    case SOLO_QUEEN_JACK:
      if (party.rule()(Rule::SOLO_QUEEN_JACK)) {
        solovalue = checksolo(ai.hand(), Card::QUEEN, Card::JACK, solotype);
        configvalue = ai.value(Aiconfig::DOUBLESOLO);
      }
      break;
    case SOLO_KING_JACK:
       if (party.rule()(Rule::SOLO_KING_JACK)) {
        solovalue = checksolo(ai.hand(), Card::KING, Card::JACK, solotype);
        configvalue = ai.value(Aiconfig::DOUBLESOLO);
      }
      break;
    case SOLO_KING_QUEEN:
       if (party.rule()(Rule::SOLO_KING_QUEEN)) {
        solovalue = checksolo(ai.hand(), Card::KING, Card::QUEEN, solotype);
        configvalue = ai.value(Aiconfig::DOUBLESOLO);
      }
      break;
    case SOLO_KOEHLER:
       if (party.rule()(Rule::SOLO_KOEHLER)) {
        solovalue = checksolo(ai.hand(), Card::KING, Card::QUEEN, Card::JACK, solotype);
        configvalue = ai.value(Aiconfig::TRIPLESOLO);
      }
      break;
    case SOLO_CLUB:
      if (party.rule()(Rule::SOLO_COLOR)) {
        solovalue = checksolo(ai.hand(), Card::CLUB, solotype);
        configvalue = ai.value(Aiconfig::COLORSOLO);
      }
      break;
    case SOLO_HEART:
      if (party.rule()(Rule::SOLO_COLOR)) {
        solovalue = checksolo(ai.hand(), Card::HEART, solotype);
        configvalue = ai.value(Aiconfig::COLORSOLO);
      }
      break;
    case SOLO_SPADE:
      if (party.rule()(Rule::SOLO_COLOR)) {
        solovalue = checksolo(ai.hand(), Card::SPADE, solotype);
        configvalue = ai.value(Aiconfig::COLORSOLO);
      }
      break;
    case SOLO_DIAMOND:
      if (party.rule()(Rule::SOLO_COLOR)) {
        solovalue = checksolo(ai.hand(), Card::DIAMOND, solotype);
        configvalue = ai.value(Aiconfig::COLORSOLO);
      }
      break;
    default:
      // nothing to do
      break;
    }

    if (solovalue >= 0) {
      solovalue -= configvalue;
      if ((solovalue > 0) && (solovalue > solo_max)) {
        solo_max = solovalue;
        solo_best = solovalue;
        res.game_type = solotype;
        best_res.game_type = solotype;
      } else if (solovalue >= solo_best) {
        solo_best = solovalue;
        best_res.game_type = solotype;
      }
    }
  } // for [game types]

  if (is_duty_solo)
    return best_res;

  res.marriage_selector = get_Marriage(ai.hand());

  // special case: with swines announced the player does not have a poverty
  if (   (res.game_type == GAMETYPE::POVERTY)
      && (ai.hand().numberofpovertycards()
          > ai.game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)) )
    res.swines = false;

  return res;
} //


/**
 ** evaluates the hand of the given ai to determine if an announcement should be made
 **
 ** @param      ai    the ai to check
 **
 ** @return     announcement of the ai
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.7.6
 **/
Announcement
HandEvaluation::evaluate_announcement(Ai const& ai)
{
  Announcement res = ANNOUNCEMENT::NOANNOUNCEMENT;

  if (make_announcement(ai))
    res = ANNOUNCEMENT::NO120;

  for (unsigned a = max(int(ANNOUNCEMENT::NO90),
                             ai.announcement() + 1);
       a < ANNOUNCEMENT::NUMBER;
       a++) {
    // make the announcement as late as possible
    if (   ai.game().announcement_valid(Announcement(a), ai)
        && (ai.game().rule().remaining_cards(Announcement(a))
            == ai.hand().cardsnumber())
        && (ai.game().player_current() == ai)
       ) {
      switch(Announcement(a)) {
      case ANNOUNCEMENT::NOANNOUNCEMENT:
      case ANNOUNCEMENT::REPLY:
      case ANNOUNCEMENT::NO120_REPLY:
      case ANNOUNCEMENT::NO90_REPLY:
      case ANNOUNCEMENT::NO60_REPLY:
      case ANNOUNCEMENT::NO30_REPLY:
      case ANNOUNCEMENT::NO0_REPLY:
      case ANNOUNCEMENT::NO120:
        DEBUG_ASSERTION(false,
                        "HandEvaluation::evaluate_announcement():\n"
                        "  wrong announcement in 'switch': "
                        << Announcement(a));
        break;
      case ANNOUNCEMENT::NO90:
        if (say_no90(ai))
          res = Announcement(a);
        break;
      case ANNOUNCEMENT::NO60:
        if (say_no60(ai))
          res = Announcement(a);
        break;
      case ANNOUNCEMENT::NO30:
        if (say_no30(ai))
          res = Announcement(a);
        break;
      case ANNOUNCEMENT::NO0:
        if (say_no0(ai))
          res = Announcement(a);
        break;
      } // switch(Announcement(a))
    } // if (last chance for announcement)
  }
  // for (a \in announcements)

  if ( ( res == ANNOUNCEMENT::NOANNOUNCEMENT )
      && ai.game().trick_current().isfull()
      && ai.game().announcement_valid(ANNOUNCEMENT::REPLY, ai) )
    res = make_reply(ai);

  return res;
} // Announcement HandEvaluation::evaluate_announcement(Ai const& ai)


/**
 ** evaluate the hand of the given ai to check if a poverty can be accepted
 **
 ** @param      cardno  the number of shifted cards
 **
 ** @return     whether to accept the cards
 **
 ** @version    0.7.6
 **
 ** @author     Borg Enders
 **
 **/
bool
HandEvaluation::evaluate_accept_poverty(Ai const& ai, unsigned const cardno)
{

  unsigned trumpno_possible = cardno;
  if (!ai.game().rule()(Rule::POVERTY_SHIFT_ONLY_TRUMP)) {
    if (!ai.game().rule()(Rule::THROW_WITH_ONE_TRUMP))
      trumpno_possible = 2;
    else
      trumpno_possible = 1;
  }

  int value = CalcHandValue(ai);

  if( ai.hand().numberoftrumps() >= 12 - cardno )
    value += ai.hand().numberof( Card::QUEEN );

  multimap<unsigned, Card::Color> color_nums;
  for (vector<Card::Color>::const_iterator
       c = ai.game().rule().card_colors().begin();
       c != ai.game().rule().card_colors().end();
       ++c) {
    color_nums.insert(make_pair(ai.hand().numberof(*c), *c));
  }

  for (multimap<unsigned, Card::Color>::const_iterator
       c = color_nums.begin();
       c != color_nums.end();
       ++c) {
    if (c->first > trumpno_possible)
      break;

    value += 2;
    trumpno_possible -= c->first;
  }

  if( ai.hand().numberoftrumps() >= 12 - cardno - 1 )
    value += (ai.hand().numberof( Card::QUEEN )
              + ai.hand().numberof( Card::JACK )
              + ai.hand().numberoftrumpaces());

  if ( ai.game().rule()(Rule::DOLLEN))
    value -= 2 - ai.hand().numberof(Card::HEART, Card::TEN);

  value -= 2 - ai.hand().numberofclubqueens();

  //COUT << no() << "\t" << value << "\t" << static_cast<signed>(ai.value(Aiconfig::TAKEPOVERTY)) << std::endl;

  return (value >= static_cast<signed>(ai.value(Aiconfig::TAKEPOVERTY)));
} // bool HandEvaluation::evaluate_accept_poverty(Ai const& ai, unsigned const cardno)


