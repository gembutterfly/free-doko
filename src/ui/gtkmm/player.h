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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_PLAYER_HEADER
#define GTKMM_PLAYER_HEADER

#include "base.h"
#include "players.h"
#include "../../player/player.h"
typedef ::Player::Type PlayerType;

#include <sigc++/trackable.h>
namespace Gtk {
class Label;
class Button;
class StockButton;
class Entry;
class RadioButton;
class FileMenu;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the configuration of one player (without aiconfig)
 **/
class Players::Player : public Base, public sigc::trackable {
  public:
    friend class Players;

  public:
    class AiConfig;

  public:
    // constructor
    Player(Players* players, unsigned playerno);
    // destructor
    ~Player();

    // the player
    ::Player const& player() const;
    // the player
    ::Player& player();
    // the player number
    unsigned playerno() const;

    // the aiconfig
    AiConfig& aiconfig();

    // create a backup
    void create_backup();
    // reset the player
    void reset();

    // event: the name has changed
    bool name_changed_event(GdkEventFocus*);
    // set a random name
    void random_name();
    // select a voice
    void voice_selected(string const& voice);
    // change the type
    void type_change(PlayerType type);
    // update the aiconfig
    void aiconfig_update();

    // update the sensitivity
    void sensitivity_update();
    // update all
    void update();
    // update the name
    void name_update();
    // update the voice
    void voice_update();
    // update the translation
    void language_update();

  private:
    // initialize all elements
    void init();

  private:
    Players* const players = nullptr;

    std::unique_ptr<AiConfig> aiconfig_;

    unsigned const playerno_ = UINT_MAX;
    std::unique_ptr<::Player const> player_bak;

  protected:
    Gtk::Label* name_label = nullptr;
    Gtk::Entry* name_entry = nullptr;
    Gtk::Button* random_name_button = nullptr;
    Gtk::Button* voice_button = nullptr;
    Gtk::FileMenu* voice_menu = nullptr;
    vector<Gtk::RadioButton*> type_button;
    Gtk::StockButton* reset_button = nullptr;

  private: // unused
    Player() = delete;
    Player(Player const&) = delete;
    Player& operator=(Player const&) = delete;
}; // class Players::Player : public Base, public Gtk::Alignment

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PLAYER_HEADER

#endif // #ifdef USE_UI_GTKMM
