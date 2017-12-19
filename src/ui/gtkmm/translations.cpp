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

#include "translations.h"

#include "ui.h"

#include "widgets/stock_button.h"
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>

namespace UI_GTKMM_NS {

  /**
   ** Constructor
   **
   ** @param     ui   ui
   **
   ** @return    -
   **/
  Translations::Translations(UI_GTKMM* const ui) :
    Base(ui),
    sigc::trackable()
  { }

  /**
   ** updates all translations
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Translations::language_update()
    {
      for (auto l : this->label)
        l->set_text((static_cast<Translator::Translation*>(l->get_data("translation"))->translation()));

      for (auto b : this->button)
        b->set_label((static_cast<Translator::Translation*>(b->get_data("translation"))->translation()));

      for (auto w : this->window)
        w->set_title((static_cast<Translator::Translation*>(w->get_data("translation"))->translation()));

      for (auto tv : this->text_view)
        tv->get_buffer()->set_text((static_cast<Translator::Translation*>(tv->get_data("translation"))->translation()));

      return ;
    } // void Translations::language_update()

  /**
   ** add 'label' for automatic translation managed by the ui
   **
   ** @param     label   	label to add
   ** @param     translation   translation for the label
   **
   ** @return    -
   **/
  void
    Translations::add(Gtk::Label& label,
                      Translator::Translation const& translation)
    {
      if (!label.get_realized()) {
        // postphone the adding till the label is realized
        label.signal_realize().connect(sigc::bind<Gtk::Label* const, Translator::Translation const>(sigc::mem_fun(*this, &Translations::add_label_signal), &label, translation));

        return ;
      } // if (label.get_realized())

      label.set_text(translation.translation());

      label.set_data("translation", new Translator::Translation(translation));

      this->label.push_back(&label);

      label.signal_unrealize().connect(sigc::bind<Gtk::Widget const* const>(sigc::mem_fun(*this, &Translations::remove_signal), &label));

      return ;
    } // void Translations::add(Gtk::Label& label, Translator::Translation const& translation)

  /**
   ** add 'stock_button' for automatic translation managed by the ui
   **
   ** @param     stock_button   stock_button to add
   ** @param     translation   translation for the label
   **
   ** @return    -
   **/
  void
    Translations::add(Gtk::StockButton& stock_button,
                      Translator::Translation const& translation)
    {
      this->add(*stock_button.label, translation);

      return ;
    } // void Translations::add(Gtk::StockButton& stock_button, Translator::Translation const& translation)

  /**
   ** add 'button' for automatic translation managed by the ui
   **
   ** @param     button   	button to add
   ** @param     translation   translation for the button text
   **
   ** @return    -
   **/
  void
    Translations::add(Gtk::Button& button,
                      Translator::Translation const& translation)
    {
      if (!button.get_realized()) {
        // postphone the adding till the button is realized
        button.signal_realize().connect(sigc::bind<Gtk::Button* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::add_button_signal), &button, translation));

        return ;
      } // if (button.get_realized())

      button.set_label(translation.translation());

      button.set_data("translation", new Translator::Translation(translation));
      this->button.push_back(&button);

      button.signal_unrealize().connect(sigc::bind<Gtk::Widget const* const>(sigc::mem_fun(*this, &Translations::remove_signal), &button));

      return ;
    } // void Translations::add(Gtk::Button& button, Translator::Translation const& translation)

  /**
   ** add 'window' for automatic translation of the title, managed by the ui
   ** The window has to be treated in a special way (mapped instead of realized)
   **
   ** @param     window   window to add
   ** @param     title   title of the window
   **
   ** @return    -
   **/
  void
    Translations::add(Gtk::Window& window,
                      Translator::Translation const& title)
    {
      if (!window.get_mapped()) {
        // postphone the adding till the window is mapped
        window.signal_map().connect(sigc::bind<Gtk::Window* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::add_window_signal), &window, title));

        return ;
      } // if (window.get_mapped())

      window.set_title(title.translation());

      window.set_data("translation", new Translator::Translation(title));
      this->window.push_back(&window);

      window.signal_unmap().connect(sigc::bind<Gtk::Widget const* const>(sigc::mem_fun(*this, &Translations::remove_signal), &window));

      return ;
    } // void Translations::add(Gtk::Window& window, Translator::Translation const& title)

  /**
   **
   ** add 'text_view' for automatic translation of the text, managed by the ui
   **
   ** @param     text_view   text buffer to add
   ** @param     translation   translation for the text bufferk
   **
   ** @return    -
   **/
  void
    Translations::add(Gtk::TextView& text_view,
                      Translator::Translation const& translation)
    {
      if (!text_view.get_realized()) {
        // postphone the adding till the text_view is realized
        text_view.signal_realize().connect(sigc::bind<Gtk::TextView* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::add_text_view_signal), &text_view, translation));

        return ;
      } // if (text_view.get_realized())

      text_view.get_buffer()->set_text(translation.translation());

      text_view.set_data("translation", new Translator::Translation(translation));
      this->text_view.push_back(&text_view);

      text_view.signal_unrealize().connect(sigc::bind<Gtk::Widget const* const>(sigc::mem_fun(*this, &Translations::remove_signal), &text_view));

      return ;
    } // void Translations::add(Gtk::TextView& text_view, Translator::Translation const& translation)

  /**
   ** changes the translation for 'widget'
   **
   ** @param     widget   	widget to change the translation for
   ** @param     translation   new translation
   **
   ** @return    -
   **/
  void
    Translations::change(Gtk::Widget& widget,
                         Translator::Translation const& translation)
    {
      if (dynamic_cast<Gtk::Window*>(&widget)) {
        if (!static_cast<Gtk::Window&>(widget).get_mapped()) {
          widget.signal_map().connect(sigc::bind<Gtk::Widget* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::change_signal), &widget, translation));
          return ;
        } // if (widget not mapped)
      } else { // if (widget is Gt::Window)
        if (!widget.get_realized()) {
          widget.signal_realize().connect(sigc::bind<Gtk::Widget* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::change_signal), &widget, translation));
          return ;
        } // if (widget not realized)
      } // if (widget is Gt::Window)

      DEBUG_ASSERTION(widget.get_data("translation"),
                      "Translations::change(widget, translation):\n"
                      "  'widget' does not contain a translation.\n"
                      "  New translation: '" << translation.translation()
                      << "'");

      delete static_cast<Translator::Translation*>(widget.get_data("translation"));
      widget.set_data("translation", new Translator::Translation(translation));

      if (dynamic_cast<Gtk::Label*>(&widget))
        static_cast<Gtk::Label&>(widget).set_text(translation.translation());
      else if (dynamic_cast<Gtk::Button*>(&widget))
        static_cast<Gtk::Button&>(widget).set_label(translation.translation());
      else if (dynamic_cast<Gtk::Window*>(&widget))
        static_cast<Gtk::Window&>(widget).set_title(translation.translation());
      else if (dynamic_cast<Gtk::TextView*>(&widget))
        static_cast<Gtk::TextView&>(widget).get_buffer()->set_text(translation.translation());
      else
        DEBUG_ASSERTION(false,
                        "Translations::change_translation(object, translation):\n"
                        "  object not found.\n"
                        "  translation: " << translation.translation());

      return ;
    } // void Translations::change(Gtk::Widget& widget, Translator::Translation const& translation)

  /**
   ** changes the translation for 'stock_button'
   **
   ** @param     stock_button   stock button to change the translation for
   ** @param     translation   new translation
   **
   ** @return    -
   **/
  void
    Translations::change(Gtk::StockButton& stock_button,
                         Translator::Translation const& translation)
    {
      this->change(*stock_button.label, translation);

      return ;
    } // void Translations::change(Gtk::StockButton& stock_button, Translator::Translation const& translation)

  /**
   ** changes the translation for 'text_view'
   **
   ** @param     text_view     text view to change the translation for
   ** @param     translation   new translation
   **
   ** @return    -
   **/
  void
    Translations::change(Gtk::TextView& text_view,
                         Translator::Translation const& translation)
    {
      if (!text_view.get_realized()) {
        text_view.signal_realize().connect(sigc::bind<Gtk::Widget* const, Translator::Translation const >(sigc::mem_fun(*this, &Translations::change_signal), &text_view, translation));
      }

      delete static_cast<Translator::Translation*>(text_view.get_data("translation"));
      text_view.set_data("translation", new Translator::Translation(translation));

      text_view.get_buffer()->set_text(translation.translation());

      return ;
    } // void Translations::change(Gtk::TextView& text_view, Translator::Translation const& translation)

  /**
   ** remove 'widget' from the automatic translation list
   **
   ** @param     widget   widget to remove
   **
   ** @return    -
   **/
  void
    Translations::remove(Gtk::Widget const& widget)
    {
      for (vector<Gtk::Label*>::iterator l = this->label.begin();
           l != this->label.end();
          )
        if (*l == &widget)
          l = this->label.erase(l);
        else
          ++l;

      for (vector<Gtk::Button*>::iterator b = this->button.begin();
           b != this->button.end();
          )
        if (*b == &widget)
          b = this->button.erase(b);
        else
          ++b;

      for (vector<Gtk::Window*>::iterator w = this->window.begin();
           w != this->window.end();
          )
        if (*w == &widget)
          w = this->window.erase(w);
        else
          ++w;

      for (vector<Gtk::TextView*>::iterator tv
           = this->text_view.begin();
           tv != this->text_view.end();
          )
        if (*tv == &widget)
          tv = this->text_view.erase(tv);
        else
          ++tv;

      return ;
    } // void Translations::remove(Gtk::Widget const& widget)

  /**
   ** signal: a label shall be automatically translated
   **
   ** @param    label         widget to translate
   ** @param    translation   translation
   **
   ** @return   -
   **/
  void
    Translations::add_label_signal(Gtk::Label* const label,
                                   Translator::Translation const translation)
    {
      this->add(*label, translation);
      return ;
    }

  /**
   ** signal: a button shall be automatically translated
   **
   ** @param    button        widget to translate
   ** @param    translation   translation
   **
   ** @return   -
   **/
  void
    Translations::add_button_signal(Gtk::Button* const button,
                                    Translator::Translation const translation)
    {
      this->add(*button, translation);
      return ;
    }

  /**
   ** signal: a window shall be automatically translated
   **
   ** @param    window        widget to translate
   ** @param    translation   translation
   **
   ** @return   -
   **/
  void
    Translations::add_window_signal(Gtk::Window* const window,
                                    Translator::Translation const title)
    {
      this->add(*window, title);
      return ;
    }

  /**
   ** signal: a text view shall be automatically translated
   **
   ** @param    text_view     widget to translate
   ** @param    translation   translation
   **
   ** @return   -
   **/
  void
    Translations::add_text_view_signal(Gtk::TextView* const text_view,
                                       Translator::Translation const translation)
    {
      this->add(*text_view, translation);
      return ;
    }

  /**
   ** signal: the translation has been changed
   **
   ** @param    widget        widget
   ** @param    translation   new translation
   **
   ** @return   -
   **/
  void
    Translations::change_signal(Gtk::Widget* const widget,
                                Translator::Translation const translation)
    {
      this->change(*widget, translation);
      return ;
    }

  /**
   ** signal: remove a widget from the automatically translation
   **
   ** @param    widget    widget to remove
   **
   ** @return   -
   **/
  void
    Translations::remove_signal(Gtk::Widget const* const widget)
    {
      this->remove(*widget);
      return ;
    }

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
