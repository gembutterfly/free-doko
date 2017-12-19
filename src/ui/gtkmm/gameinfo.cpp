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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "gameinfo.h"

#include "ui.h"
#include "translations.h"
#include "icons.h"

#include <gtkmm/image.h>
#include <gtkmm/label.h>

#ifdef USE_NETWORK
#include "../../network/server.h"
#endif

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   the parent object
 ** @param     title    the title
 **
 ** @return    -
 **/
GameInfoDialog::Information::Information(Base* const parent,
                                         ::Translator::Translation const& title) :
  Base(parent),
  Gtk::VBox(false, 1 EX),
  title(title),
  icon(this->ui->icons->new_managed_image()),
  label(Gtk::manage(new Gtk::Label("")))
{
  this->ui->translations->add(*this->label, ::translation(""));
  this->set_border_width(1 EX);

  return ;
} // GameInfoDialog::Information::Information(Base* parent, Translation title)

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **/
GameInfoDialog::Information::~Information()
{ }

/**
 ** initializes the information
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameInfoDialog::Information::init()
{
  if (this->icon)
    this->add(*this->icon);
  if (this->label)
    this->add(*this->label);

  this->show_all();

  this->update_texts();

  return ;
} // virtual void GameInfoDialog::Information::init()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   player whose name has changed
 **
 ** @return    -
 **/
void
GameInfoDialog::Information::name_changed(Player const& player)
{
  this->update_texts();
  return ;
} // void GameInfoDialog::Information::name_changed(Player player)

/**
 ** update the texts
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameInfoDialog::Information::update_texts()
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the information is blocking the gameplay
 **/
bool
GameInfoDialog::Information::is_blocking() const
{
#ifdef USE_NETWORK
  return (!server || server->empty());
#else
  return true;
#endif
} // virtual bool GameInfoDialog::Information::is_blocking() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
