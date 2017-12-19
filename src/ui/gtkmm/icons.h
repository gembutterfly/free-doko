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

#ifndef GTKMM_ICONS_HEADER
#define GTKMM_ICONS_HEADER


#include "base.h"

#include "widgets/scaled_pixbuf_rotations.h"

#include "../../basistypes.h"
#include "../../card/card.h"

class Player;

#include <glibmm/refptr.h>
namespace Gtk {
class Image;
}; // namespace Gtk

namespace UI_GTKMM_NS {

/**
 **
 ** @brief	the icons
 **
 ** @author	Diether Knof
 **
 **/
class Icons : public Base, public sigc::trackable {
  friend class Icongroup;
  public:
  enum class Type {
    NONE = 0,
    RE,
    CONTRA,
    THROWN_NINES,
    THROWN_KINGS,
    THROWN_NINES_AND_KINGS,
    THROWN_RICHNESS,
    FOX_HIGHEST_TRUMP,
    MARRIAGE,
    MARRIAGE_SOLO,
    MARRIAGE_FOREIGN,
    MARRIAGE_TRUMP,
    MARRIAGE_COLOR,
    MARRIAGE_CLUB,
    MARRIAGE_SPADE,
    MARRIAGE_HEART,
    MARRIAGE_PARTNER,
    GENSCHER,
    POVERTY,
    POVERTY_TRUMPS_0,
    POVERTY_TRUMPS_1,
    POVERTY_TRUMPS_2,
    POVERTY_TRUMPS_3,
    POVERTY_PARTNER,
    SWINES_CLUB,
    SWINES_SPADE,
    SWINES_HEART,
    SWINES_DIAMOND,
    SWINES = SWINES_DIAMOND,
    HYPERSWINES_CLUB,
    HYPERSWINES_SPADE,
    HYPERSWINES_HEART,
    HYPERSWINES_DIAMOND,
    HYPERSWINES = HYPERSWINES_DIAMOND,
    HYPERSWINES_KING_CLUB,
    HYPERSWINES_KING_SPADE,
    HYPERSWINES_KING_HEART,
    HYPERSWINES_KING_DIAMOND,
    SWINES_HYPERSWINES_CLUB,
    SWINES_HYPERSWINES_SPADE,
    SWINES_HYPERSWINES_HEART,
    SWINES_HYPERSWINES_DIAMOND,
    SWINES_HYPERSWINES = SWINES_HYPERSWINES_DIAMOND,
    SWINES_HYPERSWINES_KING_CLUB,
    SWINES_HYPERSWINES_KING_SPADE,
    SWINES_HYPERSWINES_KING_HEART,
    SWINES_HYPERSWINES_KING_DIAMOND,
    DOLLEN,
    DOPPELKOPF,
    SOLO_CLUB,
    SOLO_SPADE,
    SOLO_HEART,
    SOLO_DIAMOND,
    SOLO_JACK,
    SOLO_QUEEN,
    SOLO_KING,
    SOLO_QUEEN_JACK,
    SOLO_KING_JACK,
    SOLO_KING_QUEEN,
    SOLO_KOEHLER,
    SOLO_MEATLESS,
    NO_120_REPLY,
    NO_90_REPLY,
    NO_60_REPLY,
    NO_30_REPLY,
    NO_0_REPLY,
    NO_120,
    NO_90,
    NO_60,
    NO_30,
    NO_0,
    LAST = NO_0
  }; // enum Type
  static size_t const NUMBER_OF_TYPES; // number of icons

  static Type type(GameType gametype);
  static Type type(MarriageSelector marriage_selector);
  static Type type(Team team);
  static Type type(Announcement announcement);
  static Type type_swines(Card::Color color);
  Type type_hyperswines(Card::Color color);
  Type type_swines_hyperswines(Card::Color color);

  public:
  Icons(Base* base);
  ~Icons() = default;

  double scaling() const;
  void update_scaling();

  int max_width(Rotation rotation = Rotation::UP) const;
  int max_height(Rotation rotation = Rotation::UP) const;
  int max_width(vector<Glib::RefPtr<Gdk::Pixbuf>> const& icons,
                Rotation rotation = Rotation::UP) const;
  int max_height(vector<Glib::RefPtr<Gdk::Pixbuf>> const& icons,
                 Rotation rotation = Rotation::UP) const;

  // the specific icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    icon(Type type, Rotation rotation = Rotation::UP) ;

  // the specific gametype icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    icon(GameType gametype, Rotation rotation = Rotation::UP)
    { return this->icon(this->type(gametype), rotation); }
  // the specific marriage selector icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    icon(MarriageSelector marriage_selector, Rotation rotation = Rotation::UP)
    { return this->icon(this->type(marriage_selector), rotation); }
  // the specific team icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    icon(Team team, Rotation rotation = Rotation::UP)
    { return this->icon(this->type(team), rotation); }
  // the specific announcement icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    icon(Announcement announcement, Rotation rotation = Rotation::UP)
    { return this->icon(this->type(announcement), rotation); }
  // the specific swines icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    swines(Card::Color color, Rotation rotation = Rotation::UP)
    { return this->icon(this->type_swines(color), rotation); }
  // the specific hyperswines icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    hyperswines(Card::Color color, Rotation rotation = Rotation::UP)
    { return this->icon(this->type_hyperswines(color), rotation); }
  // the specific swines-hyperswines icon
  Glib::RefPtr<Gdk::Pixbuf> const&
    swines_hyperswines(Card::Color color, Rotation rotation = Rotation::UP)
    { return this->icon(this->type_swines_hyperswines(color), rotation); }


  // managed images
  Gtk::Image* new_managed_image(Type type = Type::NONE, double scaling = 1);

  void change_managed(Gtk::Image* image, Type type);
  void change_managed(Gtk::Image* image,
                      GameType gametype)
  { this->change_managed(image, this->type(gametype)); return ; }
  void change_managed(Gtk::Image* image,
                      MarriageSelector marriage_selector)
  { this->change_managed(image, this->type(marriage_selector)); return ; }
  void change_managed(Gtk::Image* image,
                      Team team)
  { this->change_managed(image, this->type(team)); return ; }
  void change_managed(Gtk::Image* image,
                      Announcement announcement)
  { this->change_managed(image, this->type(announcement)); return ; }
  void change_managed_swines(Gtk::Image* image,
                             Card::Color color)
  { this->change_managed(image, this->type_swines(color)); return ; }
  void change_managed_hyperswines(Gtk::Image* image,
                                  Card::Color color)
  { this->change_managed(image, this->type_hyperswines(color)); return ; }
  void change_scaling(Gtk::Image* image, double scaling);

  void remove_managed(Gtk::Image const* image);

  void update_managed();

  // update the maximal width and height
  void update_max_values();

  void load();

  Gdk::ScaledPixbufRotations load(Type type);
  private:
  Gdk::ScaledPixbufRotations load_from_file(string const& name_a,
                                            string const& name_b = "");
  Gdk::ScaledPixbufRotations load_from_file_or_construct(string const& file,
                                                         vector<Card> const& cards);
  Gdk::ScaledPixbufRotations load_from_file_or_construct(string const& name_a,
                                                         string const& name_b,
                                                         vector<Card> const& cards);
  Gdk::ScaledPixbufRotations construct(vector<Card> const& cards);

  private:
  // the icons
  vector<Gdk::ScaledPixbufRotations> icon_;
  // maximal width of the icons
  int max_width_;
  // maximal height of the icons
  int max_height_;

#ifdef OUTDATED
  // a reference to a drawable
  Glib::RefPtr<Gdk::Drawable> drawable_ref;
#endif

  // the managed images
  vector<Gtk::Image*> managed_image_;

  private: // unused
  Icons() = delete;
  Icons(Icons const&) = delete;
  Icons& operator=(Icons const&) = delete;
}; // class Icons : public Base

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_ICONS_HEADER

#endif // #ifdef USE_UI_GTKMM
