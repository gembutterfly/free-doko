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

#ifdef USE_SOUND

#ifndef CLASS_SOUND_HEADER
#define CLASS_SOUND_HEADER

#include "../os/os.h"
class Player;

/**
 ** @brief    the base sound class
 **
 ** @author   Diether Knof
 **
 ** @todo     enum (see UI::GTKMM::Icon)
 **/
class Sound : public OS {
  public:
#ifdef USE_SOUND_ALUT
    class Alut;
#endif
#ifdef USE_SOUND_COMMAND
    class Command;
#endif
#ifdef USE_SOUND_PLAYSOUND
    class WinPlaySound;
#endif

  public:
    enum Type {
      FIRST,
      GAMETYPE_NORMAL = FIRST,
      GAMETYPE_THROWN_NINES,
      GAMETYPE_THROWN_KINGS,
      GAMETYPE_THROWN_NINES_AND_KINGS,
      GAMETYPE_THROWN_RICHNESS,
      GAMETYPE_FOX_HIGHEST_TRUMP,
      GAMETYPE_POVERTY,
      GAMETYPE_GENSCHER,
      GAMETYPE_MARRIAGE,
      GAMETYPE_MARRIAGE_SOLO,
      GAMETYPE_MARRIAGE_SILENT,
      GAMETYPE_SOLO_MEATLESS,
      GAMETYPE_SOLO_JACK,
      GAMETYPE_SOLO_QUEEN,
      GAMETYPE_SOLO_KING,
      GAMETYPE_SOLO_QUEEN_JACK,
      GAMETYPE_SOLO_KING_JACK,
      GAMETYPE_SOLO_KING_QUEEN,
      GAMETYPE_SOLO_KOEHLER,
      GAMETYPE_SOLO_CLUB,
      GAMETYPE_SOLO_HEART,
      GAMETYPE_SOLO_SPADE,
      GAMETYPE_SOLO_DIAMOND,
      MARRIAGE_SELECTOR_TEAM_SET,
      MARRIAGE_SELECTOR_FIRST_FOREIGN,
      MARRIAGE_SELECTOR_FIRST_TRUMP,
      MARRIAGE_SELECTOR_FIRST_COLOR,
      MARRIAGE_SELECTOR_FIRST_CLUB,
      MARRIAGE_SELECTOR_FIRST_SPADE,
      MARRIAGE_SELECTOR_FIRST_HEART,
      MARRIAGE_SELECTOR_SILENT,
      CARDS_DISTRIBUTED,
      TRICK_MOVE_IN_PILE,
      CARD_PLAYED,
      POVERTY,
      POVERTY_DENIED,
      POVERTY_ACCEPTED,
      MARRIAGE,
      GENSCHER,
      ANNOUNCEMENT_NOANNOUNCEMENT,
      ANNOUNCEMENT_RE,
      ANNOUNCEMENT_CONTRA,
      ANNOUNCEMENT_NO120,
      ANNOUNCEMENT_NO90,
      ANNOUNCEMENT_NO60,
      ANNOUNCEMENT_NO30,
      ANNOUNCEMENT_NO0,
      ANNOUNCEMENT_REPLY,
      ANNOUNCEMENT_NO120_REPLY,
      ANNOUNCEMENT_NO90_REPLY,
      ANNOUNCEMENT_NO60_REPLY,
      ANNOUNCEMENT_NO30_REPLY,
      ANNOUNCEMENT_NO0_REPLY,
      SWINES,
      HYPERSWINES,
      NO_WINNER,
      SOLOPLAYER_WON,
      SOLOPLAYER_LOST,
      RE_WON,
      CONTRA_WON,
      LAST = CONTRA_WON
    }; // enum Type

    // returns the name of the sound
    static string name(Sound::Type const type);
    // returns the text of the sound
    static string text(Sound::Type const type);
    // returns the file name for the sound
    static string filename(Sound::Type const type);
    // returns the path of the soundfile
    static string path(Position const& position,
                       string const& voice,
                       Type const type);

    // returns the type for the gametype
    static Type type(GameType const gametype);
    // returns the type for the gametype
    static Type type(MarriageSelector const marriage_selector);
    // returns the type for the announcement
    static Type type(Announcement const announcement);

  public:
    // create a sound class
    static Sound* new_();

  public:
    // constructor
    Sound();
    // destructor
    virtual ~Sound();


    // play the sound of the voice
    virtual void play(Position const& position,
                      string const& voice,
                      Type const type) = 0;

    // play the sound (global group)
    virtual void play(Type const type);
    // play the sound (group of the player)
    virtual void play(Player const& player, Type const type);

    // whether the sound is still playing
    virtual bool playing() const = 0;


    // card sounds

    // the cards are distributed
    void game_cards_distributed();
    // the trick is moved into a pile
    void trick_move_in_pile();
    // the card is played
    void card_played(HandCard const& card);


    // reservations

    // the game is started
    void game_start();
    // the game is finished
    void game_finish();


    // special games

    // the marriage partner has found a bride
    void marriage(Player const& bridegroom, Player const& bride);
    // a player has announced a genscher
    void genscher(Player const& genscher, Player const& partner);


    // poverty

    // 'player' shifts 'cardno' cards
    void poverty_shift(Player const& player, unsigned const cardno);
    // the player 'player' has denied the poverty trumps
    void poverty_take_denied(Player const& player);
    // the player 'player' has accepted the poverty trumps
    // and has returned 'cardno' cards with 'trumpno' trumps
    void poverty_take_accepted(Player const& player,
                               unsigned const cardno,
                               unsigned const trumpno);


    // announcements

    // an announcement is made
    void announcement_made(Announcement const& announcement,
                           Player const& player);
    // the player has swines
    void swines_announced(Player const& player);
    // the player has hyperswines
    void hyperswines_announced(Player const& player);

  private: // unused
    Sound(Sound const&);
    Sound& operator=(Sound const&);
}; // class Sound : public OS

#endif // #ifndef CLASS_SOUND_HEADER

#endif // #ifdef USE_SOUND
