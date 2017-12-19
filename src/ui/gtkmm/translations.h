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

#ifndef GTKMM_TRANSLATIONS_HEADER
#define GTKMM_TRANSLATIONS_HEADER

#include "base.h"

#include "../../misc/translations.h"

namespace Gtk {
class Label;
class StockButton;
class Button;
class Window;
class TextView;
class Widget;
} // namespace Gtk
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

namespace UI_GTKMM_NS {
class UI_GTKMM;

/**
 ** @brief	automatic translation for the widgets
 **/
class Translations : public Base, public sigc::trackable {
  public:
    // constructor
    Translations(UI_GTKMM* ui);
    // destructor
    ~Translations() = default;

    // updates the translations
    void language_update();


    // automatic translate the text of the label
    void add(Gtk::Label& label,
             Translator::Translation const& translation);
    // automatic translate the text of the stock button
    void add(Gtk::StockButton& stock_button,
             Translator::Translation const& translation);
    // automatic translate the text of the button
    void add(Gtk::Button& button,
             Translator::Translation const& translation);
    // automatic translate the text of the window title
    void add(Gtk::Window& window,
             Translator::Translation const& title);
    // automatic translate the text of the textview
    void add(Gtk::TextView& text_view,
             Translator::Translation const& translation);
    // change the translation of the widget
    void change(Gtk::Widget& widget,
                Translator::Translation const& translation);
    // change the translation of the stock button
    void change(Gtk::StockButton& stock_button,
                Translator::Translation const& translation);
    // change the translation of the text view
    void change(Gtk::TextView& text_view,
                Translator::Translation const& translation);
    // remove the widget from the automatic translation
    void remove(Gtk::Widget const& widget);


    // signal: add a label to the translations
    void add_label_signal(Gtk::Label* label,
                          Translator::Translation translation);
    // signal: add a button to the translations
    void add_button_signal(Gtk::Button* button,
                           Translator::Translation translation);
    // signal: add a window to the translations
    void add_window_signal(Gtk::Window* window,
                           Translator::Translation title);
    // signal: add a text view to the translations
    void add_text_view_signal(Gtk::TextView* text_view,
                              Translator::Translation translation);

    // signal: a translation has changed
    void change_signal(Gtk::Widget* widget,
                       Translator::Translation translation);

    // signal: a widget shall be removed
    void remove_signal(Gtk::Widget const* widget);

  private:
    // for the automatic translation
    // the data "translation" is a pointer to the translation
    vector<Gtk::Label*> label;
    vector<Gtk::Button*> button;
    vector<Gtk::Window*> window; // translates the title
    vector<Gtk::TextView*> text_view;

  private: // unused
    Translations(Translations const&) = delete;
    Translations& operator=(Translations const&) = delete;
}; // class Translations : public Base, public sigc::trackable

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TRANSLATIONS_HEADER

#endif // #ifdef USE_UI_GTKMM
