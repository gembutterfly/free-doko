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

#include "heuristics.h"
#include "heuristics_utils.h"

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"
#include "../../card/card_signals.h"

namespace HEURISTICS_UTILS {


/**
 ** best pfund on hand: a trump Ass, color ten or Ace, trump ten, a king,
 ** a notrump queen or jack, nine
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
choose_pfund_card( Trick const& trick, HeuristicInterface const& hi )
{
  // @heuristic::name   ?  choose pfund card  ?
  // @heuristic::idea   find a pfund card

  Trick t = trick;


  unsigned i;

  bool allmyteam = checkAllMyTeam( t, hi );
  if (t.startcard().istrump()) {
    allmyteam = allmyteam && ownTeamCanWinTrick(t, hi);
  } else { // if !(t.startcard().istrump())
    allmyteam = allmyteam && hi.jabbedbyownteam( t.startcard().color() );
  } // if !(t.startcard().istrump())

  allmyteam =    allmyteam
    || (   hi.game().type() == GAMETYPE::POVERTY
        && hi.no() == hi.game().soloplayer().no()
        && trick.cardno_of_player(hi.game().poverty_partner()) == 3 );

  // marriage determination for the bride
  if (   (hi.game().type() == GAMETYPE::MARRIAGE)
      && is_selector(t.startcard().tcolor(),
                     hi.game().marriage_selector())
      && (hi.no() == hi.game().soloplayer().no()) )
    allmyteam = true;

  unsigned trumps = hi.hand().numberoftrumps();
  Card c;

  HandCards const ha = hi.hand().validcards( t );

  bool solo_check =
    (   GAMETYPE::is_solo( hi.game().type() )
     && hi.color_runs(t.startcard().color()) == 0
     && !t.startcard().istrump()
     && t.winnerplayer() == hi.game().soloplayer()
     && t.winnercard().less( hi.trump_card_limit() )
     && !t.islastcard() ) ;

  if( !(   hi.teamofplayer( t.winnerplayer() )== hi.team()
        && t.islastcard()
        && t.cardno_of_player( t.winnerplayer() ) == 0
        && !t.winnercard().istrump() ) ) //only use trump ace when partner is not winning alredy in first position
  {
    // then find first trump ace
    for( i = 0; i < ha.cardsnumber(); i++ ) {
      if(   ha.card(i).value() == Card::ACE
         && ha.card(i).istrump()
         && (   t.islastcard()
             || allmyteam
             || trumps < 3
             || (   hi.color_runs(t.startcard().color()) == 0
                 && !t.startcard().istrump()
                )
             || (   ! hi.cards_information().higher_card_exists( t.winnercard())
                 //!t.winnercard().less( Card( Card::CLUB, Card::QUEEN ) )
                 && hi.teamofplayer( t.winnerplayer() )== hi.team()
                )
             || (    hi.game().type() == GAMETYPE::MARRIAGE
                 && hi.game().soloplayer().no() == hi.no()
                 && hi.game().marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET
                 && hi.game().marriage_selector() == MARRIAGE_SELECTOR::FIRST_FOREIGN
                )
            )
         && !ha.card(i).possible_swine()
        )
      {
        Trick temptrick = t;
        temptrick += ha.card( i );
        if(   (   (hi.teamofplayer( temptrick.winnerplayer() )== hi.team())
               && !(   hi.game().rule()(Rule::GENSCHER)
                    && (hi.game().type() == GAMETYPE::NORMAL)
                    && !hi.cards_information().played(Card(Card::DIAMOND,
                                                           Card::KING))
                    && (trumps >= 6) // *Value*
                   )
              )
           || (    hi.game().type() == GAMETYPE::MARRIAGE
               && hi.game().soloplayer().no() == hi.no()
               && hi.game().marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET
               // ToDo: other marriage selectors
               && (hi.game().marriage_selector()
                   == MARRIAGE_SELECTOR::FIRST_FOREIGN )
              )
          )
        {
          return ha.card(i);
        }
      }
    } // for (i < ha.cardsnumber)
  } // if partner is start player and with none trump card.


  // see if we can use standard pfund priorities
  bool use_standard_priorities = !hi.game().undetermined_marriage()
                                 && (t.startcard().istrump() || !ha.existstcolor(t.startcard().color()));
  if (use_standard_priorities) {
    switch (hi.game().type()) {
    case GAMETYPE::SOLO_JACK:
    case GAMETYPE::SOLO_QUEEN:
    case GAMETYPE::SOLO_KING:
      use_standard_priorities = true;
      break;
    case GAMETYPE::SOLO_KOEHLER:
    case GAMETYPE::SOLO_MEATLESS:
      use_standard_priorities = false;
      break;
    default:
      use_standard_priorities = hi.game().rule()(Rule::WITH_NINES);
      break;
    }
  }

  if (use_standard_priorities) {

    // calc trump pfund


    HandCard const& trump_pfund =
      HandCard(hi.hand(), most_precious_trump(hi, t, PT_SPARE_FOX | ((trumps > hi.hand().cardsnumber() / 2) ? PT_SPARE_CHARLIE : PT_NO_FLAGS)));

    if (!ha.hascolor())
      return trump_pfund;

    bool dont_trump =    !trump_pfund.is_empty()
                      && t.islastcard()
                      && t.is_jabbed(trump_pfund)
                      && GAMETYPE::is_solo(hi.game().type())
                      && (hi.game().player_previous(hi.game().player(hi.no())) == hi.game().soloplayer());

    Card first_run;
    Card second_run_no_void;
    Card highest_card_no_void;
    Card second_run;
    Card highest_card;
    Card single_ace;

    // cf. calc_plain_suit_pfunds for a description of the loser categories
    calc_plain_suit_pfunds(
      hi, false, false,
      first_run,
      second_run_no_void, highest_card_no_void,
      second_run, highest_card,
      single_ace);

    // chance to ruff a first run is always worth it
    if (!first_run.is_empty())
      return first_run;

    // play a plain suit pfund if it's at least worth 10 points
    if (!second_run_no_void.is_empty() && (second_run_no_void.value() >= Card::TEN))
      return second_run_no_void;
    if (!highest_card_no_void.is_empty() && (highest_card_no_void.value() >= Card::TEN))
      return highest_card_no_void;

    // play a trump ten
    if ( !dont_trump
         && !trump_pfund.is_empty()
         && (trump_pfund.value() == Card::TEN)
         && !trump_pfund.isdolle()
       )
      return trump_pfund;

    // play a plain suit pfund if it's at least worth 4 points
    if (!second_run_no_void.is_empty() && (second_run_no_void.value() >= Card::KING))
      return second_run_no_void;
    if (!highest_card_no_void.is_empty() && (highest_card_no_void.value() >= Card::KING))
      return highest_card_no_void;
    if (!second_run.is_empty() && (second_run.value() >= Card::KING))
      return second_run;
    if (!highest_card.is_empty() && (highest_card.value() >= Card::KING))
      return highest_card;

    // play a trump king
    if ( !dont_trump
         && !trump_pfund.is_empty()
         && (trump_pfund.value() == Card::KING)
         && !trump_pfund.ishyperswine()
         && (hi.game().type() != GAMETYPE::SOLO_KING)
         && (hi.game().type() != GAMETYPE::SOLO_KING_QUEEN)
         && (hi.game().type() != GAMETYPE::SOLO_KING_JACK)
       )
      return trump_pfund;

    // play a plain suit nine
    if (!second_run_no_void.is_empty())
      return second_run_no_void;
    if (!highest_card_no_void.is_empty())
      return highest_card_no_void;
    if (!second_run.is_empty())
      return second_run;
    if (!highest_card.is_empty())
      return highest_card;

    // we only have singleton aces from playable suits on our hand, we may as well use one as pfund
    if (!single_ace.is_empty())
      return single_ace;

    // finally, return trump (we don't have anything else to play)
    return trump_pfund;
  }

  Card best_ace;
  // choose an ace of a color if the player has two aces
  // or if there are not enough remaining color cards
  for( vector<Card::Color>::const_iterator
      color = hi.game().rule().card_colors().begin();
      color != hi.game().rule().card_colors().end();
      ++color )
  {
    Card const local_ace( *color, Card::ACE );
    if(   (   hi.hand().numberof( local_ace )
           == hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS)
           || (    hi.hand().numberof(local_ace) == 1
               && (  (   (hi.cards_information().remaining_others(*color)
                          < hi.game().playerno() - 1)
                      && (hi.game().type() != GAMETYPE::SOLO_MEATLESS))
                   || (hi.hand().numberof(*color) == 1)
                  )
              ) )
       && !local_ace.istrump( hi.game() )
       && t.isvalid(local_ace, hi.hand())
       && (   hi.teamofplayer(t.winnerplayer()) == hi.team()
           || solo_check
           || allmyteam
          )
      )
    {
      if(   best_ace.is_empty()
         || (hi.cards_information().remaining_others(*color)
             > hi.cards_information().remaining_others(best_ace.color()))
        )
        best_ace = local_ace;
    }
  } // for (color \in card_colors)

  if(!best_ace.is_empty())
  {
    return best_ace;
  }


  // or a ten of color
  for( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::TEN
       && !ha.card(i).istrump()
       && (   hi.teamofplayer(t.winnerplayer()) == hi.team()
           || solo_check
           || allmyteam
          )
      )
    {
      return ha.card(i);
    }


  bool to_fat_solo =    GAMETYPE::is_solo( hi.game().type() )
    && t.points() > hi.value( Aiconfig::LIMITQUEEN )
    && t.player_of_card( 3 ) == hi.game().soloplayer();
  // or a Ace of color
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    int cardsInGame
      = hi.cards_information().remaining_others(ha.card(i).color());

    if(   (ha.card(i) == Card(t.startcard().color(), Card::ACE))
       && (hi.color_runs(ha.card(i).color()) == 0)
       && (hi.hand().numberof(t.startcolor()) >= 3)
       && !ha.card(i).istrump()
       /// @todo   see 'choose pfund', check that enough cards still remain
       && (   cardsInGame >= 2
           || ( hi.game().type() == GAMETYPE::POVERTY && hi.no() == hi.game().soloplayer().no() )
          )
       && (   hi.teamofplayer(t.winnerplayer()) == hi.team()
           || solo_check
           || allmyteam
          )
      )
      return ha.card(i);

    if(   ha.card(i).value()==Card::ACE
       && (  hi.color_runs(ha.card(i).color()) > 0
           || (   t.winnercard().istrump()
               && allmyteam )
          )
       && cardsInGame >= 2
       && !ha.card(i).istrump()
       && t.isvalid( ha.card(i),hi.hand() )
       && !to_fat_solo
       && (   (   hi.game().type()!=GAMETYPE::SOLO_JACK // with not much trumps
               && hi.game().type()!=GAMETYPE::SOLO_QUEEN // it is good to
               && hi.game().type()!=GAMETYPE::SOLO_KING  // keep a second ace
              )
           || hi.color_runs(t.startcard().color()) > 0
          )
      )
    {
      return ha.card(i);
    }
  } // for ( i < ha.cardsnumber() )


  // or a ten of trump
  for( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::TEN
       && ha.card(i).istrump()
       && !ha.card(i).isdolle()
      )
    {
      return ha.card(i);
    }


  // or a King of color
  for ( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::KING
       && !ha.card(i).istrump()
      )
    {
      return ha.card(i);
    }


  // or a king of trump
  for( i = 0; i < ha.cardsnumber(); i++ )
    if(    ha.card(i).value() == Card::KING
       && !ha.card(i).ishyperswine()
       && ha.card(i).istrump()
       && hi.game().type() != GAMETYPE::SOLO_KING
       && hi.game().type() != GAMETYPE::SOLO_KING_QUEEN
       && hi.game().type() != GAMETYPE::SOLO_KING_JACK
       && hi.game().type() != GAMETYPE::SOLO_KOEHLER
      )
    {
      return ha.card(i);
    }


  // or Queen if no trump
  for ( i = 0; i < ha.cardsnumber(); i++ )
    if(    ha.card(i).value()==Card::QUEEN
       && !ha.card(i).istrump()
       && hi.game().type() != GAMETYPE::SOLO_QUEEN
       && hi.game().type() != GAMETYPE::SOLO_KING_QUEEN
       && hi.game().type() != GAMETYPE::SOLO_QUEEN_JACK
       && hi.game().type() != GAMETYPE::SOLO_KOEHLER
      )
    {
      return ha.card(i);
    }


  // or Jack if no trump
  for( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::JACK
       && !ha.card(i).istrump()
       && hi.game().type() != GAMETYPE::SOLO_JACK
       && hi.game().type() != GAMETYPE::SOLO_QUEEN_JACK
       && hi.game().type() != GAMETYPE::SOLO_KING_JACK
       && hi.game().type() != GAMETYPE::SOLO_KOEHLER
      )
    {
      return ha.card(i);
    }


  // or a nine of color
  for( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::NINE
       && !ha.card(i).istrump()
      )
    {
      return ha.card(i);
    }


  // or a nine of trump
  for ( i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::NINE
       && ha.card(i).istrump()
       && !ha.card(i).ishyperswine()
      )
    {
      return ha.card(i);
    }


  return c;
} // Card choose_pfund_card( Trick trick, HeuristicInterface hi )


/**
 ** returns the highest card of the suit color in hand hi.hand() which will lose a trick
 ** when played as first card (this can be an ace if the suit is going to be ruffed)
 **
 ** @param     hi      heuristic interface
 ** @param     color   the suit
 **
 ** @return    highest loser from the suit
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
highest_loser(HeuristicInterface const& hi, Card::Color const& color)
{
  if (hi.cards_information().remaining_others(color) < hi.game().playerno() - 1)
    return hi.hand().highest_card(color);

  Card const& highest = hi.cards_information().highest_remaining_card_of_others(color);
  Card loser = Card::EMPTY;

  for (unsigned i = 0; i < hi.hand().cardsnumber(); ++i) {
    HandCard const& card = hi.hand().card(i);
    if (card.tcolor() != color)
      continue;
    if (hi.game().less(card, highest) && hi.game().less(loser, card))
      loser = card;
  }

  return loser;
} // Card highest_loser(...)





string
card_name(Card const& card)
{
  if (card.is_empty())
    return "EMPTY";
  else
    return name(card);
}

/**
 ** calculates the plain suit pfund cards in hand devided into categories of usefulness
 ** in the following "playable" means that a suit may be played without being ruffed by the opposite team
 **
 ** @param  hi                    heuristic interface
 ** @param  to_lead               make shure that the highest card from a playable suit is always lower than the other players' highest card to guarantee losing the trick when leading
 ** @param  below_ten             return only cards below ten (e.g. for doppelkopf prevention)
 ** @param  first_run             highest singleton from the first run suit with most cards remaining, sparing aces
 ** @param  second_run_no_void    highest card that creates a chance to ruff from the suit with most cards remaining, sparing single aces and suits in which a partner is void
 ** @param  highest_card_no_void  highest card from any suit in which a partner isn't void, suits with more remaining cards come first, single aces will be spared if the suit is playable or has a ten
 ** @param  second_run            highest card that creates a chance to ruff from the suit with most cards remaining, sparing single aces
 ** @param  highest_card          highest card from any suit, suits with more remaining cards come first, single aces will be spared if the suit is playable or has a ten
 ** @param  single_ace            singleton ace from the playable suit with the least cards remaining
 **
 ** @return    (void)
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
void
calc_plain_suit_pfunds( HeuristicInterface const& hi,
                        bool to_lead,
                        bool below_ten,
                        Card& first_run,
                        Card& second_run_no_void,
                        Card& highest_card_no_void,
                        Card& second_run,
                        Card& highest_card,
                        Card& single_ace
                       )
{
  // This analysis is most useful for game types/rules for which ruffing has high priority. It's much less
  // useful for meatless and koehler solos or when playing without nines (except for single picture solos).
  // We leave it to the calling functions to ensure that the analysis will only be used when it makes sense.

  Game const& game = hi.game();

  Player const* partner = hi.team_information().guessed_partner();
  vector<bool> hastrump(game.playerno(), false);
  bool other_players_out_of_trump = false;

  if (!game.undetermined_marriage()) {
    other_players_out_of_trump = true;
    for (unsigned p = 0; p < game.playerno(); ++p) {
      if (p != hi.no()) {
        hastrump[p] = hi.handofplayer(p).hastrump();
        if (hastrump[p])
          other_players_out_of_trump = false;
      }
    }
  }

  // the infos we need for each suit
  struct SuitInfo {
    unsigned len;                  // suit length
    unsigned aces;                 // number of aces
    Card::Value highest_below_ace; // highest card below ace (exists if len > aces)
    unsigned remaining;            // remaining cards of the other players
    Card::Value highest_others;    // highest card of other players (if to_lead is true)
  };
  SuitInfo suit_info[] = { {0}, {0}, {0}, {0} };
  for (unsigned i = 0; i < 4; ++i) {
    suit_info[i].highest_below_ace = Card::NOCARDVALUE;
    suit_info[i].highest_others    = Card::NOCARDVALUE;
  }

  // get the other players' highest cards
  // since this is only useful in a marriage selector trick, we treat all other players as opponents
  if (to_lead || below_ten) {
    for (vector<Card::Color>::const_iterator
         c = game.rule().card_colors().begin();
         c != game.rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      if (to_lead)
        suit_info[color].highest_others = hi.cards_information().highest_remaining_card_of_others(color).value();
      if (   below_ten
          && (   (suit_info[color].highest_others == Card::NOCARDVALUE)
              || (suit_info[color].highest_others > Card::TEN)
             )
         )
        suit_info[color].highest_others = Card::TEN;
    }
  }

  // we have to iterate the hand to find the highest card below ace (or below the other
  // players' highest card if we're leading the trick), we can as well calc the length
  // and the number of aces, too (to avoid using slower numberof methods)
  for (unsigned i = 0; i < hi.hand().cardsnumber(); ++i) {
    HandCard const& card = hi.hand().card(i);
    if (card.istrump()) {
      hastrump[hi.no()] = true;
      continue;
    }
    Card::Value const value = card.value();
    SuitInfo &suit = suit_info[card.color()];
    suit.len++;
    if (value == Card::ACE)
      suit.aces++;
    else if ((suit.highest_below_ace == Card::NOCARDVALUE) || (suit.highest_below_ace < value))
      if ((suit.highest_others == Card::NOCARDVALUE) || (value < suit.highest_others))
        suit.highest_below_ace = value;
  }

  first_run            = Card::EMPTY;
  second_run_no_void   = Card::EMPTY;
  highest_card_no_void = Card::EMPTY;
  second_run           = Card::EMPTY;
  highest_card         = Card::EMPTY;
  single_ace           = Card::EMPTY;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    Card::Color const& color = *c;
    SuitInfo &suit = suit_info[color];
    if (!suit.len) // this will also skip trump
      continue;

    bool partner_is_void = false;
    if (!other_players_out_of_trump) {
      if (!GAMETYPE::is_solo(game.type())) {
        // if our partner is out of a suit, throwing it becomes a lower priority, unless
        // an opponent not sitting directly in front of our partner can also ruff
        partner_is_void = partner && hi.cards_information().of_player(*partner).does_not_have(color);
        if (partner_is_void) {
          for (unsigned p = 0; p < game.playerno(); ++p) {
            if ((p == hi.no()) || (p == partner->no()) || (p + 1 == partner->no()) || !hastrump[p])
              continue;
            if (hi.cards_information().of_player(p).does_not_have(color)) {
              partner_is_void = false;
              break;
            }
          }
        }
      } else if (hi.team() != TEAM::RE) {
        // we're a contra player in a solo, a suit gets a lower priority if one of our partners is
        // void while the soloplayer may still have the suit or the partner sitting right behind
        // the soloplayer is out of the suit
        unsigned soloplayer = game.soloplayer().no();
        for (unsigned i = 1; i <= game.playerno(); ++i) {
          unsigned p = (soloplayer + i) % game.playerno();
          if ((p != hi.no()) && hastrump[p]) {
            if (hi.cards_information().of_player(p).does_not_have(color)) {
              partner_is_void = (p != soloplayer);
              if (p = soloplayer + 1)
                break;
            }
          }
        }
      }
    } // if (!other_players_out_of_trump)

    // faster than remaining_others()
    suit.remaining = hi.cards_information().remaining(color) - suit.len;
    unsigned runs = hi.color_runs(color);
    bool playable = suit.remaining >= (game.playerno() - 1 - partner_is_void) * (runs + 1);


    // the highest usable card from the suit
    Card const& highest =
      Card(color, (   !below_ten
                   && suit.aces
                   && ((!playable && (suit.highest_below_ace < Card::TEN)) || (suit.aces == 2))
                  )
                  ? Card::ACE
                  : (suit.len > suit.aces)
                    ? suit.highest_below_ace
                    : Card::NOCARDVALUE
          );

    // highest priority: suits with the possibility to create a chance to ruff
    if (playable && hastrump[hi.no()]&& !highest.is_empty() && (suit.len < 3)) {
      if ((suit.len == 1) && !runs) {
        // first run ruff
        if ( first_run.is_empty()
             || (suit.remaining > suit_info[first_run.color()].remaining)
             || (   (suit.remaining == suit_info[first_run.color()].remaining)
                 && (first_run.value() < highest.value())
                )
           )
          first_run = highest;
      } else if (!partner_is_void) {
        // second run ruff, sparing a partner's void suit
        if ( second_run_no_void.is_empty()
             || (suit.remaining > suit_info[second_run_no_void.color()].remaining)
             || (   (suit.remaining == suit_info[second_run_no_void.color()].remaining)
                 && (second_run_no_void.value() < highest.value())
                )
           )
          second_run_no_void = highest;
      } else {
        // second run ruff
        if ( second_run.is_empty()
             || (suit.remaining > suit_info[second_run.color()].remaining)
             || (   (suit.remaining == suit_info[second_run.color()].remaining)
                 && (second_run.value() < highest.value())
                )
           )
          second_run = highest;
      }
    } else if (!highest.is_empty()) {
      if (!partner_is_void) {
        // highest card, sparing a partner's void suit
        if ( highest_card_no_void.is_empty()
             || (highest_card_no_void.value() < highest.value())
             || ((highest_card_no_void.value() == highest.value())
                && (suit.remaining > suit_info[highest_card_no_void.color()].remaining))
           )
          highest_card_no_void = highest;
      } else {
        // highest card
        if ( highest_card.is_empty()
             || (highest_card.value() < highest.value())
             || ((highest_card.value() == highest.value())
                && (suit.remaining > suit_info[highest_card.color()].remaining))
           )
          highest_card = highest;
      }
    } else if (suit.aces && !below_ten) {
      // singleton aces
      if (single_ace.is_empty() || (suit.remaining < suit_info[single_ace.color()].remaining))
        single_ace = Card(color, Card::ACE);
    }

  } // for (vector<Card::Color>::const_iterator


  if (!hi.game().isvirtual())
    cout << "*FR: " << card_name(first_run)
         << " SV: " << card_name(second_run_no_void)
         << " HV: " << card_name(highest_card_no_void)
         << " SR: " << card_name(second_run)
         << " HC: " << card_name(highest_card)
         << " SA: " << card_name(single_ace)
         << endl;

  return;
} // Card calc_plain_suit_pfunds(...)


/**
 ** returns the smartest (see below) plain suit loser in hand hi.hand()
 **
 ** @param     hi         heuristic interface
 ** @param     to_lead    we're looking for a card to lose a trick we're leading
 ** @param     ruff_only  return a card only if we can create a chance to ruff with it
 **
 ** @return    "smartest" plain suit loser
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
smartest_plain_suit_pfund(HeuristicInterface const& hi, bool to_lead, bool below_ten, bool ruff_only)
{
  // in the following "playable" means that a suit may be played without being ruffed

  // loser card categories, ordered by priority ("smartest" loser first):
  // * highest singleton from the first run suit with most cards remaining, sparing aces
  // * highest card that creates a chance to ruff from the suit with most cards remaining,
  //   sparing single aces and suits in which a partner is void
  // * highest card from any suit in which a partner isn't void, suits with more remaining
  //   cards come first, single aces will be spared if the suit is playable or has a ten
  // * highest card that creates a chance to ruff from the suit with most cards remaining,
  //   sparing single aces
  // * highest card from any suit, suits with more remaining cards come first, single aces
  //   will be spared if the suit is playable or has a ten
  // * singleton ace from the playable suit with the leat cards remaining

  Card first_run;
  Card second_run_no_void;
  Card highest_card_no_void;
  Card second_run;
  Card highest_card;
  Card single_ace;

  calc_plain_suit_pfunds(
    hi, to_lead, below_ten,
    first_run,
    second_run_no_void, highest_card_no_void,
    second_run,
    highest_card, single_ace);

  // return smartest loser based on the category's priority

  // highest singleton loser in a first run suit
  if (!first_run.is_empty())
    return first_run;

  // highest loser in a suit with a chance to ruff the second run
  if (!second_run_no_void.is_empty())
    return second_run_no_void;

  // highest loser in any other suit
  if (!ruff_only && !highest_card_no_void.is_empty())
    return highest_card_no_void;

  // highest loser in a suit with a chance to ruff the second run
  if (!second_run.is_empty())
    return second_run;

  // we cannot creat a chance to ruff
  if (ruff_only)
    return Card::EMPTY;

  // highest loser in any other suit
  if (!ruff_only && !highest_card.is_empty())
    return highest_card;

  // last resort: a singleton ace
  return single_ace;
} // Card smartest_plain_suit_pfund(...)


/**
 ** returns the most precious trump of the cards in hand, that is, in descending order:
 ** ace - ten - king - jack - 9 - queen - dolle (- swine - hyperswine)
 **
 ** a single charlie is spared by sorting it in as follows:
 ** - jack(diamond-heart-spade) - 9 - queen(diamond-heart) - charlie - queen(spade-club) -
 **
 ** also works for all kinds of solos, except for meatless (d'oh)
 **
 ** does optimize the points of the higher trumps, e.g. if the lowest queen isn't a better
 ** card than the lowest jack, the queen will be returned (see is_better_card for details)
 **
 ** @param     hand     the hand
 ** @param     t        the current trick
 ** @param     flags    test criteria, see PreciousTrumpFlags
 **
 ** @return    most precious trump according to the test criteria, Card::EMPTY if there's no match
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
most_precious_trump(HeuristicInterface const& hi, Trick const& t, unsigned flags)
{

  // implementation note: this could be implemented using a vector which has the trumps in
  // the order we need. But we had to use contains() in this case and as long as contains()
  // has to iterate the cards of the hand, the following solution is much faster, i.e.
  // O(n) vs. O(t*n) with n being the number of cards in the hand and t the number of trumps
  // in the game (t*n is the worst case scenario, when the only trump in hand is a dolle)

  Hand const& hand = hi.hand();

  // the card to discriminate between high and low trumps, usually the trump ace
  HandCard trumpace = (hand.game().trumpcolor() != Card::NOCARDCOLOR)
                      ? HandCard(hand, hand.game().trumpcolor(), Card::ACE)
                      : HandCard::EMPTY;
  if (trumpace.isswine()) {
    trumpace = HandCard(hand, hand.game().trumpcolor(), Card::TEN);
    if (trumpace.ishyperswine() || trumpace.isdolle())
      trumpace = HandCard(hand, hand.game().trumpcolor(), Card::KING);
  }

  // lowest trump above trump ace
  HandCard lowest_high(hand, Card::EMPTY);
  // highest trump below diamond jack
  HandCard highest_low(hand, Card::EMPTY);
  // highest trump we're allowed to return
  HandCard highest(hand, Card::EMPTY);
  // maximum points of the higher trumps (used for optimization)
  unsigned maxpoints = 0;

  // sparing a single charlie only makes sense if the extrapoint is possible
  bool spare_charlie =    (flags & PT_SPARE_CHARLIE)
                       && !GAMETYPE::is_solo(hand.game().type())
                       && hand.game().rule()(Rule::EXTRAPOINT_CHARLIE);
  bool has_charlie = false;

  // force losing implies playing to lose and overrides playing to win
  if (flags & PT_FORCE_LOSING)
    flags = (flags | PT_PLAY_TO_LOSE) & ~PT_PLAY_TO_WIN;

  // we can either play to win or to lose, not both
  bool perform_tests = flags & (PT_PLAY_TO_WIN ^ PT_PLAY_TO_LOSE);

  // find the most precious trump in the hand
  for( unsigned i = 0; i < hand.cardsnumber(); i++ ) {
    HandCard const& card = hand.card(i);
    if (   !card.istrump()
        || ((flags & PT_SPARE_FOX) && card.isfox())
       )
      continue;
    if (perform_tests) {
      if ((flags & PT_PLAY_TO_WIN) && !t.is_jabbed(card))
        continue;
      if (    (flags & PT_PLAY_TO_LOSE)
           && (t.islastcard() || (flags & PT_FORCE_LOSING))
           && (t.winnercard().istrump() && t.winnercard().less(card))
         )
        continue;
    }

    if (   highest.is_empty()
        || highest.less(card)
       )
      highest = card;

    if (spare_charlie && (card == Card::CHARLIE)) {
      spare_charlie = false;
      has_charlie = true;
      continue;
    }

    if (trumpace.less(card)) {
      // jack and higher
      if (   lowest_high.is_empty()
          || card.less(lowest_high)
         )
        lowest_high = card;
      if (card.points() > maxpoints)
        maxpoints = card.points();
    } else {
      // ace and lower
      if (   highest_low.less(card)
          && (!(flags & PT_SPARE_TEN) || (card.value() < Card::TEN))
         )
        highest_low = card;
    }

    // a shortcut - there's nothing more precious than the trump ace
    if (highest_low == trumpace)
      return highest_low;
  }

  // we will now analyse what we've found, sort in a single charlie and
  // try to optimize points (low trumps are already optimized)

  if (   !highest_low.is_empty()
      && (highest_low.points() >= maxpoints)
     )
    // we've already found the optimal card
    return highest_low;

  if (lowest_high.is_empty())
    // in this case, maxpoints is 0, so if there was a highest_low, it has already been returned
    if (has_charlie)
      return Card::CHARLIE;
    else
      return Card::EMPTY;

  // use a jack instead of a nine
  if (   (highest_low.value() == Card::NINE)
      && (lowest_high.value() == Card::JACK)
     )
    highest_low = Card::EMPTY;

  // sort in a spared charlie
  if (   has_charlie
      && hi.value(Aiconfig::CHARLIELIMIT_HIGH).less(lowest_high)
     )
    highest_low = Card::CHARLIE;

  // this should be much faster than using numberof on each card individually
  map<Card, unsigned> counted = hand.counted_cards();

  if (!highest_low.is_empty()) {

     // check if we can try out the lowest_high instead of the highest_low

     vector<Card> betweencards = hand.game().cards_between(highest_low, lowest_high);
     // we also have to check the lower bound
     betweencards.push_front(highest_low);
     for (vector<Card>::const_iterator c = betweencards.begin();
          c != betweencards.end();
          ++c)
       if (hi.cards_information().remaining(*c) - counted[*c])
         // a card that's higher than highest_low but lower then lowest_high exists,
         // no optimization possible, because we may lose a following trick
         return highest_low;

  }

  // now optimize the points of the lowest high trump

  vector<Card> highercards = hand.game().higher_cards(lowest_high);
  // we also have to check the lower bound
  highercards.push_front(highest_low);
  for (vector<Card>::const_iterator c = highercards.begin();
       c != highercards.end();
       ++c)
  {
    // if another player still has this card, no further optimization
    if (   highest.less(*c)
        || (hi.cards_information().remaining(*c) - counted[*c])
       )
      break;

    // use a higher trump if it has a higher value
    if (   counted[*c]
        && (c->value() > lowest_high.value())
       )
      lowest_high = *c;
  }

 if (   !highest_low.is_empty()
      && (highest_low.value() >= lowest_high.value())
     )
    // we didn't find a better card
    return highest_low;
 else
    return lowest_high;

} // Card most_precious_trump(...)


/**
 ** returns the most precious trump which wins the trick so far
 **
 ** @param     hand     the hand
 ** @param     t        the current trick
 ** @param     flags    see PreciousTrumpFlags, PT_PLAY_TO_WIN will be set automatically
 **
 ** @return    most precious trump winner or Card::EMPTY if no winner exists
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
most_precious_trump_winner(HeuristicInterface const& hi, Trick const& t, unsigned flags) // = PT_SPARE_CHARLIE
{
  flags = (flags | PT_PLAY_TO_WIN) & ~PT_PLAY_TO_LOSE;
  return most_precious_trump(hi, t, flags);
} // Card most_precious_trump_winner(...)


/**
 ** returns the most precious trump loser of the cards in hand
 **
 ** @param     hand     the hand
 ** @param     t        the current trick
 ** @param     flags    see PreciousTrumpFlags, PT_PLAY_TO_LOSE will be set automatically
 **
 ** @return    most precious trump loser or most precious trump if no loser or Card::EMPTY if no trump
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
most_precious_trump_loser(HeuristicInterface const& hi, Trick const& t, unsigned flags) // = PT_SPARE_CHARLIE
{
  flags = (flags | PT_PLAY_TO_LOSE) & ~PT_PLAY_TO_WIN;
  Card const& precious = most_precious_trump(hi, t, flags);
  if (precious.is_empty())
    return most_precious_trump(hi, t, (flags & ~PT_PLAY_TO_LOSE));
  else
    return precious;
} // Card most_precious_trump_loser(...)


} // namespace HEURISTICS_UTILS

