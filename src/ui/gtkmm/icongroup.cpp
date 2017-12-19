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

#include "icongroup.h"
#include "table.h"
#include "ui.h"

#include "cards.h"
#include "icons.h"
#include "hand.h"
#include "name.h"

#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../player/player.h"
#include "../../player/human/human.h"
#include "../../misc/setting.h"

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     table   the table
 ** @param     position   the position of the icongroup
 **
 ** @return    -
 **/
Icongroup::Icongroup(Table* const table, Position const position) :
  HTIN(table, position)
{ }

/**
 ** draws the icongroup
 **
 ** @param     update   whether to update the area
 **
 ** @return    -
 **/
void
Icongroup::draw(Cairo::RefPtr<::Cairo::Context> cr,
                bool const update)
{
  cr->save();
  cr->push_group();
  this->draw_announcement(cr, update);
  this->draw_swines(cr, update);
  this->draw_team(cr, update);
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void Icongroup::draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** draws the team/gametype icon
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icongroup::draw_team()
{
  this->draw_team(this->table().create_cairo_context(), true);
} // void Icongroup::draw_team()

/**
 ** draws the team/gametype icon
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area
 **
 ** @return    -
 **
 ** @todo	silent marriage
 **/
void
Icongroup::draw_team(Cairo::RefPtr<::Cairo::Context> cr,
                     bool const update)
{
  if(!(::game_status & GAMESTATUS::GAME))
    return ;

  Game const& game = this->game();

  if (   (::game_status < GAMESTATUS::GAME_RESERVATION)
      && !(   game.exists_soloplayer()
           && (this->player() == game.soloplayer()) )  )
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table().queue_resize();
    return ;
  }
#endif
#endif

  Player const* const soloplayer
    = (game.exists_soloplayer()
       ? &game.soloplayer()
       : ((game.type() == GAMETYPE::NORMAL)
          ? nullptr
          : &game.player_current()));

  // the values for the icon of maximal size
  Gdk::Rectangle max_outline;
  // the possible icons (from before, needed for overwriting old icons)
  vector<Glib::RefPtr<Gdk::Pixbuf>> possible_icons;

  // take the team of the game
  Team const team = game.teaminfo_for_humans(this->player());

  Glib::RefPtr<Gdk::Pixbuf> icon;

  if (::game_status == GAMESTATUS::GAME_RESERVATION) {
    for (int t = GAMETYPE::FIRST; t <= GAMETYPE::LAST; ++t)
      if (t != GAMETYPE::POVERTY)
        possible_icons.push_back(this->ui->icons->icon(static_cast<GameType>(t),
                                                       this->rotation()));
    possible_icons.push_back(this->ui->icons->icon(Icons::Type::POVERTY,
                                                   this->rotation()));

    if (this->player().type() == Player::HUMAN) {
      ::Reservation const& reservation = this->player().reservation();
      switch (reservation.game_type) {
      case GAMETYPE::NORMAL:
        icon = this->ui->icons->icon(this->player().hand().contains(Card::CLUB_QUEEN)
                                     ? TEAM::RE : TEAM::CONTRA,
                                     this->rotation());
        break;
      case GAMETYPE::POVERTY:
        {
          Icons::Type icon_type = Icons::Type::POVERTY;
          switch (this->player().hand().numberoftrumps()) {
          case 0:
            icon_type = Icons::Type::POVERTY_TRUMPS_0;
            break;
          case 1:
            icon_type = Icons::Type::POVERTY_TRUMPS_1;
            break;
          case 2:
            icon_type = Icons::Type::POVERTY_TRUMPS_2;
            break;
          case 3:
          case 4:
          case 5:
            icon_type = Icons::Type::POVERTY_TRUMPS_3;
            break;
          case UINT_MAX:
            icon_type = Icons::Type::POVERTY;
            break;
          default:
            DEBUG_ASSERTION(false,
                            "IconGroup::draw_team():\n"
                            "  gametype: poverty\n"
                            "  number of poverty cards for player " << this->player().game().no(this->player()) << ' ' << this->player().name() << " invalid: "
                            << this->player().hand().numberoftrumps() << '\n'
                            << "reservation:\n"
                            << this->player().reservation() << '\n'
                            << "game status: " << ::game_status);
            icon_type = Icons::Type::POVERTY;
            break;
          } // switch (this->player().hand().numberoftrumps())
          icon = this->ui->icons->icon(icon_type,
                                       this->rotation());
        }
        break;
      case GAMETYPE::GENSCHER:
        icon = this->ui->icons->icon(Icons::Type::GENSCHER,
                                     this->rotation());
        break;
      case GAMETYPE::MARRIAGE:
        icon = this->ui->icons->icon(reservation.marriage_selector,
                                     this->rotation());
        break;
      case GAMETYPE::MARRIAGE_SOLO:
      case GAMETYPE::MARRIAGE_SILENT:
      case GAMETYPE::THROWN_NINES:
      case GAMETYPE::THROWN_KINGS:
      case GAMETYPE::THROWN_NINES_AND_KINGS:
      case GAMETYPE::THROWN_RICHNESS:
      case GAMETYPE::FOX_HIGHEST_TRUMP:
      case GAMETYPE::SOLO_DIAMOND:
      case GAMETYPE::SOLO_JACK:
      case GAMETYPE::SOLO_QUEEN:
      case GAMETYPE::SOLO_KING:
      case GAMETYPE::SOLO_QUEEN_JACK:
      case GAMETYPE::SOLO_KING_JACK:
      case GAMETYPE::SOLO_KING_QUEEN:
      case GAMETYPE::SOLO_KOEHLER:
      case GAMETYPE::SOLO_CLUB:
      case GAMETYPE::SOLO_HEART:
      case GAMETYPE::SOLO_SPADE:
      case GAMETYPE::SOLO_MEATLESS:
        icon = this->ui->icons->icon(reservation.game_type,
                                     this->rotation());
        break;
      } // switch (this->player().reservation().game_type())

    } // if (this->player().type() == Player::HUMAN)

  } else if (   game.is_finished()
             || (   game.rule()(Rule::SHOW_SOLOPLAYER_IN_GAME)
                 && ::setting(Setting::SHOW_SOLOPLAYER_IN_GAME))) {
    switch (game.type()) {
    case GAMETYPE::NORMAL:
      break;
    case GAMETYPE::POVERTY:
      if (this->player() == *soloplayer) {
        icon = this->ui->icons->icon(game.type(), this->rotation());
        possible_icons.push_back(icon);
        possible_icons.push_back(this->ui->icons->icon(Icons::Type::POVERTY,
                                                       this->rotation()));
        possible_icons.push_back(icon);
      } else if (   (::game_status >= GAMESTATUS::GAME_START)
                 && (this->player().team() == TEAM::RE) ) {
        icon = this->ui->icons->icon(Icons::Type::POVERTY_PARTNER, this->rotation());
        possible_icons.push_back(icon);
      }
      break;
    case GAMETYPE::GENSCHER:
      if (this->player() == *soloplayer)
        icon = this->ui->icons->icon(Icons::Type::GENSCHER,
                                     this->rotation());
      else if (this->player().team() == TEAM::RE)
        icon = this->ui->icons->icon(TEAM::RE, this->rotation());
      else
        icon = this->ui->icons->icon(TEAM::CONTRA, this->rotation());
      break;
    case GAMETYPE::MARRIAGE:
    case GAMETYPE::MARRIAGE_SOLO:
      if (this->player() == *soloplayer) {
        icon = this->ui->icons->icon(game.marriage_selector(),
                                     this->rotation());
        for (int s = MARRIAGE_SELECTOR::FIRST;
             s <= MARRIAGE_SELECTOR::LAST;
             s++)
          possible_icons.push_back(this->ui->icons->icon(MarriageSelector(s),
                                                         this->rotation()));
      } else if (game.marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET) {
        if (team == TEAM::RE) {
          icon = this->ui->icons->icon(Icons::Type::MARRIAGE_PARTNER, this->rotation());
        } else {
          icon = this->ui->icons->icon(team, this->rotation());
        }
      } // if !(this->player == soloplayer)
      break;
    case GAMETYPE::MARRIAGE_SILENT:
      if (this->player() == *soloplayer) {
        icon = this->ui->icons->icon(game.type(),
                                     this->rotation());
      } else {
        icon.clear();
        possible_icons.push_back(this->ui->icons->icon(TEAM::CONTRA,
                                                       this->rotation()));
      }
    case GAMETYPE::THROWN_NINES:
    case GAMETYPE::THROWN_KINGS:
    case GAMETYPE::THROWN_NINES_AND_KINGS:
    case GAMETYPE::THROWN_RICHNESS:
    case GAMETYPE::FOX_HIGHEST_TRUMP:
    case GAMETYPE::SOLO_DIAMOND:
    case GAMETYPE::SOLO_JACK:
    case GAMETYPE::SOLO_QUEEN:
    case GAMETYPE::SOLO_KING:
    case GAMETYPE::SOLO_QUEEN_JACK:
    case GAMETYPE::SOLO_KING_JACK:
    case GAMETYPE::SOLO_KING_QUEEN:
    case GAMETYPE::SOLO_KOEHLER:
    case GAMETYPE::SOLO_CLUB:
    case GAMETYPE::SOLO_HEART:
    case GAMETYPE::SOLO_SPADE:
    case GAMETYPE::SOLO_MEATLESS:
      if (this->player() == *soloplayer)
        icon = this->ui->icons->icon(game.type(), this->rotation());
      else
        icon.clear();
      break;
    } // switch(game.type())
  } // if (icon shall be shown for soloplayer)

  // special case: silent marriage
  if (   !icon
      && (game.number_of_humans() == 1)
      && (this->player().type() == Player::HUMAN)
      && (   (   (game.type() == GAMETYPE::MARRIAGE_SILENT)
              && (this->player() == game.soloplayer()) )
          || (   (game.type() == GAMETYPE::NORMAL)
              && (this->player().hand().numberofall(Card::CLUB_QUEEN)
                  == game.rule()(Rule::NUMBER_OF_SAME_CARDS)))
         )
     ) {
    icon = this->ui->icons->icon(GAMETYPE::MARRIAGE_SILENT,
                                 this->rotation());
  }

  if (!icon) {
    if (   game.is_finished()
        || (   game.rule()(Rule::SHOW_KNOWN_TEAMS_IN_GAME)
            && ::setting(Setting::SHOW_KNOWN_TEAMS_IN_GAME))) {
      icon = this->ui->icons->icon(team,
                                   this->rotation());
      icon = this->ui->icons->icon(team, this->rotation());
    } // if (show teams)
  } // if (!icon)

  if (   !icon
      && ::setting(Setting::SHOW_AI_INFORMATION_TEAMS)
      && game.rule()(Rule::SHOW_KNOWN_TEAMS_IN_GAME)
      && (game.number_of_humans() == 1)
      && (game.human_player()->teaminfo(this->player()) != TEAM::UNKNOWN)
      && (game.type() == GAMETYPE::NORMAL)
     ) {
    icon = this->ui->icons->icon(::maybe_to_team(game.human_player()->teaminfo(this->player())), this->rotation());
    if (icon
        && !::is_real(game.human_player()->teaminfo(this->player()))) {
      Glib::RefPtr<Gdk::Pixbuf> copy = icon->copy();
      icon->saturate_and_pixelate(copy, 5, true);
      icon = copy;
    } // if (team is maybe)
  } // if (icon)

  if (icon)
    possible_icons.push_back(icon);
  possible_icons.push_back(this->ui->icons->icon(TEAM::RE,
                                                 this->rotation()));
  possible_icons.push_back(this->ui->icons->icon(TEAM::CONTRA,
                                                 this->rotation()));

  cr->save();
  cr->push_group();
  if (update) {
    max_outline.set_width(this->ui->icons->max_width(possible_icons));
    max_outline.set_height(this->ui->icons->max_height(possible_icons));
    max_outline.set_x(this->team_start_x(max_outline.get_width()));
    max_outline.set_y(this->team_start_y(max_outline.get_height()));

    this->table().clear(cr, max_outline);
  } // if (update)

  if (icon)
    this->draw(cr, icon,
               this->team_start_x(icon->get_width()),
               this->team_start_y(icon->get_height()));
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void Icongroup::draw_team(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** draws the announcement icon
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icongroup::draw_announcement()
{
  this->draw_announcement(this->table().create_cairo_context(), true);
} // void Icongroup::draw_announcement()

/**
 ** draws the announcement icon
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area
 **
 ** @return    -
 **/
void
Icongroup::draw_announcement(Cairo::RefPtr<::Cairo::Context> cr,
                             bool const update)
{
  if (! (   (::game_status & GAMESTATUS::GAME)
         && (::game_status > GAMESTATUS::GAME_RESERVATION)) )
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table().queue_resize();
    return ;
  }
#endif
#endif

  // the values for the icon of maximal size
  Gdk::Rectangle max_outline;

  auto announcement = this->player().announcement();

  if (   ::setting(Setting::ANNOUNCE_IN_TABLE)
      && (this->player().type() == Player::HUMAN)
      && (this->mouse_over_next_announcement()
          || (   (this->player().announcement()
                  == ANNOUNCEMENT::NOANNOUNCEMENT)
              && (this->player().next_announcement()
                  != ANNOUNCEMENT::NOANNOUNCEMENT)))) {
    announcement = this->player().next_announcement();
    if (!announcement)
      announcement = this->player().game().announcement_valid(ANNOUNCEMENT::REPLY, this->player());
  }

  if (announcement == ANNOUNCEMENT::REPLY)
    announcement = ANNOUNCEMENT::to_reply(this->game().announcement_of_team(::opposite(this->player().team())));

  if (this->table().in_game_review()) {
    announcement = ANNOUNCEMENT::NOANNOUNCEMENT;
    for (vector<Game::AnnouncementWithTrickno>::const_reverse_iterator
         a = this->game().announcements_of_player(this->player()).rbegin();
         a != this->game().announcements_of_player(this->player()).rend();
         ++a)
      if (a->trickno < this->table().game_review_trickno()) {
        announcement = a->announcement;
        break;
      }
  } // if (this->table().in_game_review())
  if (announcement == ANNOUNCEMENT::NOANNOUNCEMENT)
    return ;

  auto icon = this->ui->icons->icon(announcement, this->rotation());
  if (!this->table().in_game_review()
      && !(is_reply(announcement) && this->player().announcement())
      && (announcement != this->player().announcement())) {
    Glib::RefPtr<Gdk::Pixbuf> copy = icon->copy();
    icon->saturate_and_pixelate(copy, 4, true);
    icon = copy;
  } // if (!card valid)

  cr->save();
  cr->push_group();
  if (update) {
    vector<Glib::RefPtr<Gdk::Pixbuf>> possible_icons;
    for (unsigned a = ANNOUNCEMENT::NO120; a <= ANNOUNCEMENT::NO0; a++)
      possible_icons.push_back(this->ui->icons->icon(Announcement(a),
                                                     this->rotation()));
    max_outline.set_width(this->ui->icons->max_width(possible_icons));
    max_outline.set_height(this->ui->icons->max_height(possible_icons));
    max_outline.set_x(this->announcement_start_x(max_outline.get_width()));
    max_outline.set_y(this->announcement_start_y(max_outline.get_height()));

    this->table().clear(cr, max_outline);
    // ToDo: remove
    this->name().draw(cr, false);
  } else {
    this->table().clear(cr, Gdk::Rectangle(this->announcement_start_x(icon->get_width()), this->announcement_start_y(icon->get_height()), icon->get_width(), icon->get_height()));
  }
  this->draw(cr, icon,
             this->announcement_start_x(icon->get_width()),
             this->announcement_start_y(icon->get_height()));
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void Icongroup::draw_announcement(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** draws the swines/hyperswines icon
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icongroup::draw_swines()
{
  this->draw_swines(this->table().create_cairo_context(), true);
} // void Icongroup::draw_swines()

/**
 ** draws the swines/hyperswines icon
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area
 **
 ** @return    -
 **/
void
Icongroup::draw_swines(Cairo::RefPtr<::Cairo::Context> cr,
                       bool const update)
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table().queue_resize();
    return ;
  }
#endif
#endif

  // no clearing necessary, since the icon can only get bigger

  Glib::RefPtr<Gdk::Pixbuf> icon;

  Gdk::Rectangle max_outline;
  max_outline.set_x(this->swines_start_x(this->ui->icons->max_width(this->rotation())));
  max_outline.set_y(this->swines_start_y(this->ui->icons->max_height(this->rotation())));
  max_outline.set_width(this->ui->icons->max_width(this->rotation()));
  max_outline.set_height(this->ui->icons->max_height(this->rotation()));

  if (   (::game_status == GAMESTATUS::GAME_RESERVATION)
      && (this->player() == this->game().player_current())) {
    if (   this->player().reservation().swines
        && this->game().swines_announcement_valid(this->player())
        && this->player().reservation().hyperswines
        && this->game().hyperswines_announcement_valid(this->player()))
      icon = this->ui->icons->swines_hyperswines(this->game().trumpcolor(),
                                                 this->rotation());
    else if (   this->player().reservation().swines
             && this->game().swines_announcement_valid(this->player()))
      icon = this->ui->icons->swines(this->game().trumpcolor(),
                                     this->rotation());
    else if (   this->player().reservation().hyperswines
             && this->game().hyperswines_announcement_valid(this->player()))
      icon = this->ui->icons->hyperswines(this->game().trumpcolor(),
                                          this->rotation());
    else if (update) {
      // the values for the icon of maximal size
      this->table().clear(cr, max_outline);
    }
  } else { // if !(reservation && current player)
    // setting the pixmap, if the player has swines/hyperswines
    if (   this->player().has_swines()
        && this->player().has_hyperswines()) {
      icon = this->ui->icons->swines_hyperswines(this->game().trumpcolor(),
                                                 this->rotation());
    } else if (this->player().has_swines()) {
      icon = this->ui->icons->swines(this->game().trumpcolor(),
                                     this->rotation());
    } else if (this->player().has_hyperswines()) {
      icon = this->ui->icons->hyperswines(this->game().trumpcolor(),
                                          this->rotation());
    } // if !(player has swines/hyperswines)
  } // if !(reservation && current player)

  if (!icon)
    return ;

  this->table().clear(cr, Gdk::Rectangle(this->swines_start_x(icon->get_width()), this->swines_start_y(icon->get_height()), icon->get_width(), icon->get_height()));

  cr->save();
  cr->push_group();
  this->draw(cr, icon,
             this->swines_start_x(icon->get_width()),
             this->swines_start_y(icon->get_height()));

  if (update) {
    // ToDo: remove
    this->draw(cr, false);
    this->name().draw(cr, false);
  } // if (update)
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void Icongroup::draw_swines(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** -> result
 **
 ** @param     width   the width of the icon
 **
 ** @return    upper left x position of the team/gametype icon
 **/
int
Icongroup::team_start_x(int const width) const
{
  switch(this->position()) {
  case NORTH:
  case SOUTH:
    return (this->hand().start_x()
            + std::min(this->hand().width() / 3,
                       2 * this->ui->icons->max_width()));
  case WEST:
    return (this->hand().start_x()
            + this->hand().width()
            + this->hand().margin_x());
  case EAST:
    return (this->hand().start_x()
            - this->hand().margin_x()
            - width);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::team_start_x(width)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::team_start_x(int width)

/**
 ** -> result
 **
 ** @param     height   the height of the icon
 **
 ** @return    upper left y position of the team/gametype icon
 **/
int
Icongroup::team_start_y(int const height) const
{
  switch(this->position()) {
  case NORTH:
    return (this->hand().start_y()
            + this->hand().height()
            + this->hand().margin_y());
  case SOUTH:
    return (this->hand().start_y()
            - this->hand().margin_y()
            - height);
  case WEST:
  case EAST:
    return this->hand().start_y();
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::team_start_y(height)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::team_start_y(int height)

/**
 ** -> result
 **
 ** @param     width   the width of the icon
 **
 ** @return    upper left x position of the announcement icon
 **/
int
Icongroup::announcement_start_x(int const width) const
{
  switch(this->position()) {
  case NORTH:
  case SOUTH:
    return (this->hand().start_x()
            + std::max(this->hand().width() * 2 / 3,
                       this->hand().width()
                       - this->ui->icons->max_width() * 3 / 2)
            - width / 2);
  case WEST:
    return (this->hand().start_x()
            + this->hand().width()
            + this->hand().margin_x());
  case EAST:
    return (this->hand().start_x()
            - this->hand().margin_x()
            - width);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::announcement_start_x(width)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::announcement_start_x(int width)

/**
 ** -> result
 **
 ** @param     height   the height of the icon
 **
 ** @return    upper left y position of the announcement icon
 **/
int
Icongroup::announcement_start_y(int const height) const
{
  switch(this->position()) {
  case NORTH:
    return (this->hand().start_y()
            + this->hand().height()
            + this->hand().margin_y());
  case SOUTH:
    return (this->hand().start_y()
            - this->hand().margin_y()
            - height);
  case WEST:
  case EAST:
    return (this->hand().start_y()
            + this->hand().height() / 2
            - height / 2);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::announcement_start_y(height)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::announcement_start_y(int height)

/**
 ** -> result
 **
 ** @param     width   the width of the icon
 **
 ** @return    upper left x position of the team icon
 **/
int
Icongroup::swines_start_x(int const width) const
{
  switch(this->position()) {
  case NORTH:
  case SOUTH:
    return (this->hand().start_x()
            + this->hand().width()
            - width);
  case WEST:
    return (this->hand().start_x()
            + this->hand().width()
            + this->hand().margin_x());
  case EAST:
    return (this->hand().start_x()
            - this->hand().margin_x()
            - width);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::swines_start_x(width)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::swines_start_x(int width)

/**
 ** -> result
 **
 ** @param     height   the height of the icon
 **
 ** @return    upper left y position of the swines icon
 **/
int
Icongroup::swines_start_y(int const height) const
{
  switch(this->position()) {
  case NORTH:
    return (this->hand().start_y()
            + this->hand().height()
            + this->hand().margin_y());
  case SOUTH:
    return (this->hand().start_y()
            - this->hand().margin_y()
            - height);
  case WEST:
  case EAST:
    return (this->hand().start_y()
            + this->hand().height()
            - height);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::swines_start_y(height)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int Icongroup::swines_start_y(int height)

/**
 ** -> result
 **
 ** @param     position   Position of the icongroup
 **
 ** @return    width of the icongroup at 'position'
 **/
int
Icongroup::width(Position const position) const
{
  switch(position) {
  case WEST:
  case EAST:
    return this->ui->icons->max_height();
  case CENTER:
  case NORTH:
  case SOUTH:
    DEBUG_ASSERTION(false,
                    "Icongroup::width(" << position << "):\n"
                    "  'position' must be 'WEST' or 'EAST'");
    break;
  } // switch(position)

  return 0;
} // int Icongroup::width(Position position)

/**
 ** -> result
 **
 ** @param     position   Position of the icongroup
 **
 ** @return    height of the icongroup at 'position'
 **/
int
Icongroup::height(Position const position) const
{
  switch(position) {
  case NORTH:
  case SOUTH:
    return this->ui->icons->max_height();
  case WEST:
  case EAST:
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::height(" << position << ")\n"
                    "  'position' must be 'NORTH' or 'SOUTH'");
    break;
  } // switch(position)

  return 0;
} // int Icongroup::height(Position position)

/**
 ** -> result
 **
 ** @param     position   Position of the icongroup
 **
 ** @return    width of the icongroup at 'position'
 **/
int
Icongroup::margin_x(Position const position) const
{
  switch(position) {
  case NORTH:
  case SOUTH:
    DEBUG_ASSERTION(false,
                    "Icongroup::margin_x(position):\n"
                    "  'position' is 'NORTH' or 'SOUTH'");
    break;
  case WEST:
  case EAST:
    return (this->ui->cards->height() / 10);
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::margin_x(position)\n"
                    "  wrong position 'CENTER' "
                    << "(" << position << ")");
    break;
  } // switch(position)

  return 0;
} // int Icongroup::margin_x(Position position)

/**
 ** -> result
 **
 ** @param     position   Position of the icongroup
 **
 ** @return    margin_y of the icongroup at 'position'
 **/
int
Icongroup::margin_y(Position const position) const
{
  switch(position) {
  case NORTH:
  case SOUTH:
    return (this->ui->cards->height() / 10);
  case WEST:
  case EAST:
    DEBUG_ASSERTION(false,
                    "Icongroup::margin_y(position):\n"
                    "  'position' is 'WEST' or 'EAST'");
    break;
  case CENTER:
    DEBUG_ASSERTION(false,
                    "Icongroup::margin_y(position)\n"
                    "  wrong position 'CENTER' "
                    << "(" << position << ")");
    break;
  } // switch(position)

  return 0;
} // int Icongroup::margin_y(Position position)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    width of the icongroup
 **/
int
Icongroup::width() const
{
  return this->width(this->position());
} // int Icongroup::width()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the icongroup
 **/
int
Icongroup::height() const
{
  return this->height(this->position());
} // int Icongroup::height()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    margin_x of the icongroup
 **/
int
Icongroup::margin_x() const
{
  return this->margin_x(this->position());
} // int Icongroup::margin_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    margin_y of the icongroup
 **/
int
Icongroup::margin_y() const
{
  return this->margin_y(this->position());
} // int Icongroup::margin_y()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the mouse is over the next announcement
 **/
bool
Icongroup::mouse_over_next_announcement() const
{
  if (!::setting(Setting::ANNOUNCE_IN_TABLE))
    return false;

  if (!::is_real(this->player().team()))
    return false;

  Announcement const announcement
    = this->player().next_announcement();

  if (announcement == ANNOUNCEMENT::NOANNOUNCEMENT)
    return false;

  int x, y;
  this->table().get_pointer(x, y);

  Glib::RefPtr<Gdk::Pixbuf> icon
    = this->ui->icons->icon(announcement,
                            this->rotation());
  return (   (x >= this->announcement_start_x(icon->get_width()))
          && (x < (this->announcement_start_x(icon->get_width())
                   + icon->get_width()))
          && (y >= this->announcement_start_y(icon->get_height()))
          && (y < (this->announcement_start_y(icon->get_height())
                   + icon->get_height())));
} // bool Icongroup::mouse_over_next_announcement() const
} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
