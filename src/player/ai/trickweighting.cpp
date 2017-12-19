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

#include "trickweighting.h"

#include "VirtualGamesInterface.h"
#include "ai.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../game/exception.h"
#include "../../card/trick.h"
#include "../../misc/setting.h"
#include "../../ui/ui.h"

#ifdef BENDERS
#ifndef RELEASE
// whether to print some weighting informations
#define PLAYERCHECK 10

  // Playernumber for which to produce output
  unsigned playerCheck;
#endif
#endif


bool 
oppositeTeamCanWinTrick( Trick const &t, VirtualGamesInterface const& hi )
{
  bool trickLost = (t.winnerplayer().team() != hi.team());

  // take a look if all players coming in this trick of other team can still win the trick
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ )
    if( maybe_to_team(hi.teamofplayer(t.player_of_card( i ))) != hi.team() )
    {
      trickLost = (trickLost
                   || hi.handofplayer( 
                                      t.player_of_card( i ) 
                                     ).higher_card_exists( t.winnercard() ));
    } // if( maybe_to_team(hi.teamofplayer(t.player_of_card(i))) != hi.team() )
  return trickLost;
} // bool oppositeTeamCanWinTrick ( Trick t, HeuristicInterface hi )

int TrickWeighting::modi_dolle_swines(VirtualGamesInterface const& vgi,
                              Trick const& trick,
                               Team const team,
          HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  if( !card.istrump() )
    return modi;

  if (!(   card.isswine()
        || card.ishyperswine()
        || card.isdolle()))
    return modi;

  // have played a very high card


  // 2005-08-06: swines in the last three tricks
     // modi -= 29; // previous values: 23,14,21,24, 29, 33, 35,31
     modi -= (31 - std::max((int)ai.game().played_cards_no() - 9 , 0));
     // previous: 34, 31 (Reference 92672)
     // previous: no()-12

     if( trick.startcard().istrump() )
    	 modi -= 11; // reference

     if (  (static_cast<int>(trick.points()) - card.value()
               < ai.value(Aiconfig::LIMITQUEEN)))
         modi-= 4;

     if ( maybe_to_team(winnerteam) != team)
       modi -= 26;

     if (maybe_to_team(winnerteam) == team)
     {
       if (    // review Reference 260689
           trick.startplayer().no() == vgi.no() )  // serving with a high trump and something potential higher is still out there
       {
         if (    ai.hand().numberofswines() +
             ai.hand().numberofhyperswines() +
             ai.hand().numberof( Card::DOLLE ) != ai.hand().numberoftrumps() )
         {
           modi -= 10; // reference
         }

         //high trump serving is generally not a good choice
         if (!(GAMETYPE::is_solo(vgi.game().type())
               && (vgi.game().soloplayer() == ai)))
           modi -= 19; // reference 274918
       }
     }

     if(  (  vgi.no() != trick.winnerplayer().no()
           && !(  trick.cardno_of_player( card.player() ) == 3
                && trick.winnerplayer().team() == vgi.team() )
          )
       )
     {
       modi -= (36 - std::max((int)ai.game().played_cards_no() - 13, 0));
       modi += trick.points()/2;

       if( trick.cardno_of_player( trick.winnerplayer() )
          < trick.cardno_of_player( card.player() )
          || !trick.jabs_cards_before(  card.player() ))
         modi -= 16; // previously 9, 12
     }

     // not for all three cards

     if (  ! card.isdolle()) // so that a dolle is prefered to a swine
       modi -= 8;  //previous values: 2, 7,9

     if (  trick.startplayer().no() == vgi.no() )
     {
       // previous: 12, 18
       modi -= 30;  // 23: 002645, 30:
       if (    ai.hand().numberofswines() +
           ai.hand().numberofhyperswines() +
           ai.hand().numberof( Card::DOLLE ) == ai.hand().numberoftrumps() )
       {

         if( card.isdolle()
            && (ai.game().swines_owner() == NULL
                || ai.teamofplayer( *ai.game().swines_owner() ) == ai.team() )
            && (ai.game().hyperswines_owner() == NULL
                || ai.teamofplayer( *ai.game().hyperswines_owner() ) == ai.team()) )
         {
           modi += 10;
         }
         if( card.isswine()
            && (ai.game().hyperswines_owner() == NULL
                || ai.teamofplayer( *ai.game().hyperswines_owner() ) == ai.team()) )
         {
           modi += 30;
         }
         if( card.ishyperswine() )
         {
           modi += 50;
         }

       } else
       {
         if( GAMETYPE::is_solo( vgi.game().type() )  // reference 246985
            && vgi.is_soloplayer() ) //2do no higher card still in game
           modi += 21;
       }

     }


     if (  trick.cardno_of_player(card.player()) == 1
            && vgi.game().played_no(Card::DOLLE) == 0 //2do before trick
            && card.isdolle()
            && vgi.game().second_dolle_over_first() )
          modi -= 5;

     if (  trick.cardno_of_player(card.player()) == 0
            && vgi.game().played_no(Card::DOLLE) == 0 //2do before trick
            && card.isdolle()
            && vgi.game().second_dolle_over_first() )
          modi -= 1; // reference 197840


     // depending on data in the trick

     Trick t( trick.startplayer());
     for(unsigned i = 0; i < trick.cardno_of_player(card.player()); i++ )
     {
       t += trick.card( i );
     }

     if(  trick.cardno_of_player(card.player()) == 2
         && card.isdolle()
         && (ai.game().swines_owner() != NULL  //2do extend to hyperswine, do same for swine hyperswine
              && trick.cardno_of_player(*ai.game().swines_owner()) == 3 ) )
       { // swine behind dolle
         modi -= 15;  // reference 176246
       }

     if(  trick.cardno_of_player(card.player()) == 1
             && card.isdolle()
             && (ai.game().swines_owner() != NULL  //2do extend to hyperswine, do same for swine hyperswine
                  && trick.cardno_of_player(*ai.game().swines_owner()) >1 ) )
           { // swine behind dolle
             modi -= 3;  // reference 010523
           }


     if(  card.isdolle()
         && (ai.game().swines_owner() != NULL  //2do extend to hyperswine, do same for swine hyperswine
             && trick.cardno_of_player(*ai.game().swines_owner()) < trick.cardno_of_player(card.player()) )
            )
       { //swines in game, but before player with dolle
         modi +=13; // reference 088597, 114803
       }

     if (   trick.cardno_of_player(card.player()) == 3 )
     {
       modi -= 21; // previous 18, 20, 24



       if (t.winnerplayer().team() == vgi.team()) { // reference 43530
         modi -= 9; // previous
       }

       if (t.winnerplayer().team() != vgi.team()) { // reference
               modi -= 19; // previous
             }

     }



     if ( ai.value(Aiconfig::LIMITHIGH).less(t.winnercard()) // !oppositeTeamCanWinTrick( t, vgi ) ) 2do XXX
       && t.winnerplayer().team() == team )
       {
         modi -= 26;
         if( GAMETYPE::is_solo( vgi.game().type() )
            && !vgi.is_soloplayer())
           modi -= 4;

#if 0 //2do XXX versuch Bugfix für 125461.Reference.FreeDoko
         if (vgi.no() == playerCheck) {
           COUT << "X " << (ai.hand().numberofswines() +
                            ai.hand().numberofhyperswines() +
                            ai.hand().numberof( Card::DOLLE )) << "/" << ai.hand().numberoftrumps() << " X"  <<endl;
         }

         if(  trick.cardno_of_player(card.player()) == 3
            && (ai.hand().numberofswines() +
                ai.hand().numberofhyperswines() +
                ai.hand().numberof( Card::DOLLE ) < ai.hand().numberoftrumps() )
           )
           modi -= 22;
#endif

       }


     if ( ( card.possible_swine() //2do for a swine possible_swine should return false?
           && !card.isswine() )
         || (  card.possible_hyperswine()
             && !card.ishyperswine()))
       modi += 9;


     // only for soli
     if( GAMETYPE::is_solo( vgi.game().type() )
        && vgi.is_soloplayer())
       modi += 3;

#ifdef PLAYERCHECK
     if (vgi.no() == playerCheck) {

       COUT <<"MDS"<< modi << "\t";

     }
#endif
     return modi;
}


int TrickWeighting::modi_queen(VirtualGamesInterface const& vgi,
                               Trick const& trick,
                               Team const team,
                               HandCard const& card, 
                               Team winnerteam,
                               Ai const& ai)
{
  int modi = 0;
  if ( !card.istrump () )
    return modi;
  if( card.value() != Card::QUEEN )
    return modi;

  Player const& ai_virt = trick.game().player(ai.no());

  modi -= 4; // previous 2, 3
  // 4: Reference 188872

  if (
      (static_cast<int>(trick.points()) - card.value()
       < ai.value(Aiconfig::LIMITQUEEN)))
    modi-= 4;

  if ( trick.cardno_of_player(card.player()) == 3 )
  {
    modi -= 13; // previous 11, 13, 17, 14
   // 13 reference 043530

    Trick t( trick.startplayer());
    t += trick.card( 0 );
    t += trick.card( 1 );
    t += trick.card( 2 );

    if (t.winnerplayer().team() == vgi.team()) {
      modi -= 7; // previous

    }
  }

  if (GAMETYPE::is_solo(vgi.game().type())
      && (vgi.game().soloplayer().no() == ai.no())
      && vgi.trump_card_limit().less(card)
      && card.less(ai.cards_information().highest_remaining_trump_of_others()))
    modi -= 8; // reference 246985



  if(   (  vgi.no() != trick.winnerplayer().no()
         && !(  trick.cardno_of_player( card.player() ) == 3
              && trick.winnerplayer().team() == vgi.team() )
        )

    )
  {
    modi -= (36 - std::max((int)ai.game().played_cards_no() - 13, 0));
    modi += trick.points()/2;

    if( trick.cardno_of_player( trick.winnerplayer() )
       < trick.cardno_of_player( card.player() )
       || !trick.jabs_cards_before(  card.player() ))
      modi -= 18; // previously 9, 12, 16
     // 18 261053
  }

  if (   card == Card(Card::SPADE, Card::QUEEN) )
   {

     if( vgi.no() != trick.winnerplayer().no() )
       {
         modi -= 3; // reference
       }
   }

  if ( GAMETYPE::is_solo(vgi.game().type())
          && Card(Card::DIAMOND, Card::QUEEN) == card )
       modi += 1; // reference 126305

  if (   card == Card(Card::CLUB, Card::QUEEN) )
  {

    if( vgi.no() != trick.winnerplayer().no() )
      {
        modi -= 5; // reference 156330
      }

    if( vgi.game().teaminfo( ai.no() ) != ai.team() )
    {
      modi += 3; // previous 1
    } else
      modi -= 4; // previous 2, 3

    // show your own team.
    if (    vgi.game().teaminfo( ai.no() ) != ai.team()
        && ai.hand().numberoftrumps() <= 3 + (ai.hand().has_swines()?0:ai.hand().numberoftrumpaces())
        && vgi.game().tricks_remaining_no() - 1 > ai.hand().numberoftrumps()
        && (trick.cardno_of_player(trick.winnerplayer())
            > trick.cardno_of_player(card.player())
            && trick.jabs_cards_before( card.player() ))
       )
    {
      modi += 4; // previous
      if (  vgi.game().played_no(Card::CLUB_QUEEN) == 0
          && vgi.game().announcement_of_team( ai.team() ).announcement == ANNOUNCEMENT::NOANNOUNCEMENT
         )
        modi += 12; // previous
    }

    if(    trick.cardno_of_player(ai_virt) == 3
        && trick.winnerplayer().team() != vgi.team() )
    {
      modi -= 3; // reference 234467
    }

    if(    trick.cardno_of_player(ai_virt) == 0 )
      modi -= 12; // reference 168423


  }


#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MQ"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_jack(VirtualGamesInterface const& vgi,
                              Trick const& trick,
                              Team const team,
                              HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  if( !card.istrump() )
  {
    return modi;
  }
  if( card.value() != Card::JACK )
    return modi;

  if (  trick.cardno_of_player(card.player()) == 3 )
    modi -= 2;

  if (  trick.cardno_of_player(card.player()) == 1 )
      modi += 14; // reference 028089

  if (  trick.cardno_of_player(card.player()) == 0 )
     modi += 22; //reference 114558

  if( trick.startcard().istrump()
      && trick.cardno_of_player(card.player()) !=0 )
    modi += 5; //reference 099840


  // force enemy with a jack to play a queen
  if (   vgi.no() != trick.winnerplayer().no()
      && (trick.cardno_of_player(trick.winnerplayer())
          > trick.cardno_of_player(card.player())
          && trick.jabs_cards_before( card.player() ) )
      && (trick.winnercard().value() == Card::QUEEN)
      && (ai.team() != winnerteam)
      && trick.winnercard().istrump() )
    modi += 1;

  Trick t( trick.startplayer());
  for(unsigned i = 0; i == trick.cardno_of_player(card.player()); i++ )
  {
    t += trick.card( i );
  }

  if (   vgi.no() != trick.winnerplayer().no() ) // playing jack and not winning trick
  {
    modi -= 6; // reference 133381

  }

  // save charlie in endgame for extra point
  if(    card == Card::CHARLIE
      && vgi.game().rule()(Rule::EXTRAPOINT_CHARLIE)
      && vgi.game().tricks_remaining_no() -1 == ai.hand().numberoftrumps())
    modi -= 23; // reference 71916

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MJ"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_fox(VirtualGamesInterface const& vgi,
                             Trick const& trick,
                             Team const team,
                             HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  Player const& ai_virt = trick.game().player(ai.no());

  // calculations for fox in trick
  if (   vgi.no() != trick.winnerplayer().no()
      && (trick.cardno_of_player(trick.winnerplayer())
          > trick.cardno_of_player(card.player())
          && trick.jabs_cards_before( card.player() ) )
      && ai.value(Aiconfig::LIMITHIGH).less(trick.winnercard())
      && ai.team() != winnerteam )
  {
    for (unsigned c = 0; c < trick.actcardno(); c++)
    {
      if (trick.card(c).isfox() )
        modi += 11;
    }

  }

  if ((  (vgi.no() != trick.startplayer().no())
       || card.istrump())
     ) {
    for (unsigned c = 0; c < trick.actcardno(); c++) {
      // test, whether the fox has been brought home

      if (trick.card(c).isfox() )
      {

        if( (vgi.game().type() != GAMETYPE::POVERTY)
            || !vgi.is_soloplayer())
        {
          unsigned fact = 25;
          if(   ai.value(Aiconfig::HANDS_KNOWN)
             || ai.value(Aiconfig::TRUSTING) )
          {
            fact += 10;
            if( card.isfox() )
              fact += 5;
          }
          if( !card.isfox() )
            fact += 5;

          bool allmyteam = true;

          // take a look if all players coming in this trick are of mine team
          for( unsigned i = trick.cardno_of_player(ai_virt); i < vgi.game().playerno(); i++ )
            allmyteam = allmyteam && ( ai.team() == ai.teaminfo( trick.player_of_card( i ) ) );

          if (    card.isfox()
              && !trick.startcard().istrump()
              && ai.color_runs( trick.startcard().color() ) > 0
              && !allmyteam
              && oppositeTeamCanWinTrick( trick, vgi ))
          {
            fact -= 21;
          }



          modi += fact * ((team == winnerteam)
                          ? +1
                          : -1);

        } else { // fox from above neutralized.
          if ( card.isfox() )
            modi += 30 * ((team != winnerteam)
                          ? +1
                          : 0);
        }
      }
    } // for (c < trick.actcardno())
  } // if (  (vgi.no() != trick.startplayer().no()) || card.istrump())


  // own fox
  if( !card.isfox() )
  {
#ifdef PLAYERCHECK
    if (vgi.no() == playerCheck) {

      COUT <<"MFA"<< modi << "\t";

    }
#endif
    return modi;
  }

  // to play a fox it must be worth it
  {
    modi -= 12; // previous 7, 16 (ref 208166)
  }

  if (  (vgi.no() != trick.startplayer().no())) {

    if (maybe_to_team(winnerteam) == team)
    {
      if (  winnerteam == team
          && ai.value(Aiconfig::LIMITHIGH).less(trick.winnercard())
         )
        modi += 30; // fox home

    }
  }

  // bad style to play fox as first card
  if( trick.startplayer().no() == vgi.no() )
  {
    modi -= 90; // previous
  }

  if( trick.cardno_of_player(card.player()) == 3
     && maybe_to_team(winnerteam) != team )
  {
    modi -= 6; // previous

    if( ai.hand().numberoftrumps() <= 3 )
      modi += 2; // reference
  }

  if( trick.cardno_of_player(card.player()) == 3
     && winnerteam != team )
  {// reference 93822
    modi -= 35; // previous

   }

  if( trick.cardno_of_player(card.player()) == 3
     && winnerteam == team )
  {
    modi += 5; // previous
  }

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MFB"<< modi << "\t";

  }
#endif

  return modi;
}
int TrickWeighting::modi_soli_meat_less(VirtualGamesInterface const& vgi,
                                        Trick const& trick,
                                        Team const team,
                                        HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  if ( vgi.game().type() != GAMETYPE::SOLO_MEATLESS)
    return modi;
  if (   ai.no() != vgi.game().soloplayer().no()
      && vgi.no() != trick.winnerplayer().no()
      && ai.hand().numberof(card.color(), vgi.game()) > 0
      && ai.hand().numberof(card.color(), vgi.game()) < 3
      && (   ai.hand().numberof(Card(card.color(),Card::ACE)) > 0
          || ai.hand().numberof(Card(card.color(),Card::TEN)) > 0
          || card.value() == Card::ACE || card.value() == Card::TEN )
     )
  { // prevent non soloplayer from blank playing of cards
    modi -= 12;
  }

  if (   ai.no() != vgi.game().soloplayer().no()
        && vgi.no() != trick.winnerplayer().no()
        && (card.value() == Card::ACE || card.value() == Card::TEN) )
	  modi -=3;  // reference 004655


#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MSM"<< modi << "\t";

  }
#endif

  return modi;
}

int TrickWeighting::modi_marriage(VirtualGamesInterface const& vgi,
                                  Trick const& trick,
                                  Team const team,
                                  HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;


  if (   (vgi.game().type() != GAMETYPE::MARRIAGE) )
    return modi;

  if (
      (vgi.game().marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)
      && card.value() >= 10 )
    modi -= 9;

  if ( trick.cardno_of_player(card.player()) == 0) {
    switch (vgi.game().marriage_selector()) {
    case MARRIAGE_SELECTOR::SILENT:
    case MARRIAGE_SELECTOR::TEAM_SET:
      // the marriage is determined
      break;
    case MARRIAGE_SELECTOR::FIRST_COLOR:
    case MARRIAGE_SELECTOR::FIRST_FOREIGN:
      if ( !card.istrump() )
      {
        modi -= 5;
      }
      break;
    case MARRIAGE_SELECTOR::FIRST_TRUMP:
      if ( card.istrump() )
      {
        modi -= 5;
      }
      break;
    case MARRIAGE_SELECTOR::FIRST_CLUB:
      if ( !card.istrump()
          && card.color() == Card::CLUB )

      {
        modi -= 5;
      }
      break;
    case MARRIAGE_SELECTOR::FIRST_SPADE:
      if ( !card.istrump()
          && card.color() == Card::SPADE )

      {
        modi -= 5;
      }
      break;
    case MARRIAGE_SELECTOR::FIRST_HEART:
      if ( !card.istrump()
          && card.color() == Card::HEART )

      {
        modi -= 5;
      }
      break;
    } // switch (this->game().marriage_selector())
  } // if !(marriage && !card is trickstartcard())

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MM"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_special_points(VirtualGamesInterface const& vgi,
                                        Trick const& trick,
                                        Team const team,
                                        HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  Game const& game_virt = trick.game();

  // create the teams-vector
  vector<Team> teams;
  for (vector<Player*>::const_iterator
       player = game_virt.players_begin();
       player != game_virt.players_end();
       player++)
  {
    // determine team of player from ai out of virtual game
    Team const t = ai.teamofplayer( **player );
    if ( t != TEAM::UNKNOWN)
    {
      teams.push_back( maybe_to_team( t ) );
    } else
    {
      teams.push_back( opposite( maybe_to_team(ai.team()) ) );
    }

  } // for (player)

  if (  (vgi.no() != trick.startplayer().no())
      || card.istrump()) {

    if (maybe_to_team(winnerteam) == team)
    {
      Specialpointsvector spv=trick.specialpoints(teams);
      modi += (30
               * Sum_of_Specialpoints(spv,
                                      winnerteam,
                                      trick.startplayer().game()));
    }
    else
    {
      Specialpointsvector spv=trick.specialpoints(teams);
      modi -= (30
               * Sum_of_Specialpoints(spv,
                                      winnerteam,
                                      trick.startplayer().game()));
    }

  } // if (  (vgi.no() != trick.startplayer().no()) || card.istrump())
#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MSP"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_no_trump(VirtualGamesInterface const& vgi,
                                  Trick const& trick,
                                  Team const team,
                                  HandCard const& card,
                                  Team winnerteam, 
                                  Ai const& ai)
{
  int modi = 0;
  if (card.istrump () )
    return modi;

  Player const& ai_virt = trick.game().player(ai.no());


  if(  vgi.no() != trick.winnerplayer().no()
     && GAMETYPE::is_solo( vgi.game().type() )
     && ( trick.cardno_of_player( trick.winnerplayer() ) > trick.cardno_of_player( trick.game().soloplayer() ) )
     && !trick.card_of_player( trick.game().soloplayer() ).less( vgi.trump_card_limit() )
    )
  {
    modi -= card.value();
  }

  if( GAMETYPE::is_solo( vgi.game().type() )
     && trick.cardno_of_player( trick.game().soloplayer() ) == 0 )
  {
    modi += card.value();
    if ( card.value() >= 10 )
      modi += 3;
  }

  if( GAMETYPE::is_solo( vgi.game().type() )
     && trick.cardno_of_player(ai_virt) == 0 )
  {
    // last card with small value to serve a trick
    if(   ai.hand().numberof( card.color() ) == 0
         && card.value() < 10 )
       modi += 34; // reference 64029
  }

  // game nears it end, no real sense to keep non trump carts any longer, better to hold trumps
  if (    trick.cardno_of_player(ai_virt) == 0
	   && vgi.game().tricks_remaining_no() < 4 )
  {
	  modi += 23; // reference 058753

  }

  if (    trick.cardno_of_player(ai_virt) == 0
	   && vgi.game().tricks_remaining_no() < 5 )
  {

	  if ( vgi.team() == TEAM::RE
		   &&  vgi.game().type() != GAMETYPE::MARRIAGE
		   && ai.cards_information().played(Card::CLUB_QUEEN)+ai.hand().numberofclubqueens()<2)
		    modi += 3;  // reference 043205
  }

  if(  vgi.no() != trick.winnerplayer().no()
     && GAMETYPE::is_solo( vgi.game().type() )
     && ( trick.cardno_of_player( trick.winnerplayer() ) > trick.cardno_of_player( trick.game().soloplayer() ) )
     && trick.card_of_player( trick.game().soloplayer() ).less( vgi.trump_card_limit() )
    )
    modi += card.value();



  // prefer color cards a little bit
  if(    card.value() == Card::NINE
     && trick.winnerplayer().team() != vgi.team() )
    {
      modi += 8; // previous: 6
      if( trick.cardno_of_player(ai_virt) == 0 )
        modi +=10; // reference 203953
    }
  if(    card.value() == Card::KING
     && trick.winnerplayer().team() != vgi.team() )
    modi += 8;

  if( vgi.game().announcement_of_team( trick.winnerplayer().team() ).announcement != ANNOUNCEMENT::NOANNOUNCEMENT
	  && trick.winnerplayer().team() != vgi.team() )
  {
	  // preserve trumps against announcement
	  modi += 6;  //reference 006877
  }

  if ( card.value() == Card::ACE )
  {

    modi -= 2 - vgi.color_runs( card.color() );
  }

  if( card.value() == Card::JACK )
  	modi +=1; // good choice if not trump
  	          // reference 112599

  if( GAMETYPE::is_solo( vgi.game().type() )
      && vgi.team() == TEAM::CONTRA )
    modi -=  2* ai.hand().numberof( card.color() ); // reference  137807


  if (  vgi.no() != trick.winnerplayer().no()
      && winnerteam != team
      && trick.startcard().color() != card.color()
      && !trick.startcard().istrump() )
  {
    modi -= 9; //previous 6, 16, 14, 8
    // 9 : Reference 001236

    if(    trick.cardno_of_player(ai_virt) == 3
        && card.value() >= 10 ) // reference 234467
      modi -= 29;

  }

  if(    trick.cardno_of_player(ai_virt) == 0
      && card.value() == Card::NINE
      && vgi.color_runs( trick.startcard().color() ) > 0)
    { modi+=7; } //reference 272170

  bool allmyteam=true;

  for( unsigned i = trick.cardno_of_player(ai_virt);
      i < ai.game().playerno();
      i++ )
    allmyteam=allmyteam && ( ai.team() == ::maybe_to_team( vgi.teamofplayer( trick.player_of_card(i) ) ) );

  if(  !trick.startcard().istrump()
     && vgi.color_runs( trick.startcard().color() ) > 0
     && card.color() != trick.startcard().color()
     && !allmyteam)
    modi -= card.value()+2; //+2 reference 133381

  if( (ai.hand().numberoftrumps() - 1 <= (vgi.game().tricks_remaining_no() + 1) / 2 ) )
    {
      // reference 61146
      modi += 6;
      //if fox is still on hand, protect fox.
      if( ai.hand().numberoftrumpaces() > 0 && ai.hand().numberofswines() == 0 )
      {
        modi += 6; //reference 052672
        if( ai.hand().numberoftrumps() == 2 )
          modi += 15; // reference 076221
      }
    }

  if (  vgi.color_runs( card.color() ) == 0
      && card.value() == Card::ACE
      && ai.team() == winnerteam
     )
  {
    modi += 2;

  }

  if( GAMETYPE::is_solo( vgi.game().type() )
       && trick.cardno_of_player(trick.game().soloplayer()) > trick.cardno_of_player(card.player())
     && vgi.color_runs( card.color() ) == 0
          && card.value() == Card::ACE
  )
    {
    modi += 22; // reference 9751
    }

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MNT"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_picture_soli(VirtualGamesInterface const& vgi,
                                      Trick const& trick,
                                      Team const team,
                                      HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;

  if(  ( vgi.game().type()==GAMETYPE::SOLO_KING  ||
        vgi.game().type()==GAMETYPE::SOLO_KING_JACK ||
        vgi.game().type()==GAMETYPE::SOLO_KING_QUEEN ||
        vgi.game().type()==GAMETYPE::SOLO_KOEHLER ) &&
     card.value() == Card::KING  )

  {
    if( vgi.no() != trick.winnerplayer().no() )
    {
      modi -= (36 - std::max((int)ai.game().played_cards_no() - 13, 0));
      modi += trick.points()/2;

      if( trick.cardno_of_player( trick.winnerplayer() )
         < trick.cardno_of_player( card.player() )
         || !trick.jabs_cards_before( card.player() ) )
        modi -= 4;
    } else
      if(     trick.cardno_of_player( card.player() ) == 3
         && trick.points() < 16 )  // reference 131296
      {
        modi -= 25;
      }
  }

  if( card.istrump()
     && (   vgi.no() != trick.winnerplayer().no()
         || trick.winnercardno() != 3)
     && trick.cardno_of_player(card.player()) == 3 )
  {

    if(  vgi.game().type()!=GAMETYPE::SOLO_KING  &&
       vgi.game().type()!=GAMETYPE::SOLO_QUEEN_JACK &&
       vgi.game().type()!=GAMETYPE::SOLO_KING_JACK &&
       vgi.game().type()!=GAMETYPE::SOLO_KING_QUEEN &&
       vgi.game().type()!=GAMETYPE::SOLO_KOEHLER  &&
       (card.value() == Card::JACK  || card.value() == Card::QUEEN)  ) // a king is better, but has two points more
      modi -= 3;
  }

  if(  ( vgi.game().type()==GAMETYPE::SOLO_KING  ||
        vgi.game().type()==GAMETYPE::SOLO_KING_JACK ||
        vgi.game().type()==GAMETYPE::SOLO_KING_QUEEN ||
        vgi.game().type()==GAMETYPE::SOLO_KOEHLER ) &&
     card.value() == Card::KING )
    modi -= 2;


  if(  card.istrump()
     && (    vgi.game().type()==GAMETYPE::SOLO_KING
         ||  vgi.game().type()==GAMETYPE::SOLO_QUEEN
         ||  vgi.game().type()==GAMETYPE::SOLO_JACK
        )
    )
    modi -= 4;

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MPS"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_other_trumps(VirtualGamesInterface const& vgi,
                                      Trick const& trick,
                                      Team const team,
                                      HandCard const& card, 
                                      Team winnerteam,
                                      Ai const& ai)
{
  int modi = 0;
  Game const& game_virt = trick.game();
  Player const& ai_virt = game_virt.player(ai.no());

  if( !card.istrump () )
    return modi;

  if (   trick.startcard().istrump()
      // ToDo: solo
      && !card.less(Card(Card::DIAMOND, Card::QUEEN))
      && (static_cast<int>(trick.points()) >= ai.value(Aiconfig::LIMITQUEEN)) )
  {
    modi += 2;
  }

  if (  GAMETYPE::is_solo( vgi.game().type() ) )
  {
    modi -= 2;
    if( vgi.is_soloplayer()
    	&& card.value() == 0	)
    {
      modi -= 12;
    }
  }

  if(  (   vgi.no() != trick.winnerplayer().no()
        || trick.winnercardno() != 3)
     && trick.cardno_of_player(card.player()) == 3 )
  {
    modi -= 5; // previous -2
  }


  // have played a high card
  if ((ai.value(Aiconfig::LIMITHIGH).less(card)
       || ai.value(Aiconfig::LIMITHIGH) == card)
      && (static_cast<int>(trick.points()) - card.value()
          < ai.value(Aiconfig::LIMITQUEEN) ) )
  {
    modi -= 3;
    if ((2 * ai.hand().numberof_ge(ai.value(Aiconfig::LIMITHIGH))
         < ai.hand().numberoftrumps())
        && (ai.hand().numberof_ge(ai.value(Aiconfig::LIMITHIGH))
            < 3))
      modi -= 13; // previous: 18
  }

  if (   !trick.startcard().istrump()
      && card.less(ai.value(Aiconfig::LIMITHIGH))
      && (static_cast<int>(trick.points()) >= ai.value(Aiconfig::LIMITQUEEN))
      && trick.jabs_cards_before( card.player() ) )
  {
    modi -= 3;
    if ( vgi.color_runs( trick.startcard().color() ) > 0 )
      modi -= 6;

    if( GAMETYPE::is_solo( vgi.game().type() )
       && trick.cardno_of_player(trick.game().soloplayer()) > trick.cardno_of_player(card.player()) )
      modi -= 7; // reference 197069
  }

  if(   (card.value() == Card::TEN)
     && (!card.isdolle()) )
  {
    modi -= 10; // reference 246985
  }

  if(   (card.value() == Card::ACE)
     && !card.is_special() )
  {
    modi -= 9; // reference 246985
  }

  if(   (card.value() == Card::KING)
     &&  (winnerteam != team) )
  {
    modi -= 14; // reference 256027
  }

  if(   (card.value() == Card::NINE)
	  && !card.is_special()
	  && trick.winnercardno() == 3
	     && trick.cardno_of_player(card.player()) == 3 )
	  modi -= 3; // reference 064027_1


  if( game_virt.next_higher_card( card ).value() != card.value() //2do nect_higher_card_still_in_game compar reference 260689
     && (trick.cardno_of_player(trick.winnerplayer())
         > trick.cardno_of_player(card.player()))
     && trick.jabs_cards_before( card.player() ))
  { // a higher card in value is needed to jab my card
    // for example a queen to jab my jack
    modi += 1;
  }


  bool allmyteam=true;

  for( unsigned i = trick.cardno_of_player(ai_virt);
      i < ai.game().playerno();
      i++ )
    allmyteam=allmyteam && ( ai.team() == ::maybe_to_team( vgi.teamofplayer( trick.player_of_card(i) ) ) );

  if(   card.less( ai.value(Aiconfig::LIMITHIGH) )
     && !trick.startcard().istrump()
     && (trick.cardno_of_player(trick.winnerplayer())
         > trick.cardno_of_player(card.player())
         && trick.jabs_cards_before( card.player() ) )
     && !(    trick.islastcard()
          || allmyteam )
    )
  {
    modi -= card.value() * std::min(vgi.color_runs( trick.startcard().color() ),
                                    2u);
  }

  if (  (vgi.no() == trick.startplayer().no()))
  {
    modi -= 2;

    if( ai.hand().numberoftrumps() == ai.hand().cardsnumber()
        && card.value() < 3 )
      modi +=5; // Reference 012664

    if( (card.value() == Card::KING)
       && (winnerteam == team) )
      modi += 8;
  }

  { // the player has overjabbed his partner without need
    if (trick.winnerplayer() == ai) {
      Trick const t2 = trick.before_player(ai);

      if (  (::maybe_to_team(t2.winnerplayer().team()) == ai.team())
          && (vgi.trump_card_limit().less(t2.winnercard()))
         ) {
        modi -= 8;
      }
    }
  } // the player has overjabbed his partner without need

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MOT"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_force_trump(VirtualGamesInterface const& vgi,
                                     Trick const& trick,
                                     Team const team,
                                     HandCard const& card, 
                                     Team winnerteam,
                                     Ai const& ai)
{
  int modi = 0;

  // force enemy to play high card
  if (   vgi.no() != trick.winnerplayer().no()
      && (trick.cardno_of_player(trick.winnerplayer())
          > trick.cardno_of_player(card.player())
          && trick.jabs_cards_before( card.player() ) )
      && ai.value(Aiconfig::LIMITHIGH).less(trick.winnercard())
      && ai.team() != winnerteam )
  {

    bool bIsForceTrump = true;
    // ToDo: start with
    for (unsigned c = trick.cardno_of_player(card.player()) + 1; c < trick.actcardno(); c++)
    {
      if (!trick.card(c).less(card))
        bIsForceTrump = false;
    } // for(c)
    if ( bIsForceTrump )
      modi += 27;
  }

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT <<"MFT"<< modi << "\t";

  }
#endif
  return modi;
}

int TrickWeighting::modi_poverty(VirtualGamesInterface const& vgi,
                                 Trick const& trick,
                                 Team const team,
                                 HandCard const& card,  Team winnerteam, Ai const& ai)
{
  int modi = 0;
  if ( vgi.game().type() != GAMETYPE::POVERTY )
    return modi;
  Player const& ai_virt = trick.game().player(ai.no());

  if(    card.istrump()
     && trick.cardno_of_player(ai_virt) == 0)
    modi -= 3; // 1:reference 208016, 3: reference 194579

  //  get large cards of poverty player
  if((  //only for not poverty-player
      vgi.no() == vgi.game().poverty_partner().no() ))
  {

    if( (trick.cardno_of_player(trick.game().soloplayer()) > trick.cardno_of_player(card.player())) //
       && !card.less(ai.value(Aiconfig::LIMITHIGH))
       &&  card.player().no() != vgi.game().player_previous(vgi.game().soloplayer()).no())
    {
      modi += 21;
      if( ai.value(Aiconfig::LIMITHIGH).less(trick.winnercard()) )
        modi += 5;
      if ( trick.winnercard().isdolle()
          || trick.winnercard().isswine()
          || trick.winnercard().ishyperswine() )
        modi +=4;
    }
  }
#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {
    // 4. block
    COUT << "MPV" << modi << "\t";
  }
#endif

  return modi;
}
/**
 **
 ** -> result
 **
 ** @param      vgi	the virtual games interface
 ** @param      trick   the trick to be examined
 ** @param      t	the team which should make points
 ** @param      card    the card played by the player
 **
 ** @return     the weighting of the trick for the team 'team'
 **
 ** @version    0.4.4
 **
 ** @author     Borg Enders
 **
 ** @todo       perhaps points for getting the fox home
 ** @todo       perhaps when playing the highest card, subtract in modi
 **		the distance to the next lower card
 **/

int
TrickWeighting::modi(VirtualGamesInterface const& vgi,
                     Trick const& trick,
                     Team const t,
                     HandCard const& card)
{
  DEBUG_ASSERTION(trick.isfull(),
                  "TrickWeighting::modi()\n"
                  "  trick is not full");
  Game const& game_virt = trick.game();
  Ai const& ai = (dynamic_cast<Ai const*>(&game_virt.player(vgi.no()))
                  ? static_cast<Ai const&>(game_virt.player( vgi.no() ))
                  : static_cast<Ai const&>(vgi.game().player(vgi.no()))
                 );
  Player const& ai_virt = game_virt.player(ai.no());
  //Team const winnerteam = ai.teamofplayer(trick.winnerplayer());
  Team winnerteam
    = ::maybe_to_team(vgi.teamofplayer(trick.winnerplayer()));
  if (winnerteam == TEAM::UNKNOWN)
    winnerteam = ::opposite(t);

  if (game_virt.is_undetermined_marriage()) {
    // in an undetermined marriage all players shall play for themselves
    if (t == TEAM::RE)
      winnerteam = ::maybe_to_team(ai.teaminfo(trick.winnerplayer()));
    else if (trick.winnerplayer().no() == vgi.no())
      winnerteam = ::maybe_to_team(t);
    else
      winnerteam = ::opposite(::maybe_to_team(t));
  }


  Team const team = ((game_virt.type() == GAMETYPE::MARRIAGE) 
                     ? maybe_to_team(t)
                     : t);

  int modi = 0;


  if (winnerteam == team)
    modi += trick.points();
  else
    modi -= trick.points();

#ifdef PLAYERCHECK
  // Playernumber for which to produce output
  playerCheck = PLAYERCHECK;

  // so in virtual games no informatin is shown
  if (vgi.game().isvirtual())
    playerCheck = 10;
#endif




#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {
    //COUT << trick.game().player(0).hand();
    COUT << '\n';
    COUT << trick << endl;  
    COUT << team << "\t" << trick.winnerplayer().no() << " " << winnerteam << std::endl;
    for( unsigned i = 0; i < ai.game().playerno(); i++ )
      COUT << game_virt.player( i ).team() << "\t";
    COUT << std::endl;
    // 1. block
    COUT << card << "\t";
  }
#endif

#ifdef PLAYERCHECK
  if (vgi.no() == playerCheck) {

    COUT << "START" << modi << "\t";

  }
#endif

  modi += modi_dolle_swines( vgi, trick, t, card, winnerteam, ai );

  modi += modi_queen( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_jack( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_fox( vgi, trick, t, card, winnerteam, ai );

  modi += modi_soli_meat_less( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_marriage( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_special_points( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_no_trump( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_picture_soli( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_other_trumps( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_force_trump( vgi, trick, t, card, winnerteam, ai  );

  modi += modi_poverty( vgi, trick, t, card, winnerteam, ai  );











  bool allmyteam=true;

  for( unsigned i = trick.cardno_of_player(ai_virt);
      i < ai.game().playerno();
      i++ )
    allmyteam=allmyteam && ( ai.team() == ::maybe_to_team( vgi.teamofplayer( trick.player_of_card(i) ) ) );

  /// @todo jabbed by player of other team behind me
  if(   !trick.startcard().istrump()
     && vgi.jabbedbyotherteam( trick.startcard().color() )
     && card.value() >= 10 
     && !allmyteam
     && oppositeTeamCanWinTrick( trick, vgi )
    )
  {
    modi -= 4;
#ifdef PLAYERCHECK
    if (vgi.no() == playerCheck) {

      COUT << "XX " << modi << "\t";
    }
#endif
  }




#ifdef PLAYERCHECK

  if (vgi.no() == playerCheck) {


    COUT << "END" << modi << endl;
  }
#endif


  return modi;
} // int WVirtualGames::modi(Trick const& trick, Team const& team, Card const& card)


/**
 **
 ** -> result
 **
 ** @param      vgi         virtual game interface
 ** @param      c           the card played by the player
 ** @param      game_virt   actual virtual game
 **
 ** @return     weighting modifier for getting own team in backhand
 **
 ** @version    0.6.3
 **
 ** @author     Borg Enders
 **
 **
 **/
int
TrickWeighting::backhand(VirtualGamesInterface const& vgi,
                         HandCard const& c,
                         Game const& game_virt )                         
{
  int result = 0;

  // add some points, if the own team is in the back,
  if (game_virt.trick_current_no() < game_virt.trickno() - 1) {
    if (game_virt.player_previous( game_virt.trick_current().winnerplayer() ).team()
        == vgi.team() && !c.isfox() &&
        vgi.game().type() != GAMETYPE::SOLO_MEATLESS )
    {
      result = 5;

      if( game_virt.trick_current().points() > 10 )
        result -= 3;  // Bugreport reference 1236

      if (game_virt.type() == GAMETYPE::POVERTY )
        result -= 3;
    }
    else
      result = -6;


    return result;
  } // if (game_virt.trick_current_no() < game_virt.trickno() - 1)

  return 0;
}

