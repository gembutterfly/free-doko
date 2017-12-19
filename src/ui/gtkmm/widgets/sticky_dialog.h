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

#ifndef GTKMM_WIDGET_STICKY_DIALOG_HEADER
#define GTKMM_WIDGET_STICKY_DIALOG_HEADER

#include <gtkmm/dialog.h>

namespace UI_GTKMM_NS {
  class MainWindow;
}
namespace Gtk {
  /**
   ** @brief	a dialog which remembers its place and forwards the key presses
   **		(workaround)
   **
   ** @author	Diether Knof
   **/
  class StickyDialog : public Dialog {
    public:
      // constructor
      StickyDialog(Glib::ustring const& title,
                   bool modal = false);

      // constructor
      StickyDialog(Glib::ustring const& title,
                   UI_GTKMM_NS::MainWindow& main_window,
                   bool modal = false);

      // destructor
      ~StickyDialog() = default;

    protected:
      virtual bool on_key_press_event(GdkEventKey* key);
      virtual void on_hide();
      virtual void on_show();

    private:
      int x = INT_MAX;
      int y = INT_MAX;
      UI_GTKMM_NS::MainWindow* const main_window = nullptr;

    private: // unused
      StickyDialog();
      StickyDialog(StickyDialog const&);
      StickyDialog& operator=(StickyDialog const&);
  }; // class StickyDialog : public Dialog

} // namespace Gtk

#endif // #ifdef GTKMM_WIDGET_STICKY_DIALOG_HEADER

#endif // #ifdef USE_UI_GTKMM
