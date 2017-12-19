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

#ifndef GTKMM_BUG_REPORT_HEADER
#define GTKMM_BUG_REPORT_HEADER

#include "base.h"

#include "widgets/sticky_dialog.h"

namespace Gtk {
class TextView;
class StockButton;
class CheckButton;
class FileChooserDialog;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the bug report dialog and selector
 **
 ** @author	Diether Knof
 **/
class BugReport : public Base, public Gtk::StickyDialog {
  public:
    BugReport(Base* parent, string message = "");
    ~BugReport();

    bool load(string const& filename);
    void load_report();

    // sets the widget as destination for the drop of a bug report
    void set_dnd_destination(Gtk::Widget& widget);

    void create_report();
    void save();

    void setting_update(int type);

  private:
    void init();

    void save_on_desktop_toggled_event();

    // drag-and-drop: a bug report was dropped
    void on_bug_report_dropped(Glib::RefPtr<Gdk::DragContext> const& context,
                               int x, int y,
                               Gtk::SelectionData const& selection_data,
                               guint info, guint time);
  private:
    Gtk::TextView* description = nullptr;
    Gtk::Label* file_label = nullptr;
    Gtk::CheckButton* save_on_desktop_button = nullptr;
    Gtk::TextView* message = nullptr;
    Gtk::StockButton* dismiss_button = nullptr;
    Gtk::StockButton* save_button = nullptr;

  public:
    Gtk::FileChooserDialog* load_file_chooser = nullptr;

  private: // unused
    BugReport() = delete;
    BugReport(BugReport const&) = delete;
    BugReport& operator=(BugReport const&) = delete;
}; // class BugReport : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_BUG_REPORT_HEADER

#endif // #ifdef USE_UI_GTKMM
