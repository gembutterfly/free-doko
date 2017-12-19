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
#include "poverty.h"
#include "party_points.h"
#include "party_finished.h"
#include "last_trick.h"
#include "trick.h"
#include "hand.h"
#include "trickpile.h"
#include "icongroup.h"
#include "name.h"
#include "reservation.h"

#include "ui.h"
#include "cards.h"
#include "utils.h"
#include "gameinfo_dialog.h"
#include "full_trick.h"
#include "game_finished.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../player/player.h"
#include "../../card/trick.h"
#include "../../misc/setting.h"
#include "../../utils/string.h"

#include <gtkmm/drawingarea.h>
#include <gdkmm/general.h>
#include <cmath>
using std::pow;

namespace UI_GTKMM_NS {

/**
 ** clear the whole table
 **
 ** @param     cr     drawing context
 **
 ** @return    -
 **/
void
  Table::clear(Cairo::RefPtr<::Cairo::Context> cr)
  {
    this->clear(cr, Gdk::Rectangle(0, 0, this->width(), this->height()));
  } // void Table::clear(Cairo::RefPtr<::Cairo::Context> cr)

/**
 ** clears the rectangle
 **
 ** @param     cr     drawing context
 ** @param     area   area to clear
 **
 ** @return    -
 **/
void
  Table::clear(Cairo::RefPtr<::Cairo::Context> cr,
               Gdk::Rectangle const& area)
  {
    if (!this->get_realized())
      return ;
    if (!this->background)
      return ;

    cr->save();
    cr->set_source(this->background);
    cr->rectangle(area.get_x(), area.get_y(),
                  area.get_width(), area.get_height());
    cr->fill();
    cr->restore();

    return ;
  } // void Table::clear(Cairo::RefPtr<::Cairo::Context> cr, Gdk::Rectangle area)

#ifndef RELEASE
/**
 ** clears the rectangle (debug mode)
 **
 ** @param     cr     drawing context
 ** @param     area   area to clear
 **
 ** @return    -
 **/
void
  Table::clear_debug(Cairo::RefPtr<::Cairo::Context> cr,
                     Gdk::Rectangle const& area)
  {
    if (!this->get_realized())
      return ;

    CLOG << "clear " << area.get_x() << '+' << area.get_y() << ','
      << area.get_width() << 'x' << area.get_height() << '\n';
    cr->save();
    cr->set_source_rgb(0.5, 0.5, 0.5);
    cr->rectangle(area.get_x(), area.get_y(),
                  area.get_width(), area.get_height());
    cr->fill();
    cr->restore();
  } // void Table::clear_debug(Cairo::RefPtr<::Cairo::Context> cr, Gdk::Rectangle area)
#endif

/**
 ** draw all
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::draw_all()
  {
    if (!this->get_realized())
      return ;

    auto cr = this->create_cairo_context();

    cr->push_group();
    this->clear(cr);

    if (::game_status & GAMESTATUS::GAME) {

      for (auto& h : this->hand_)
        h.second->draw(cr);
      for (auto& t : this->trickpile_)
        t.second->draw(cr);
      for (auto& i : this->icongroup_)
        i.second->draw(cr);
      for (auto& n : this->name_) {
        n.second->draw(cr);
      }
      if (this->trick_)
        this->trick_->draw(cr);

      if (   (this->ui->game().type() == GAMETYPE::POVERTY)
          && (    (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
              || (::game_status == GAMESTATUS::GAME_REDISTRIBUTE) )
          && this->poverty_)
        this->poverty().draw(cr);

    } else { // if (::game_status)
      this->draw_logo(cr);
    } // if (::game_status)
    cr->pop_group_to_source();
    cr->paint();

    this->mouse_cursor_update();

    return ;
  } // void Table::draw_all()

/**
 ** draw the logo
 **
 ** @param     cr     drawing context
 **
 ** @return    -
 **
 ** @bug	clip mask does not work (GTK-bug?)
 **/
void
  Table::draw_logo(Cairo::RefPtr<::Cairo::Context> cr)
  {
    if (!this->get_realized())
      return ;
    if (!this->ui->logo)
      return ;

    cr->push_group();
    Gdk::Cairo::set_source_pixbuf(cr, this->ui->logo,
                                  (this->width()
                                   - this->ui->logo->get_width()) / 2,
                                  (this->height()
                                   - this->ui->logo->get_height()) / 2);
    cr->paint();
    { // draw a border with the cards
      vector<Card> border_card; // the border cards
      unsigned const card_width = this->ui->cards->width();
      unsigned const card_height = this->ui->cards->height();

#ifdef TODO
      // the bitmap of the border cards on the top
      vector<Glib::RefPtr<Gdk::Bitmap> > border_card_top_bitmap;
#endif

      // the border of the top card row
      unsigned border_top_x;

      { // top
        vector<Card> const border_card =  {
          {Card(Card::HEART,	Card::TEN)},
          {Card(Card::CLUB,	Card::QUEEN)},
          {Card(Card::SPADE,	Card::QUEEN)},
          {Card(Card::HEART,	Card::QUEEN)},
          {Card(Card::DIAMOND,	Card::QUEEN)},
          {Card(Card::CLUB,	Card::JACK)},
          {Card(Card::SPADE,	Card::JACK)}};

        border_top_x
          = (card_width
             + ( (border_card.size() * card_height
                  + card_width >= this->width())
                ? 0
                : ((this->width() - (border_card.size() * card_height
                                     + card_width)
                   ) / border_card.size())
               )
            );

        for (size_t n = 0; n < border_card.size(); n++) {
          Gdk::Cairo::set_source_pixbuf(cr, 
                                        this->ui->cards->card(border_card[n], Rotation::LEFT),
                                        border_top_x +
                                        n * (this->width() - 2 * border_top_x
                                             - card_height)
                                        / (border_card.size() - 1),
                                        card_width / 2);
          cr->paint();
#ifdef TODO
          // set the bitmap
          border_card_top_bitmap.push_back(bitmap(this->ui->cards->card(border_card[n], Rotation::LEFT)));
#endif
        } // for (n < border_card.size())
      } // top
      { // right
        vector<Card> const border_card = {
          {Card(Card::HEART,	Card::JACK)},
          {Card(Card::DIAMOND,	Card::JACK)},
          {Card(Card::DIAMOND,	Card::ACE)},
          {Card(Card::DIAMOND,	Card::TEN)},
          {Card(Card::DIAMOND,	Card::KING)}};

        unsigned const border_y
          = (card_width
             + ( (border_card.size() * card_height
                  + card_width >= this->height())
                ? 0
                : ((this->height() - (border_card.size() * card_height
                                      + card_width)
                   ) / border_card.size())
               )
            );

        for (size_t n = 0; n < border_card.size(); n++) {
          Gdk::Cairo::set_source_pixbuf(cr, 
                                        this->ui->cards->card(border_card[n], Rotation::UP),
                                        this->width() - 3 * card_width / 2,
                                        border_y +
                                        n * (this->height() - 2 * border_y
                                             - card_height)
                                        / (border_card.size() - 1)
                                       );
          cr->paint();
        }
      } // right
      { // bottom
        vector<Card> const border_card = {
          {Card(Card::DIAMOND,	Card::NINE)},
          {Card(Card::CLUB,	Card::ACE)},
          {Card(Card::CLUB,	Card::TEN)},
          {Card(Card::CLUB,	Card::KING)},
          {Card(Card::CLUB,	Card::NINE)},
          {Card(Card::HEART,	Card::ACE)},
          {Card(Card::HEART,	Card::KING)}};

        unsigned const border_x
          = (card_width
             + ( (border_card.size() * card_height
                  + card_width >= this->width())
                ? 0
                : ((this->width() - (border_card.size() * card_height
                                     + card_width)
                   ) / border_card.size())
               )
            );

        for (size_t n = 0; n < border_card.size(); n++) {
          Gdk::Cairo::set_source_pixbuf(cr, 
                                        this->ui->cards->card(border_card[n], Rotation::RIGHT),
                                        border_x +
                                        (border_card.size() - 1 - n)
                                        * (this->width() - 2 * border_x
                                           - card_height)
                                        / (border_card.size() - 1),
                                        this->height()
                                        - 3 * card_width / 2
                                       );
          cr->paint();
        }
      } // bottom
      { // left
        vector<Card> const border_card = {
          {Card(Card::HEART,	Card::NINE)},
          {Card(Card::SPADE,	Card::ACE)},
          {Card(Card::SPADE,	Card::TEN)},
          {Card(Card::SPADE,	Card::KING)},
          {Card(Card::SPADE,	Card::NINE)}};


        unsigned const border_y
          = (card_width
             + ( (border_card.size() * card_height
                  + card_width >= this->height())
                ? 0
                : ((this->height() - (border_card.size() * card_height
                                      + card_width)
                   ) / border_card.size())
               )
            );

        for (size_t n = 0; n < border_card.size(); n++) {
          int const pos_x = card_width / 2;
          int const pos_y = (border_y +
                             (border_card.size() - 1 - n)
                             * (this->height() - 2 * border_y
                                - card_height)
                             / (border_card.size() - 1));

#ifdef TODO
          // for the overlapping of the left with the upper row
          // use a separate gc
          Glib::RefPtr<Gdk::Bitmap> bitmap_overlapping
            = bitmap(this->ui->cards->card(border_card[n],
                                           DOWN));
          // for adding the bitmaps
          Glib::RefPtr<Gdk::GC> gc_add
            = (!bitmap_overlapping
               ? Gdk::GC::create(this->pixmap)
               : Gdk::GC::create(bitmap_overlapping));
          gc_add->set_function(Gdk::AND_INVERT);
          if (bitmap_overlapping)
            for (unsigned i = 0; i < border_card_top_bitmap.size(); i++) {
              bitmap_overlapping->draw_drawable(gc_add,
                                                border_card_top_bitmap[i],
                                                0, 0,
                                                border_top_x +
                                                i * (this->width() - 2 * border_top_x
                                                     - card_height)
                                                / (border_card.size() - 1)
                                                - pos_x,
                                                card_width / 2
                                                - pos_y,
                                                card_height, card_width);
            } // for (i < border_card_top_bitmap.size())

          Glib::RefPtr<Gdk::GC> gc_overlapping
            = Gdk::GC::create(this->pixmap);
          if (bitmap_overlapping)
            gc_overlapping->set_clip_mask(bitmap_overlapping);
          gc_overlapping->set_clip_origin(pos_x, pos_y);
#endif

          Gdk::Cairo::set_source_pixbuf(cr, 
                                        this->ui->cards->card(border_card[n], Rotation::DOWN),
                                        pos_x, pos_y);
          cr->paint();
        }
      } // left
    } // draw a border with the cards
    cr->pop_group_to_source();
    cr->paint();

    return ;
  } // void Table::draw_logo(Cairo::RefPtr<::Cairo::Context> cr)

/**
 ** draw all hands
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::draw_all_hands()
  {
    if (!(::game_status & GAMESTATUS::GAME))
      return;

    auto cr = this->create_cairo_context();
    cr->push_group();
    for (auto& h : this->hand_)
      h.second->draw(cr);
    cr->pop_group_to_source();
    cr->paint();

    return ;
  } // void Table::draw_all_hands()

/**
 ** update all cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::cards_update()
  {
    this->draw_all();

    return ;
  } // void Table::cards_update()

/**
 ** update all cards back
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::cards_back_update()
  {
    this->draw_all_hands();

    return ;
  } // void Table::cards_back_update()

/**
 ** update all icons
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::icons_update()
  {
    // update all, because the size of the icons can have changed
    this->draw_all();

    return ;
  } // void Table::icons_update()

/**
 ** loads the background
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::load_background()
  {
    try {
      auto back_new = Gdk::Pixbuf::create_from_file(::setting.path(Setting::BACKGROUND));
      if (!back_new)
        throw Glib::FileError(Glib::FileError::FAILED,
                              "error loading pixmap from '"
                              + ::setting.path(Setting::BACKGROUND) + "'");

      auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, 
                                                 back_new->get_width(),
                                                 back_new->get_height());
      auto cr = Cairo::Context::create(surface);
      Gdk::Cairo::set_source_pixbuf(cr, back_new, 0, 0);
      cr->paint();
      this->background = Cairo::SurfacePattern::create(surface);
      this->background->set_extend(Cairo::EXTEND_REPEAT);

      this->draw_all();
    } catch (Glib::FileError const& file_error) {
      this->ui->error(::translation("Error::loading the background %sbackground%",
                                    ::setting.value(Setting::BACKGROUND)
                                   ).translation());
    } // try

    return ;
  } // void Table::load_background()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
  Table::name_changed(Player const& player)
  {
    if (::game_status < GAMESTATUS::GAME_INIT)
      return ;
    this->draw_all();
    if (this->party_points_)
      this->party_points_->name_changed(player);

    this->gameinfo_->name_changed(player);

    if (this->last_trick_)
      this->last_trick_->name_changed(player);

    if (this->game_finished_)
      this->game_finished_->name_changed(player);

    if (this->party_finished_)
      this->party_finished_->name_changed(player);

    if (this->name_.size())
      this->name(player).update_name();

    return ;
  } // void Table::name_changed(Player const& player)

/**
 ** updates the font
 **
 ** @param     fontname   the name of the new font
 ** @param     type   	  the type of the font
 **
 ** @return    -
 **/
void
  Table::new_font(string const& fontname, Setting::TypeString const type)
  {
    auto cr = this->create_cairo_context();
    cr->push_group();
    switch (type) {
    case Setting::NAME_FONT:
      for (auto& n : this->name_)
        n.second->update_layout();

      this->draw_all();
      break;

    case Setting::TRICKPILE_POINTS_FONT:
      for (auto& t : this->trickpile_)
        t.second->update_layout();

      this->draw_all();
      break;
      break;
    default:
      break;
    } // switch (type)
    cr->pop_group_to_source();
    cr->paint();

    return ;
  } // void Table::new_font(string fontname, Setting::TypeString type)

/**
 ** updates the  color
 **
 ** @param     colorname   the name of the new color
 ** @param     type   	the type of the font
 **
 ** @return    -
 **
 ** @todo	using 'set_foreground'
 **/
void
  Table::new_color(string const& colorname, Setting::TypeString const type)
  {
    auto cr = this->create_cairo_context();
    cr->push_group();
    switch (type) {
    case Setting::NAME_FONT_COLOR:
    case Setting::NAME_ACTIVE_FONT_COLOR:
    case Setting::NAME_RESERVATION_FONT_COLOR: {
#ifdef TODO
      Glib::RefPtr<Gdk::GC> gc;
      switch (type) {
      case Setting::NAME_FONT_COLOR:
        gc = this->name_gc;
        break;
      case Setting::NAME_ACTIVE_FONT_COLOR:
        gc = this->name_active_gc;
        break;
      case Setting::NAME_RESERVATION_FONT_COLOR:
        gc = this->name_reservation_gc;
        break;
      default:
        break;
      } // switch (type)
      if (!gc)
        break;

      gc->set_rgb_fg_color(Gdk::Color(colorname));
#endif

      for (auto& n : this->name_)
        n.second->draw(cr);

    } break;

    case Setting::TRICKPILE_POINTS_FONT_COLOR:
#ifdef TODO
      if (!this->trickpile_points_gc)
        break;

      this->trickpile_points_gc->set_rgb_fg_color(Gdk::Color(colorname));
#ifdef POSTPONED
      this->trickpile_points_gc->set_foreground(Gdk::Color(colorname));
#endif
#endif

      for (auto& t : this->trickpile_)
        t.second->draw(cr);
      break;

    case Setting::POVERTY_SHIFT_ARROW_COLOR:
#ifdef TODO
      if (!this->poverty_shift_arrow_gc)
        break;

      this->poverty_shift_arrow_gc->set_rgb_fg_color(Gdk::Color(colorname));
#endif
      if ((::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
          && this->poverty_)
        this->poverty().draw(cr);

      break;

    default:
      break;
    } // switch(type)
    cr->pop_group_to_source();
    cr->paint();

    return ;
  } // void Table::new_color(string colorname, Setting::TypeString type)

/**
 ** the setting has changed
 **
 ** @param     type   the type that has changed
 **
 ** @return    -
 **/
void
  Table::setting_update(int const type)
  {
    auto cr = this->create_cairo_context();
    cr->push_group();
    switch(type) {
    case Setting::SHOW_ALL_HANDS:
      this->draw_all();
      break;
    case Setting::EMPHASIZE_VALID_CARDS:
      if ( (::game_status == GAMESTATUS::GAME_PLAY)
          && (this->ui->game().player_current().type() == Player::HUMAN) ) {
        this->draw(cr);
        this->mouse_cursor_update();
      }
      break ;
    case Setting::AUTOMATIC_CARD_SUGGESTION:
      if (::setting(Setting::AUTOMATIC_CARD_SUGGESTION))
        if (   (::game_status == GAMESTATUS::GAME_PLAY)
            && this->ui->party().rule()(Rule::CARD_HINT)
            && !this->ui->game().trick_current().isfull()
            && (this->ui->game().player_current().type() == Player::HUMAN))
          this->show_card_suggestion(false);
      break;
    case Setting::SHOW_TRICKPILES_POINTS:
      for (auto& t : this->trickpile_)
        t.second->draw(cr);
      break;
    case Setting::ANNOUNCE_SWINES_AUTOMATICALLY:
      if (::game_status == GAMESTATUS::GAME_RESERVATION) {
        for (auto& r : this->reservation_)
          r.second->sensitivity_update();
      } // if (::game_status == GAMESTATUS::GAME_RESERVATION)
    case Setting::BACKGROUND:
      this->load_background();
      break;
    case Setting::ROTATE_TRICK_CARDS:
#ifdef POSTPONED
      // The old trick is not cleared fully
      this->trick_->draw(cr);
#else
      this->draw_all();
#endif
      break;
    case Setting::NAME_FONT:
    case Setting::TRICKPILE_POINTS_FONT:
      this->new_font(::setting(static_cast<Setting::TypeString const>(type)),
                     static_cast<Setting::TypeString const>(type));
      break;
    case Setting::NAME_FONT_COLOR:
    case Setting::NAME_ACTIVE_FONT_COLOR:
    case Setting::TRICKPILE_POINTS_FONT_COLOR:
    case Setting::POVERTY_SHIFT_ARROW_COLOR:
      this->new_color(::setting(static_cast<Setting::TypeString const>(type)),
                      static_cast<Setting::TypeString const>(type));
      break;
    case Setting::OWN_HAND_ON_TABLE_BOTTOM:
    case Setting::TABLE_ROTATION:
      for (auto& n : this->name_) {
        n.second->update_name();
      }
      this->draw_all();
      break;

    default:
      break;
    } // switch(type)
    cr->pop_group_to_source();
    cr->paint();

    return ;
  } // void Table::setting_update(int type)

/**
 ** update the mouse cursor
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::mouse_cursor_update()
  {
    if (!this->get_realized())
      return ;
    int x, y;
    this->get_pointer(x, y);

    CursorType new_cursor_type = CursorType::DEFAULT;
    if (this->ui->busy())
      new_cursor_type = CursorType::BUSY;

    switch (::game_status) {
    case GAMESTATUS::GAME_POVERTY_SHIFT:
      DEBUG_ASSERTION(this->poverty_,
                      "Table::mouse_cursor_update():\n"
                      "  'this->poverty_' == nullptr");
      switch (this->poverty().possible_action(x, y)) {
      case Poverty::Action::NONE:
        break;
      case Poverty::Action::SHIFT_CARDS:
        new_cursor_type = CursorType::POVERTY_SHIFT;
        break;
      case Poverty::Action::ACCEPT_CARDS:
        new_cursor_type = CursorType::POVERTY_ACCEPT;
        break;
      case Poverty::Action::TAKE_CARD:
        new_cursor_type = CursorType::POVERTY_GET_CARD;
        break;
      case Poverty::Action::PUT_CARD:
        new_cursor_type = CursorType::POVERTY_PUT_CARD;
        break;
      case Poverty::Action::FILL_UP:
        new_cursor_type = CursorType::POVERTY_FILL_UP;
        break;
      case Poverty::Action::RETURN_CARDS:
        new_cursor_type = CursorType::POVERTY_SHIFT_BACK;
        break;
      case Poverty::Action::GET_CARDS_BACK:
        new_cursor_type = CursorType::POVERTY_GETTING_BACK;
        break;

      } // switch (this->poverty().possible_action(x, y))
      break;

    case GAMESTATUS::GAME_PLAY: {
      // the human player has to play a card
      Player& player = this->ui->game().player_current();
      if (this->get_card()) {
        DEBUG_ASSERTION((player.type() == Player::HUMAN),
                        "UI_GTKMM::Table::mouse_cursor_update():\n"
                        "  a card should be get, but the player '"
                        << this->ui->game().no(player) << "' is not human but '"
                        << player.type() << "'");
        auto const card = this->hand(player).card_at_position(x, y);
        if (!card.is_empty()) {
          if (::setting(Setting::SHOW_IF_VALID)
              && player.game().rule()(Rule::SHOW_IF_VALID)) {
            if (player.game().trick_current().isvalid(card, player.hand()))
              new_cursor_type = CursorType::CARD_VALID;
            else
              new_cursor_type = CursorType::CARD_INVALID;
          } else { // if !(SHOW_IF_VALID)
            new_cursor_type = CursorType::PLAY_CARD;
          } // if !(SHOW_IF_VALID)

        } // if (card)
      } // if (player.type() == Player::HUMAN)
      break;
    } // case GAMESTATUS::GAME_PLAY:
    default:
      if (   ::in_running_game()
          && this->ui->game().trick_current().isfull()
          && this->full_trick_
          && this->trick().mouse_over_cards_sloppy()) {
        new_cursor_type = CursorType::CLOSE_FULL_TRICK;
      } // if (::game_status == ...)
      break;
    } // switch(::game_type)

    if (::in_running_game()) {
      Game const& game = this->ui->game();
      for (auto& p : game.players_const()) {
        if (this->trickpile(*p).over_trick(x, y))
          new_cursor_type = CursorType::SHOW_LAST_TRICK;
        if (p->type() == Player::HUMAN) {
          if (this->icongroup(*p).mouse_over_next_announcement())
            new_cursor_type = CursorType::NEXT_ANNOUNCEMENT;
        } // if (p->type() == Player::HUMAN)
      } // for (p : game.player)
    }	// if (::game_status in game)


    if (this->in_game_review())
      new_cursor_type = CursorType::GAME_REVIEW;

    if (this->cursor_type != new_cursor_type) {
      if (   (this->cursor_type == CursorType::NEXT_ANNOUNCEMENT)
          || (new_cursor_type == CursorType::NEXT_ANNOUNCEMENT) ) {
        this->cursor_type = new_cursor_type;
        this->draw_all();
      }
      this->cursor_type = new_cursor_type;
      this->get_window()->set_cursor(Table::cursor(this->cursor_type));
    } // if (this->cursor_type != new_cursor_type)

    return ;
  } // void Table::mouse_cursor_update()

/**
 ** event: draw the table
 **
 ** @param     c   the graphic context
 **
 ** @return    true
 **/
bool
  Table::on_draw(::Cairo::RefPtr<::Cairo::Context> const& c)
  {
    this->draw_all();

    return true;
  } // bool Table::on_draw(::Cairo::RefPtr<::Cairo::Context> c)

/**
 ** event: the size has changed
 **
 ** @param     event   the event
 **
 ** @return    true
 **/
bool
  Table::on_configure_event(GdkEventConfigure* const event)
  {
    this->draw_all();

    return true;
  } // bool Table::on_configure_event(GdkEventConfigure* event)

/**
 ** event: mouse motion
 **
 ** @param     event   the event
 **
 ** @return    whether there was a reaction
 **/
bool
  Table::on_motion_notify_event(GdkEventMotion* const event)
  {
    this->mouse_cursor_update();

    return false;
  } // bool Table::on_motion_notify_event(GdkEventMotion* event)

/**
 ** -> result
 **
 ** @param     type   the cursor type
 **
 ** @return    the cursor for 'type'
 **/
Glib::RefPtr<Gdk::Cursor>
  Table::cursor(CursorType const type)
  {
    switch (type) {
    case CursorType::NONE:
      DEBUG_ASSERTION(false,
                      "Table::cursor(type):\n"
                      "  'type' == NONE");
      break;
    case CursorType::DEFAULT: {
      static auto const cursor = Gdk::Cursor::create(this->get_display(), "default");
      return cursor;
    }
      break;
    case CursorType::BUSY: {
#ifdef POSTPONED
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file("ui/gtkmm/cursor/busy.png");
      {
        GdkCursor* cursor = gdk_cursor_new_from_pixbuf(Gdk::Cursor(Gdk::X_CURSOR).get_display()->gobj(), pixbuf->gobj(), 0, 0);
        //Gdk::Cursor(Gdk::Pixbuf::create_from_file("ui/gtkmm/cursor/busy.png"),);
        return cursor;
      }
#endif
    }
      {
        static auto const cursor = Gdk::Cursor::create(Gdk::WATCH);
        return cursor;
      }
      //return Gdk::Cursor(Gdk::EXCHANGE);
      //return Gdk::Cursor(Gdk::CLOCK);
    case CursorType::POVERTY_SHIFT: {
      static auto const cursor_left = Gdk::Cursor::create(Gdk::SB_LEFT_ARROW);
      static auto const cursor_right = Gdk::Cursor::create(Gdk::SB_RIGHT_ARROW);
      static auto const cursor_up = Gdk::Cursor::create(Gdk::SB_UP_ARROW);
      static auto const cursor_down = Gdk::Cursor::create(Gdk::SB_DOWN_ARROW);
      switch (this->position(this->ui->game().player_current())) {
      case SOUTH:
        return cursor_left;
      case NORTH:
        return cursor_right;
      case WEST:
        return cursor_up;
      case EAST:
        return cursor_down;
      default:
        return cursor_left;
      }
    }
    case CursorType::POVERTY_SHIFT_BACK: {
      static auto const cursor_left = Gdk::Cursor::create(Gdk::SB_LEFT_ARROW);
      static auto const cursor_right = Gdk::Cursor::create(Gdk::SB_RIGHT_ARROW);
      static auto const cursor_up = Gdk::Cursor::create(Gdk::SB_UP_ARROW);
      static auto const cursor_down = Gdk::Cursor::create(Gdk::SB_DOWN_ARROW);
      switch (this->position(this->ui->game().player_current())) {
      case SOUTH:
        return cursor_right;
      case NORTH:
        return cursor_left;
      case WEST:
        return cursor_down;
      case EAST:
        return cursor_up;
      default:
        return cursor_right;
      }
    }
    case CursorType::POVERTY_GET_CARD:
    case CursorType::POVERTY_PUT_CARD:
    case CursorType::POVERTY_FILL_UP: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND1);
      return cursor;
    }
    case CursorType::POVERTY_ACCEPT:
    case CursorType::POVERTY_GETTING_BACK: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND2);
      return cursor;
    }
    case CursorType::NEXT_ANNOUNCEMENT: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND2);
      return cursor;
    }
    case CursorType::PLAY_CARD: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND1);
      return cursor;
    }
    case CursorType::CARD_VALID: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND1);
      return cursor;
    }
    case CursorType::CARD_INVALID: {
      static auto const cursor = Gdk::Cursor::create(Gdk::PIRATE);
      return cursor;
    }
    case CursorType::CLOSE_FULL_TRICK: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND1);
      return cursor;
    }
    case CursorType::SHOW_LAST_TRICK: {
      static auto const cursor = Gdk::Cursor::create(Gdk::HAND2);
      return cursor;
    }
    case CursorType::GAME_REVIEW: {
      static auto const cursor = Gdk::Cursor::create(Gdk::SB_H_DOUBLE_ARROW);
      return cursor;
    }
    } // switch (type)

    return {};
  } // static Gdk::Cursor const& Table::cursor(CursorType type)

/**
 ** the progress has changed -- redraw the name of the active player
 **
 ** @param     progress   new progress
 **
 ** @return    -
 **/
void
  Table::progress_changed(double const progress)
  {
    if (::game_status & GAMESTATUS::GAME)
      this->name(this->ui->game().player_current()).draw();
    return ;
  } // void Table::progress_changed(double progress)

/**
 ** the progress has finished -- redraw the name of the active player
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  Table::progress_finished()
  {
    if (::game_status & GAMESTATUS::GAME)
      this->name(this->ui->game().player_current()).draw();
    return ;
  } // void Table::progress_finished()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
