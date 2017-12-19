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

#include "icons.h"

#include "ui.h"
#include "cards.h"
#include "utils.h"
#include "splash_screen.h" // ToDo: remove it

#include "../status_message.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../misc/setting.h"
#include "../../misc/translations.h"

#include <gtkmm/image.h>

namespace UI_GTKMM_NS {
size_t const Icons::NUMBER_OF_TYPES = static_cast<size_t>(Type::LAST) + 1;

/**
 ** -> result
 **
 ** @param     gametype   the gametype
 **
 ** @return    icon type for 'gametype'
 **/
Icons::Type
  Icons::type(GameType const gametype)
  {
    switch(gametype) {
    case GAMETYPE::NORMAL:
      return Type::RE;
    case GAMETYPE::THROWN_NINES:
      return Type::THROWN_NINES;
    case GAMETYPE::THROWN_KINGS:
      return Type::THROWN_KINGS;
    case GAMETYPE::THROWN_NINES_AND_KINGS:
      return Type::THROWN_NINES_AND_KINGS;
    case GAMETYPE::THROWN_RICHNESS:
      return Type::THROWN_RICHNESS;
    case GAMETYPE::POVERTY:
      if (::game_status == GAMESTATUS::GAME_RESERVATION) {
        auto const& player = ::party.game().player_current();
        if (player.type() == Player::HUMAN) {
          switch (player.hand().numberoftrumps()) {
          case 0:
            return Type::POVERTY_TRUMPS_0;
          case 1:
            return Type::POVERTY_TRUMPS_1;
          case 2:
            return Type::POVERTY_TRUMPS_2;
          case 3:
          case 4:
          case 5:
            return Type::POVERTY_TRUMPS_3;
          case UINT_MAX:
            return Type::POVERTY;
          default:
            DEBUG_ASSERTION(false,
                            "Icons::icon(gametype):\n"
                            "  gametype: poverty\n"
                            "  number of poverty cards for player " << player.game().no(player) << ' ' << player.name() << " invalid: "
                            << player.hand().numberoftrumps());
            return Type::POVERTY;
          } // switch (::party.game().poverty_trumpno_returned())
        } else {
          return Type::POVERTY;
        }
      }
      if (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT) {
        return Type::POVERTY;
      }

      switch (::party.game().poverty_trumpno_returned()) {
      case 0:
        return Type::POVERTY_TRUMPS_0;
      case 1:
        return Type::POVERTY_TRUMPS_1;
      case 2:
        return Type::POVERTY_TRUMPS_2;
      case 3:
      case 4:
      case 5:
        return Type::POVERTY_TRUMPS_3;
      case UINT_MAX:
        return Type::POVERTY;
      default:
        DEBUG_ASSERTION(false,
                        "Icons::icon(gametype):\n"
                        "  gametype: poverty\n"
                        "  number of poverty cards invalid: "
                        << ::party.game().poverty_trumpno_returned());
        return Type::POVERTY;
      } // switch (::party.game().poverty_trumpno_returned())
    case GAMETYPE::FOX_HIGHEST_TRUMP:
      return Type::FOX_HIGHEST_TRUMP;
    case GAMETYPE::GENSCHER:
      return Type::GENSCHER;
    case GAMETYPE::MARRIAGE:
      return Type::MARRIAGE;
    case GAMETYPE::MARRIAGE_SOLO:
    case GAMETYPE::MARRIAGE_SILENT:
      return Type::MARRIAGE_SOLO;
    case GAMETYPE::SOLO_DIAMOND:
      return Type::SOLO_DIAMOND;
    case GAMETYPE::SOLO_JACK:
      return Type::SOLO_JACK;
    case GAMETYPE::SOLO_QUEEN:
      return Type::SOLO_QUEEN;
    case GAMETYPE::SOLO_KING:
      return Type::SOLO_KING;
    case GAMETYPE::SOLO_QUEEN_JACK:
      return Type::SOLO_QUEEN_JACK;
    case GAMETYPE::SOLO_KING_JACK:
      return Type::SOLO_KING_JACK;
    case GAMETYPE::SOLO_KING_QUEEN:
      return Type::SOLO_KING_QUEEN;
    case GAMETYPE::SOLO_KOEHLER:
      return Type::SOLO_KOEHLER;
    case GAMETYPE::SOLO_CLUB:
      return Type::SOLO_CLUB;
    case GAMETYPE::SOLO_HEART:
      return Type::SOLO_HEART;
    case GAMETYPE::SOLO_SPADE:
      return Type::SOLO_SPADE;
    case GAMETYPE::SOLO_MEATLESS:
      return Type::SOLO_MEATLESS;
    default:
      DEBUG_ASSERTION(false,
                      "Icons::type(gametype):\n"
                      "  unknown gametype '" << gametype << "'");
      return Type::NONE;
    } // switch(gametype)
  } // static Icons::Type Icons::type(GameType gametype)

/**
 ** -> result
 **
 ** @param     marriage_selector   the marriage selector
 **
 ** @return    icon type for 'marriage_selector'
 **/
Icons::Type
  Icons::type(MarriageSelector const marriage_selector)
  {
    switch(marriage_selector) {
    case MARRIAGE_SELECTOR::SILENT:
      return Type::MARRIAGE_SOLO;
    case MARRIAGE_SELECTOR::TEAM_SET:
      return Type::MARRIAGE;
    case MARRIAGE_SELECTOR::FIRST_FOREIGN:
      return Type::MARRIAGE_FOREIGN;
    case MARRIAGE_SELECTOR::FIRST_TRUMP:
      return Type::MARRIAGE_TRUMP;
    case MARRIAGE_SELECTOR::FIRST_COLOR:
      return Type::MARRIAGE_COLOR;
    case MARRIAGE_SELECTOR::FIRST_CLUB:
      return Type::MARRIAGE_CLUB;
    case MARRIAGE_SELECTOR::FIRST_SPADE:
      return Type::MARRIAGE_SPADE;
    case MARRIAGE_SELECTOR::FIRST_HEART:
      return Type::MARRIAGE_HEART;
    } // switch (marriage_selector)
    DEBUG_ASSERTION(false,
                    "Icons::type(marriage_selector):\n"
                    "  unknown marriage selecter '" << marriage_selector << "'");
    return Type::NONE;
  } // static Icons::Type Icons::type(MarriageSelector marriage_selector)

/**
 ** -> result
 **
 ** @param     team   	the team
 **
 ** @return    icon type for 'team'
 **/
Icons::Type
  Icons::type(Team const team)
  {
    switch(team) {
    case TEAM::RE:
      return Type::RE;
    case TEAM::CONTRA:
      return Type::CONTRA;
    default:
      return Type::NONE;
    } // switch(team)
  } // static Icons::Type Icons::type(Team team)

/**
 ** -> result
 **
 ** @param     announcement   the announcement
 **
 ** @return    icon type for 'announcement'
 **/
Icons::Type
  Icons::type(Announcement const announcement)
  {
    switch(announcement) {
    case ANNOUNCEMENT::NOANNOUNCEMENT:
      DEBUG_ASSERTION(false,
                      "Icon::icon(announcement, rotation):\n"
                      "  invalid announcement 'NOANNOUNCEMENT'");
      break;
    case ANNOUNCEMENT::NO120:
      return Type::NO_120;
    case ANNOUNCEMENT::NO90:
      return Type::NO_90;
    case ANNOUNCEMENT::NO60:
      return Type::NO_60;
    case ANNOUNCEMENT::NO30:
      return Type::NO_30;
    case ANNOUNCEMENT::NO0:
      return Type::NO_0;
    case ANNOUNCEMENT::REPLY:
#ifdef POSTPONED
      DEBUG_ASSERTION(false,
                      "Icon::icon(announcement, rotation):\n"
                      "  invalid announcement 'REPLY'");
      return Type::NONE;
#else
      return Type::NO_120;
#endif
    case ANNOUNCEMENT::NO120_REPLY:
      return Type::NO_120_REPLY;
    case ANNOUNCEMENT::NO90_REPLY:
      return Type::NO_90_REPLY;
    case ANNOUNCEMENT::NO60_REPLY:
      return Type::NO_60_REPLY;
    case ANNOUNCEMENT::NO30_REPLY:
      return Type::NO_30_REPLY;
    case ANNOUNCEMENT::NO0_REPLY:
      return Type::NO_0_REPLY;
    default:
      DEBUG_ASSERTION(false,
                      "Icons::type(announcement):\n"
                      "  unknown announcement '" << announcement << "'");
      return Type::NONE;
    } // switch(announcement)
    return Type::NONE;
  } // static Icons::Type Icons::type(Announcement announcement)

/**
 ** -> result
 **
 ** @param     color   	the color
 **
 ** @return    swines icon type for 'color'
 **/
Icons::Type
  Icons::type_swines(Card::Color const color)
  {
    switch(color) {
    case Card::CLUB:
      return Type::SWINES_CLUB;
    case Card::SPADE:
      return Type::SWINES_SPADE;
    case Card::HEART:
      return Type::SWINES_HEART;
    case Card::DIAMOND:
      return Type::SWINES_DIAMOND;
    default:
      DEBUG_ASSERTION(false,
                      "Icon::swines(color)\n"
                      "  Color is not valid\n");
      return Type::NONE;
    } // switch(color)
  } // static Icons::Type Icons::type_swines(Card::Color color)

/**
 ** -> result
 **
 ** @param     color   	the color
 **
 ** @return    hyperswines icon type for 'color'
 **/
Icons::Type
  Icons::type_hyperswines(Card::Color const color)
  {
    switch(color) {
    case Card::CLUB:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::HYPERSWINES_CLUB
              : Type::HYPERSWINES_KING_CLUB);
    case Card::SPADE:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::HYPERSWINES_SPADE
              : Type::HYPERSWINES_KING_SPADE);
    case Card::HEART:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::HYPERSWINES_HEART
              : Type::HYPERSWINES_KING_HEART);
    case Card::DIAMOND:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::HYPERSWINES_DIAMOND
              : Type::HYPERSWINES_KING_DIAMOND);
    default:
      DEBUG_ASSERTION(false,
                      "Icon::hyperswines(color)\n"
                      "  Color is not valid\n");
      return Type::NONE;
    } // switch(color)
  } // static Icons::Type Icons::type_hyperswines(Card::Color color)

/**
 ** -> result
 **
 ** @param     color   	the color
 **
 ** @return    swines-hyperswines icon type for 'color'
 **/
Icons::Type
  Icons::type_swines_hyperswines(Card::Color const color)
  {
    switch(color) {
    case Card::CLUB:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::SWINES_HYPERSWINES_CLUB
              : Type::SWINES_HYPERSWINES_KING_CLUB);
    case Card::SPADE:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::SWINES_HYPERSWINES_SPADE
              : Type::SWINES_HYPERSWINES_KING_SPADE);
    case Card::HEART:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::SWINES_HYPERSWINES_HEART
              : Type::SWINES_HYPERSWINES_KING_HEART);
    case Card::DIAMOND:
      return (this->ui->party().rule()(Rule::WITH_NINES)
              ? Type::SWINES_HYPERSWINES_DIAMOND
              : Type::SWINES_HYPERSWINES_KING_DIAMOND);
    default:
      DEBUG_ASSERTION(false,
                      "Icon::swines_hyperswines(color)\n"
                      "  Color is not valid\n");
      return Type::NONE;
    } // switch(color)
  } // static Icons::Type Icons::type_swines_hyperswines(Card::Color const color)

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **
 ** @return    -
 **/
Icons::Icons(Base* const parent) :
  Base(parent)
{
  this->load();
} // Icons::Icons(Base* parent)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the scaling
 **/
double
Icons::scaling() const
{
  return this->ui->cards->scaling();
} // double Icons::scaling() const

/**
 ** update the scaling
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icons::update_scaling()
{
  // the scaling value is already updated in 'Cards'

  // set the scaling of the 'ScaledPixbufRotations'
  for (auto& i : this->icon_)
    i.set_scaling(this->scaling());

  return ;
} // void Icons::update_scaling()

/**
 ** update the maximal width and height
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icons::update_max_values()
{
  this->max_width_ = 0;
  for (auto const& i : this->icon_)
    if (i.get_width() > this->max_width_)
      this->max_width_ = i.get_width();

  this->max_height_ = 0;
  for (auto const& i : this->icon_)
    if (i.get_height() > this->max_height_)
      this->max_height_ = i.get_height();

  return ;
} // void Icons::update_max_values()

/**
 ** -> result
 **
 ** @param     rotation   the rotation
 **
 ** @return    maximal icon width
 **/
int
Icons::max_width(Rotation const rotation) const
{
  if (   (rotation == Rotation::LEFT)
      || (rotation == Rotation::RIGHT))
    return this->max_height(clockwise(rotation));

  return this->max_width_;
} // int Icons::max_width(Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     rotation   the rotation
 **
 ** @return    maximal icon height
 **/
int
Icons::max_height(Rotation const rotation) const
{
  if (   (rotation == Rotation::LEFT)
      || (rotation == Rotation::RIGHT))
    return this->max_width(clockwise(rotation));

  return this->max_height_;
} // int Icons::max_height(Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     icons      the icons to look at
 ** @param     rotation   the rotation
 **
 ** @return    maximal icon width of the icons
 **/
int
Icons::max_width(vector<Glib::RefPtr<Gdk::Pixbuf>> const& icons,
                 Rotation const rotation) const
{
  if (   (rotation == Rotation::LEFT)
      || (rotation == Rotation::RIGHT))
    return this->max_height(icons, clockwise(rotation));

  int width = 0;

  for (auto const& i : icons)
    if (i->get_width() > width)
      width = i->get_width();

  return width;
} // int Icons::max_width(vector<Glib::RefPtr<Gdk::Pixbuf>> icons, Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     icons      the icons to look at
 ** @param     rotation   the rotation
 **
 ** @return    maximal icon height of the icons
 **/
int
Icons::max_height(vector<Glib::RefPtr<Gdk::Pixbuf>> const& icons,
                  Rotation const rotation) const
{
  if (   (rotation == Rotation::LEFT)
      || (rotation == Rotation::RIGHT))
    return this->max_width(icons, clockwise(rotation));

  int height = 0;

  for (auto const& i : icons)
    if (i->get_height() > height)
      height = i->get_height();

  return height;
} // int Icons::max_height(vector<Glib::RefPtr<Gdk::Pixbuf>> icons, Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     type   	the type
 ** @param     rotation   the rotation
 **
 ** @return    the pixbuf of 'icon'
 **		(with rotation 'rotation')
 **/
Glib::RefPtr<Gdk::Pixbuf> const&
Icons::icon(Type const type, Rotation const rotation)
{
  return this->icon_[static_cast<size_t>(type)][rotation];
} // Glib::RefPtr<Gdk::Pixbuf> Icons::icon(Type type, Rotation rotation = UP)

/**
 ** -> result
 **
 ** @param     type   icon type
 **			(default: NONE)
 ** @param     scaling   additional scaling
 **			(default: 1)
 **
 ** @return    new image with the icon 'type' that is managed
 **		(that is size and theme)
 **/
Gtk::Image*
Icons::new_managed_image(Type const type, double const scaling)
{
  auto icon = this->icon(type);
  auto image = Gtk::manage(new Gtk::Image(icon));
  image->set_data("type", new Type(type));
  image->set_data("scaling", new double(scaling));
  if (scaling != 1)
    image->set(icon->scale_simple(static_cast<int>(icon->get_width() * scaling),
                                  static_cast<int>(icon->get_height() * scaling),
                                  Gdk::INTERP_TILES));

  image->signal_unrealize().connect(sigc::bind<Gtk::Image const* const>(sigc::mem_fun(*this, &Icons::remove_managed), image));

  this->managed_image_.push_back(image);

  return image;
} // Gtk::Image* Icons::new_managed_image(Type type = NONE, double scaling = 1)

/**
 ** changes the type of 'image'
 **
 ** @param     image   image to change the type of
 ** @param     type   new type
 **
 ** @return    -
 **/
void
Icons::change_managed(Gtk::Image* const image, Type const type)
{
  DEBUG_ASSERTION(image->get_data("type"),
                  "Icons::change_managed:\n"
                  "  image has no data 'type'");
  // ToDo: Test whether the image is in the vector

  delete static_cast<Type*>(image->steal_data("type"));
  image->set_data("type", new Type(type));

  if (type == Type::NONE)
    image->clear();
  else
    image->set(this->icon(type));

  return ;
} // void Icons::change_managed(Gtk::Image* image, Type type)

/**
 ** changes the scaling of 'image'
 **
 ** @param     image   image to change the type of
 ** @param     scaling   new scaling
 **
 ** @return    -
 **/
void
Icons::change_scaling(Gtk::Image* const image, double const scaling)
{
  DEBUG_ASSERTION(image->get_data("scaling"),
                  "Icons::change_scaling:\n"
                  "  image has no data 'scaling'");
  // ToDo: Test whether the image is in the vector

  delete static_cast<double*>(image->steal_data("scaling"));
  image->set_data("scaling", new double(scaling));

  auto const type = *(static_cast<Type*>(image->get_data("type")));
  if (type == Type::NONE)
    return ;

  if (scaling == 1)
    image->set(this->icon(type));
  else {
    auto icon = this->icon(type);
    image->set(icon->scale_simple(static_cast<int>(icon->get_width() * scaling),
                                  static_cast<int>(icon->get_height() * scaling),
                                  Gdk::INTERP_TILES));
  }

  return ;
} // void Icons::change_scaling(Gtk::Image* image, double scaling)

/**
 ** remove 'image' from the managed image list
 **
 ** @param     image   image to remove
 **
 ** @return    -
 **/
void
Icons::remove_managed(Gtk::Image const* const image)
{
  for (auto i = this->managed_image_.begin();
       i != this->managed_image_.end();
      )
    if (*i == image) {
      delete static_cast<Type*>((*i)->steal_data("type"));
      delete static_cast<double*>((*i)->steal_data("scaling"));
      i = this->managed_image_.erase(i);
      break;
    } else {
      ++i;
    }

  return ;
} // void Icons::remove_managed(Gtk::Image const* image)

/**
 ** update the managed images
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icons::update_managed()
{
  for (auto& i : this->managed_image_) {
    auto const type = *(static_cast<Type*>(i->get_data("type")));
    if (type == Type::NONE)
      continue;

    auto const scaling = *(static_cast<double*>(i->get_data("scaling")));
    if ( (scaling == 1)
        || (scaling == 0) )
      i->set(this->icon(type));
    else {
      auto icon = this->icon(type);
      i->set(icon->scale_simple(static_cast<int>(icon->get_width() * scaling),
                                static_cast<int>(icon->get_height() * scaling),
                                Gdk::INTERP_TILES));
    }
  }

  return ;
} // void Icons::update_managed()

/**
 ** load the icons
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Icons::load()
{
  if (Glib::file_test(::setting.path(Setting::CARDSET) + "/"
                      + ::setting(Setting::ICONS_DIRECTORY) + "/"
                      + "no_120"
                      + "." + ::setting(Setting::GRAPHIC_EXTENSION),
                      Glib::FILE_TEST_IS_REGULAR)) {

    if (this->icon_.empty()) {
      this->ui->error("Error at loading the icons from the cardset "
                      + ::setting(Setting::CARDSET) + ".\n"
                      + "Perhaps you should validate your 'FreeDokorc'-File "
                      + "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                      + "Exiting.");
    } else { // if !(this->icon_.empty())
      this->ui->information(::translation("Error at loading the icons from the cardset "
                                          + ::setting(Setting::CARDSET) + ".\n"
                                          "Keeping the old icons."),
                            INFORMATION::WARNING);
    } // if !(this->icon_.empty())

    return ;
  } // if (directory is valid)

  StatusMessages sm(*this->ui);
  if (   (::game_status != GAMESTATUS::PROGRAMSTART)
      && (::game_status != GAMESTATUS::PARTY_INITIAL_LOADED) ) {
    sm.add(::translation("loading the icons"));
  } // if (::game_status != GAMESTATUS::PROGRAMSTART)

  vector<Gdk::ScaledPixbufRotations> icon_new;
  for (unsigned type = 0; type < NUMBER_OF_TYPES; type++) {
    sm.add(::translation("~loading the icons (%ui%/%un%)", type+1, NUMBER_OF_TYPES));
    icon_new.push_back(this->load(static_cast<Type>(type)));
    if (   (::game_status == GAMESTATUS::PROGRAMSTART)
        || (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED) )
      this->ui->add_progress(1.0 / NUMBER_OF_TYPES);
  } // for (n \in names)

  this->icon_ = icon_new;

  this->update_scaling();
  this->update_managed();
  this->update_max_values();

  return ;
} // void Icons::load()

/**
 ** load the icon from the file
 **
 ** @param     name_a   name of the file
 ** @param     name_b   alternate name of the file
 **
 ** @return    loaded or empty icon
 **/
Gdk::ScaledPixbufRotations
Icons::load_from_file(string const& name_a, string const& name_b)
{
  auto icon = Gdk::ScaledPixbufRotations(::setting.path(Setting::ICONSET) + "/"
                                         + name_a + "."
                                         + ::setting(Setting::GRAPHIC_EXTENSION));
  if (icon)
    return icon;
  if (!name_b.empty())
    icon = Gdk::ScaledPixbufRotations(::setting.path(Setting::ICONSET) + "/"
                                      + name_b + "."
                                      + ::setting(Setting::GRAPHIC_EXTENSION));
  if (icon)
    return icon;

  // fallback in the cardset directory
  icon = Gdk::ScaledPixbufRotations(::setting.path(Setting::CARDSET) + "/"
                                    + ::setting.value(Setting::ICONS_FROM_CARDSET_DIRECTORY) + "/"
                                    + name_a + "."
                                    + ::setting(Setting::GRAPHIC_EXTENSION));
  if (icon)
    return icon;
  if (!name_b.empty())
    icon = Gdk::ScaledPixbufRotations(::setting.path(Setting::CARDSET) + "/"
                                      + ::setting.value(Setting::ICONS_FROM_CARDSET_DIRECTORY) + "/"
                                      + name_b + "."
                                      + ::setting(Setting::GRAPHIC_EXTENSION));
  return icon;
} // Gdk::ScaledPixbufRotations Icons::load_from_file(string name_a, string name_b = "")

/**
 ** load the icon from the file or construct it
 **
 ** @param     name     name of the file
 ** @param     cards    cards to construct from
 **
 ** @return    loaded or empty icon
 **/
Gdk::ScaledPixbufRotations
Icons::load_from_file_or_construct(string const& name,
                                   vector<Card> const& cards)
{
  auto icon = this->load_from_file(name);
  if (icon)
    return icon;
  return this->construct(cards);
} // Gdk::ScaledPixbufRotations Icons::load_from_file_or_construct(string name, vector<Card> cards)

/**
 ** load the icon from the file or construct it
 **
 ** @param     name_a   name of the file
 ** @param     name_b   alternative name
 ** @param     cards    cards to construct from
 **
 ** @return    loaded or empty icon
 **/
Gdk::ScaledPixbufRotations
Icons::load_from_file_or_construct(string const& name_a,
                                   string const& name_b,
                                   vector<Card> const& cards)
{
  auto icon = this->load_from_file(name_a, name_b);
  if (icon)
    return icon;
  return this->construct(cards);
} // Gdk::ScaledPixbufRotations Icons::load_from_file_or_construct(string name, vector<Card> cards)

/**
 ** load or construct the icons
 **
 ** @param     type   	the icon type
 **
 ** @return    loaded/constructed icon
 **/
Gdk::ScaledPixbufRotations
Icons::load(Type const type)
{
  Gdk::ScaledPixbufRotations icon;

  switch(type) {
  case Type::NONE:
    return Gdk::ScaledPixbufRotations();
  case Type::RE:
    return this->load_from_file_or_construct("re",
                                             {Card::CLUB_QUEEN});
  case Type::CONTRA:
    // ToDo: strike out the icon
    return this->load_from_file_or_construct("contra",
                                             {Card::CLUB_QUEEN});
  case Type::THROWN_NINES:
    return this->load_from_file_or_construct("thrown_nines",
                                             {{Card::CLUB, Card::NINE},
                                             {Card::SPADE, Card::NINE},
                                             {Card::HEART, Card::NINE},
                                             {Card::DIAMOND, Card::NINE}});
  case Type::THROWN_KINGS:
    return this->load_from_file_or_construct("thrown_kings",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::KING),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::THROWN_NINES_AND_KINGS:
    return this->load_from_file_or_construct("thrown_nines_and_kings",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::NINE),
                                             Card(Card::DIAMOND, Card::NINE) });
  case Type::THROWN_RICHNESS:
    return this->load_from_file_or_construct("thrown_richness",
                                             {Card(Card::CLUB,    Card::TEN),
                                             Card(Card::SPADE,   Card::TEN),
                                             Card(Card::HEART,   Card::TEN),
                                             Card(Card::DIAMOND, Card::TEN)});
  case Type::FOX_HIGHEST_TRUMP:
    return this->load_from_file_or_construct("fox_highest_trump",
                                             {Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::DIAMOND, Card::TEN),
                                             Card(Card::DIAMOND, Card::KING),
                                             Card(Card::DIAMOND, Card::NINE)});
  case Type::MARRIAGE:
    return this->load_from_file_or_construct("marriage",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN});
  case Type::MARRIAGE_SOLO:
    return this->load_from_file_or_construct("marriage.solo", "marriage",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN});
  case Type::MARRIAGE_FOREIGN:
    return this->load_from_file_or_construct("marriage.foreign",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::DIAMOND, Card::JACK),
                                             Card(Card::SPADE, Card::ACE)});
  case Type::MARRIAGE_TRUMP:
    return this->load_from_file_or_construct("marriage.trump",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::DIAMOND, Card::JACK),
                                             Card(Card::SPADE, Card::ACE)});
  case Type::MARRIAGE_COLOR:
    return this->load_from_file_or_construct("marriage.color",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::HEART, Card::ACE),
                                             Card(Card::SPADE, Card::ACE)});
  case Type::MARRIAGE_CLUB:
    return this->load_from_file_or_construct("marriage.club",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::CLUB, Card::ACE),
                                             Card(Card::CLUB, Card::ACE)});
  case Type::MARRIAGE_SPADE:
    return this->load_from_file_or_construct("marriage.spade",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::SPADE, Card::ACE),
                                             Card(Card::SPADE, Card::ACE)});
  case Type::MARRIAGE_HEART:
    return this->load_from_file_or_construct("marriage.heart",
                                             {Card::CLUB_QUEEN,
                                             Card::CLUB_QUEEN,
                                             Card(Card::HEART, Card::ACE),
                                             Card(Card::HEART, Card::ACE)});
  case Type::MARRIAGE_PARTNER: {
    auto icon = this->load_from_file("marriage.partner");
    if (!icon)
      icon = this->load(Type::RE);
    return icon;
  }
  case Type::GENSCHER:
    return this->load_from_file_or_construct("genscher",
                                             {Card(Card::DIAMOND, Card::KING),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::POVERTY:
    return this->load_from_file_or_construct("poverty",
                                             {Card(Card::HEART, Card::QUEEN),
                                             Card(Card::SPADE, Card::JACK),
                                             Card(Card::DIAMOND, Card::TEN)});
  case Type::POVERTY_TRUMPS_0:
    return this->load_from_file_or_construct("poverty.0",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::HEART, Card::KING),
                                             Card(Card::SPADE, Card::TEN)});
  case Type::POVERTY_TRUMPS_1:
    return this->load_from_file_or_construct("poverty.1",
                                             {Card(Card::SPADE, Card::QUEEN),
                                             Card(Card::HEART, Card::NINE),
                                             Card(Card::CLUB, Card::KING)});
  case Type::POVERTY_TRUMPS_2:
    return this->load_from_file_or_construct("poverty.2",
                                             {Card(Card::DIAMOND, Card::QUEEN),
                                             Card(Card::SPADE, Card::JACK),
                                             Card(Card::CLUB, Card::ACE)});
  case Type::POVERTY_TRUMPS_3:
    return this->load_from_file_or_construct("poverty.3",
                                             {Card(Card::HEART, Card::QUEEN),
                                             Card(Card::CLUB, Card::JACK),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::POVERTY_PARTNER: {
    auto icon = this->load_from_file("poverty.partner");
    if (!icon)
      icon = this->load(Type::RE);
    return icon;
  }
  case Type::SWINES_CLUB:
    return this->load_from_file_or_construct("swines.club", "swines",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::CLUB, Card::ACE)});
  case Type::SWINES_SPADE:
    return this->load_from_file_or_construct("swines.spade", "swines",
                                             {Card(Card::SPADE, Card::ACE),
                                             Card(Card::SPADE, Card::ACE)});
  case Type::SWINES_HEART:
    return this->load_from_file_or_construct("swines.heart", "swines",
                                             {Card(Card::HEART, Card::ACE),
                                             Card(Card::HEART, Card::ACE)});
  case Type::SWINES_DIAMOND:
    return this->load_from_file_or_construct("swines.diamond", "swines",
                                             {Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::DIAMOND, Card::ACE)});
  case Type::HYPERSWINES_CLUB:
    return this->load_from_file_or_construct("hyperswines.club", "hyperswines",
                                             {Card(Card::CLUB, Card::NINE),
                                             Card(Card::CLUB, Card::NINE)});
  case Type::HYPERSWINES_SPADE:
    return this->load_from_file_or_construct("hyperswines.spade", "hyperswines",
                                             {Card(Card::SPADE, Card::NINE),
                                             Card(Card::SPADE, Card::NINE)});
  case Type::HYPERSWINES_HEART:
    return this->load_from_file_or_construct("hyperswines.heart", "hyperswines",
                                             {Card(Card::HEART, Card::NINE),
                                             Card(Card::HEART, Card::NINE)});
  case Type::HYPERSWINES_DIAMOND:
    return this->load_from_file_or_construct("hyperswines.diamond", "hyperswines",
                                             {Card(Card::DIAMOND, Card::NINE),
                                             Card(Card::DIAMOND, Card::NINE)});
  case Type::HYPERSWINES_KING_CLUB:
    return this->load_from_file_or_construct("hyperswines.king.club",
                                             "hyperswines",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::CLUB, Card::KING)});
  case Type::HYPERSWINES_KING_SPADE:
    return this->load_from_file_or_construct("hyperswines.king.spade",
                                             "hyperswines",
                                             {Card(Card::SPADE, Card::KING),
                                             Card(Card::SPADE, Card::KING)});
  case Type::HYPERSWINES_KING_HEART:
    return this->load_from_file_or_construct("hyperswines.king.heart",
                                             "hyperswines",
                                             {Card(Card::HEART, Card::KING),
                                             Card(Card::HEART, Card::KING)});
  case Type::HYPERSWINES_KING_DIAMOND:
    return this->load_from_file_or_construct("hyperswines.king.diamond",
                                             "hyperswines",
                                             {Card(Card::DIAMOND, Card::KING),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::SWINES_HYPERSWINES_CLUB:
    return this->load_from_file_or_construct("swines-hyperswines.club",
                                             "swines-hyperswines",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::CLUB, Card::NINE)});
  case Type::SWINES_HYPERSWINES_SPADE:
    return this->load_from_file_or_construct("swines-hyperswines.spade",
                                             "swines-hyperswines",
                                             {Card(Card::SPADE, Card::ACE),
                                             Card(Card::SPADE, Card::NINE)});
  case Type::SWINES_HYPERSWINES_HEART:
    return this->load_from_file_or_construct("swines-hyperswines.heart",
                                             "swines-hyperswines",
                                             {Card(Card::HEART, Card::ACE),
                                             Card(Card::HEART, Card::NINE)});
  case Type::SWINES_HYPERSWINES_DIAMOND:
    return this->load_from_file_or_construct("swines-hyperswines.diamond",
                                             "swines-hyperswines",
                                             {Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::DIAMOND, Card::NINE)});
  case Type::SWINES_HYPERSWINES_KING_CLUB:
    return this->load_from_file_or_construct("swines-hyperswines.king.club",
                                             "swines-hyperswines",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::CLUB, Card::KING)});
  case Type::SWINES_HYPERSWINES_KING_SPADE:
    return this->load_from_file_or_construct("swines-hyperswines.king.spade",
                                             "swines-hyperswines",
                                             {Card(Card::SPADE, Card::ACE),
                                             Card(Card::SPADE, Card::KING)});
  case Type::SWINES_HYPERSWINES_KING_HEART:
    return this->load_from_file_or_construct("swines-hyperswines.king.heart",
                                             "swines-hyperswines",
                                             {Card(Card::HEART, Card::ACE),
                                             Card(Card::HEART, Card::KING)});
  case Type::SWINES_HYPERSWINES_KING_DIAMOND:
    return this->load_from_file_or_construct("swines-hyperswines.king.diamond",
                                             "swines-hyperswines",
                                             {Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::DOLLEN:
    return this->load_from_file_or_construct("dollen",
                                             {Card(Card::HEART, Card::TEN),
                                             Card(Card::HEART, Card::TEN)});

  case Type::DOPPELKOPF:
    return this->load_from_file_or_construct("doppelkopf",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::SPADE, Card::TEN),
                                             Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::HEART, Card::TEN)});
  case Type::SOLO_CLUB:
    return this->load_from_file_or_construct("solo.club",
                                             {Card(Card::SPADE, Card::QUEEN),
                                             Card(Card::HEART, Card::JACK),
                                             Card(Card::CLUB, Card::ACE),
                                             Card(Card::CLUB, Card::KING)});
  case Type::SOLO_SPADE:
    return this->load_from_file_or_construct("solo.spade",
                                             {Card(Card::CLUB, Card::QUEEN),
                                             Card(Card::DIAMOND, Card::JACK),
                                             Card(Card::SPADE, Card::ACE),
                                             Card(Card::SPADE, Card::KING)});
  case Type::SOLO_HEART:
    return this->load_from_file_or_construct("solo.heart",
                                             {Card(Card::DIAMOND, Card::QUEEN),
                                             Card(Card::CLUB, Card::JACK),
                                             Card(Card::HEART, Card::ACE),
                                             Card(Card::HEART, Card::KING)});
  case Type::SOLO_DIAMOND:
    return this->load_from_file_or_construct("solo.diamond",
                                             {Card(Card::HEART, Card::QUEEN),
                                             Card(Card::SPADE, Card::JACK),
                                             Card(Card::DIAMOND, Card::ACE),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::SOLO_JACK:
    return this->load_from_file_or_construct("solo.jack",
                                             {Card(Card::CLUB, Card::JACK),
                                             Card(Card::SPADE, Card::JACK),
                                             Card(Card::HEART, Card::JACK),
                                             Card(Card::DIAMOND, Card::JACK)});
  case Type::SOLO_QUEEN:
    return this->load_from_file_or_construct("solo.queen",
                                             {Card(Card::CLUB, Card::QUEEN),
                                             Card(Card::SPADE, Card::QUEEN),
                                             Card(Card::HEART, Card::QUEEN),
                                             Card(Card::DIAMOND, Card::QUEEN)});
  case Type::SOLO_KING:
    return this->load_from_file_or_construct("solo.king",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::KING),
                                             Card(Card::DIAMOND, Card::KING)});
  case Type::SOLO_QUEEN_JACK:
    return this->load_from_file_or_construct("solo.queen-jack",
                                             {Card(Card::CLUB, Card::QUEEN),
                                             Card(Card::SPADE, Card::QUEEN),
                                             Card(Card::HEART, Card::JACK),
                                             Card(Card::DIAMOND, Card::JACK)});
  case Type::SOLO_KING_JACK:
    return this->load_from_file_or_construct("solo.king-jack",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::JACK),
                                             Card(Card::DIAMOND, Card::JACK)});
  case Type::SOLO_KING_QUEEN:
    return this->load_from_file_or_construct("solo.king-queen",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::QUEEN),
                                             Card(Card::DIAMOND, Card::QUEEN)});
  case Type::SOLO_KOEHLER:
    return this->load_from_file_or_construct("solo.koehler",
                                             {Card(Card::CLUB, Card::KING),
                                             Card(Card::SPADE, Card::KING),
                                             Card(Card::HEART, Card::QUEEN),
                                             Card(Card::DIAMOND, Card::JACK)});
  case Type::SOLO_MEATLESS:
    return this->load_from_file_or_construct("solo.meatless",
                                             {Card(Card::CLUB, Card::ACE),
                                             Card(Card::SPADE, Card::ACE),
                                             Card(Card::HEART, Card::ACE),
                                             Card(Card::DIAMOND, Card::ACE)});
  case Type::NO_120: {
    auto icon = this->load_from_file("no_120");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 120'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO120)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_90: {
    auto icon = this->load_from_file("no_90");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 90'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO90)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_60: {
    auto icon = this->load_from_file("no_60");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 60'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO60)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_30: {
    auto icon = this->load_from_file("no_30");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 30'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO30)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_0: {
    auto icon = this->load_from_file("no_0");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 0'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO0)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_120_REPLY: {
    auto icon = this->load_from_file("no_120_reply");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 120 reply'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO120_REPLY)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_90_REPLY: {
    auto icon = this->load_from_file("no_90_reply");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 90 reply'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO90_REPLY)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_60_REPLY: {
    auto icon = this->load_from_file("no_60_reply");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 60 reply'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO60_REPLY)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_30_REPLY: {
    auto icon = this->load_from_file("no_30_reply");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 30 reply'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO30_REPLY)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  case Type::NO_0_REPLY: {
    auto icon = this->load_from_file("no_0_reply");
    if (!icon) {
      if (this->icon_.size() <= static_cast<unsigned>(type)) {
        // no icon loaded so far -- exiting
        this->ui->error("Cannot construct the icon for the announcement 'no 0 reply'.\n"
                        "Perhaps you should validate your 'FreeDokorc'-File "
                        "(" + ::setting.path(Setting::SETTINGS_FILE) + ").\n"
                        "Exiting.");
      } // if (this->icon_.size() <= type)
      this->ui->information(::translation("Cannot construct the icon for the announcement '%tannouncement%'.\n"
                                          "Keeping the old one.",
                                          ::translation(ANNOUNCEMENT::NO0_REPLY)),
                            INFORMATION::WARNING);
      icon = this->icon(type);
    } // if (!icon)
    return icon;
  }
  } // switch(type)

  return icon;
} // Gdk::ScaledPixbufRotations Icons::load(Type type)

/**
 ** construct an icon from the cards
 **
 ** @param     cards   cards
 **
 ** @return    constructed icon
 **
 ** @bug	the transparent regions are copied, too
 **/
Gdk::ScaledPixbufRotations
Icons::construct(vector<Card> const& cards)
{
  // take the half height and overlap with a fourth width

  int const width = (this->ui->cards->width_original()
                     * (4 + (cards.size() - 1))
                     / 4);
  int const height = (this->ui->cards->height_original() / 2);

  auto pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
                                    true, 8,
                                    width, height);

  for (unsigned i = 0; i < cards.size(); i++)
    this->ui->cards->card_original(cards[i])->copy_area(0, 0,
                                                        this->ui->cards->width_original(),
                                                        this->ui->cards->height_original() / 2,
                                                        pixbuf,
                                                        i * this->ui->cards->width_original() / 4, 0);


  return Gdk::ScaledPixbufRotations(pixbuf->scale_simple(width * 4 / 5,
                                                         height * 4 / 5,
                                                         Gdk::INTERP_BILINEAR));
} // Gdk::ScaledPixbufRotations Icons::construct(vector<Card> cards)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
