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

#include "bug_report.h"

#include "ui.h"
#include "translations.h"
#include "party_settings.h"
#include "table.h"

#include "../../misc/setting.h"
#include "../../os/bug_report.h"
#include "../../os/bug_report_replay.h"
#include "../ui.wrap.h"

#include "../../utils/file.h"
#include "../../utils/string.h"

#include "widgets/stock_button.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/alignment.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/checkbutton.h>

#include <cstring>

namespace UI_GTKMM_NS {

/// @todo   Mail the bug report directly:
///         ShellExecute(NULL,"open","mailto:adresse@bla.at?cc=adresse2@bla.at&subject=Überschrift&body=Text",NULL,NULL,NULL);
///         http://www.ianr.unl.edu/internet/mailto.html

/**
 ** constructor
 **
 ** @param     parent   the parent widget
 ** @param     message   the default message
 **
 ** @return    -
 **/
BugReport::BugReport(Base* const parent, string const message) :
  Base(parent),
  StickyDialog("BugReport", false)
{
  this->ui->add_window(*this);

  this->message = Gtk::manage(new Gtk::TextView());
  this->message->get_buffer()->set_text(message);

  { // load file chooser
    this->load_file_chooser = new Gtk::FileChooserDialog("load bug report",
                                                         Gtk::FILE_CHOOSER_ACTION_OPEN);
    this->ui->translations->add(*this->load_file_chooser, ::translation("~load bug report"));
#ifndef RELEASE
    // ToDo: add current directory
    if (DK::Utils::File::isdirectory(Glib::get_current_dir()
                                     + "/../Bugreports"))
      this->load_file_chooser->set_current_folder(Glib::get_current_dir()
                                                  + "/../Bugreports");
    else
      this->load_file_chooser->set_current_folder(Glib::get_current_dir());
#else
    this->load_file_chooser->set_current_folder(Glib::get_current_dir());
#endif
#ifdef POSTPONED
    {
      auto filter = new Gtk::FileFilter; // memory bug?
      filter->add_pattern("*.BugReport.FreeDoko");
      this->load_file_chooser->add_filter(*filter);
    }
#endif

    auto cancel_button
      = this->load_file_chooser->add_button(Gtk::Stock::CANCEL,
                                            Gtk::RESPONSE_CANCEL);
    auto ok_button
      = this->load_file_chooser->add_button(Gtk::Stock::OPEN,
                                            Gtk::RESPONSE_OK);

    { // signals
      this->load_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this, &BugReport::load_report));
      this->load_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this->load_file_chooser, &Widget::hide));
      ok_button->signal_clicked().connect(sigc::mem_fun(*this, &BugReport::load_report));
      ok_button->signal_clicked().connect(sigc::mem_fun(*this->load_file_chooser, &Widget::hide));
      cancel_button->signal_clicked().connect(sigc::mem_fun(*this->load_file_chooser, &Widget::hide));

      this->signal_realize().connect(sigc::mem_fun(*this, &BugReport::init));
    } // signals
  } // load file chooser

  this->set_dnd_destination(*this);

  return ;
} // BugReport::BugReport(Base* parent, string message = "")

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
BugReport::~BugReport()
{
  delete this->load_file_chooser;

  return ;
} // BugReport::~BugReport()

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReport::init()
{
  this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                   ::translation("~bug report")));

  this->set_icon(this->ui->icon);

  this->description
    = Gtk::manage(new Gtk::TextView());
  this->ui->translations->add(*this->description,
                              ::translation("BugReport::save"));
  this->file_label
    = Gtk::manage(new Gtk::Label("File: " + OS_NS::BUG_REPORT_FILE));
  this->ui->translations->add(*this->file_label,
                              ::translation("~file: %sfile%",
                                            ::bug_report->filename()));
  this->save_on_desktop_button
    = Gtk::manage(new Gtk::CheckButton("save on the desktop"));
  this->ui->translations->add(*dynamic_cast<Gtk::Label*>(this->save_on_desktop_button->get_child()),
                              ::translation("save on desktop"));
#if 0
  // this is initialized in the constructor
  this->message
    = Gtk::manage(new Gtk::TextView());
#endif
  this->dismiss_button
    = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CANCEL, "dismiss"));
  this->ui->translations->add(*this->dismiss_button,
                              ::translation("dismiss"));
  this->add_action_widget(*this->dismiss_button, Gtk::RESPONSE_CLOSE);
  this->save_button
    = Gtk::manage(new Gtk::StockButton(Gtk::Stock::SAVE, "save"));
  this->ui->translations->add(*this->save_button,
                              ::translation("save"));
  this->add_action_widget(*this->save_button, Gtk::RESPONSE_CLOSE);

  this->save_button->grab_default();

  this->set_default_size(this->ui->logo->get_width(),
                         static_cast<int>(this->ui->logo->get_height() * 1.5));

  { // the description
    this->description->set_editable(false);
    this->description->set_wrap_mode(Gtk::WRAP_WORD);
    this->description->set_cursor_visible(false);

    this->get_vbox()->pack_start(*this->description, false, true);
  } // the description
  this->get_vbox()->pack_start(*this->file_label, false, true);
  this->get_vbox()->pack_start(*this->save_on_desktop_button, false, true);
  { // the message
    auto message_window = Gtk::manage(new Gtk::ScrolledWindow());
    message_window->add(*this->message);
    message_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    this->message->set_editable(true);
    this->message->set_wrap_mode(Gtk::WRAP_WORD);
    this->message->set_cursor_visible(true);

    this->get_vbox()->pack_end(*message_window, true, true);
  } // the message

  this->show_all_children();
#ifndef WINDOWS
#ifndef LINUX
  this->save_on_desktop_button->hide();
#endif
#endif

  // signals
  this->dismiss_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  this->save_button->signal_clicked().connect(sigc::mem_fun(*this, &BugReport::save));
  this->save_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));

  this->language_update();

  // events
  this->save_on_desktop_button->signal_toggled().connect(sigc::mem_fun(*this, &BugReport::save_on_desktop_toggled_event));
  this->setting_update(Setting::SAVE_BUG_REPORTS_ON_DESKTOP);

  return ;
} // void BugReport::init()

/**
 ** load the given bug report
 **
 ** @param     filename   bug report to load
 **
 ** @return    whether the bug report could be loaded
 **/
bool
BugReport::load(string const& filename)
{
  if (::bug_report_replay)
    static_cast<UI_Wrap*>(::ui)->remove(bug_report_replay);

  // Note: '::bug_report_replay' is set automatically to the
  //       newly opened bug report.
  auto bug_report_replay_new
    = new OS_NS::BugReportReplay(filename,
                                 OS_NS::BugReportReplay::VERBOSE_ALL);
  // this is true, when the loading was successfull
  if (!bug_report_replay_new->loaded()) {
    delete bug_report_replay_new;
    return false;
  }

  if (   (::game_status == GAMESTATUS::PARTY_NEW)
      || (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED) )
    this->ui->party_settings->update();
  else if (::game_status >= GAMESTATUS::PARTY_LOADED) {
    this->ui->thrower(GAMESTATUS::PARTY_LOADED,
                      __FILE__, __LINE__);
  }

  return true;
} // bool BugReport::load(string const& filename)

/**
 ** load a report
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReport::load_report()
{
  this->load(this->load_file_chooser->get_filename());

  return ;
} // void BugReport::load_report()

/**
 ** creates a report
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReport::create_report()
{
  this->realize();

  this->message->get_buffer()->set_text("");
  if (this->file_label->get_realized())
    this->ui->translations->change(*this->file_label,
                                   ::translation("~file: %sfile%",
                                                 ::bug_report->filename()));
  this->message->grab_focus();

  this->present();

  return ;
} // void BugReport::create_report()

/**
 ** save the report
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReport::save()
{
  auto const filename
    = ::bug_report->report(this->message->get_buffer()->get_text()
                           + "\n"
                           + (this->ui->table->in_game_review()
                              ? ("---\n"
                                 + ("Game review, trick: " + DK::Utils::String::to_string(this->ui->table->game_review_trickno()) + "\n\n"
                                    + DK::Utils::String::to_string(this->ui->table->game_review_trick())
                                    +"\n")
                                )
                              : string()));

  cerr << "Saved bug report in the file '" << filename << "'." << endl;

  return ;
} // void BugReport::save()

/**
 ** the button 'save on desktop' has been toggled
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReport::save_on_desktop_toggled_event()
{
  ::setting.set(Setting::SAVE_BUG_REPORTS_ON_DESKTOP,
                this->save_on_desktop_button->get_active());

  return ;
} // void BugReport::save_on_desktop_toggled_event()

/**
 ** a setting has changed
 ** SAVE_BUG_REPORTS_ON_DESKTOP:
 **	update the toggle button
 **
 ** @param     type   the type that has changed
 **
 ** @return    -
 **/
void
BugReport::setting_update(int const type)
{
  if (!this->get_realized())
    return ;

  switch (type) {
  case Setting::SAVE_BUG_REPORTS_ON_DESKTOP:
    this->save_on_desktop_button->set_active(::setting(Setting::SAVE_BUG_REPORTS_ON_DESKTOP));
    break;
  default:
    break;
  } // switch(type)

  return ;
} // void BugReport::setting_update(int const type)

/**
 ** sets the widget as destination for the drop of a bug report
 **
 ** @param     widget   widget to set as destination
 **
 ** @return    -
 **/
void
BugReport::set_dnd_destination(Gtk::Widget& widget)
{
  std::vector<Gtk::TargetEntry> list_targets;
  list_targets.push_back(Gtk::TargetEntry("text/plain"));

  // set what is received
  widget.drag_dest_set(list_targets);

  //Connect signals:
  widget.signal_drag_data_received().connect(sigc::mem_fun(*this, &BugReport::on_bug_report_dropped));

  return ;
} // void BugReport::set_dnd_destination(Gtk::Widget& widget)

/**
 ** drag-and-drop:
 ** - bug report
 ** - tournament
 **
 ** @param     context          ?
 ** @param     x                ?
 ** @param     y                ?
 ** @param     selection_data   ?
 ** @param     info             ?
 ** @param     time             ?
 **
 ** @return    -
 ** 
 ** @todo	tournament
 ** 
 ** @bug	copied code from the gtkmm example, not really understood
 **/
void
BugReport::on_bug_report_dropped(Glib::RefPtr<Gdk::DragContext> const&
                                 context,
                                 int const x, int const y,
                                 Gtk::SelectionData const& selection_data,
                                 guint const info, guint const time)
{
  // what is 'format'?
  if (   (selection_data.get_length() >= 0)
      && (selection_data.get_format() == 8)) { // !!! what is the format?

    // if a file is dropped, 'data' is 'file:///path/to/file'
    // so check whether the data is of the form

    // !!! Bug?
    // 'selection_data.data' is 'guchar*', but there is no
    // constructor 'Glib::ustring(guchar*)'
    Glib::ustring type(selection_data.get_text());

    if (string(type, 0, strlen("file://")) != "file://")
      return ;

    // If more than one file is dropped each file is in one line.
    if (type.find('\n') != type.rfind('\n'))
      return ;

    // so we have here a file

    string path(type, strlen("file://"));

    // remove the EOL
    if (*(path.end() - 1) == '\n')
      path.erase(path.end() - 1);
    if (*(path.end() - 1) == '\r')
      path.erase(path.end() - 1);

    // simple check for a bug report
    if (string(DK::Utils::File::basename(path),
               0, strlen("FreeDoko.BugReport"))
        == "FreeDoko.BugReport") {

      if (this->ui->bug_report->load(path))
        context->drag_finish(false, false, time);
    } // a bug report

  } // if ()

  return ;
} // void BugReport::on_bug_report_dropped(...)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
