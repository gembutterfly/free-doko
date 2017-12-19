/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Trick
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

#include "trick_summary.h"

#include "trick_drawing.h"
#include "ui.h"
#include "translations.h"
#include "table.h"

#include "../../game/game.h"
#include "../../player/player.h"
#include "../../card/trick.h"

#include <gtkmm/alignment.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gtkmm/treeview.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent               parent widget
 ** @param     show_trick_picture   whether to show the trick picture
 **
 ** @return    -
 **/
TrickSummary::TrickSummary(Base* const parent,
                           bool const show_trick_picture) :
  Base{parent},
  HBox(),
  show_trick_picture{show_trick_picture},
  trick_drawing{this}
{
  this->signal_realize().connect(sigc::mem_fun(*this,
                                               &TrickSummary::init));

  return ;
} // TrickSummary::TrickSummary(Base* parent, bool show_trick_picture = true)

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
TrickSummary::~TrickSummary()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickSummary::init()
{
  this->set_spacing(10);

  if (show_trick_picture) {
    if (this->trick)
      this->trick_drawing.set_trick(*this->trick);

    this->trick_picture = Gtk::manage(new Gtk::DrawingArea());
    this->trick_picture->set_size_request(this->trick_drawing.width()
                                          * 6 / 5,
                                          this->trick_drawing.height()
                                          * 6 / 5);
    this->trick_picture->signal_draw().connect(sigc::mem_fun(*this,
                                                             &TrickSummary::on_draw_trick));

    this->pack_start(*this->trick_picture, false, true);
  } // if (show_trick_picture)

  { // info box
    auto info_box = Gtk::manage(new Gtk::VBox(true));
    info_box->set_border_width(2 EX);
    this->pack_end(*info_box);

    { // the labels
      auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_END, 0, 0));
      auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX));
      vbox->set_border_width(2 EX);
      alignment->add(*vbox);

      this->winner = Gtk::manage(new Gtk::Label("Winner:"));
      this->ui->translations->add(*(this->winner),
                                  ::translation("The trick goes to %swinner%.",
                                                this->trick->winnerplayer().name()));
      vbox->pack_start(*this->winner, false, true);

      this->points = Gtk::manage(new Gtk::Label("Points:"));
      this->ui->translations->add(*this->points,
                                  ::translation("Points in the trick: %upoints%",
                                                this->trick->points()));

      vbox->pack_start(*this->points, false, true);

      info_box->pack_start(*alignment, false, true);
    } // the labels

    { // specialpoints
      auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_START, 0, 0));
      auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX));
      vbox->set_border_width(2 EX);
      alignment->add(*vbox);

      this->specialpoints_list
        = Gtk::ListStore::create(this->specialpoints_model);
      this->specialpoints_treeview
        = Gtk::manage(new Gtk::TreeView(this->specialpoints_list));
      this->specialpoints_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

      this->specialpoints_treeview->append_column("~specialpoint",
                                                  this->specialpoints_model.special_point);
      this->specialpoints_treeview->append_column("Player",
                                                  this->specialpoints_model.player);

      vbox->pack_start(*(this->specialpoints_treeview), false, true);

      info_box->pack_end(*alignment, false, true);
    } // specialpoints
  } // info box

  this->show_all();
  this->update();
  this->language_update();

  return ;
} // void TrickSummary::init()

/**
 ** set the trick
 **
 ** @param     trick   the trick to show the info of
 **
 ** @return    -
 **/
void
TrickSummary::set_trick(::Trick const& trick)
{
  this->trick = &trick;
  this->trick_drawing.set_trick(trick);
  this->update();

  return ;
} // void TrickSummary::set_trick(::Trick trick)

/**
 ** removes the trick
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickSummary::remove_trick()
{
  this->trick = nullptr;
  this->trick_drawing.remove_trick();

  return ;
} // void TrickSummary::remove_trick()

/**
 ** update (rewrite) all information
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	if there is no 'player_of' don't show the column
 **/
void
TrickSummary::update()
{
  if (!this->get_realized())
    return ;

  if (this->trick == nullptr)
    return ;

  this->ui->translations->change(*this->winner,
                                 ::translation("The trick goes to %swinner%.",
                                               this->trick->winnerplayer().name()));
  this->ui->translations->change(*this->points,
                                 ::translation("Points in the trick: %upoints%",
                                               this->trick->points()));

  this->update_trick_picture();

  { // special points

    this->specialpoints_list->clear();

    bool valid_point = false;
    auto const specialpoints = this->trick->specialpoints();
    for (auto sp : specialpoints) {
      // test whether to show the point
      if (sp.player_get_no == UINT_MAX)
#ifndef RELEASE
        // no 'player of' (can this happen?)
        DEBUG_ASSERTION(false,
                        "Gtkmm::TrickSummary(trick):\n"
                        "  no 'player_get_no' in specialpoint");
#else
      continue;
#endif
      else if (sp.player_of_no != UINT_MAX) {
        auto const team_get = this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_get_no));
        auto const team_of = this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_of_no));

        if ( ( (team_get == TEAM::RE)
              || (team_get == TEAM::CONTRA) )
            && (team_get == team_of) )
          continue;
      } // if !(player_get_no == UINT_MAX) && (player_of_no != UINT_MAX)

      valid_point |= true;

      auto row = *(this->specialpoints_list->append());
      row[this->specialpoints_model.special_point]
        = ::translation(sp.type).translation();
      if (sp.player_of_no != UINT_MAX)
        row[this->specialpoints_model.player]
          = this->trick->game().player(sp.player_of_no).name();
      else
        row[this->specialpoints_model.player] = "";
    } // for (sp : specialpoints)

    this->specialpoints_treeview->show_all();

    if (valid_point)
      this->specialpoints_treeview->show();
    else
      this->specialpoints_treeview->hide();

  } // special points

  return ;
} // void TrickSummary::update()

/**
 ** update the trick in the drawing area
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickSummary::update_trick_picture()
{
  if (   (this->trick == nullptr)
      || (this->trick_picture == nullptr))
    return ;

  this->trick_drawing.set_center({this->trick_picture->get_width() / 2,
                                 this->trick_picture->get_height() / 2});
  this->trick_drawing.draw(this->trick_picture->get_window()->create_cairo_context());

  return ;
} // void TrickSummary::update_trick_picture()

/**
 ** update the trick in the drawing area
 **
 ** @param     cr    cairo context
 **
 ** @return    true
 **/
bool
TrickSummary::on_draw_trick(Cairo::RefPtr<Cairo::Context> const& cr)
{
  this->trick_drawing.set_center({this->trick_picture->get_width() / 2,
                                 this->trick_picture->get_height() / 2});
  this->trick_drawing.draw(cr);
  return true;
} // bool TrickSummary::on_draw_trick(Cairo::RefPtr<Cairo::Context> const& cr)

/**
 ** updates the cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickSummary::cards_update()
{
  if (!this->get_realized())
    return ;

  if (this->trick_picture != nullptr)
    this->trick_picture->set_size_request(this->trick_drawing.width()
                                          * 6 / 5,
                                          this->trick_drawing.height()
                                          * 6 / 5);

  return ;
} // void TrickSummary::cards_update()

/**
 ** the setting has changed
 **
 ** @param     type   the type that has changed
 **
 ** @return    -
 **/
void
TrickSummary::setting_update(int const type)
{
  switch(type) {
  case Setting::ROTATE_TRICK_CARDS:
    if (this->trick_picture != nullptr) {
      this->trick_picture->set_size_request(this->trick_drawing.width()
                                            * 6 / 5,
                                            this->trick_drawing.height()
                                            * 6 / 5);
    } // if (this->trick_picture != nullptr)
    this->update_trick_picture();
    break;
  case Setting::LANGUAGE:
    this->update();
    break;
  default:
    break;
  } // switch(type)

  return ;
} // void TrickSummary::setting_update(int const type)

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickSummary::language_update()
{
  if (!this->get_realized())
    return ;

  this->specialpoints_treeview->get_column(0)->set_title(::translation("specialpoint").translation());
  this->specialpoints_treeview->get_column(1)->set_title(::translation("of player").translation());

  this->update();

  return ;
} // void TrickSummary::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
