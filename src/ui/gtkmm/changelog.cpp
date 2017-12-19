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

#include "changelog.h"

#include "ui.h"
#include "cards.h"
#include "translations.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/alignment.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#ifdef CHANGELOG_USE_TREEVIEW
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#endif
#include <gtkmm/textview.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **/
ChangeLog::ChangeLog(Base* const parent) :
  Base(parent),
  StickyDialog("ChangeLog", false),
  title_buffer(nullptr),
#ifdef CHANGELOG_USE_TREEVIEW
  text(),
  model(new Model())
#else
    text_buffer(nullptr)
#endif
    {
      this->ui->add_window(*this);
      this->signal_realize().connect(sigc::mem_fun(*this,
                                                   &ChangeLog::init));

      return ;
    } // ChangeLog::ChangeLog(Base* const parent)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
ChangeLog::init()
{

  this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                   ::translation("ChangeLog")));
  this->set_icon(this->ui->icon);

  this->set_skip_taskbar_hint();

  this->set_default_size((this->ui->logo->get_width() * 3) / 2,
                         (this->ui->logo->get_height() * 6) / 2);
#ifdef POSTPONED
  this->get_window()->set_decorations(Gdk::DECOR_BORDER
                                      | Gdk::DECOR_RESIZEH
                                      | Gdk::DECOR_TITLE
                                      | Gdk::DECOR_MENU);
#endif


  { // action area
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
    // signals
    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));

    close_button->show_all();

    close_button->grab_default();
  } // action area

  { // the image
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->set_border_width(parent->ui->logo->get_width() / 20);
    auto image = Gtk::manage(new Gtk::Image(parent->ui->logo));
    alignment->add(*image);

    this->get_vbox()->pack_start(*alignment, Gtk::PACK_SHRINK);
  } // the image
  { // title
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->set_border_width(2 EX);
    //this->get_vbox()->pack_start(*alignment, false, true);

    auto title_text = Gtk::manage(new Gtk::TextView());
    title_text->set_editable(false);
    title_text->set_cursor_visible(false);

    this->title_buffer = title_text->get_buffer();
    {
      auto tag_title = this->title_buffer->create_tag("title");
      //tag_title->set_property("weight", Pango::WEIGHT_BOLD);
      tag_title->set_property("scale", Pango::SCALE_X_LARGE);
      tag_title->set_property("justification", Gtk::JUSTIFY_CENTER);
    }

#if 0
    this->ui->translations->add(*title,
                                ::translation("ChangeLog::till %sversion%",
                                              ::version.number_to_string())
                                + ":");
#endif

    //alignment->add(*title_text);
    this->get_vbox()->pack_start(*title_text, false, true);
  } // title

  { // ChangeLog
#ifdef CHANGELOG_USE_TREEVIEW
    // the lines in the table do not wrap
    auto list = Gtk::TreeStore::create(*this->model);
    auto treeview = Gtk::manage(new Gtk::TreeView(list));
    treeview->set_headers_visible(false);

    treeview->append_column("asdf", this->model->text);

    for (auto v = ::all_versions.rbegin();
         v != ::all_versions.rend();
         ++v) {
      auto version_row = *list->append();
      version_row[this->model->text] = (*v)->number_to_string();
      this->text[**v] = *list->append(version_row.children());
      this->text[**v][this->model->text] = "changes";
    } // for (v : ::all_versions)

    { // the scrolled window
      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                  Gtk::POLICY_AUTOMATIC);
      scrolled_window->add(*treeview);

      this->get_vbox()->add(*scrolled_window);
    } // the scrolled window
#else // #ifdef POSTPONED
    auto text = Gtk::manage(new Gtk::TextView());
    text->set_wrap_mode(Gtk::WRAP_WORD);
    text->set_editable(false);
    this->text_buffer = text->get_buffer();

    {
      auto tag_subtitle = this->text_buffer->create_tag("subtitle");
      tag_subtitle->set_property("weight", Pango::WEIGHT_BOLD);
    }
#if 0
    vector<DK::Utils::Version const*>::reverse_iterator
      v = ::all_versions.rbegin();
    Translator::Translation
      changelog((*v)->number_to_string()
                + "\n"
                + ::translation("ChangeLog::" + (*v)->number_to_string()));
    for (v += 1; v != ::all_versions.rend(); ++v) {
      changelog
        += ("\n\n"
            + (*v)->number_to_string()
            + "\n"
            + ::translation("ChangeLog::" + (*v)->number_to_string()));
    } // for (v : ::all_versions
    this->ui->translations->add(*text, changelog);
#endif

    { // the scrolled window
      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                  Gtk::POLICY_AUTOMATIC);
      scrolled_window->add(*text);

      this->get_vbox()->pack_start(*scrolled_window);
    } // the scrolled window
#endif // #ifdef POSTPONED
  } // ChangeLog

  this->language_update();

  this->show_all_children();

  return ;
} // void ChangeLog::init()

/**
 ** update the language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
ChangeLog::language_update()
{
  if (!this->get_realized())
    return ;

  { // title
    this->title_buffer->erase(this->title_buffer->begin(),
                              this->title_buffer->end());
    this->title_buffer->insert_with_tag(this->title_buffer->begin(),
                                        "\n"
                                        + ::translation("ChangeLog::till %sversion%", ::version->number_to_string()).translation()
                                        + "\n",
                                        "title");
  } // title

#ifdef CHANGELOG_USE_TREEVIEW
  // see above
  for (auto const& v : ::all_versions) {
    this->text[*v][model->text]
      = ::translation("ChangeLog::"
                      + v->number_to_string()
                     ).translation();
  } // for (v : ::all_versions)
#else
  this->text_buffer->begin_user_action();
  this->text_buffer->erase(this->text_buffer->begin(), this->text_buffer->end());
  for (auto v = ::all_versions.rbegin();
       v != ::all_versions.rend();
       ++v) {
    if (v != ::all_versions.rbegin())
      this->text_buffer->insert_with_tag(this->text_buffer->end(),
                                         "\n\n", "subtitle");
    this->text_buffer->insert_with_tag(this->text_buffer->end(),
                                       (*v)->number_to_string() + "\n",
                                       "subtitle");
    this->text_buffer->insert(this->text_buffer->end(),
                              ::translation("ChangeLog::" + (*v)->number_to_string()).translation());
  } // for (v : ::all_versions)
  this->text_buffer->end_user_action();
#endif

  return ;
} // void ChangeLog::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
