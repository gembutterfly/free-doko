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

#ifdef USE_SOUND

#include "sound.h"
#ifdef USE_SOUND_ALUT
#include "alut.h"
#endif
#ifdef USE_SOUND_COMMAND
#include "command.h"
#endif
#ifdef USE_SOUND_PLAYSOUND
#include "playsound.h"
#endif

#include "../game/game.h"
#include "../player/player.h"

#include "../misc/setting.h"

#include "../utils/string.h"
#include "../utils/file.h"


/**
 ** -> result
 **
 ** @param     announcement   the announcement
 **
 ** @return    the type for the announcement
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Sound::Type
Sound::type(GameType const gametype)
{
  switch (gametype) {
  case GAMETYPE::NORMAL:
    return GAMETYPE_NORMAL;
  case GAMETYPE::THROWN_NINES:
    return GAMETYPE_THROWN_NINES;
  case GAMETYPE::THROWN_KINGS:
    return GAMETYPE_THROWN_KINGS;
  case GAMETYPE::THROWN_NINES_AND_KINGS:
    return GAMETYPE_THROWN_NINES_AND_KINGS;
  case GAMETYPE::THROWN_RICHNESS:
    return GAMETYPE_THROWN_RICHNESS;
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    return GAMETYPE_FOX_HIGHEST_TRUMP;
  case GAMETYPE::POVERTY:
    return GAMETYPE_POVERTY;
  case GAMETYPE::GENSCHER:
    return GAMETYPE_GENSCHER;
  case GAMETYPE::MARRIAGE:
    return GAMETYPE_MARRIAGE;
  case GAMETYPE::MARRIAGE_SOLO:
    return GAMETYPE_MARRIAGE_SOLO;
  case GAMETYPE::MARRIAGE_SILENT:
    return GAMETYPE_MARRIAGE_SILENT;
  case GAMETYPE::SOLO_MEATLESS:
    return GAMETYPE_SOLO_MEATLESS;
  case GAMETYPE::SOLO_JACK:
    return GAMETYPE_SOLO_JACK;
  case GAMETYPE::SOLO_QUEEN:
    return GAMETYPE_SOLO_QUEEN;
  case GAMETYPE::SOLO_KING:
    return GAMETYPE_SOLO_KING;
  case GAMETYPE::SOLO_QUEEN_JACK:
    return GAMETYPE_SOLO_QUEEN_JACK;
  case GAMETYPE::SOLO_KING_JACK:
    return GAMETYPE_SOLO_KING_JACK;
  case GAMETYPE::SOLO_KING_QUEEN:
    return GAMETYPE_SOLO_KING_QUEEN;
  case GAMETYPE::SOLO_KOEHLER:
    return GAMETYPE_SOLO_KOEHLER;
  case GAMETYPE::SOLO_CLUB:
    return GAMETYPE_SOLO_CLUB;
  case GAMETYPE::SOLO_HEART:
    return GAMETYPE_SOLO_HEART;
  case GAMETYPE::SOLO_SPADE:
    return GAMETYPE_SOLO_SPADE;
  case GAMETYPE::SOLO_DIAMOND:
    return GAMETYPE_SOLO_DIAMOND;
  } // switch (gametype)

  return FIRST;
} // static Sound::Type Sound::type(GameType gametype)

/**
 ** -> result
 **
 ** @param     marriage_selector   the marriage selector
 **
 ** @return    the type for the marriage selector
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Sound::Type
Sound::type(MarriageSelector const marriage_selector)
{
  switch (marriage_selector) {
  case MARRIAGE_SELECTOR::TEAM_SET:
    return MARRIAGE_SELECTOR_TEAM_SET;
  case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    return MARRIAGE_SELECTOR_FIRST_FOREIGN;
  case MARRIAGE_SELECTOR::FIRST_TRUMP:
    return MARRIAGE_SELECTOR_FIRST_TRUMP;
  case MARRIAGE_SELECTOR::FIRST_COLOR:
    return MARRIAGE_SELECTOR_FIRST_COLOR;
  case MARRIAGE_SELECTOR::FIRST_CLUB:
    return MARRIAGE_SELECTOR_FIRST_CLUB;
  case MARRIAGE_SELECTOR::FIRST_SPADE:
    return MARRIAGE_SELECTOR_FIRST_SPADE;
  case MARRIAGE_SELECTOR::FIRST_HEART:
    return MARRIAGE_SELECTOR_FIRST_HEART;
  case MARRIAGE_SELECTOR::SILENT:
    return MARRIAGE_SELECTOR_SILENT;
  } // switch (marriage_selector)

  return FIRST;
} // static Sound::Type Sound::type(MarriageSelector marriage_selector)

/**
 ** -> result
 **
 ** @param     announcement   the announcement
 **
 ** @return    the type for the announcement
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Sound::Type
Sound::type(Announcement const announcement)
{
  switch (announcement) {
  case ANNOUNCEMENT::NOANNOUNCEMENT:
    return ANNOUNCEMENT_NOANNOUNCEMENT;
  case ANNOUNCEMENT::NO120:
    return ANNOUNCEMENT_NO120;
  case ANNOUNCEMENT::NO90:
    return ANNOUNCEMENT_NO90;
  case ANNOUNCEMENT::NO60:
    return ANNOUNCEMENT_NO60;
  case ANNOUNCEMENT::NO30:
    return ANNOUNCEMENT_NO30;
  case ANNOUNCEMENT::NO0:
    return ANNOUNCEMENT_NO0;
  case ANNOUNCEMENT::REPLY:
    return ANNOUNCEMENT_REPLY;
  case ANNOUNCEMENT::NO120_REPLY:
    return ANNOUNCEMENT_NO120_REPLY;
  case ANNOUNCEMENT::NO90_REPLY:
    return ANNOUNCEMENT_NO90_REPLY;
  case ANNOUNCEMENT::NO60_REPLY:
    return ANNOUNCEMENT_NO60_REPLY;
  case ANNOUNCEMENT::NO30_REPLY:
    return ANNOUNCEMENT_NO30_REPLY;
  case ANNOUNCEMENT::NO0_REPLY:
    return ANNOUNCEMENT_NO0_REPLY;
  } // switch (announcement)

  return FIRST;
} // static Sound::Type Sound::type(Announcement announcement)


/**
 ** -> result
 **
 ** @param     type   the sound type
 **
 ** @return    the name of the sound type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
string
Sound::name(Sound::Type const type)
{
  switch (type) {
  case GAMETYPE_NORMAL:
    return "gametype: normal";
  case GAMETYPE_THROWN_NINES:
    return "gametype: thrown nines";
  case GAMETYPE_THROWN_KINGS:
    return "gametype: thrown kings";
  case GAMETYPE_THROWN_NINES_AND_KINGS:
    return "gametype: thrown nines and kings";
  case GAMETYPE_THROWN_RICHNESS:
    return "gametype: thrown richness";
  case GAMETYPE_FOX_HIGHEST_TRUMP:
    return "gametype: fox highest trump";
  case GAMETYPE_POVERTY:
    return "gametype: poverty";
  case GAMETYPE_GENSCHER:
    return "gametype: genscher";
  case GAMETYPE_MARRIAGE:
    return "gametype: marriage";
  case GAMETYPE_MARRIAGE_SOLO:
    return "gametype: marriage solo";
  case GAMETYPE_MARRIAGE_SILENT:
    return "gametype: marriage silent";
  case GAMETYPE_SOLO_MEATLESS:
    return "gametype: solo meatless";
  case GAMETYPE_SOLO_JACK:
    return "gametype: solo jack";
  case GAMETYPE_SOLO_QUEEN:
    return "gametype: solo queen";
  case GAMETYPE_SOLO_KING:
    return "gametype: solo king";
  case GAMETYPE_SOLO_QUEEN_JACK:
    return "gametype: solo queen jack";
  case GAMETYPE_SOLO_KING_JACK:
    return "gametype: solo king jack";
  case GAMETYPE_SOLO_KING_QUEEN:
    return "gametype: solo king queen";
  case GAMETYPE_SOLO_KOEHLER:
    return "gametype: solo koehler";
  case GAMETYPE_SOLO_CLUB:
    return "gametype: solo club";
  case GAMETYPE_SOLO_HEART:
    return "gametype: solo heart";
  case GAMETYPE_SOLO_SPADE:
    return "gametype: solo spade";
  case GAMETYPE_SOLO_DIAMOND:
    return "gametype: solo diamond";

  case MARRIAGE_SELECTOR_TEAM_SET:
    return "marriage selector: team set";
  case MARRIAGE_SELECTOR_FIRST_FOREIGN:
    return "marriage selector: first foreign";
  case MARRIAGE_SELECTOR_FIRST_TRUMP:
    return "marriage selector: first trump";
  case MARRIAGE_SELECTOR_FIRST_COLOR:
    return "marriage selector: first color";
  case MARRIAGE_SELECTOR_FIRST_CLUB:
    return "marriage selector: first club";
  case MARRIAGE_SELECTOR_FIRST_SPADE:
    return "marriage selector: first spade";
  case MARRIAGE_SELECTOR_FIRST_HEART:
    return "marriage selector: first heart";
  case MARRIAGE_SELECTOR_SILENT:
    return "marriage selector: silent";

  case CARDS_DISTRIBUTED:
    return "cards distributed";
  case TRICK_MOVE_IN_PILE:
    return "trick move in pile";
  case CARD_PLAYED:
    return "card played";

  case POVERTY:
    return "poverty";
  case POVERTY_DENIED:
    return "poverty denied";
  case POVERTY_ACCEPTED:
    return "poverty accepted";

  case MARRIAGE:
    return "marriage";
  case GENSCHER:
    return "genscher";

  case ANNOUNCEMENT_NOANNOUNCEMENT:
    return "announcement: no announcement";
  case ANNOUNCEMENT_RE:
    return "announcement: re";
  case ANNOUNCEMENT_CONTRA:
    return "announcement: contra";
  case ANNOUNCEMENT_NO120:
    return "announcement: no 120";
  case ANNOUNCEMENT_NO90:
    return "announcement: no 90";
  case ANNOUNCEMENT_NO60:
    return "announcement: no 60";
  case ANNOUNCEMENT_NO30:
    return "announcement: no 30";
  case ANNOUNCEMENT_NO0:
    return "announcement: no 0";
  case ANNOUNCEMENT_REPLY:
    return "announcement: reply";
  case ANNOUNCEMENT_NO120_REPLY:
    return "announcement: no 120 reply";
  case ANNOUNCEMENT_NO90_REPLY:
    return "announcement: no 90 reply";
  case ANNOUNCEMENT_NO60_REPLY:
    return "announcement: no 60 reply";
  case ANNOUNCEMENT_NO30_REPLY:
    return "announcement: no 30 reply";
  case ANNOUNCEMENT_NO0_REPLY:
    return "announcement: no 0 reply";

  case SWINES:
    return "swines";
  case HYPERSWINES:
    return "hyperswines";

  case NO_WINNER:
    return "no winner";
  case SOLOPLAYER_WON:
    return "soloplayer won";
  case SOLOPLAYER_LOST:
    return "soloplayer lost";
  case RE_WON:
    return "re won";
  case CONTRA_WON:
    return "contra won";
  } // switch (type)

  return "";
} // static string Sound::name(Sound::Type type)

/**
 ** -> result
 **
 ** @param     type   the sound type
 **
 ** @return    the text for the sound type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
string
Sound::text(Sound::Type const type)
{
  switch (type) {
  case GAMETYPE_NORMAL:
    return "normal";
  case GAMETYPE_THROWN_NINES:
    return "thrown nines";
  case GAMETYPE_THROWN_KINGS:
    return "thrown kings";
  case GAMETYPE_THROWN_NINES_AND_KINGS:
    return "thrown nines and kings";
  case GAMETYPE_THROWN_RICHNESS:
    return "thrown richness";
  case GAMETYPE_FOX_HIGHEST_TRUMP:
    return "fox highest trump";
  case GAMETYPE_POVERTY:
    return "poverty";
  case GAMETYPE_GENSCHER:
    return "genscher";
  case GAMETYPE_MARRIAGE:
    return "marriage";
  case GAMETYPE_MARRIAGE_SOLO:
    return "marriage solo";
  case GAMETYPE_MARRIAGE_SILENT:
    return "marriage silent";
  case GAMETYPE_SOLO_MEATLESS:
    return "solo meatless";
  case GAMETYPE_SOLO_JACK:
    return "solo jack";
  case GAMETYPE_SOLO_QUEEN:
    return "solo queen";
  case GAMETYPE_SOLO_KING:
    return "solo king";
  case GAMETYPE_SOLO_QUEEN_JACK:
    return "solo queen jack";
  case GAMETYPE_SOLO_KING_JACK:
    return "solo king jack";
  case GAMETYPE_SOLO_KING_QUEEN:
    return "solo king queen";
  case GAMETYPE_SOLO_KOEHLER:
    return "solo koehler";
  case GAMETYPE_SOLO_CLUB:
    return "solo club";
  case GAMETYPE_SOLO_HEART:
    return "solo heart";
  case GAMETYPE_SOLO_SPADE:
    return "solo spade";
  case GAMETYPE_SOLO_DIAMOND:
    return "solo diamond";

  case MARRIAGE_SELECTOR_TEAM_SET:
    return ""; // "team set";
  case MARRIAGE_SELECTOR_FIRST_FOREIGN:
    return "first foreign";
  case MARRIAGE_SELECTOR_FIRST_TRUMP:
    return "first trump";
  case MARRIAGE_SELECTOR_FIRST_COLOR:
    return "first color";
  case MARRIAGE_SELECTOR_FIRST_CLUB:
    return "first club";
  case MARRIAGE_SELECTOR_FIRST_SPADE:
    return "first spade";
  case MARRIAGE_SELECTOR_FIRST_HEART:
    return "first heart";
  case MARRIAGE_SELECTOR_SILENT:
    return "silent";

  case CARDS_DISTRIBUTED:
    return "cards distributed";
  case TRICK_MOVE_IN_PILE:
    return "trick move in pile";
  case CARD_PLAYED:
    return "card played";

  case POVERTY:
    return "poverty";
  case POVERTY_DENIED:
    return "poverty denied";
  case POVERTY_ACCEPTED:
    return "poverty accepted";

  case MARRIAGE:
    return "marriage";
  case GENSCHER:
    return "genscher";

  case ANNOUNCEMENT_NOANNOUNCEMENT:
    return ""; // "no announcement";
  case ANNOUNCEMENT_RE:
    return "re";
  case ANNOUNCEMENT_CONTRA:
    return "contra";
  case ANNOUNCEMENT_NO120:
    return "no 120";
  case ANNOUNCEMENT_NO90:
    return "no 90";
  case ANNOUNCEMENT_NO60:
    return "no 60";
  case ANNOUNCEMENT_NO30:
    return "no 30";
  case ANNOUNCEMENT_NO0:
    return "no 0";
  case ANNOUNCEMENT_REPLY:
    return "reply";
  case ANNOUNCEMENT_NO120_REPLY:
    return "no 120 reply";
  case ANNOUNCEMENT_NO90_REPLY:
    return "no 90 reply";
  case ANNOUNCEMENT_NO60_REPLY:
    return "no 60 reply";
  case ANNOUNCEMENT_NO30_REPLY:
    return "no 30 reply";
  case ANNOUNCEMENT_NO0_REPLY:
    return "no 0 reply";

  case SWINES:
    return "swines";
  case HYPERSWINES:
    return "hyperswines";

  case NO_WINNER:
    return "no winner";
  case SOLOPLAYER_WON:
    return "soloplayer won";
  case SOLOPLAYER_LOST:
    return "soloplayer lost";
  case RE_WON:
    return "re won";
  case CONTRA_WON:
    return "contra won";
  } // switch (type)

  return "";
} // static string Sound::text(Sound::Type type)

/**
 ** -> result
 **
 ** @param     type   the sound type
 **
 ** @return    the file name for the sound type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
string
Sound::filename(Sound::Type const type)
{
  string filename = Sound::text(type);
  DK::Utils::String::replace_all(filename, " ",  "_");
  return filename;
} // static string Sound::filename(Sound::Type type)

/**
 ** -> result
 **
 ** @param     position   position of the sound
 ** @param     voice      voice (directory to take the sound from)
 ** @param     type       sound to play
 **
 ** @return    the filename of the sound, empty if not found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
string
Sound::path(Position const& position,
                string const& voice,
                Type const type)
{
  // search the sound file
  string const basename = (Sound::filename(type)
                           + "." + ::setting(Setting::SOUND_EXTENSION));
  for (vector<string>::const_iterator
       dir = ::setting.data_directories().begin();
       dir != ::setting.data_directories().end();
       ++dir) {
    string const path
      = (*dir + "/"
         + ::setting(Setting::SOUNDS_DIRECTORY) + "/"
         + voice + "/"
         + basename);
    if (DK::Utils::File::isfile(path)) {
      return path;
    } // if (DK::Utils::File::isfile(path_tmp))
  } // for (dir \in ::setting.data_directories)

  return "";
} // static string Sound::path(Position position, string voice, Type type)


/**
 ** create a sound class
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Sound*
Sound::new_()
{
#ifdef USE_SOUND_ALUT
  return new Alut;
#endif
#ifdef USE_SOUND_COMMAND
  return new Command;
#endif
#ifdef USE_SOUND_PLAYSOUND
  return new WinPlaySound;
#endif

  DEBUG_ASSERTION(false,
                  "Sound::new_()"
                  "  no sound object created");

  return NULL;
} // static Sound* Sound::new_()

/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Sound::Sound() :
  OS(OS_TYPE::SOUND)
{ }

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Sound::~Sound()
{ }


/**
 ** play the sound (global group)
 **
 ** @param     type   sound to play
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::play(Type const type)
{
  if (::setting(Setting::SOUND))
    this->play(CENTER, "general", type);

  return ;
} // virtual void Sound::play(Type type)

/**
 ** play the sound (group of the player)
 **
 ** @param     player   player who says it
 ** @param     type     sound to play
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::play(Player const& player, Type const type)
{
  if (::setting(Setting::SOUND))
    this->play(::setting.position(player.game().no(player)), player.voice(), type);

  return ;
} // virtual void Sound::play(Player player, Type type)

/**
 ** the cards are distributed
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::game_cards_distributed()
{
  this->play(CARDS_DISTRIBUTED);
  return ;
} // void Sound::game_cards_distributed()

/**
 ** the trick is moved into a pile
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::trick_move_in_pile()
{
  this->play(this->game().player_current(), TRICK_MOVE_IN_PILE);
  return ;
} // void Sound::trick_move_in_pile()

/**
 ** the card is played
 **
 ** @param     card   card that is played
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::card_played(HandCard const& card)
{
  this->play(card.player(), CARD_PLAYED);
  return ;
} // void Sound::card_played(HandCard card)


/**
 ** the game is started
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      announce solo games
 **/
void
Sound::game_start()
{
  switch (this->game().type()) {
  case GAMETYPE::MARRIAGE:
    this->play(this->game().soloplayer(), Sound::type(this->game().type()));
    this->play(this->game().soloplayer(), Sound::type(this->game().marriage_selector()));
    break;
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_MEATLESS:
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_DIAMOND:
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::POVERTY:
    this->play(this->game().soloplayer(), Sound::type(this->game().type()));
    break;
  default:
    break;
  } // switch (this->game().type())
  return ;
} // void Sound::game_start()

/**
 ** the game is finished
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      announce winner
 **/
void
Sound::game_finish()
{
  if (this->game().winnerteam() == TEAM::NOTEAM) {
    this->play(NO_WINNER);
    return ;
  }

  if (GAMETYPE::is_solo(this->game().type())) {
    this->play(  (this->game().winnerteam() == TEAM::RE)
               ? SOLOPLAYER_WON
               : SOLOPLAYER_LOST);
  } else { // if (GAMETYPE::is_solo(this->game().type()))
    this->play(  (this->game().winnerteam() == TEAM::RE)
               ? RE_WON
               : CONTRA_WON);
  } // if !(GAMETYPE::is_solo(this->game().type()))
  return ;
} // void Sound::game_finish()


/**
 ** the marriage partner has found a bride
 **
 ** @param     bridegroom   player with the marriage
 ** @param     bride        player who is married
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::marriage(Player const& bridegroom, Player const& bride)
{
  this->play(bridegroom, MARRIAGE);
  return ;
} // void Sound::marriage(Player bridegroom, Player bride)

/**
 ** a player has announced a genscher
 **
 ** @param     genscher   player with the genscher
 ** @param     partner    selected partner
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::genscher(Player const& genscher, Player const& partner)
{
  this->play(genscher, GENSCHER);
  return ;
} // void Sound::genscher(Player genscher, Player partner)


// poverty

/**
 ** 'player' shifts 'cardno' cards
 **
 ** @param     player   player with the poverty
 ** @param     cardno   number of shifted cards
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::poverty_shift(Player const& player, unsigned const cardno)
{
#ifdef POSTPONED
  this->play(player, "poverty: "
             + DK::Utils::String::to_string(cardno) + " cards");
#else
  this->play(player, POVERTY);
#endif
  return ;
} // void Sound::poverty_shift(Player player, unsigned cardno)

/**
 ** the player 'player' has denied the poverty trumps
 **
 ** @param     player   player who has denied the poverty
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::poverty_take_denied(Player const& player)
{
  this->play(player, POVERTY_DENIED);
  return ;
} // void Sound::poverty_take_denied(Player player)

/**
 ** and has returned 'cardno' cards with 'trumpno' trumps
 **
 ** @param     player    player who has accepted the poverty
 ** @param     cardno    number of reshifted cards
 ** @param     trumpno   number of reshifted trumps
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::poverty_take_accepted(Player const& player,
                             unsigned const cardno,
                             unsigned const trumpno)
{
#ifdef POSTPONED
  this->play(player, "poverty accepted: "
             + DK::Utils::String::to_string(trumpno) + " trumps");
#else
  this->play(player, POVERTY_ACCEPTED);
#endif
  return ;
} // void Sound::poverty_take_accepted(Player player, unsigned cardno, unsigned trumpno)


/**
 ** an announcement is made
 **
 ** @param     announcement   announcement made
 ** @param     player         player who has announced
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::announcement_made(Announcement const& announcement, Player const& player)
{
  if (announcement == ANNOUNCEMENT::NO120)
    this->play(player, ((player.team() == TEAM::RE)
                        ? ANNOUNCEMENT_RE
                        : ANNOUNCEMENT_CONTRA));
  else
    this->play(player, Sound::type(announcement));
  return ;
} // void Sound::announcement_made(Announcement announcement, Player player)

/**
 ** the player has swines
 **
 ** @param     player    player with the swines
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void Sound::swines_announced(Player const& player)
{
  this->play(player, SWINES);
  return ;
} // void Sound::swines_announced(Player const& player)

/**
 ** the player has hyperswines
 **
 ** @param     player   player with the hyperswines
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void Sound::hyperswines_announced(Player const& player)
{
  this->play(player, HYPERSWINES);
  return ;
} // void Sound::hyperswines_announced(Player player)

#endif // #ifdef USE_SOUND
