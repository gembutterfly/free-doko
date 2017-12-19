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

#ifndef GTKMM_TABLE_HEADER
#define GTKMM_TABLE_HEADER

#include "base.h"

#include "../../basistypes.h"
#include "../../game/reservation.h"
#include "../../misc/setting.h"
class Player;
class Trick;
class GameplayAction;

#include <map>

#include <gtkmm/drawingarea.h>
#include <pangomm/layout.h>
#include <gdkmm/pixbuf.h>
#include <gdkmm/cursor.h>

namespace UI_GTKMM_NS {
class Reservation;
class Poverty;
class Trick;
class Hand;
class TrickPile;
class Icongroup;
class Name;
class GameFinished;
class PartyPoints;
class PartyFinished;

class GameInfoDialog;
class FullTrick;
class LastTrick;
class CardSuggestion;
class GenscherSelection;

/**
 ** @brief	the table
 **
 ** @author	Diether Knof
 **
 ** @todo	keep 'game_finished' window and 'poverty' over the games
 ** @todo	close full trick with click
 ** @todo	use 'Gdk::Point' and 'Gdk::Rectangle'
 **/
class Table : public Base, public Gtk::DrawingArea {
  public:
    enum class CursorType {
      NONE,
      DEFAULT,
      BUSY,
      POVERTY_SHIFT,
      POVERTY_ACCEPT,
      POVERTY_GET_CARD,
      POVERTY_PUT_CARD,
      POVERTY_FILL_UP,
      POVERTY_SHIFT_BACK,
      POVERTY_GETTING_BACK,
      NEXT_ANNOUNCEMENT,
      PLAY_CARD,
      CARD_VALID,
      CARD_INVALID,
      CLOSE_FULL_TRICK,
      SHOW_LAST_TRICK,
      GAME_REVIEW
    }; // enum CursorType

  public:
    class HTIN;

    friend class HTIN;
    friend class UI_GTKMM;
  public:
    // constructor
    Table(Base* parent);
    // destructor
    ~Table();

    Poverty& poverty();
    Trick& trick();
    Hand& hand(Player const& player);
    Hand const& hand(Player const& player) const;
    Hand& hand(Position position);
    Hand const& hand(Position position) const;
    TrickPile& trickpile(Player const& player);
    TrickPile const& trickpile(Player const& player) const;
    TrickPile& trickpile(Position position);
    TrickPile const& trickpile(Position position) const;
    Icongroup& icongroup(Player const& player);
    Icongroup const& icongroup(Player const& player) const;
    Icongroup& icongroup(Position position);
    Icongroup const& icongroup(Position position) const;
    Name& name(Player const& player);
    Name const& name(Player const& player) const;
    Name& name(Position position);
    Name const& name(Position position) const;
    Reservation& reservation(Player const& player);
    Reservation const& reservation(Player const& player) const;
    Reservation& reservation(Position position);
    Reservation const& reservation(Position position) const;

    bool get_card() const;

    Player const& player(Position position) const;
    Position position(Player const& player) const;
    Rotation rotation(Player const& player) const;
    Rotation rotation(Position position) const;

    Cairo::RefPtr<Cairo::Context> create_cairo_context();

    unsigned width() const;
    unsigned height() const;
    unsigned depth() const;

    unsigned border_x() const;
    unsigned border_y() const;

    void party_open();
    void party_start();
    void party_finish();
    void party_close();

    // a gameplay action
    void gameplay_action(GameplayAction const& action);

    void game_open();
    void game_cards_distributed();
    void game_redistribute();
    void reservation_ask(Player const& player);
    ::Reservation reservation_get(Player const& player);
    void game_start();
    void card_get(Player const& player);
    void card_got();
    void card_played(Player const& player);
    void trick_full();
    void trick_move_in_pile();
    void show_last_trick();
    void show_trick(::Trick const& trick);
    void show_card_suggestion(bool show_window = true);
    void game_finished();
    void game_close();

    bool in_game_review() const;
    unsigned game_review_trickno() const;
    ::Trick const& game_review_trick() const;
    bool game_review_trick_visible() const;

    void announcement_made(Announcement announcement,
                           Player const& player);
    void swines_announced(Player const& player);
    void hyperswines_announced(Player const& player);

    void marriage(Player const& bridegroom, Player const& bride);
    Player const* select_genscher_partner(Player const& genscher);
    void genscher(Player const& genscher, Player const& partner);

    void update_human_teaminfo();

    void clear(Cairo::RefPtr<::Cairo::Context> cr);
    void clear(Cairo::RefPtr<::Cairo::Context> cr,
               Gdk::Rectangle const& area);
#ifndef RELEASE
    void clear_debug(Cairo::RefPtr<::Cairo::Context> cr,
                     Gdk::Rectangle const& area);
#endif
    void draw_all();
    void draw_all_hands();
    void draw_logo(Cairo::RefPtr<::Cairo::Context> cr);
    void draw_party_summary();
    void cards_update();
    void cards_back_update();
    void icons_update();

    void load_background();

    void name_changed(Player const& player);

    void new_font(string const& fontname, Setting::TypeString type);
    void new_color(string const& colorname, Setting::TypeString type);

    void setting_update(int type);
    void mouse_cursor_update();

    void progress_changed(double progress);
    void progress_finished();

  private:
    bool on_draw(::Cairo::RefPtr<::Cairo::Context> const& cr);
    bool on_configure_event(GdkEventConfigure* event);
    bool on_button_press_event(GdkEventButton* event);
    bool on_scroll_event(GdkEventScroll* event);
    bool on_motion_notify_event(GdkEventMotion* event);

  private:

    Glib::RefPtr<Gdk::Cursor> cursor(CursorType type);

  public:
    CursorType cursor_type = CursorType::NONE;

    Cairo::RefPtr<Cairo::SurfacePattern> background;

  protected:
    std::map<Position, unique_ptr<Reservation>> reservation_;
    unique_ptr<Poverty> poverty_;
  public: // for ui
    unique_ptr<GameFinished> game_finished_;
    unique_ptr<PartyPoints> party_points_;
    unique_ptr<PartyFinished> party_finished_;
  protected:
    unique_ptr<FullTrick> full_trick_;
    unique_ptr<LastTrick> last_trick_;
  public:
    unique_ptr<CardSuggestion> card_suggestion_;
  protected:
    unique_ptr<GenscherSelection> genscher_selection_;

    unique_ptr<GameInfoDialog> gameinfo_;

    unique_ptr<Trick> trick_;
    std::map<Position, unique_ptr<Hand>> hand_;
    std::map<Position, unique_ptr<TrickPile>> trickpile_;
    std::map<Position, unique_ptr<Icongroup>> icongroup_;
    std::map<Position, unique_ptr<Name>> name_;

  private:

    bool get_card_ = false;

  private: // unused
    Table() = delete;
    Table(Table const&) = delete;
    Table& operator=(Table const&) = delete;
}; // class Table : public Base, protected Gtk::DrawingArea

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TABLE_HEADER

#endif // #ifdef USE_UI_GTKMM
