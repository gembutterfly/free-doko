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
#ifdef USE_NETWORK_DOKOLOUNGE

#include "lounge.h"

#include "help.h"
#include "blog.h"
#include "pin_board.h"
#include "messages.h"
#include "alert.h"
#include "chat.h"

#include "../ui.h"
#include "../translations.h"
#include "../main_window.h"
#include "../table.h"
#include "../../ui.wrap.h"

#include "../../../misc/setting.h"

#include "../../../misc/lounge.h"
#include "../../../network/server.h"
#include "../../../network/connection.h"
#include "../../../network/DokoLounge/sender.h"
#include "../../../utils/string.h"

#include "../widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/alignment.h>
#include <gtkmm/table.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/paned.h>

// todo: use dummy connection (text output) for debugging
//
// Umlaute konvertieren
// Eigenes Flüstern farbig
// Fett
// neuer Tisch
// Tisch löschen
// chat: Str-Return: an alle
// chat: Shift-Return: an letzten Empfänger flüstern
// Tastenkombinationen: an Hauptfenster durchreichen
// Größe vom Kartensatz abhängig machen

namespace UI_GTKMM_NS {
  namespace DokoLounge {

    /**
     ** constructor
     **
     ** @param     parent   the parent object
     **
     ** @return    -
     **/
    Lounge::Lounge(Base* const parent) :
      Base(parent),
      Gtk::StickyDialog("DokoLounge: Lounge")
    {
      this->ui->add_window(*this);
      //this->signal_realize().connect(sigc::mem_fun(*this, &Lounge::init));
      this->init();

      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(1, 0, 0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(0, 0.8, 0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(0.5, 0.5, 0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(0.5, 0, 0.5);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(0, 0.5, 0.5);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(138/256.0,43/256.0,226/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(65/256.0,105/256.0,225/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(200/256.0,0/256.0,0/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(205/256.0,102/256.0,0/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(165/256.0,42/256.0,42/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(16/256.0,78/256.0,139/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(0/256.0,100/256.0,0/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(108/256.0,19/256.0,53/256.0);
      this->player_colors_reserve.push(Gdk::Color());
      this->player_colors_reserve.back().set_rgb_p(47/256.0,79/256.0,79/256.0);

      return ;
    } // Lounge::Lounge(Base* parent)

    /**
     ** destruktor
     **
     ** @param     -
     **
     ** @return    -
     **/
    Lounge::~Lounge()
    { 
      if (this->connection != NULL) {
        this->sender->logout();
        ::server->delete_connection(this->connection);
        delete this->connection;
        this->connection = NULL;
      }
      delete this->blog;
    } // Lounge::~Lounge()

    /**
     ** the lounge is shown
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::on_show()
      {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
        this->Gtk::Window::on_show();
#endif
         if (::setting(Setting::DOKOLOUNGE_AUTO_LOGIN)
             && !::lounge->is_logged_in())
           this->login_signal();

        return;
      } // void Lounge::on_show()

    /**
     ** create all subelements
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::init()
      {
        this->help = new Help(this);
        this->blog = new Blog(this);
        this->pin_board = new PinBoard(this);
        this->messages = new Messages(this);
        this->alert = new Alert(this);

        this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                         ::translation("DokoLounge: Lounge")));
        this->set_icon(this->ui->icon);

        this->set_skip_taskbar_hint();

        this->set_default_size((this->ui->logo->get_width() * 3),
                               (this->ui->logo->get_height() * 4));

        { // action area
          this->close_button
            = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
          this->ui->translations->add(*close_button,
                                      ::translation("close"));
          this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
          close_button->show_all();
        } // action area

        { // main area
          { // name
            this->name
              = Gtk::manage(new Gtk::Label("name: Name"));
            this->name->set_use_markup();
            this->ui->translations->add(*this->name,
                                        ::translation("Name: %sname%",
                                                      ::setting(Setting::DOKOLOUNGE_NAME))
                                       );
            Pango::AttrList attrlist;
            auto attribute
              = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
            attrlist.insert(attribute);
            attribute = Pango::Attribute::create_attr_size(20 * 1000);
            attrlist.insert(attribute);
            this->name->set_attributes(attrlist);
            //this->name->set_markup("<b><big>Name</big></b>");
            auto image = Gtk::manage(new Gtk::Image(::setting.dokolounge_logo()));
            if (image == NULL)
              this->get_vbox()->pack_start(*this->name, Gtk::PACK_SHRINK, 2 EX);
            else
              this->get_vbox()->pack_start(*image, Gtk::PACK_SHRINK, 1 EX);
          } // name
          { // buttons
            auto box = Gtk::manage(new Gtk::HBox(true, 0));

            this->create_account_button
              = Gtk::manage(new Gtk::StockButton("create account"));
            this->ui->translations->add(*this->create_account_button,
                                        ::translation("create account"));
            box->add(*this->create_account_button);

            this->login_button
              = Gtk::manage(new Gtk::StockButton("login"));
            this->ui->translations->add(*this->login_button,
                                        ::translation("login"));
            box->add(*this->login_button);

            this->logout_button
              = Gtk::manage(new Gtk::StockButton("logout"));
            this->ui->translations->add(*this->logout_button,
                                        ::translation("logout"));
            // add at the end

#if 0
            this->refresh_button
              = Gtk::manage(new Gtk::StockButton("refresh"));
            this->ui->translations->add(*this->refresh_button,
                                        ::translation("refresh"));
            box->add(*this->refresh_button);
#endif

            this->klingel_button
              = Gtk::manage(new Gtk::StockButton("klingel"));
            this->ui->translations->add(*this->klingel_button,
                                        ::translation("klingel"));
            box->add(*this->klingel_button);

            this->hilfe_button
              = Gtk::manage(new Gtk::StockButton("hilfe"));
            this->ui->translations->add(*this->hilfe_button,
                                        ::translation("hilfe"));
            box->add(*this->hilfe_button);

            this->blog_button
              = Gtk::manage(new Gtk::StockButton("blog"));
            this->ui->translations->add(*this->blog_button,
                                        ::translation("blog"));
            box->add(*this->blog_button);

            this->pinnwand_button
              = Gtk::manage(new Gtk::StockButton("pinnwand"));
            this->ui->translations->add(*this->pinnwand_button,
                                        ::translation("pinnwand"));
            box->add(*this->pinnwand_button);

            this->messages_button
              = Gtk::manage(new Gtk::StockButton("messages"));
            this->ui->translations->add(*this->messages_button,
                                        ::translation("messages"));
            box->add(*this->messages_button);

            box->add(*this->logout_button);

            this->get_vbox()->pack_start(*box, Gtk::PACK_SHRINK);
          } // buttons
          auto box = Gtk::manage(new Gtk::HBox(true, 0));
          { // lounge chat
            this->lounge_chat = new Chat(this);
            box->add(*this->lounge_chat);
          } // lounge chat
          { // table chat and audience
            this->table_chat = new Chat(this);
            this->audience_text = Gtk::manage(new Gtk::TextView());
            this->audience_text->set_wrap_mode(Gtk::WRAP_WORD);
            this->audience_text->set_editable(false);
            this->audience_text->set_cursor_visible(false);
            { // pack the widgets next to the table
              // this part is tricky
              this->table_paned = new Gtk::HPaned();
              {
                auto frame = Gtk::manage(new Gtk::Frame("Audience"));
                this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                            ::translation("Audience"));
                auto scroll_window = Gtk::manage(new Gtk::ScrolledWindow());
                scroll_window->add(*this->audience_text);
                scroll_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
                frame->add(*scroll_window);
                auto paned = Gtk::manage(new Gtk::VPaned());
                paned->add1(*frame);
                paned->add2(*this->table_chat);
                paned->set_position(10 EX);
                paned->show_all();
                this->table_paned->pack1(*paned);
              }
              this->table_paned->set_position(15 EM);
              this->table_paned->show();
            } // pack the widgets next to the table
          } // table chat and audience
          { // table list
            auto frame = Gtk::manage(new Gtk::Frame("Table list"));
            this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                        ::translation("Table list"));
            auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_START, 1, 0));
            auto scroll_window = Gtk::manage(new Gtk::ScrolledWindow());
            scroll_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            this->tables_list = Gtk::manage(new Gtk::Table(1, 1));
            this->tables_list->set_row_spacings(1 EX);
            this->tables_list->set_border_width(1 EX);
            alignment->add(*this->tables_list);
            scroll_window->add(*alignment);
            frame->add(*scroll_window);
            box->add(*frame);
          } // table list
          { // player list
            auto frame = Gtk::manage(new Gtk::Frame("Player list"));
            this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                        ::translation("player list"));
            auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_START, 1, 0));
            auto scroll_window = Gtk::manage(new Gtk::ScrolledWindow());
            scroll_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            this->players_list = Gtk::manage(new Gtk::Table(1, 1));
            this->players_list->set_row_spacings(1 EX);
            this->players_list->set_border_width(1 EX);
            alignment->add(*this->players_list);
            scroll_window->add(*alignment);
            frame->add(*scroll_window);
            box->add(*frame);
          } // player list
          this->get_vbox()->pack_end(*box, Gtk::PACK_EXPAND_WIDGET);
        } // main area

        { // signals
          // this->signal_show().connect(sigc::mem_fun(*this, &Network::update_sensitivity));
          this->close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
          this->create_account_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::create_account_signal));
          this->login_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::login_signal));
        } // signals

        { // the text
          this->language_update();
        } // the text

        this->sensitivity_update();
        this->show_all_children();
        this->recreate_players_list();
        this->recreate_tables_list();

        return ;
      } // void Lounge::init()

    /**
     ** update the sensitivity
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::sensitivity_update()
      {
        if (!this->get_realized())
          return;

        auto const logged_in = ::lounge->is_logged_in();
        this->create_account_button->set_sensitive(!logged_in);
        this->login_button->set_sensitive(         !logged_in);

        this->logout_button->set_sensitive(         logged_in);
        this->klingel_button->set_sensitive(        logged_in);
        this->hilfe_button->set_sensitive(          logged_in);
        this->blog_button->set_sensitive(           logged_in);
        this->pinnwand_button->set_sensitive(       logged_in);
        this->messages_button->set_sensitive(       logged_in);

        return;
      } // void Lounge::sensitivity_update()

    /**
     ** the game has logged in
     **
     ** @param     name   name of the player
     **
     ** @return    -
     **/
    void
      Lounge::logged_in(string const& name)
      {
        this->ui->translations->change(*this->name,
                                       ::translation("Name: %sname%",
                                                     ::lounge->account()));
        { // add chat to the table
          this->ui->main_window->container->remove(*this->ui->table);
          this->table_paned->pack2(*this->ui->table);
          this->ui->main_window->container->add(*this->table_paned);
          if (!(   (this->ui->main_window->get_window()->get_state() == Gdk::WINDOW_STATE_MAXIMIZED)
                || (this->ui->main_window->get_window()->get_state() == Gdk::WINDOW_STATE_FULLSCREEN)) ) {
            // a bit ugly, but we need a wider window
            int width, height;
            this->ui->main_window->get_size(width, height);
            if (width + this->table_paned->property_position()
                <= this->ui->main_window->get_window()->get_screen()->get_width())
              this->ui->main_window->resize(width + this->table_paned->property_position(),
                                            height);
          }
        } // add chat to the table
        this->sensitivity_update();
        return;
      } // void Lounge::logged_in(string name)

    /**
     ** the game is logged out
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::logged_out()
      {
        this->ui->translations->change(*this->name,
                                       ::translation("Name: %sname%",
                                                     ::setting(Setting::DOKOLOUNGE_NAME)));
        { // remove chat from the table
          this->table_paned->remove(*this->ui->table);
          this->ui->main_window->container->add(*this->table_paned);
          this->ui->main_window->container->add(*this->ui->table);
        } // remove chat from the table
        this->sensitivity_update();
        return;
      } // void Lounge::logged_out()

    /**
     ** the location has changed
     **
     ** @param     location   new location
     **
     ** @return    -
     **/
    void
      Lounge::location_changed(::Lounge::Location const& location)
      {
        switch (location) {
        case ::Lounge::LOUNGE:
          this->table_audience_changed("");
          this->raise();
          break;
        case ::Lounge::TABLE:
          this->ui->main_window->raise();
          break;
        case ::Lounge::OBSERVER:
          this->ui->main_window->raise();
          break;
        } // switch (location)
        return ;
      } // void Lounge::location_changed(::Lounge::Location const& location)

    /**
     ** -> result
     **
     ** @param     name   name of the player
     **
     ** @return    color of the player
     **/
    Gdk::Color const&
      Lounge::player_color(string const& name)
      {
        return this->player_colors[name];
      } // Gdk::Color const& Lounge::player_color(string name)

    /**
     ** a chat entry has been added
     **
     ** @param     entry    new chat entry
     **
     ** @return    -
     **/
    void
      Lounge::lounge_chat_entry_added(::LoungeChatEntry const& entry)
      {
        this->lounge_chat->entry_added(entry);
        return;
      } // void Lounge::lounge_chat_entry_added(::LoungeChatEntry entry)

    /**
     ** a chat entry has been added at a table
     **
     ** @param     entry    new chat entry
     **
     ** @return    -
     **
     ** @todo      seperate chat window
     **/
    void
      Lounge::table_chat_entry_added(::LoungeChatEntry const& entry)
      {
        this->table_chat->entry_added(entry);
        return;
      } // void Lounge::table_chat_entry_added(::LoungeChatEntry entry)

    /**
     ** the help has changed
     **
     ** @param     text   new help text
     **
     ** @return    -
     **/
    void
      Lounge::help_changed(string const& text)
      {
        this->help->show(text);
        return;
      } // void Lounge::help_changed(string text)

    /**
     ** the blog has changed
     **
     ** @param     text   new blog text
     **
     ** @return    -
     **/
    void
      Lounge::blog_changed(string const& text)
      {
        this->blog->show(text);
        return;
      } // void Lounge::blog_changed(string text)

    /**
     ** the pin_board has changed
     **
     ** @param     text   new pin_board text
     **
     ** @return    -
     **/
    void
      Lounge::pin_board_changed(string const& text)
      {
        this->pin_board->show(text);
        return;
      } // void Lounge::pin_board_changed(string text)

    /**
     ** the messages has changed
     **
     ** @param     text   new messages text
     **
     ** @return    -
     **/
    void
      Lounge::messages_changed(string const& text)
      {
        this->messages->show(text);
        return;
      } // void Lounge::messages_changed(string text)

    /**
     ** an alert
     **
     ** @param     title   alert title
     ** @param     text    alert text
     **
     ** @return    -
     **/
    void
      Lounge::show_alert(string const& title, string const& text)
      {
#ifdef DKNOF
        if (title != "Willkommen FreeDoko")
#endif
          this->alert->show(title, text);
        return;
      } // void Lounge::show_alert(string title, string text)

    /**
     ** connect to the server
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::connect_server()
      {
#if 0
        // take a file instead of real internet
        this->connection
          = ::server->create_connection("DokoLounge.Test/network.in", 0,
                                        ::Network::DOKOLOUNGE);
#else
        // connect to the DokoLounge server
        this->connection
          = ::server->create_connection(::Network::DokoLounge::Interpreter::address,
                                        ::Network::DokoLounge::Interpreter::port, 
                                        ::Network::DOKOLOUNGE);
        if (this->connection == NULL) {
#ifndef RELEASE
          CLOG << "Failed to create the connection to the DokoLounge." << endl;
#endif
          return;
        }
#endif

        this->sender = dynamic_cast< ::Network::DokoLounge::Interpreter::Sender*>(&this->connection->interpreter().sender());
        if (this->sender == NULL) {
#ifndef RELEASE
          CLOG << "Failed to get the DokoLounge-sender." << endl;
#endif
          return;
        }

        { // create the signals
          //this->create_account->signal_clicked().connect(sigc::mem_fun(*this->sender, &::Network::DokoLounge::Interpreter::Sender::create_account));
          //this->login->signal_clicked().connect(sigc::mem_fun(*this->sender, &::Network::DokoLounge::Interpreter::Sender::login));
          this->logout_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::logout_signal));
#if 0
          this->refresh_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::refresh_signal));
#endif
          this->klingel_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::klingel_signal));
          this->hilfe_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::hilfe_signal));
          this->blog_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::blog_signal));
          this->pinnwand_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::pinnwand_signal));
          this->messages_button->signal_clicked().connect(sigc::mem_fun(*this, &Lounge::messages_signal));
        } // create the signals

        // ToDo: update/set the icon

        return;
      } // void Lounge::connect_server()

    /**
     ** the players have changed
     **
     ** @param     players    players list
     **
     ** @return    -
     **/
    void
      Lounge::players_changed(vector< ::Lounge::Player> const& players)
      {
        this->recreate_players_list();
        this->update_player_colors();
        return ;
      } // void Lounge::players_changed(vector< ::Lounge::Player> players)

    /**
     ** the tables have changed
     **
     ** @param     tables    tables list
     **
     ** @return    -
     **/
    void
      Lounge::tables_changed(vector< ::Lounge::Table> const& tables)
      {
        this->recreate_tables_list();
        this->recreate_players_list();
        return ;
      } // void Lounge::tables_changed(vector< ::Lounge::Table> tables)

    /**
     ** the table audience has changed
     **
     ** @param     audience   audience text
     **
     ** @return    -
     **/
    void
      Lounge::table_audience_changed(string const& audience)
      {
        this->audience_text->get_buffer()->set_text(audience);
        return ;
      } // void Lounge::table_audience_changed(string audience)

    /**
     ** update the chat colors
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::update_player_colors()
      {
        for (auto const& p : ::lounge->players()) {
          auto const& name = p.name;
          if (this->player_colors.count(name) == 0) {
            this->player_colors[name] = this->player_colors_reserve.front();
            this->player_colors_reserve.push(this->player_colors_reserve.front());
            this->player_colors_reserve.pop();
          }
        } // for (i)
        this->lounge_chat->update_chat_colors();
        this->table_chat->update_chat_colors();
      } // void Lounge::update_player_colors()

    /**
     ** recreate the players list
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @todo      Icongröße beschränken
     ** @todo      Eigenen Spieler fett
     ** @todo      Abstand zwischen den Spielern
     ** @todo      Wo ist der Spieler?
     ** @todo      beobachten
     ** @todo      Nachricht schreiben
     ** @todo      verschiedene Spaltenbreite
     ** @todo      eigenes Icon prüfen und gegebenenfalls neu zusenden
     ** @todo      Namen größer
     **/
    void
      Lounge::recreate_players_list()
      {
        auto const& players = ::lounge->players();

        // an error is shown: Gtk-CRITICAL **: IA__gtk_table_resize: assertion `n_rows > 0 && n_rows <= 65535' failed
        // But the number of rows is 1
        this->players_list->resize(max(players.size(), 1ul), 1);
        this->players_list->get_children().clear();

        //cout << "Players: " << players.size() << "\n";
        Gtk::Table* table;
        Gtk::Image* image; 
        Gtk::Label* label;
        Gtk::Alignment* alignment;
        for (unsigned i = 0; i < players.size(); ++i) {
          //cout << "  " << i << ": " << players[i].name << "\n";
          int const table_no = ::lounge->table_of_player(players[i].name);
          table = Gtk::manage(new Gtk::Table(2, 2));
          table->set_col_spacings(1 EX);
          image = Gtk::manage(new Gtk::Image(this->player_icon(players[i].name)));
          // name
          alignment = Gtk::manage(new Gtk::Alignment());
          alignment->set_size_request(64, -1);
          alignment->add(*image);
          table->attach(*alignment, 0, 1, 0, 2, Gtk::SHRINK);
          label = Gtk::manage(new Gtk::Label(players[i].name));
          Pango::AttrList attrlist;
          auto attribute = Pango::Attribute::create_attr_size(14 * 1000);
          attrlist.insert(attribute);
          if (players[i].name == ::lounge->account()) {
            // bold font for the player
            Pango::Attribute attribute
              = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
            attrlist.insert(attribute);
          }
          label->set_attributes(attrlist);
          label->set_alignment(0, 0.5);
          table->attach(*label, 1, 2, 0, 1);
          if (table_no >= 0) {
            label = Gtk::manage(new Gtk::Label(""));
            this->ui->translations->add(*label,
                                        "     "
                                        + ::translation("table %itable% (%stype%)",
                                                        table_no + 1,
                                                        ::lounge->table(static_cast<unsigned>(table_no)).type));
            label->set_alignment(0, 0.5);
            table->attach(*label, 1, 2, 1, 2);
          }
          this->players_list->attach(*table,
                                     0, 1, i, i+1
                                    );

        }
        this->players_list->show_all_children();
        return ;
      } // void Lounge::recreate_players_list()

    /**
     ** recreate the tables list
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @todo      Eigenen Spieler fett
     ** @todo      Icon der Spieler -- testen
     ** @todo      Abstand zwischen den Tischen
     ** @todo      Spieler beobachten -- testen
     ** @todo      „Spieler beobachten“ ersetzen durch „einspringen“
     ** @todo      Tisch betreten -- testen
     **/
    void
      Lounge::recreate_tables_list()
      {
        vector< ::Lounge::Table> const& tables = ::lounge->tables();

        // an error is shown: Gtk-CRITICAL **: IA__gtk_table_resize: assertion `n_rows > 0 && n_rows <= 65535' failed
        // But the number of rows is 1
        this->tables_list->resize(max(tables.size(), 1ul), 1);
        this->tables_list->get_children().clear();

        auto const at_table = (::lounge->location() != ::Lounge::LOUNGE);
        //cout << "Tables: " << tables.size() << "\n";
        Gtk::Table* table;
        Gtk::Image* image;
        Gtk::EventBox* box;
        Gtk::Alignment* alignment;
        Gtk::Button* button;
        Gtk::Label* label;
        for (unsigned t = 0; t < tables.size(); ++t) {
          //cout << "  " << t << ": " << tables[t].type << "\n";
          table = Gtk::manage(new Gtk::Table(4, 3));
          table->set_row_spacings(2);

          image = Gtk::manage(new Gtk::Image(this->table_icon(tables[t].type)));
          alignment = Gtk::manage(new Gtk::Alignment());
          alignment->set_size_request(150, -1);
          box = Gtk::manage(new Gtk::EventBox());
          box->add(*image);
          if (!at_table) {
            box->add_events(Gdk::BUTTON_PRESS_MASK);
#ifdef POSTPONED
            // does not work, compile error
            box->signal_button_press_event().connect(sigc::bind<int const>(sigc::mem_fun(*this, &Lounge::enter_table_signalw), t));
#endif
          } // if (!at_table)
          alignment->add(*box);
          table->attach(*alignment, 0, 1, 0, 4);
          for (unsigned p = 0; p < tables[t].players.size(); ++p) {
            //cout << "     " << p << " " << tables[t].players[p] << "\n";
            string name = tables[t].players[p];
            if (name == "frei")
              name = "";
            if (!name.empty())
              image = Gtk::manage(new Gtk::Image(this->player_table_icon(name)));
            if (at_table) {
              if (name.empty()) {
                label = Gtk::manage(new Gtk::Label("--"));
                table->attach(*label, 2, 3, p, p + 1);
              } else {
                auto box = Gtk::manage(new Gtk::HBox());
                label = Gtk::manage(new Gtk::Label("  " + name + "  "));
                label->set_alignment(0, 0.5);
                if (name == ::lounge->account()) {
                  // bold font for the player
                  Pango::AttrList attrlist;
                  auto attribute = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
                  attrlist.insert(attribute);
                  label->set_attributes(attrlist);
                }
                box->pack_start(*image, Gtk::PACK_SHRINK);
                box->pack_end(*label);
                table->attach(*box, 2, 3, p, p + 1);
              }
            } else { // if !(at_table)
              if (name.empty()) {
                button = Gtk::manage(new Gtk::Button("--"));
                button->signal_clicked().connect(sigc::bind<int const>(sigc::mem_fun(*this, &Lounge::enter_table_signal), t));
              } else {
                button = Gtk::manage(new Gtk::Button("  " + name + "  "));
                button->set_alignment(0, 0.5);
                button->set_image(*image);
                // ToDo: Ersetzen durch Einspringen
                button->signal_clicked().connect(sigc::bind<string const>(sigc::mem_fun(*this, &Lounge::follow_player_signal), name));
              }
              table->attach(*button, 2, 3, p, p + 1);
            } // if !(at_table)
          } // for (unsigned p < tables[t].players.size())
          this->tables_list->attach(*table, 0, 1, t, t+1);
        } // for (t < tables.size)
        this->tables_list->show_all_children();

        return ;
      } // void Lounge::recreate_tables_list()

    /**
     ** -> result
     ** the icon is loaded if needed
     **
     ** @param     player   player name
     **
     ** @return    the player icon for the tables list
     **/
    Gdk::ScaledPixbuf&
      Lounge::player_icon(string const& player)
      {
        auto const icon_name = ::lounge->player(player).icon;
        if (this->player_icons.find(icon_name) == this->player_icons.end()) {
          auto icon_path = ::setting.dokolounge_player_icon(icon_name);
          if (icon_path.empty())
            icon_path = ::setting.dokolounge_player_icon("face1.jpg");
          this->player_icons[icon_name] = Gdk::ScaledPixbuf(icon_path);
        }

        this->player_icons[icon_name].set_to_max_size(100, 150);
        return this->player_icons[icon_name];
      } // Gdk::ScaledPixbuf& Lounge::player_icon(string player)

    /**
     ** -> result
     ** the icon is loaded if needed
     **
     ** @param     player   player name
     **
     ** @return    the player icon for the table
     **/
    Gdk::ScaledPixbuf&
      Lounge::player_table_icon(string const& player)
      {
        auto const icon_name = ::lounge->player(player).icon;
        if (this->player_icons.find(icon_name) == this->player_icons.end()) {
          auto icon_path = ::setting.dokolounge_player_icon(icon_name);
          if (icon_path.empty())
            icon_path = ::setting.dokolounge_player_icon("face1.jpg");
          this->player_icons[icon_name] = Gdk::ScaledPixbuf(icon_path);
        }

        this->player_icons[icon_name].set_to_max_size(30, 30);
        return this->player_icons[icon_name];
      } // Gdk::ScaledPixbuf& Lounge::player_table_icon(string player)

    /**
     ** -> result
     ** the icon is loaded if needed
     **
     ** @param     type   table type
     **
     ** @return    the table icon
     **/
    Gdk::ScaledPixbuf&
      Lounge::table_icon(string const& type)
      {
        if (this->table_icons.find(type) == this->table_icons.end()) {
          auto icon_path = ::setting.dokolounge_table_icon("Lobby" + type + "150.jpg");
          if (icon_path.empty())
            icon_path = ::setting.dokolounge_table_icon("tischhintergrund.jpg");
          this->table_icons[type] = Gdk::ScaledPixbuf(icon_path);
          this->table_icons[type].set_to_max_size(200, 150);
        }

        return this->table_icons[type];
      } // Gdk::ScaledPixbuf& Lounge::table_icon(string type)

  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
