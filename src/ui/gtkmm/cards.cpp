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

#include "cards.h"

#include "ui.h"
#include "utils.h"
#include "splash_screen.h"
#include "help.h"

#include "../status_message.h"

#include "../../misc/setting.h"
#include "../../misc/translations.h"

#include "../../utils/file.h"

#include <gtkmm/image.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/alignment.h>
#include "widgets/stock_button.h"

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **
 ** @return    -
 **/
Cards::Cards(Base* const parent) :
  Base(parent),
  scaling_(1)
{
  this->load_all();

  return ;
} // Cards::Cards(Base* const parent)

/**
 ** update the scaling
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Cards::update_scaling()
{
  if (::setting(Setting::ORIGINAL_CARDS_SIZE))
    this->scaling_ = 1;
  else
    this->scaling_ = (static_cast<double>(::setting(Setting::CARDS_HEIGHT))
                      / this->height_original());

  // make that the width cannot get '0'
  if (static_cast<unsigned>(this->scaling_ * this->width_original())
      < ::setting.min(Setting::CARDS_HEIGHT))
    this->scaling_
      = (static_cast<double>(::setting.min(Setting::CARDS_HEIGHT))
         / this->width_original());


  // set the scaling of the 'ScaledPixbufRotations'
  this->back_.set_scaling(this->scaling());
  for (auto& c : this->card_)
    for (auto& v : c)
      v.set_scaling(this->scaling_);

  return ;
} // void Cards::update_scaling()

/**
 ** -> result
 **
 ** @param     rotation   the rotation of the card
 **
 ** @return    the original width of the cards
 **/
unsigned
Cards::width_original(Rotation const rotation) const
{
  DEBUG_ASSERTION(this->back_,
                  "Cards::width_original(rotation):\n"
                  "  'this->back_' is empty");

  return this->back_.get_orig_width(rotation);
} // unsigned Cards::width_original(Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     rotation   the rotation of the card
 **
 ** @return    the original height of the cards
 **/
unsigned
Cards::height_original(Rotation const rotation) const
{
  DEBUG_ASSERTION(this->back_,
                  "Cards::height_original(rotation):\n"
                  "  'this->back_' is empty");

  return this->back_.get_orig_height(rotation);
} // unsigned Cards::height_original(Rotation const rotation = UP) const


/**
 ** -> result
 **
 ** @param     rotation   the rotation of the card
 **
 ** @return    the width of the cards
 **/
unsigned
Cards::width(Rotation const rotation) const
{
  return this->back_.get_width(rotation);
} // unsigned Cards::width(Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     rotation   the rotation of the card
 **
 ** @return    the height of the cards
 **/
unsigned
Cards::height(Rotation const rotation) const
{
  return this->back_.get_height(rotation);
} // unsigned Cards::height(Rotation rotation = UP) const

/**
 ** -> result
 **
 ** @param     rotation   the rotation
 **
 ** @return    the back of the cards (with rotation 'rotation')
 **/
Gdk::ScaledPixbuf&
Cards::back(Rotation const rotation)
{
  return this->back_[rotation];
} // Gdk::ScaledPixbuf& Cards::back(Rotation rotation = UP)

/**
 ** -> result
 **
 ** @param     card   	the card
 ** @param     rotation   the rotation
 **
 ** @return    the pixbuf of 'card' (with rotation 'rotation')
 **/
Gdk::ScaledPixbuf&
Cards::card(Card const card, Rotation const rotation)
{
  if (   (card == Card::EMPTY)
      || (card == Card::UNKNOWN))
    return this->back(rotation);

  // a pointer to the vector with all cards with 'card.color'
  vector<Gdk::ScaledPixbufRotations>* card_color = nullptr;
  switch (card.color()) {
  case Card::CLUB:
    card_color = &(this->card_[0]);
    break;
  case Card::SPADE:
    card_color = &(this->card_[1]);
    break;
  case Card::HEART:
    card_color = &(this->card_[2]);
    break;
  case Card::DIAMOND:
    card_color = &(this->card_[3]);
    break;
  default:
    DEBUG_ASSERTION(false,
                    "Cards::card(card, rotation):\n"
                    "  card.color '" << card.color() << "' not valid.");
    break;
  } // switch(card.color())

  // a pointer to the vector with all cards with 'card'
  Gdk::ScaledPixbufRotations* card_color_value = nullptr;
  switch (card.value()) {
  case Card::ACE:
    card_color_value = &((*card_color)[0]);
    break;
  case Card::TEN:
    card_color_value = &((*card_color)[1]);
    break;
  case Card::KING:
    card_color_value = &((*card_color)[2]);
    break;
  case Card::QUEEN:
    card_color_value = &((*card_color)[3]);
    break;
  case Card::JACK:
    card_color_value = &((*card_color)[4]);
    break;
  case Card::NINE:
    card_color_value = &((*card_color)[5]);
    break;
  default:
    DEBUG_ASSERTION(false,
                    "Cards::card(card, rotation):\n"
                    "  card.value '" << card.value() << "' not valid.");
    break;
  } // switch(card.color())

  return (*card_color_value)[rotation];
} // Gdk::ScaledPixbuf& Cards::card(Card card, Rotation rotation = UP)

/**
 ** -> result
 **
 ** @param     rotation   the rotation
 **
 ** @return    the back of the cards (with rotation 'rotation') (scaled)
 **/
Glib::RefPtr<Gdk::Pixbuf> const&
Cards::back_original(Rotation const rotation)
{
  return this->back().orig_pixbuf();
} // Glib::RefPtr<Gdk::Pixbuf> const& Cards::back_original(Rotation rotation = UP)

/**
 ** -> result
 **
 ** @param     card   	the card
 ** @param     rotation   the rotation
 **
 ** @return    the pixbuf of 'card' (with rotation 'rotation') (original)
 **/
Glib::RefPtr<Gdk::Pixbuf> const&
Cards::card_original(Card const card, Rotation const rotation)
{
  DEBUG_ASSERTION(this->card(card, rotation),
                  "Cards::card_original(card, rotation):\n"
                  "  'card' is null");
  DEBUG_ASSERTION(this->card(card, rotation).orig_pixbuf(),
                  "Cards::card_original(card, rotation):\n"
                  "  'card.orig_pixbuf()' is null");

  return this->card(card, rotation).orig_pixbuf();
} // Glib::RefPtr<Gdk::Pixbuf> const& Cards::card_original(Card card, Rotation rotation = UP)

/**
 ** -> result
 **
 ** @param     card   card
 **			(default: Card())
 **
 ** @return    new image with the card 'card' that is managed
 **		(that is size and theme)
 **/
Gtk::Image*
Cards::new_managed_image(Card const card)
{
  auto image = Gtk::manage(new Gtk::Image(this->card(card)));
  image->set_data("card", new Card(card));

  image->signal_unrealize().connect(sigc::bind<Gtk::Image const* const>(sigc::mem_fun(*this, &Cards::remove_managed), image));

  this->managed_image_.push_back(image);

  return image;
} // Gtk::Image* Cards::new_managed_image(Card card = Card())

/**
 ** changes the type of 'image'
 **
 ** @param     image   image to change the type of
 ** @param     card   new card
 **
 ** @return    -
 **/
void
Cards::change_managed(Gtk::Image* const image, Card const card)
{
  DEBUG_ASSERTION(image->get_data("card"),
                  "Cards::change_managed:\n"
                  "  image has no data 'type'");
  // ToDo: Test whether the image is in the vector

  delete static_cast<Card*>(image->steal_data("card"));
  image->set_data("card", new Card(card));

  image->set(this->card(card));

  return ;
} // void Cards::change_managed(Gtk::Image* image, Card card)

/**
 ** remove 'image' from the managed image list
 **
 ** @param     image   image to remove
 **
 ** @return    -
 **/
void
Cards::remove_managed(Gtk::Image const* const image)
{
  for (auto i = this->managed_image_.begin();
       i != this->managed_image_.end();
      )
    if (*i == image) {
      delete static_cast<Card*>((*i)->steal_data("card"));
      i = this->managed_image_.erase(i);
      break;
    } else {
      ++i;
    }

  return ;
} // void Cards::remove_managed(Gtk::Image const* image)

/**
 ** update the managed images
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Cards::update_managed()
{
  for (auto i : this->managed_image_) {
    auto const card = *(static_cast<Card*>(i->get_data("card")));
    i->set(this->card(card));
  }

  return ;
} // void Cards::update_managed()

/**
 ** load the cards and the back
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Cards::load_all()
{
  this->load_back();
  this->load_cards();

  return ;
} // void Cards::load_all()

/**
 ** load the cards back
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Cards::load_back()
{
  this->ui->add_status_message(::translation("~loading the cards (cards back)"));
  auto back_new
    = Gdk::ScaledPixbufRotations(::setting.path(Setting::CARDS_BACK));
  if (!back_new) {
    if (!this->back_) {
      this->ui->error(::translation("Error::loading the card back %sback% (%scardset%)",
                                    ::setting.value(Setting::CARDS_BACK),
                                    ::setting.value(Setting::CARDSET)).translation());
    }
    return ;
  } // if (!back_new)

  this->back_ = back_new;
  this->back_.set_scaling(this->scaling());

  if (::setting(Setting::ORIGINAL_CARDS_SIZE) == true)
    ::setting.set(Setting::CARDS_HEIGHT, this->height_original());

  this->update_scaling();

  this->ui->remove_status_message(::translation("~loading the cards (cards back)"));
  return ;
} // void Cards::load_back()

/**
 ** load the cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Cards::load_cards()
{
  StatusMessages sm(*this->ui);
  if (   (::game_status != GAMESTATUS::PROGRAMSTART)
      && (::game_status != GAMESTATUS::PARTY_INITIAL_LOADED) ) {
    sm.add(::translation("loading the cards"));
  } // if (::game_status != GAMESTATUS::PROGRAMSTART)

  // load the cards
  vector<Card::Color> color = {Card::CLUB, Card::SPADE, Card::HEART, Card::DIAMOND};
  vector<Card::Value> value = {Card::ACE, Card::TEN, Card::KING, Card::QUEEN, Card::JACK, Card::NINE};

  vector<vector<Gdk::ScaledPixbufRotations>> card_new;
  for (auto c : color) {
    card_new.push_back({});
    for (auto v : value) {
      sm.add(::translation("~loading the cards (%tcard%)", ::translation(Card(c, v))));
      card_new.back().push_back(Gdk::ScaledPixbufRotations(::setting.path(Setting::CARDSET) + "/"
                                                           + ::setting.value(Setting::CARDS_DIRECTORY) + "/"
                                                           + ::name(c)
                                                           + "/"
                                                           + ::name(v)
                                                           + "."
                                                           + ::setting(Setting::GRAPHIC_EXTENSION)));
      card_new.back().back().set_scaling(this->scaling());
      if (!card_new.back().back())
        this->ui->error(::translation("Error::loading the card %tcard% (%scardset%)",
                                      ::translation(Card(c, v)),
                                      ::setting.value(Setting::CARDSET)).translation());
      if (   (::game_status == GAMESTATUS::PROGRAMSTART)
          || (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED) )
        this->ui->add_progress(1.0 / (color.size() * value.size()));
    } // for (v \in value)
  } // for (c \in color)

  this->card_ = card_new;

  return ;
} // void Cards::load_cards()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
