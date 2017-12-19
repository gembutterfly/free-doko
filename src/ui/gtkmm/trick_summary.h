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
 **********************************************************************/

#ifdef USE_UI_GTKMM

#ifndef GTKMM_TRICK_SUMMARY_HEADER
#define GTKMM_TRICK_SUMMARY_HEADER

#include "base.h"
#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include "trick_drawing.h"

class Trick;
class Player;

namespace Gtk {
class DrawingArea;
class Label;
class TreeView;
} // namespace Gtk

namespace UI_GTKMM_NS {
class TrickDrawing;

/**
 ** @brief	the summary of a trick
 **
 ** @todo	make the background as in 'table'
 ** @todo	name update
 **/
class TrickSummary : public Base, public Gtk::HBox {

  /**
   ** @brief	the columns of the trickpoints table
   **/
  class SpecialpointsModel : public Gtk::TreeModel::ColumnRecord {
    public:
      SpecialpointsModel()
      {
        this->add(this->special_point);
        this->add(this->player);
      }
      Gtk::TreeModelColumn<Glib::ustring> special_point;
      Gtk::TreeModelColumn<Glib::ustring> player;
  }; // class SpecialpointsModel : public Gtk::TreeModel::ColumnRecord

  public:
  // constructor
  TrickSummary(Base* parent, bool show_trick_picture = true);
  // destructor
  ~TrickSummary();

  // set the trick
  void set_trick(::Trick const& trick);
  // remove the trick
  void remove_trick();
  // update all
  void update();
  // update the trick picture
  void update_trick_picture();

  private:
  bool on_draw_trick(Cairo::RefPtr<Cairo::Context> const& cr);
  // update the cards
  void cards_update();
  // a setting has been updated
  void setting_update(int type);
  // update the language
  void language_update();

  private:
  // initialize the window
  void init();

  private:
  ::Trick const* trick;

  bool const show_trick_picture = true;
  TrickDrawing trick_drawing;
  Gtk::DrawingArea* trick_picture = nullptr;
  Gtk::Label* winner = nullptr;
  Gtk::Label* points = nullptr;
  SpecialpointsModel specialpoints_model;
  Glib::RefPtr<Gtk::ListStore> specialpoints_list;
  Gtk::TreeView* specialpoints_treeview = nullptr;

  private: // unused
  TrickSummary() = delete;
  TrickSummary(TrickSummary const&) = delete;
  TrickSummary& operator=(TrickSummary const&) = delete;
}; // class TrickSummary : public Base, public Gtk::VBox

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TRICK_SUMMARY_HEADER

#endif // #ifdef USE_UI_GTKMM
