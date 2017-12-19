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

#ifndef GTKMM_GAMEINFO_HEADER
#define GTKMM_GAMEINFO_HEADER

#include "gameinfo_dialog.h"

#include "../../misc/translation.h"

class Player;

#include <gtkmm/box.h>
namespace Gtk {
class Image;
class Label;
}; // namespace Gtk

namespace UI_GTKMM_NS {
class Table;

/**
 ** @brief	base class for game informations
 **
 ** @author	Diether Knof
 **
 ** @todo     reply button
 **/
class GameInfoDialog::Information : public Base, public Gtk::VBox {
  public:
    // child classes
    class GameType;
    class Marriage;
    class Genscher;
    class Announcement;
    class Swines;
    class Hyperswines;

  public:
    friend class GameInfoDialog;

  public:
    // constructor
    Information(Base* parent, ::Translator::Translation const& title);
    // destructor
    virtual ~Information();

    // the name of 'player' has changed
    virtual void name_changed(Player const& player);

  protected:
    // initialize the game information
    // to be called from the children
    virtual void init();

    // update the texts
    virtual void update_texts();

    // whether the information is blocking the gameplay
    virtual bool is_blocking() const;

  protected:
    // the title
    ::Translator::Translation title;
    // the icon
    Gtk::Image* icon = nullptr;
    // the text
    Gtk::Label* label = nullptr;

  private: // unused
    Information() = delete;
    Information(Information const&) = delete;
    Information& operator=(Information const&) = delete;
}; // class GameInfoDialog::Information : public Base, public Gkt::HBox

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAMEINFO_HEADER

#endif // #ifdef USE_UI_GTKMM
