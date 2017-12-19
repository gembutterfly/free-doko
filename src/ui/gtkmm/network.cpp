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

#include "network.h"

#include "ui.h"
#include "cards.h"
#include "translations.h"

#include "../../network/server.h"
#include "../../network/listener.h"
#include "../../network/connection.h"

#include "../../party/rule.h"
#include "../../misc/translations.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include "widgets/labelspinbutton.h"
#include "widgets/labelentry.h"
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/alignment.h>
#include <gtkmm/frame.h>
#include <gtkmm/notebook.h>
#include <gtkmm/alignment.h>
#ifndef OUTDATED
// will be outdated when the network support is finished,
// t.i. with version 0.8.0
#include <gtkmm/textview.h>
#endif
namespace UI_GTKMM_NS {

// how long an urgency hint is shown (in milisec)
unsigned const Network::URGENCY_HINT_TIMEOUT = 5000;

/**
 ** Constructor for the model
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::ConnectionModel::ConnectionModel() :
  Gtk::TreeModel::ColumnRecord()
{
  this->add(this->connection);
  this->add(this->name);
  this->add(this->address);
  this->add(this->port);
  this->add(this->relation);
  this->add(this->type);
  this->add(this->incoming_bytes);
  this->add(this->outgoing_bytes);
#ifdef POSTPONED
  this->add(this->found);
  this->add(this->color);
  this->add(this->accept);
#endif

  return ;
} // Network::ConnectionModel::ConnectionModel()

/**
 ** Constructor for the model
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::LocalAddressesModel::LocalAddressesModel() :
  Gtk::TreeModel::ColumnRecord()
{
  this->add(this->name);

  return ;
} // Network::LocalAddressesModel::LocalAddressesModel()

/**
 ** Constructor for the model
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::ListenersModel::ListenersModel() :
  Gtk::TreeModel::ColumnRecord()
{
  this->add(this->port);
  return ;
} // Network::ListenersModel::ListenersModel()

/**
 ** Constructor for the model
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::RuleDiffsModel::RuleDiffsModel() :
  Gtk::TreeModel::ColumnRecord()
{
  this->add(this->name);
  this->add(this->self_value);
  this->add(this->other_value);
  return ;
} // Network::RuleDiffsModel::RuleDiffsModel()

/**
 ** Constructor
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::Network(Base* const parent) :
  Base(parent),
  StickyDialog("Network")
{
  this->ui->add_window(*this);

  this->init();
  this->signal_realize().connect(sigc::mem_fun(*this, &Network::recreate_connections_list));
  this->signal_realize().connect(sigc::mem_fun(*this, &Network::recreate_listeners_list));
  this->signal_realize().connect(sigc::mem_fun(*this, &Network::recreate_local_addresses_list));
  this->signal_realize().connect(sigc::mem_fun(*this, &Network::update_sensitivity));

  return ;
} // Network::Network(Base* const parent)

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
Network::~Network()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::init()
{
  this->set_icon(this->ui->icon);
  this->ui->translations->add(*this, ::translation("~network settings"));

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

  { // the elements
    this->new_connection_address
      = Gtk::manage(new Gtk::LabelEntry("address"));
    this->ui->translations->add(this->new_connection_address->get_label(),
                                ::translation("address") + ":");

    this->new_connection_port
      = Gtk::manage(new Gtk::LabelSpinButton("port"));
#ifdef RELEASE
    this->new_connection_port->set_range(1024, 65535);
#else
    this->new_connection_port->set_range(1, 65535);
#endif
    this->new_connection_port->set_value(::Network::Server::FREEDOKO_STANDARD_PORT);
    this->ui->translations->add(this->new_connection_port->get_label(),
                                ::translation("port") + ":");

#if GTKMM_VERSION_GE(2,8)
    this->connect
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CONNECT, "connect"));
#else
    this->connect
      = Gtk::manage(new Gtk::StockButton("connect"));
#endif
    this->ui->translations->add(*this->connect->label,
                                ::translation("connect"));
  } // the elements

  { // the connection table
    this->connection_treeview
      = Gtk::manage(new Gtk::TreeView(Gtk::ListStore::create(this->connection_model)));

    this->connection_list = Gtk::ListStore::create(this->connection_model);
    this->connection_treeview = Gtk::manage(new Gtk::TreeView(this->connection_list));
    this->connection_treeview->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

    this->connection_treeview->append_column("name", this->connection_model.name);
    this->connection_treeview->append_column("address", this->connection_model.address);
    this->connection_treeview->append_column("port", this->connection_model.port);
    this->connection_treeview->append_column("relation", this->connection_model.relation);
    this->connection_treeview->append_column("type", this->connection_model.type);
    this->connection_treeview->append_column("incoming", this->connection_model.incoming_bytes);
    this->connection_treeview->append_column("outgoing", this->connection_model.outgoing_bytes);
  } // the connection table
  { // the listeners table
    this->create_listener
      = Gtk::manage(new Gtk::Button("create listener"));
    this->ui->translations->add(*this->create_listener,
                                ::translation("create listener"));

    this->listener_port
      = Gtk::manage(new Gtk::LabelSpinButton("port"));
#ifdef RELEASE
    this->listener_port->set_range(1024, 65535);
#else
    this->listener_port->set_range(1, 65535);
#endif
    this->listener_port->set_value(::Network::Server::FREEDOKO_STANDARD_PORT);
    this->ui->translations->add(this->listener_port->get_label(),
                                ::translation("port") + ":");

    this->standard_listener_port
      = Gtk::manage(new Gtk::StockButton("standard port"));
    this->ui->translations->add(*this->standard_listener_port->label,
                                ::translation("standard port"));



    this->listeners_treeview
      = Gtk::manage(new Gtk::TreeView(Gtk::ListStore::create(this->listeners_model)));

    this->listeners_list
      = Gtk::ListStore::create(this->listeners_model);
    this->listeners_treeview
      = Gtk::manage(new Gtk::TreeView(this->listeners_list));

    this->listeners_treeview->append_column("port", this->listeners_model.port);

    this->listeners_treeview->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  } // the listeners table
  { // the local addresses table
    this->local_addresses_treeview
      = Gtk::manage(new Gtk::TreeView(Gtk::ListStore::create(this->local_addresses_model)));

    this->local_addresses_list
      = Gtk::ListStore::create(this->local_addresses_model);
    this->local_addresses_treeview
      = Gtk::manage(new Gtk::TreeView(this->local_addresses_list));

    this->local_addresses_treeview->append_column("address", this->local_addresses_model.name);

    this->local_addresses_treeview->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
  } // the local addresses table
  { // the rule differences
    this->rule_diffs_treeview
      = Gtk::manage(new Gtk::TreeView(Gtk::ListStore::create(this->rule_diffs_model)));

    this->rule_diffs_list
      = Gtk::ListStore::create(this->rule_diffs_model);
    this->rule_diffs_treeview
      = Gtk::manage(new Gtk::TreeView(this->rule_diffs_list));

    this->rule_diffs_treeview->append_column("rule", this->rule_diffs_model.name);
    this->rule_diffs_treeview->append_column("self value", this->rule_diffs_model.self_value);
    this->rule_diffs_treeview->get_column_cell_renderer(1)->set_property("xalign", 0.5);
    this->rule_diffs_treeview->append_column("other value", this->rule_diffs_model.other_value);
    this->rule_diffs_treeview->get_column_cell_renderer(2)->set_property("xalign", 0.5);

    this->rule_diffs_treeview->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
    this->rule_diffs_treeview->get_column_cell_renderer(0)->set_property("is_expander", true);
    this->rule_diffs_treeview->get_column_cell_renderer(1)->set_property("is_expander", false);
    this->rule_diffs_treeview->get_column_cell_renderer(2)->set_property("is_expander", false);
  } // the rule differences

  { // Layout
    auto vbox = Gtk::manage(new Gtk::VBox(false, 2 EX));

#ifndef OUTDATED
    // will be outdated when the network support is finished,
    // t.i. with version 0.8.0
    { // network support message
      auto message = Gtk::manage(new Gtk::TextView());
      message->set_editable(false);
      message->set_wrap_mode(Gtk::WRAP_WORD);
      message->set_cursor_visible(false);
      this->ui->translations->add(*message,
                                  ::translation("Message::network support"));

      vbox->pack_start(*message, false, true);
    } // network support message
#endif
    { // notebook
      auto notebook = Gtk::manage(new Gtk::Notebook);

      { // connections
        auto label = Gtk::manage(new Gtk::Label("connections"));
        this->ui->translations->add(*label, ::translation("connections"));

        auto vbox = Gtk::manage(new Gtk::VBox(false, 2 EX));

        { // connection list
          auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

          scrolled_window->set_border_width(1 EX);
          scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                      Gtk::POLICY_AUTOMATIC);
          scrolled_window->add(*this->connection_treeview);
          scrolled_window->set_hadjustment(this->connection_treeview->get_hadjustment());
          scrolled_window->set_vadjustment(this->connection_treeview->get_vadjustment());

          vbox->add(*scrolled_window);
        } // connection list

        { // sender
          this->sender = Gtk::manage(new Gtk::Entry());
          this->sender->signal_activate().connect(sigc::mem_fun(*this, &Network::send_event));
#ifndef RELEASE
          vbox->pack_start(*this->sender, false, true);
#endif
        } // sender

        { // create connection
          this->new_connection_container = Gtk::manage(new Gtk::Frame);

          auto label = Gtk::manage(new Gtk::Label("new connection"));
          this->ui->translations->add(*label, ::translation("new connection"));
          static_cast<Gtk::Frame*>(this->new_connection_container
                                  )->set_label_widget(*label);

          auto hbox = Gtk::manage(new Gtk::HBox(false, 1 EM));

          hbox->set_border_width(1 EX);

          hbox->add(*this->new_connection_address);
          hbox->add(*this->new_connection_port);
          hbox->add(*this->connect);

          this->new_connection_container->add(*hbox);
          vbox->pack_end(*this->new_connection_container, false, false);
        } // create connection

        notebook->append_page(*vbox, *label);
      } // connections
      { // listeners
        auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX));

        auto label = Gtk::manage(new Gtk::Label("listeners"));
        this->ui->translations->add(*label, ::translation("listeners"));

        auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        scrolled_window->set_border_width(1 EX);
        scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                    Gtk::POLICY_AUTOMATIC);
        scrolled_window->add(*this->listeners_treeview);
        scrolled_window->set_hadjustment(this->listeners_treeview->get_hadjustment());
        scrolled_window->set_vadjustment(this->listeners_treeview->get_vadjustment());

        { // new listener
          this->new_listener_container = Gtk::manage(new Gtk::Frame);
          auto label = Gtk::manage(new Gtk::Label("new listener"));
          this->ui->translations->add(*label, ::translation("new listener"));
          static_cast<Gtk::Frame*>(this->new_listener_container
                                  )->set_label_widget(*label);

          auto hbox = Gtk::manage(new Gtk::HBox(false, 1 EM));
          hbox->set_border_width(1 EX);

          hbox->add(*this->standard_listener_port);
          hbox->add(*this->listener_port);
          hbox->add(*this->create_listener);

          this->new_listener_container->add(*hbox);
          vbox->pack_end(*this->new_listener_container, false, true);
        } // new listener

        vbox->add(*scrolled_window);
        notebook->append_page(*vbox, *label);
      } // listeners
      { // ip addresses
        auto label = Gtk::manage(new Gtk::Label("local addresses"));
        this->ui->translations->add(*label, ::translation("local addresses"));

        auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        scrolled_window->set_border_width(1 EX);
        scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                    Gtk::POLICY_AUTOMATIC);
        scrolled_window->add(*this->local_addresses_treeview);
        scrolled_window->set_hadjustment(this->local_addresses_treeview->get_hadjustment());
        scrolled_window->set_vadjustment(this->local_addresses_treeview->get_vadjustment());

        notebook->append_page(*scrolled_window, *label);
      } // ip addresses
      { // rule differences
        this->rule_diffs_label = Gtk::manage(new Gtk::Label("rule differences"));
        this->ui->translations->add(*this->rule_diffs_label, ::translation("rule differences"));

        auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);

        scrolled_window->set_border_width(1 EX);
        scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                    Gtk::POLICY_AUTOMATIC);
        scrolled_window->add(*this->rule_diffs_treeview);
        scrolled_window->set_hadjustment(this->rule_diffs_treeview->get_hadjustment());
        scrolled_window->set_vadjustment(this->rule_diffs_treeview->get_vadjustment());

        notebook->append_page(*scrolled_window, *this->rule_diffs_label);
      } // rule differences

      vbox->pack_start(*notebook, true, true);
    } // notebook
    { // synchronize
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));

      this->synchronize_button
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::REFRESH, "synchronize"));
      this->ui->translations->add(*this->synchronize_button,
                                  ::translation("synchronize network"));
      dynamic_cast<Gtk::Container*>(this->synchronize_button->get_child()
                                   )->set_border_width(1 EX / 2);

      alignment->add(*this->synchronize_button);
      vbox->pack_end(*alignment, false, true);
    } // synchronize
    { // clear
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));

      this->clear_button
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLEAR, "clear"));
      this->ui->translations->add(*this->clear_button,
                                  ::translation("clear network"));
      dynamic_cast<Gtk::Container*>(this->clear_button->get_child()
                                   )->set_border_width(1 EX / 2);

      alignment->add(*this->clear_button);
      vbox->pack_end(*alignment, false, true);
    } // clear

    this->get_vbox()->add(*vbox);
  } // Layout

#ifdef DKNOF
  this->new_connection_address->get_entry().set_text("localhost");
#endif

  { // signals
    this->signal_show().connect(sigc::mem_fun(*this, &Network::update_sensitivity));

    this->new_connection_address->get_entry().signal_changed().connect(sigc::mem_fun(*this, &Network::update_sensitivity));
    this->new_connection_address->get_entry().signal_activate().connect(sigc::mem_fun(*this, &Network::connect_event));

    this->new_connection_port->get_spin_button().signal_value_changed().connect(sigc::mem_fun(*this, &Network::update_sensitivity));
    this->connect->signal_clicked().connect(sigc::mem_fun(*this, &Network::connect_event));

    this->listener_port->get_spin_button().signal_value_changed().connect(sigc::mem_fun(*this, &Network::update_sensitivity));
    this->standard_listener_port->signal_clicked().connect(sigc::mem_fun(*this, &Network::reset_port_event));
    this->create_listener->signal_clicked().connect(sigc::mem_fun(*this, &Network::create_listener_event));

    this->clear_button->signal_clicked().connect(sigc::mem_fun(*this, &Network::clear_event));
    this->synchronize_button->signal_clicked().connect(sigc::mem_fun(*this, &Network::synchronize_event));
  } // signals

  this->language_update();

  this->set_default_size(4 * this->ui->cards->width(),
                         4 * this->ui->cards->height());
  this->show_all_children();

  return ;
} // void Network::init()

/**
 ** the party has opened
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::party_open()
{
  // check whether this is the first listener
  if (::server->listeners().size() != 1)
    return ;

  // create a backup of the rules
  this->rule_backup = std::make_unique<Rule>(this->ui->party().rule());

  this->recreate_rule_differences_list();

  this->update_sensitivity();

  return ;
} // void Network::party_open()

/**
 ** updates the sensitivity
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::update_sensitivity()
{
  { // new connection
    this->new_connection_container->set_sensitive(!::server->has_parent_connection());

    { // check that the address is not already connected
      this->connect->set_sensitive(true);
      auto const address = this->new_connection_address->get_text();
      if (address.empty())
        this->connect->set_sensitive(false);
      else { // if !(address.empty())
        auto const port
          = static_cast<unsigned>(this->new_connection_port->get_value_as_int());
        for (auto c : ::server->connections()) {
          if ((address == c->address())
              && (port == c->port())) {
            this->connect->set_sensitive(false);
            break;
          }
        } // for (c : ::server.connections())
      } // if !(address.empty())
    }
  } // new connection

  { // listeners
    auto const listener_ports = ::server->ports();
    this->create_listener->set_sensitive(std::find(listener_ports.begin(), listener_ports.end(), static_cast<unsigned>(this->listener_port->get_value_as_int()))
                                         == listener_ports.end());
  } // listeners

  return ;
} // void Network::update_sensitivity()

/**
 ** recreates the connections list
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::recreate_connections_list()
{
  this->connection_list->clear();
  this->connection_rows.clear();

  if (::game_status == GAMESTATUS::PROGRAMSTART)
    return ;

  for (auto c : ::server->connections()) {
    auto row = *this->connection_list->append();
    this->connection_rows.push_back(row);
    row[this->connection_model.connection] = c;
    row[this->connection_model.name] = c->name();
    row[this->connection_model.address] = c->address();
    row[this->connection_model.port] = c->port();
    row[this->connection_model.relation] = ::translation(c->relation()).translation();
    row[this->connection_model.type] = ::translation(c->type()).translation();
    row[this->connection_model.incoming_bytes] = ".";
    row[this->connection_model.outgoing_bytes] = ".";
    this->update_traffic(*c);
  }
  this->update_sensitivity();

  return ;
} // void Network::recreate_connections_list()

/**
 ** recreates the connections list
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::recreate_listeners_list()
{
  this->listeners_list->clear();
  for (auto l : ::server->listeners()) {
    auto row = *this->listeners_list->append();
    row[this->listeners_model.port] = static_cast<int>(l->port());
  }

  return ;
} // void Network::recreate_listeners_list();

/**
 ** recreates the local addresses list
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::recreate_local_addresses_list()
{
  auto const local_addresses = ::server->local_addresses();
  this->local_addresses_list->clear();
  for (auto c : local_addresses) {
    auto row = *this->local_addresses_list->append();
    row[this->local_addresses_model.name] = c;
  }

  return ;
} // void Network::recreate_local_addresses_list()

/**
 ** recreates the rule differences
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::recreate_rule_differences_list()
{
  this->rule_diffs_list->clear();
  if (!this->rule_backup) {
    this->ui->translations->change(*this->rule_diffs_label, ::translation("rule differences"));
    return ;
  }

  unsigned differences = 0;
  for (int t = Rule::BOOL_FIRST; t <= Rule::BOOL_LAST; ++t) {
    if (   this->ui->party().rule()(static_cast<Rule::TypeBool>(t))
        != (*this->rule_backup)(static_cast<Rule::TypeBool>(t)) ) {
      auto row = *this->rule_diffs_list->append();
      row[this->rule_diffs_model.name]
        = ::translation(static_cast<Rule::TypeBool>(t)).translation();
      row[this->rule_diffs_model.self_value]
        = ( (*this->rule_backup)(static_cast<Rule::TypeBool>(t))
           ? ::translation("yes").translation()
           : ::translation("no").translation() );
      row[this->rule_diffs_model.other_value]
        = ( this->ui->party().rule()(static_cast<Rule::TypeBool>(t))
           ? ::translation("yes").translation()
           : ::translation("no").translation() );
      differences += 1;
    }
  } // for (t \in Rule::TypeBool)

  for (int t = Rule::UNSIGNED_FIRST; t <= Rule::UNSIGNED_LAST; ++t) {
    if (   this->ui->party().rule()(static_cast<Rule::TypeUnsigned>(t))
        != (*this->rule_backup)(static_cast<Rule::TypeUnsigned>(t)) ) {
      auto row = *this->rule_diffs_list->append();
      row[this->rule_diffs_model.name]
        = ::translation(static_cast<Rule::TypeUnsigned>(t)).translation();
      row[this->rule_diffs_model.self_value]
        = std::to_string((*this->rule_backup)(static_cast<Rule::TypeUnsigned>(t)));
      row[this->rule_diffs_model.other_value]
        = std::to_string(this->ui->party().rule()(static_cast<Rule::TypeUnsigned>(t)));
      differences += 1;
    }
  } // for (t \in Rule::TypeUnsigned)

  if (   this->ui->party().rule()(Rule::COUNTING)
      != (*this->rule_backup)(Rule::COUNTING) ) {
    auto row = *this->rule_diffs_list->append();
    row[this->rule_diffs_model.name]
      = ::translation(Rule::COUNTING).translation();
    row[this->rule_diffs_model.self_value]
      = ::translation((*this->rule_backup)(Rule::COUNTING)).translation();
    row[this->rule_diffs_model.other_value]
      = ::translation(this->ui->party().rule()(Rule::COUNTING)).translation();
    differences += 1;
  }

  this->ui->translations->change(*this->rule_diffs_label, ::translation("rule differences") + " (" + std::to_string(differences) + ")");

  return ;
} // void Network::recreate_rule_differences_list()

/**
 ** a rule has changed
 **
 ** @param     type   type of the changed rule
 **
 ** @return    -
 **/
void
Network::rule_update(int const type)
{
  this->recreate_rule_differences_list();

  return ;
} // void Network::rule_update(int type)

/**
 ** the port is resettet to the standard port
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::reset_port_event()
{
  this->listener_port->set_value(::Network::Server::FREEDOKO_STANDARD_PORT);
  return ;
} // void Network::reset_port_event()

/**
 ** there is a new listener
 **
 ** @param     listener   new listener
 **
 ** @return    -
 **/
void
Network::network_listener_new(::Network::Listener const& listener)
{
  this->recreate_listeners_list();

  // check whether this is the first listener
  if (::server->listeners().size() > 1)
    return ;

  if (::game_status == GAMESTATUS::PROGRAMSTART)
    return ;

  // the first listener
  if (   (::server->listeners().size() == 1)
      && ::server->connections().empty()
      && (::game_status != GAMESTATUS::PROGRAMSTART)) {
    DEBUG_ASSERTION(!this->rule_backup,
                    "UI_GTKMM::Network::network_listener_new(listener)\n"
                    "  rule backup does already exists");
    // create a backup of the rules
    this->rule_backup = std::make_unique<Rule>(this->ui->party().rule());

    this->recreate_rule_differences_list();

    this->update_sensitivity();
  } // if (first listener)

  return ;
} // void Network::network_listener_new(::Network::Listener listener)

/**
 ** a listener is deleted
 **
 ** @param     listener   deleted listener
 **
 ** @return    -
 **/
void
Network::network_listener_deleted(::Network::Listener const& listener)
{
  this->recreate_listeners_list();

  // if the network is 'empty' reset all
  if (::server->empty()) {
    DEBUG_ASSERTION(this->rule_backup,
                    "UI_GTKMM::Network::network_listener_deleted(listener)\n"
                    "  rule backup is empty");

    // reset the rules
    this->rule_backup.release();
    this->recreate_rule_differences_list();

    this->update_sensitivity();
  } // if (::server->empty())

  return ;
} // void Network::network_listener_deleted(::Network::Listener listener)

/**
 ** the connection 'connection' was opened
 **
 ** @param     connection   new connection
 **
 ** @return    -
 **/
void
Network::network_connection_new(::Network::Connection const& connection)
{
  this->recreate_connections_list();

  // the first connection
  if (   ::server->listeners().empty()
      && (::server->connections().size() == 1)
      && (::game_status != GAMESTATUS::PROGRAMSTART)) {
    DEBUG_ASSERTION(!this->rule_backup,
                    "UI_GTKMM::Network::network_connection_new(connection)\n"
                    "  rule backup does already exists");

    // make a backup of the rules
    this->rule_backup = std::make_unique<Rule>(this->ui->party().rule());
    this->recreate_rule_differences_list();

    this->update_sensitivity();
  } // if (first connection)

#if GTKMM_VERSION_GE(2,8)
  if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT)) {
    this->set_urgency_hint();
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &Network::stop_urgency_hint),
                                   URGENCY_HINT_TIMEOUT);
  } // if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT))
#endif

  return ;
} // Network::network_connection_new(::Network::Connection connection)

/**
 ** the connection 'connection' was changed
 **
 ** @param     connection   connection that was changed
 **
 ** @return    -
 **/
void
Network::network_connection_changed(::Network::Connection const& connection)
{
  this->recreate_connections_list();

#if GTKMM_VERSION_GE(2,8)
  if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT)) {
    this->set_urgency_hint();
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &Network::stop_urgency_hint),
                                   URGENCY_HINT_TIMEOUT);
  } // if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT))
#endif

  return ;
} // Network::network_connection_changed(::Network::Connection connection)

/**
 ** the connection 'connection' was deleted
 **
 ** @param     connection   deleted connection
 **
 ** @return    -
 **/
void
Network::network_connection_deleted(::Network::Connection const& connection)
{
  this->recreate_connections_list();

  // the first connection
  if (::server->empty()) {
    if (!this->rule_backup)
      return;
    DEBUG_ASSERTION(this->rule_backup,
                    "UI_GTKMM::Network::network_connection_deleted(connection)\n"
                    "  rule backup is empty");
    // reset the rules
    this->rule_backup.release();
    this->recreate_rule_differences_list();

    this->update_sensitivity();
  } // if (::server->empty())

#if GTKMM_VERSION_GE(2,8)
  if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT)) {
    this->set_urgency_hint();
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &Network::stop_urgency_hint),
                                   URGENCY_HINT_TIMEOUT);
  } // if (::setting(Setting::NETWORK_CONNECTIONS_URGENCY_HINT))
#endif

  return ;
} // void Network::network_connection_deleted(::Network::Connection connection)

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
Network::traffic(::Network::Connection const& connection,
                 ::Network::TrafficType const traffic_type,
                 string const& text)
{
  this->update_traffic(connection);

  return ;
} // void Network::traffic(time, name, text)

/**
 ** the address of the new connection has changed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::new_connection_address_change_event()
{
  this->update_sensitivity();
  return ;
} // void Network::new_connection_address_change_event()

/**
 ** a connection shall be made
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::connect_event()
{
  auto const& address
    = this->new_connection_address->get_text();
  auto const port
    = static_cast<unsigned>(this->new_connection_port->get_value_as_int());
  ::server->create_connection(address, port, ::Network::FREEDOKO);

  return ;
} // void Network::connect_event()

/**
 ** send the text of the 'sender' entry to all connections
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	only send to the selected connections
 **/
void
Network::send_event()
{
  auto const text = this->sender->get_text() + "\n";
  this->sender->set_text("");

  for (auto c : ::server->connections())
    c->send(text);

  return ;
} // void Network::send_event()


/**
 ** creates a listener
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::create_listener_event()
{
  ::server->create_listener(static_cast<unsigned>(this->listener_port->get_value_as_int()));

  return ;
} // void Network::create_listener_event()

/**
 ** clear all listeners and connections
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::clear_event()
{
  ::server->clear();

  return ;
} // void Network::clear_event()

/**
 ** synchronize with the server/child
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::synchronize_event()
{
  ::server->synchronize_game();

  return ;
} // void Network::synchronize_event()

/**
 ** update the traffic for the connection
 **
 ** @param     connection   the connection
 **
 ** @return    -
 **/
void
Network::update_traffic(::Network::Connection const& connection)
{
  vector<Gtk::TreeModel::Row>::iterator r;
  for (r = begin(this->connection_rows);
       r != end(this->connection_rows);
       ++r)
    if ((*r)[this->connection_model.connection] == &connection)
      break;

#ifdef WORKAROUND
  // the traffic is called before the connection is established
  if (r == this->connection_rows.end())
    return ;
#endif
  DEBUG_ASSERTION((r != this->connection_rows.end()),
                  "Network::update_traffic(connection)\n"
                  "  connection '" << connection.name() << "' not found.");

  auto& row = *r;
  if (connection.incoming_bytes() < 10 * 1024)
    row[this->connection_model.incoming_bytes]
      = std::to_string(connection.incoming_bytes());
  else
    row[this->connection_model.incoming_bytes]
      = (std::to_string(connection.incoming_bytes() / 1024)
         + "K");
  if (connection.outgoing_bytes() < 10 * 1024)
    row[this->connection_model.outgoing_bytes]
      = std::to_string(connection.outgoing_bytes());
  else
    row[this->connection_model.outgoing_bytes]
      = (std::to_string(connection.outgoing_bytes() / 1024)
         + "K");

  return ;
} // void Network::update_traffic(connection)

/**
 ** updates the language
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Network::language_update()
{
  this->connection_treeview->get_column(0)->set_title(::translation("Network::name").translation());
  this->connection_treeview->get_column(1)->set_title(::translation("Network::address").translation());
  this->connection_treeview->get_column(2)->set_title(::translation("Network::port").translation());
  this->connection_treeview->get_column(3)->set_title(::translation("Network::Connection::relation").translation());
  this->connection_treeview->get_column(4)->set_title(::translation("Network::Connection::type").translation());
  this->connection_treeview->get_column(5)->set_title("in");
  this->connection_treeview->get_column(6)->set_title("out");

  this->local_addresses_treeview->get_column(0)->set_title(::translation("Network::address").translation());

  this->listeners_treeview->get_column(0)->set_title(::translation("open port").translation());

  this->rule_diffs_treeview->get_column(0)->set_title(::translation("rule").translation());
  this->rule_diffs_treeview->get_column(1)->set_title(::translation("self value").translation());
  this->rule_diffs_treeview->get_column(2)->set_title(::translation("server value").translation());

  this->recreate_rule_differences_list();

  return ;
} // void Network::language_update()

/**
 ** stop the urgency hint
 **
 ** @param     -
 **
 ** @return    false (stop the timeout signal)
 **/
bool
Network::stop_urgency_hint()
{
#if GTKMM_VERSION_GE(2,8)
  this->set_urgency_hint(false);
#endif
  return false;
} // bool Network::stop_urgency_hint()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
