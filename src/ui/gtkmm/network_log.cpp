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
#ifdef USE_NETWORK

#include "network_log.h"

#include "ui.h"
#include "translations.h"
#include "cards.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>

#include "../../network/connection.h"

#include "../../utils/string.h"

#include <ctime>

namespace UI_GTKMM_NS {

/**
 ** Constructor for the model
 **
 ** @param     -
 **
 ** @return    -
 **/
NetworkLog::LogModel::LogModel() :
  Gtk::TreeModel::ColumnRecord()
{
  this->add(this->direction);
  this->add(this->time);
  this->add(this->address);
  this->add(this->text);

  return ;
} // NetworkLog::LogModel::LogModel()

/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **/
NetworkLog::NetworkLog(Base* const parent) :
  Base(parent),
  StickyDialog("Network log")
{
  this->ui->add_window(*this);

  this->init();

  return ;
} // NetworkLog::NetworkLog(Base* parent)

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
NetworkLog::~NetworkLog()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
NetworkLog::init()
{
  this->set_icon(this->ui->icon);
  this->ui->translations->add(*this, ::translation("~network log"));

  this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

  { // action area
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE,
                                         "close"));
    this->ui->translations->add(*close_button->label,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  } // action area

  { // the log table
    this->log_model = new LogModel();

    // of the sum treeview only the header is neeeded
    // (workaround since there exists no foot for a treeview)
    this->log_treeview
      = Gtk::manage(new Gtk::TreeView(Gtk::ListStore::create(*this->log_model)));

    this->log_list = Gtk::ListStore::create(*this->log_model);
    this->log_treeview = Gtk::manage(new Gtk::TreeView(this->log_list));
    this->log_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

    this->log_treeview->append_column("", this->log_model->direction);
    this->log_treeview->append_column("Time", this->log_model->time);
    this->log_treeview->append_column("Address", this->log_model->address);
    this->log_treeview->append_column("Text", this->log_model->text);
    // ToDo: y-align = top
    this->log_treeview->get_column_cell_renderer(0)->set_property("yalign", 0);
    this->log_treeview->get_column_cell_renderer(1)->set_property("yalign", 0);
    this->log_treeview->get_column_cell_renderer(2)->set_property("yalign", 0);
    this->log_treeview->get_column_cell_renderer(3)->set_property("yalign", 0);
  } // the log table

  { // Layout
    auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
    scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                Gtk::POLICY_AUTOMATIC);
    scrolled_window->add(*this->log_treeview);
    scrolled_window->set_hadjustment(this->log_treeview->get_hadjustment());
    scrolled_window->set_vadjustment(this->log_treeview->get_vadjustment());

    this->get_vbox()->pack_start(*scrolled_window);
  } // Layout

  { // signals
  } // signals

  this->set_default_size(8 * this->ui->cards->width(),
                         3 * this->ui->cards->height());
  this->show_all_children();

  return ;
} // void NetworkLog::init()

/**
 ** there was some network traffic
 **
 ** @param     connection   the connection
 ** @param     traffic_type   the type of the traffic
 ** @param     text   	the sent text
 **
 ** @return    -
 **/
void
NetworkLog::add_traffic(::Network::Connection const& connection,
                        ::Network::TrafficType const traffic_type,
                        string const& text)
{
  auto row = *this->log_list->append();

  switch (traffic_type) {
  case ::Network::TRAFFIC_CONNECTION:
    row[this->log_model->direction] = "o";
    break;
  case ::Network::TRAFFIC_DISCONNECTION:
    row[this->log_model->direction] = "x";
    break;
  case ::Network::TRAFFIC_INCOMING:
    row[this->log_model->direction] = "<";
    break;
  case ::Network::TRAFFIC_OUTGOING:
    row[this->log_model->direction] = ">";
    break;
  } // switch (traffic_type)
  { // time
    auto const t = time(nullptr);
    if (t != static_cast<time_t>(-1)) {
      struct tm const* const t2 = localtime(&t);
      row[this->log_model->time]
        = (        DK::Utils::String::to_string(t2->tm_hour)
           + ":" + DK::Utils::String::to_string(t2->tm_min)
           + ":" + DK::Utils::String::to_string(t2->tm_sec));


    } else {
      row[this->log_model->time] = "?";
    }
#ifndef WINDOWS
#if 0
    // 'gettimeofday' is not known by MS-Windows :-(
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == 0) {
      struct tm* const tm = localtime(&tv.tv_sec);

      row[this->log_model->time] = (asctime(tm)
                                    + DK::Utils::String::to_string(tv.tv_usec));
    } else {
      row[this->log_model->time] = "?";
    }
#endif
#else
    row[this->log_model->time] = "?";
#endif
  } // time
  row[this->log_model->address] = (connection.address()
                                   + ":"
                                   + DK::Utils::String::to_string(connection.port()));
  row[this->log_model->text] = text;

  // just scroll to the end of the log (I do not know a simplier way)
  this->log_treeview->scroll_to_row(this->log_treeview->get_model()->get_path(*(this->log_treeview->get_model()->get_iter(DK::Utils::String::to_string(this->log_treeview->get_model()->children().size() - 1)))));

  return ;
} // void NetworkLog::add_traffic(::Network::Connection connection, ::Network::TraficType traffic_type, string text)

/**
 ** updates the language
 **
 ** @param     -
 **
 ** @return    -
 **/
void
NetworkLog::language_update()
{
  this->log_treeview->get_column(0)->set_title("");
  this->log_treeview->get_column(1)->set_title(::translation("time").translation());
  this->log_treeview->get_column(2)->set_title(::translation("address").translation());
  this->log_treeview->get_column(3)->set_title(::translation("text").translation());

  return ;
} // void NetworkLog::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
