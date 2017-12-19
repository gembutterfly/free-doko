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

#ifndef GTKMM_CHANGELOG_HEADER
#define GTKMM_CHANGELOG_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"

#ifdef POSTPONED
// postponed till I can wrap the elements of a treeview
#define CHANGELOG_USE_TREEVIEW
#endif

#ifdef CHANGELOG_USE_TREEVIEW
#include <gtkmm/treemodel.h>
#endif
#include <gtkmm/textbuffer.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	the changelog dialog
 **
 ** @author	Diether Knof
 **
 ** @todo	use treeview for the different versions
 **/
class ChangeLog : public Base, public Gtk::StickyDialog {
#ifdef CHANGELOG_USE_TREEVIEW
  class Model : public Gtk::TreeModel::ColumnRecord {
    public:
      Model() :
        text()
    { this->add(this->text); }

      Gtk::TreeModelColumn<Glib::ustring> text;
  }; // class Model : public Gtk::TreeModel::ColumnRecord
#endif
  public:
  ChangeLog(Base* parent);
  ~ChangeLog() = default;

  void language_update();

  private:
  void init();

  private:
  Glib::RefPtr<Gtk::TextBuffer> title_buffer;

#ifdef CHANGELOG_USE_TREEVIEW
  std::map<DK::Utils::Version, Gtk::TreeModel::Row> text;
  Model* model = nullptr;
#else
  Glib::RefPtr<Gtk::TextBuffer> text_buffer;
#endif

  private: // unused
  ChangeLog() = delete;
  ChangeLog(ChangeLog const&) = delete;
  ChangeLog& operator=(ChangeLog const&) = delete;
}; // class ChangeLog : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_CHANGELOG_HEADER

#endif // #ifdef USE_UI_GTKMM
