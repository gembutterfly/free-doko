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

#ifdef USE_UI_GTKMM

#include "game_review.h"
#include "game_finished.h"
#include "trick_summary.h"

#include "ui.h"
#include "translations.h"
#include "table.h"

#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../player/player.h"

#include "widgets/stock_button.h"
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>
#include <gdk/gdkkeysyms.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     game_finished   parent object
 **
 ** @return    -
 **/
GameReview::GameReview(GameFinished* const game_finished) :
  Base(game_finished),
  trickno_(0)
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether a trick is visible (else: starting hands or all tricks in trickpiles)
 **/
bool
GameReview::trick_visible() const
{
  return (   (this->trickno() > 0)
          && (this->trickno() <= this->ui->game().trickno()) );
} // bool GameReview::trick_visible() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the current trick that is viewed
 **/
::Trick const&
GameReview::trick() const
{
  DEBUG_ASSERTION((this->trickno() != UINT_MAX),
                  "GameReview::trickno():\n"
                  "  'this->trickno()' == UINT_MAX");

  if (this->trickno() == 0)
    return ::Trick::EMPTY;
  return this->ui->game().trick(this->trickno() - 1);
} // ::Trick const& GameReview::trick() const

/**
 ** set the trickno
 **
 ** @param     trickno   new trickno
 **
 ** @return    -
 **/
void
GameReview::set_trickno(unsigned const trickno)
{
  this->trickno_ = trickno;

  this->ui->table->draw_all();

  return ;
} // void GameReview::set_trickno(unsigned trickno)

/**
 ** show the previous trickno
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameReview::previous_trick()
{
  if (this->trickno() > 0)
    this->set_trickno(this->trickno() - 1);

  return ;
} // void GameReview::previous_trick()

/**
 ** show the following trickno
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameReview::following_trick()
{
  if (this->trickno() <= this->ui->game().trickno())
    this->set_trickno(this->trickno() + 1);

  return ;
} // void GameReview::following_trick()

/**
 ** a key has been pressed
 **
 ** @param     key   the key
 **
 ** @return    from 'ui->key_press(key)'
 **
 ** @bug	signal does not get here for '->', '<-' and '\<Esc\>'
 **/
bool
GameReview::key_press(GdkEventKey const* const key)
{
  // whether the key was accepted
  bool accepted = false;

  if ((key->state & GDK_SHIFT_MASK) == 0) {
    switch (key->keyval) {
    case GDK_KEY_Left: // show previous trick
    case GDK_KEY_Down: // show previous trick
    case GDK_KEY_BackSpace: // show previous trick
      this->previous_trick();
      accepted = true;
      break;
    case GDK_KEY_Right: // show following trick
    case GDK_KEY_Up: // show following trick
    case GDK_KEY_space: // show following trick
      this->following_trick();
      accepted = true;
      break;
    } // switch (key->keyval)
  } // if (key->state == 0)

  return accepted;
} // bool GameReview::key_press(GdkEventKey* key)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
