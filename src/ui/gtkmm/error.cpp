/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Error as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Error, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Error for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Error
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

#include "error.h"

#include "bug_report.h"
#include "ui.h"
#include "translations.h"

#include "../../misc/setting.h"
#include "../../misc/translator.h"

#include "widgets/stock_button.h"

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     message   message to display
 ** @param     parent    the parent object
 **
 ** @return    -
 **/
Error::Error(string const& message, Base* const parent) :
  Base(parent),
  Gtk::MessageDialog("FreeDoko error",
                     false,
                     Gtk::MESSAGE_ERROR, Gtk::BUTTONS_NONE,
#ifdef RELEASE
                     true
#else
                     false
#endif
                    ),
  message(message)
  {
    this->set_title("FreeDoko " + ::translation("error").translation());
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      this->set_icon(Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE)));
    } catch (Glib::FileError file_error) {
    } catch (Gdk::PixbufError pixbuf_error) {
    } // try
#else
    std::auto_ptr<Glib::Error> error;
    this->set_icon(Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE), error));
#endif

    this->set_message(::translation("BugReport::bug found, create report, %scontact%",
                                    CONTACT).translation()
                      + "\n\n"
                      + ::translation("BugReport::message").translation() + ":"
                      + "\n"
                      + message);
    auto bug_report_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK,
                                         "BugReport::create"));
    this->ui->translations->add(*bug_report_button,
                                ::translation("BugReport::create"));
    this->add_action_widget(*bug_report_button, Gtk::RESPONSE_CLOSE);
    bug_report_button->show_all();

    bug_report_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                              &Error::create_bug_report)
                                               );

    return ;
  } // Error::Error(string const& message, Base const* parent)

/**
 ** show the bug report window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Error::create_bug_report()
{
  BugReport bug_report(this, this->message + "\n---\n\n");

  bug_report.run();
  this->hide();

  return ;
} // void Error::create_bug_report()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
