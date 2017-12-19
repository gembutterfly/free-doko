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

#include "table.h"
#include "reservation.h"
#include "poverty.h"
#include "game_finished.h"
#include "game_review.h"
#include "party_points.h"
#include "party_finished.h"
#include "full_trick.h"
#include "last_trick.h"

#include "trick.h"
#include "hand.h"
#include "trickpile.h"
#include "icongroup.h"
#include "name.h"

#include "ui.h"
#include "main_window.h"
#include "card_suggestion.h"
#include "genscher_selection.h"
#include "gameinfo_dialog.h"
#include "gameinfos.h"

#include "../../party/party.h"
#include "../../game/game.h"
#include "../../player/player.h"
#include "../../player/ai/ai.h"
#include "../../card/hand.h"
#include "../../card/trick.h"
#include "../../os/bug_report_replay.h"

#include <gtkmm/drawingarea.h>
#include <gtkmm/main.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   parent object
 **
 ** @return    -
 **/
Table::Table(Base* const parent) :
  Base(parent),
  Gtk::DrawingArea(),
  party_points_(new PartyPoints(this)),
  gameinfo_(new GameInfoDialog(this)),
  get_card_(false)
{
  this->ui->main_window->container->add(*this);

  this->show();

  this->trick_ = std::make_unique<Trick>(this);
  this->hand_[SOUTH] = std::make_unique<Hand>(this, SOUTH);
  this->hand_[WEST]  = std::make_unique<Hand>(this, WEST);
  this->hand_[NORTH] = std::make_unique<Hand>(this, NORTH);
  this->hand_[EAST]  = std::make_unique<Hand>(this, EAST);
  this->trickpile_[SOUTH] = std::make_unique<TrickPile>(this, SOUTH);
  this->trickpile_[WEST]  = std::make_unique<TrickPile>(this, WEST);
  this->trickpile_[NORTH] = std::make_unique<TrickPile>(this, NORTH);
  this->trickpile_[EAST]  = std::make_unique<TrickPile>(this, EAST);
  this->icongroup_[SOUTH] = std::make_unique<Icongroup>(this, SOUTH);
  this->icongroup_[WEST]  = std::make_unique<Icongroup>(this, WEST);
  this->icongroup_[NORTH] = std::make_unique<Icongroup>(this, NORTH);
  this->icongroup_[EAST]  = std::make_unique<Icongroup>(this, EAST);
  this->name_[SOUTH] = std::make_unique<Name>(this, SOUTH);
  this->name_[WEST]  = std::make_unique<Name>(this, WEST);
  this->name_[NORTH] = std::make_unique<Name>(this, NORTH);
  this->name_[EAST]  = std::make_unique<Name>(this, EAST);
  this->reservation_[SOUTH] = std::make_unique<Reservation>(this, SOUTH);
  this->reservation_[WEST]  = std::make_unique<Reservation>(this, WEST);
  this->reservation_[NORTH] = std::make_unique<Reservation>(this, NORTH);
  this->reservation_[EAST]  = std::make_unique<Reservation>(this, EAST);

  this->add_events(Gdk::BUTTON_PRESS_MASK);
  this->add_events(Gdk::POINTER_MOTION_MASK);

  this->load_background();

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_expose_event().connect(sigc::mem_fun(*this, &Table::on_expose_event));
  this->signal_configure_event().connect(sigc::mem_fun(*this, &Table::on_configure_event));
  this->signal_button_press_event().connect(sigc::mem_fun(*this, &Table::on_button_press_event));
  this->signal_event().connect(sigc::mem_fun(*this, &Table::on_event));
  this->signal_motion_notify_event().connect(sigc::mem_fun(*this, &Table::on_motion_notify_event));
#endif

  return ;
} // Table::Table(Base* parent)

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
Table::~Table()
{ }

/**
 ** the party is opened
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::party_open()
{
  this->party_points_->party_open();
  this->draw_all();

  return ;
} // void Table::party_open()

/**
 ** create the party points
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::party_start()
{ }

/**
 ** show the party finished window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::party_finish()
{
  if (!this->party_finished_)
    this->party_finished_ = std::make_unique<PartyFinished>(this);

  this->draw_all();
  this->party_points_->party_finish();

  if (!(FAST_PLAY & FAST_NS::PARTY_FINISHED))
    this->party_finished_->party_finished();

  return ;
} // void Table::party_finish()

/**
 ** close the party points window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::party_close()
{
  this->party_points_->party_close();

  return ;
} // void Table::party_close()

/**
 ** a gameplay action has happened
 **
 ** @param     action   gameplay action
 **
 ** @return    -
 **/
void
Table::gameplay_action(GameplayAction const& action)
{
  if (this->gameinfo_)
    this->gameinfo_->update_announcement_reply();

  return ;
} // void Table::gameplay_action(GameplayAction action)

/**
 ** create the htin-objects
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::game_open()
{
  this->poverty_ = std::make_unique<Poverty>(this, this->ui->game());
  this->draw_all();

  return ;
} // void Table::game_open()

/**
 ** the cards are distributed in the game
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo     distribute in 3er packs (animation)
 **/
void
Table::game_cards_distributed()
{
  this->draw_all();

  return ;
} // void Table::game_cards_distributed()

/**
 ** the game is redistributed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::game_redistribute()
{
  this->poverty_ = std::make_unique<Poverty>(this, this->ui->game());

  this->draw_all();

  return ;
} // void Table::game_redistribute()

/**
 ** ask 'player' for a reservation
 **
 ** @param     player   player who is asked
 **
 ** @return    -
 **/
void
Table::reservation_ask(Player const& player)
{
  // redraw the names so that the current player is highlighted
  this->draw_all();

  // if this is the first reservation to ask, show the reservation window for the next human player
  if (!(   player.game().party().is_duty_soli_round()
        && !player.game().rule()(Rule::OFFER_DUTY_SOLO))) {
    for (Player const* p = &player.game().player_following(player);
         *p != player;
         p = &player.game().player_following(*p)) {
      if (p->type() == Player::HUMAN) {
        if (player == player.game().startplayer())
          this->reservation(*p).show_for_selection();
        else
          this->reservation(*p).update_for_reservation();
        break;
      } // if (p->type() == Player::HUMAN)
    } // for (p)

  } // if (player is startplayer and not human

  return ;
} // void Table::reservation_ask(Player player)

/**
 ** gets the reservation of the given player:
 **
 ** @param     player   player, whose reservation schould be get
 **
 ** @return    reservation of the player
 **/
::Reservation
Table::reservation_get(Player const& player)
{
  this->draw_all();
  return this->reservation_[this->position(player)]->get();
} // ::Reservation Table::reservation_get(player)

/**
 ** the game starts
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::game_start()
{
  this->draw_all();
  this->party_points_->game_start();

  for (auto& r : this->reservation_)
    r.second->hide();

  // if we have a solo player show the info window
  if (this->ui->game().type() != GAMETYPE::NORMAL) {

    if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
        && !(   ::bug_report_replay
             && ::bug_report_replay->auto_action())
        && ::setting(Setting::SHOW_GAMETYPE_WINDOW))
      this->gameinfo_->show(new GameInfoDialog::Information::GameType(this, this->ui->game()),
                            ::setting(Setting::CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY),
                            ::setting(Setting::GAMETYPE_WINDOW_CLOSE_DELAY));
  } // if (this->ui->game().type() != GAMETYPE::NORMAL)

  this->draw_all();

  return ;
} // void Table::game_start()

/**
 ** the player has to play a card
 **
 ** @param     player   the active player
 **
 ** @return    -
 **/
void
Table::card_get(Player const& player)
{
  if (   this->card_suggestion_
      && this->card_suggestion_->is_visible())
    this->show_card_suggestion();

  this->get_card_ = true;
  this->mouse_cursor_update();
  this->hand(player).draw();

  return ;
} // void Table::card_get(Player player)

/**
 ** the player has played a card
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::card_got()
{
  this->get_card_ = false;

  return ;
} // void Table::card_got()

/**
 ** the player has played a card
 **
 ** @param     player   the active player
 **
 ** @return    -
 **/
void
Table::card_played(Player const& player)
{

  if (this->card_suggestion_)
    this->card_suggestion_->card_played();

  if (::setting(Setting::SHOW_AI_INFORMATION_HANDS))
    this->draw_all_hands();
  else
    this->hand(player).draw();
  if (::setting(Setting::SHOW_AI_INFORMATION_TEAMS))
    this->icons_update();
  else {
    this->icongroup(player).draw();
  }

  this->mouse_cursor_update();

  return ;
} // void Table::card_played(Player player)

/**
 ** the trick is full
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::trick_full()
{
  this->trick().draw();
  this->name(this->ui->trick().winnerplayer()).draw();

  if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
      && !(   ::bug_report_replay
           && ::bug_report_replay->auto_action())) {
    if (!this->full_trick_)
      this->full_trick_ = std::make_unique<FullTrick>(this);
    this->full_trick_->show_information(this->ui->trick());
  }

  this->trick().draw();

  return ;
} // void Table::trick_full()

/**
 ** move the trick in the trickpile
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	animation
 **/
void
Table::trick_move_in_pile()
{
  this->trickpile(this->ui->trick().winnerplayer()).draw();
  this->trick_->draw();

  if (this->last_trick_)
    this->last_trick_->current_trick_closed();

  return ;
} // void Table::trick_move_in_pile()

/**
 ** show the last trick
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::show_last_trick()
{
  if (!(   (::game_status >= GAMESTATUS::GAME_PLAY)
        && (::game_status < GAMESTATUS::GAME_FINISHED)))
    return ;

  Game const& game = this->ui->party().game();

  if (game.real_trick_current_no() == 0)
    return ;

  this->show_trick(game.trick(game.real_trick_current_no() - 1));

  return ;
} // void Table::show_last_trick()

/**
 ** show the given trick
 **
 ** @param     trick   trick to show
 **
 ** @return    -
 **/
void
Table::show_trick(::Trick const& trick)
{
  if (!this->last_trick_)
    this->last_trick_ = std::make_unique<LastTrick>(this);

  if (this->last_trick_->is_visible()) {
    this->last_trick_->set_trickno(trick.no());
    this->last_trick_->present();
    return ;
  } // if (this->last_trick_->is_visible())

  this->last_trick_->show_last_trick(trick);

  this->mouse_cursor_update();

  return ;
} // void Table::show_trick(::Trick trick)

/**
 ** show the card suggestion
 **
 ** @param     show_window   whether to show the window
 **
 ** @return    -
 **/
void
Table::show_card_suggestion(bool const show_window)
{
  DEBUG_ASSERTION((this->ui->game().player_current().type()
                   == Player::HUMAN),
                  "Table::show_card_suggestion():\n"
                  "  current player is not human.");

  if (!this->card_suggestion_)
    this->card_suggestion_ = std::make_unique<CardSuggestion>(this);

  this->card_suggestion_->show_information(show_window);

  return ;
} // void Table::show_card_suggestion(bool show_window)

/**
 ** show the game finished window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::game_finished()
{
  if (!this->game_finished_)
    this->game_finished_ = std::make_unique<GameFinished>(this);

  this->draw_all();

  this->party_points_->game_finished();

  if (!(FAST_PLAY & FAST_NS::GAME_FINISHED))
    this->game_finished_->game_finished();

  this->poverty_.release();

  return ;
} // void Table::game_finished()

/**
 ** destroy the htin-objects
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Table::game_close()
{
  if (this->last_trick_)
    this->last_trick_->close();

  if (this->card_suggestion_)
    this->card_suggestion_->card_played();

  if (this->gameinfo_)
    this->gameinfo_->close();

  return ;
} // void Table::game_close()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the game is reviewed
 **/
bool
Table::in_game_review() const
{
  return (   (::game_status == GAMESTATUS::GAME_FINISHED)
          && (this->game_finished_)
          && (this->game_finished_->game_review));
} // bool Table::in_game_review() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    current tricknumber in the game review
 **/
unsigned
Table::game_review_trickno() const
{
  DEBUG_ASSERTION(this->in_game_review(),
                  "Table::game_review_trickno():\n"
                  "  not in game review");
  return this->game_finished_->game_review->trickno();
} // unsigned Table::game_review_trickno() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    current trick in the game review
 **/
::Trick const&
Table::game_review_trick() const
{
  DEBUG_ASSERTION(this->in_game_review(),
                  "Table::game_review_trickno():\n"
                  "  not in game review");
  return this->game_finished_->game_review->trick();
} // ::Trick const& Table::game_review_trick() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether a trick is visible in the game review
 **/
bool
Table::game_review_trick_visible() const
{
  DEBUG_ASSERTION(this->in_game_review(),
                  "Table::game_review_trickno():\n"
                  "  not in game review");
  return this->game_finished_->game_review->trick_visible();
} // bool Table::game_review_trick_visible() const

/**
 ** 'player' has announced 'announcement'
 **
 ** @param     announcement   the announcement that has been made
 ** @param     player   	the player that has announced swines
 **
 ** @return    -
 **/
void
Table::announcement_made(Announcement const announcement,
                         Player const& player)
{
  // All is drawn because there are many cases:
  // - draw the team and name of the announcer (because of overlapping)
  // - draw the team for 'no 120' announcement
  // - update the in-table-announcement icon for the human player
  // - update the hands for ai-information hands
  this->draw_all();

  if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
      && !(   ::bug_report_replay
           && ::bug_report_replay->auto_action())
      && ::setting(Setting::SHOW_ANNOUNCEMENT_WINDOW))
    this->gameinfo_->show(new GameInfoDialog::Information::Announcement(this, player, announcement),
                          ::setting(Setting::CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY),
                          ::setting(Setting::ANNOUNCEMENT_WINDOW_CLOSE_DELAY));

  return ;
} // void Table::announcement_made(Announcement const announcement, Player const& player)

/**
 ** swines have been announced
 **
 ** @param     player   the player that has announced swines
 **
 ** @return    -
 **/
void
Table::swines_announced(Player const& player)
{
  this->icongroup(player).draw_swines();
  this->hand(player).draw();

  if (!::setting(Setting::SHOW_ALL_HANDS)
      && (::game_status != GAMESTATUS::GAME_FINISHED)
      && (::game_status != GAMESTATUS::GAME_REDISTRIBUTE)
      && ::setting(Setting::SHOW_AI_INFORMATION_HANDS)
      && (player.game().number_of_humans() == 1)
     ) {
    // the cards information are shown
    this->draw_all();
  }

  if (player.type() != Player::HUMAN) {

    if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
        && !(   ::bug_report_replay
             && ::bug_report_replay->auto_action())
        && ::setting(Setting::SHOW_SWINES_WINDOW))
      this->gameinfo_->show(new GameInfoDialog::Information::Swines(this, player),
                            ::setting(Setting::CLOSE_SWINES_WINDOW_AUTOMATICALLY),
                            ::setting(Setting::SWINES_WINDOW_CLOSE_DELAY));

  } // if (player.type() != Player::HUMAN)

  return ;
} // void Table::swines_announced(Player const& player)

/**
 ** hyperswines have been announced
 **
 ** @param     player   the player that has announced hyperswines
 **
 ** @return    -
 **/
void
Table::hyperswines_announced(Player const& player)
{
  this->icongroup(*player.game().swines_owner()).draw_swines();
  this->icongroup(player).draw_swines();
  this->hand(player).draw();

  if (!::setting(Setting::SHOW_ALL_HANDS)
      && (::game_status != GAMESTATUS::GAME_FINISHED)
      && (::game_status != GAMESTATUS::GAME_REDISTRIBUTE)
      && ::setting(Setting::SHOW_AI_INFORMATION_HANDS)
      && (player.game().number_of_humans() == 1)
     ) {
    // the cards information are shown
    this->draw_all();
  }

  if (player.type() != Player::HUMAN) {

    if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
        && !(   ::bug_report_replay
             && ::bug_report_replay->auto_action())
        && ::setting(Setting::SHOW_SWINES_WINDOW)) {
      this->gameinfo_->show(new GameInfoDialog::Information::Hyperswines(this, player),
                            ::setting(Setting::CLOSE_SWINES_WINDOW_AUTOMATICALLY),
                            ::setting(Setting::SWINES_WINDOW_CLOSE_DELAY));
    } // if (!(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION))

  } // if (player.type() != Player::HUMAN)

  return ;
} // void Table::hyperswines_announced(Player const& player)

/**
 ** marriage: Information of the new team
 ** if the bride is the bridegroom, the bridegroom must play a solo
 **
 ** @param     bridegroom   the player with the marriage
 ** @param     bride   	the bride
 **
 ** @return    -
 **/
void
Table::marriage(Player const& bridegroom,
                Player const& bride)
{
  this->icons_update();

  if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
      && !(   ::bug_report_replay
           && ::bug_report_replay->auto_action())
      && ::setting(Setting::SHOW_MARRIAGE_WINDOW)) {
    this->gameinfo_->show(new GameInfoDialog::Information::Marriage(this, bridegroom, bride),
                          ::setting(Setting::CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY),
                          ::setting(Setting::MARRIAGE_WINDOW_CLOSE_DELAY));
  }

  return ;
} // void Table::marriage(Player const& bridegroom, Player const& bride)

/**
 ** select a partner
 **
 ** @param     genscher   player with the genscher
 **
 ** @return    pointer to the new teammate,
 **		nullptr if the genscher is not to be announced
 **/
Player const*
Table::select_genscher_partner(Player const& genscher)
{
  if (!this->genscher_selection_)
    this->genscher_selection_ = std::make_unique<GenscherSelection>(this);

  return this->genscher_selection_->select_partner(genscher);
} // Player const* Table::select_genscher_partner(Player const& genscher)

/**
 ** genscher information
 **
 ** @param     genscher   player with the genscher
 ** @param     partner   	new partner
 **
 ** @return    -
 **/
void
Table::genscher(Player const& genscher,
                Player const& partner)
{
  this->icons_update();
  if (!::setting(Setting::SHOW_ALL_HANDS)
      && (::game_status != GAMESTATUS::GAME_FINISHED)
      && (::game_status != GAMESTATUS::GAME_REDISTRIBUTE)
      && ::setting(Setting::SHOW_AI_INFORMATION_HANDS)
      && (genscher.game().number_of_humans() == 1)
     ) {
    this->draw_all();
  }

  if (genscher.type() != Player::HUMAN) {

    if (   !(FAST_PLAY & FAST_NS::GAMEPLAY_INFORMATION)
        && !(   ::bug_report_replay
             && ::bug_report_replay->auto_action())
        && ::setting(Setting::SHOW_GENSCHER_WINDOW)) {
      this->gameinfo_->show(new GameInfoDialog::Information::Genscher(this, genscher, partner),
                            ::setting(Setting::CLOSE_GENSCHER_WINDOW_AUTOMATICALLY),
                            ::setting(Setting::GENSCHER_WINDOW_CLOSE_DELAY));
    }

  } // if (genscher.type() != Player::HUMAN)

  return ;
} // void Table::genscher(Player const& genscher, Player const& partner)

/**
 ** event: mouse click
 **
 ** @param     button_event   the event of the mouse clock
 **
 ** @return    whether there was a reaction
 **/
bool
Table::on_button_press_event(GdkEventButton* button_event)
{
  int const x = static_cast<int>(button_event->x);
  int const y = static_cast<int>(button_event->y);

  if (this->in_game_review()) {
    switch (button_event->button) {
    case 3:
    case 4:
      this->game_finished_->game_review->previous_trick();
      this->mouse_cursor_update();
      return true;
    case 2:
      this->game_finished_->game_review->set_trickno(0);
      this->mouse_cursor_update();
      return true;
    case 1:
    case 5:
      this->game_finished_->game_review->following_trick();
      this->mouse_cursor_update();
      return true;
    default:
      break;
    }
  } // if (this->in_game_review)

  // shifting cards in a poverty
  if (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
    if (this->poverty().button_press_event(button_event)) {
      this->mouse_cursor_update();
      return true;
    }

  { // test, whether the last trick shall be shown
    if (::in_running_game()) {
      Game const& game = this->ui->game();
      for (auto p = game.players_begin();
           p != game.players_end();
           ++p) {
        ::Trick const* const trick
          = this->trickpile(**p).over_trick(x, y);
        if (trick) {
          switch (button_event->button) {
          case 1: // left mouse button
            this->show_trick(*trick);
            this->mouse_cursor_update();
            return true;
          default:
            break;
          } // switch (button_event->button)
        } // if (trick)
      } // for (p \in game.player)
    } // if (::game_status in game)
  } // test, whether the last trick shall be shown

  { // test whether an announcement shall be made
    if (button_event->button == 1) {
      if (::in_running_game()) {
        for (auto p = this->ui->game().players_begin();
             p != this->ui->game().players_end();
             ++p) {
          if (   ((*p)->type() == Player::HUMAN)
              && this->icongroup(**p).mouse_over_next_announcement()) {
            this->ui->game().make_announcement(static_cast<Announcement>((*p)->game().announcement_of_team((*p)->team()) + 1),
                                               **p);
            this->mouse_cursor_update();
            return true;
          }
        } // for (p)
      } // if (::in_running_game())
    } // if (button_event->button == 3)
  } // test whether an announcement shall be made

  { // test whether the full trick shall be closed
    if (button_event->button == 1) {
      if (   ::in_running_game()
          && this->ui->game().trick_current().isfull()
          && this->full_trick_
          && this->trick().mouse_over_cards_sloppy()) {
        this->full_trick_->close();
        return true;
      } // if (::in_running_game() && trick full && ...)
    } // if (button_event->button == 1)
  } // test whether the full trick shall be closed

  { // close the gameinfo dialogs
    if (button_event->button == 3) {
      this->gameinfo_->close();
      if (this->full_trick_)
        this->full_trick_->close();
      if (this->last_trick_)
        this->last_trick_->hide();
      if (this->card_suggestion_)
        this->card_suggestion_->hide();
    } // if (button_event->button == 3)
  } // close the gameinfo dialogs

  { // test, whether a card shall be played
    if (this->get_card()) {
      Player& player = this->ui->game().player_current();
      if (player.type() == Player::HUMAN) {
        unsigned const cardno = this->hand(player).cardno_at_position(x, y);

        if (cardno != UINT_MAX) {
          switch (button_event->button) {
          case 1: // left mouse button
            player.sorted_hand().request_position_all(cardno);
            if (   !this->card_suggestion_
                || (player.sorted_hand().card_all(cardno)
                    != this->card_suggestion_->suggested_card) )
              static_cast<Ai&>(player).lastHeuristic_ = Aiconfig::MANUAL;
            // ToDo: only do this, if the card is in the hand
            //       (for a bug report replay)
            this->ui->thrower(player.sorted_hand().requested_card(),
                              __FILE__, __LINE__);

            this->mouse_cursor_update();
            return true;
          case 3: // right mouse button
            if (this->card_suggestion_)
              this->card_suggestion_->play_card();
            this->mouse_cursor_update();
            return true;
          default:
            break;
          } // switch (button_event->button)
        } // if (cardno != UINT_MAX)
      } // if (player is human)
    } // if (::game_status == GAMESTATUS::GAME_PLAY)
  } // test, whether a card shall be played


#ifndef RELEASE
#if 0
  switch (button_event->button) {
  case 1: // left mouse button
    CLOG << "mouse: " << x << '+' << y << '\n';
  } // switch (button_event->button)
#endif
#endif

#ifdef WINDOWS
#ifdef WORKAROUND
  // Under MS-Windows the information windows can get in the background of the main window, although they should stay in the front.
  // So, when a mouse click is made on the table which is not otherwise interpreted, the information windows are put in the foreground.
  { // put a gameinfo dialog, ... in the foreground
    if (this->game_finished_
        && this->game_finished_->is_visible())
      this->game_finished_->raise();
    if (this->party_finished_
        && this->party_finished_->is_visible())
      this->party_finished_->raise();
    if (this->card_suggestion_
        && this->card_suggestion_->is_visible())
      this->card_suggestion_->raise();
    if (this->gameinfo_
        && this->gameinfo_->is_visible())
      this->gameinfo_->raise();
    if (this->full_trick_
        && this->full_trick_->is_visible())
      this->full_trick_->raise();
    if (this->last_trick_
        && this->last_trick_->is_visible())
      this->last_trick_->raise();
    if (this->genscher_selection_
        && this->genscher_selection_->is_visible())
      this->genscher_selection_->raise();
  } // put gameinfo dialog, ... in the foreground
#endif
#endif

  this->mouse_cursor_update();
  return false;
} // bool Table::on_button_press_event(GdkEventButton* event)

/**
 ** event: mouse scroll
 **
 ** @param     event   the event of the scrolling
 **
 ** @return    whether there was a reaction
 **/
bool
Table::on_scroll_event(GdkEventScroll* const event)
{
  if (this->in_game_review()) {
    switch (event->direction) {
    case GDK_SCROLL_UP:
      this->game_finished_->game_review->previous_trick();
      this->mouse_cursor_update();
      break;
    case GDK_SCROLL_DOWN:
      this->game_finished_->game_review->following_trick();
      this->mouse_cursor_update();
      break;
    default:
      break;
    } // switch (event->direction)
  } // if (this->in_game_review)

  return true;
} // bool Table::on_scroll_event(GdkEventScroll* event)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
