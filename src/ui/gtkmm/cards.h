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

#ifndef GTKMM_CARDS_HEADER
#define GTKMM_CARDS_HEADER


#include "base.h"

#include "widgets/scaled_pixbuf_rotations.h"

#include "../../card/card.h"

#include <glibmm/refptr.h>
namespace Gtk {
class Image;
}; // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the cards
 **
 ** @author	Diether Knof
 **/
class Cards : public Base, public sigc::trackable {
  public:
    // constructor
    Cards(Base* base);

    // destructor
    ~Cards() = default;

    // update the scaled cards
    void update_scaling();

    // the original width of the cards
    unsigned width_original(Rotation rotation = Rotation::UP) const;
    // the original height of the cards
    unsigned height_original(Rotation rotation = Rotation::UP) const;
    // the width of the scaled cards
    unsigned width(Rotation rotation = Rotation::UP) const;
    // the width of the scaled cards
    unsigned height(Rotation rotation = Rotation::UP) const;

    // the back picture (scaled)
    Gdk::ScaledPixbuf& back(Rotation rotation = Rotation::UP);
    // the card picture (scaled)
    Gdk::ScaledPixbuf& card(Card card, Rotation rotation = Rotation::UP);
    // the back picture (original)
    Glib::RefPtr<Gdk::Pixbuf> const& back_original(Rotation rotation = Rotation::UP);
    // the card picture (original)
    Glib::RefPtr<Gdk::Pixbuf> const& card_original(Card card,
                                                   Rotation rotation = Rotation::UP);

    // managed images
    Gtk::Image* new_managed_image(Card card = Card());
    void change_managed(Gtk::Image* image, Card card);
    void remove_managed(Gtk::Image const* image);
    void update_managed();

    // load the cards and the card backs
    void load_all();
    // load the card backs
    void load_back();
    // load the cards
    void load_cards();

  private:
    // the scaling
    PRIV_VAR_R(double, scaling);

    // the back picture
    Gdk::ScaledPixbufRotations back_;
    // the card pictures
    vector<vector<Gdk::ScaledPixbufRotations>> card_;

    // the images that are managed by 'this'
    vector<Gtk::Image*> managed_image_;

  private: // unused
    Cards() = delete;
    Cards(Cards const&) = delete;
    Cards& operator=(Cards const&) = delete;
}; // class Cards : public Base

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_CARDS_HEADER

#endif // #ifdef USE_UI_GTKMM
