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
 ********************************************************************/

#include "constants.h"

#include "player.h"
#include "playersDb.h"

#include "human/human.h"
#include "ai/ai.h"
#include "ai.dummy/ai.h"
#include "ai.random/ai.h"

#include "../card/hand.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game_summary.h"
#include "../card/trick.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../ui/ui.h"

// whether to write the databasea
bool Player::write_database = true;

/**
 ** -> result
 **
 ** @param     type   playertype
 **
 ** @return    pointer to a new player of the given type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Player*
Player::create(Type const& type)
{
  switch(type) {
  case Player::UNSET:
    DEBUG_ASSERTION(false,
                    "Player::create(type):\n"
                    "  type == UNSET");
    return NULL;
  case Player::HUMAN:
    return new Human();
  case Player::AI:
    return new Ai();
  case Player::AI_DUMMY:
    return new AiDummy();
  case Player::AI_RANDOM:
    return new AiRandom();
  case Player::NETWORK:
    DEBUG_ASSERTION(false,
                    "Player::create(type):\n"
                    "  type == NETWORK");
    return NULL;
  } // switch(type)

  return NULL;
} // Player* Player::create(Type type)

/**
 ** -> result
 **
 ** @param     type   playertype
 ** @param     name   name of the player
 **
 ** @return    pointer to a new player of the given type
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Player*
Player::create(Type const& type, string const& name)
{
  Player* player = NULL;
  switch(type) {
  case Player::UNSET:
    DEBUG_ASSERTION(false,
                    "Player::create(type, name):\n"
                    "  type == UNSET");
    return NULL;
  case Player::HUMAN:
    player = new Human();
    break;
  case Player::AI:
    player = new Ai();
    break;
  case Player::AI_DUMMY:
    player = new AiDummy();
    break;
  case Player::AI_RANDOM:
    player = new AiRandom();
    break;
  case Player::NETWORK:
    DEBUG_ASSERTION(false,
                    "Player::create(type, name):\n"
                    "  type == NETWORK");
    return NULL;
  } // switch(type)

  if (player)
    player->set_name(name);

  return player;
} // Player* Player::create(Type type, string name)

/**
 ** -> result
 **
 ** @param	type     playertype
 ** @param	player   the player to be copied
 **
 ** @return	pointer to a new player of the given type and with data
 **		from 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Player*
Player::create(Type const& type, Player const& player)
{
  Player* player_new = NULL;

  switch(type) {
  case Player::UNSET:
    DEBUG_ASSERTION(false,
                    "Player::create(type, player):\n"
                    "  type == UNSET");
    break;
  case Player::HUMAN:
#ifdef WORKAROUND
    if (dynamic_cast<Ai const*>(&player) != NULL)
      player_new = new Human(dynamic_cast<Ai const&>(player));
    else if (dynamic_cast<Aiconfig const*>(&player) != NULL)
      player_new = new Human(player, dynamic_cast<Aiconfig const&>(player));
    else
      player_new = new Human(player);
#else
    // see 'AI'
    player_new = new Human(player);
#endif
    break;

  case Player::AI:
#ifdef WORKAROUND
    if (dynamic_cast<Ai const*>(&player) != NULL)
      player_new = new Ai(dynamic_cast<Ai const&>(player));
    else if (dynamic_cast<Aiconfig const*>(&player) != NULL)
      player_new = new Ai(player, dynamic_cast<Aiconfig const&>(player));
    else
      player_new = new Ai(player);
#else
    // if 'p' is of type 'Human', the GNU C++ compiler calls 'Ai(Player)' but
    // not 'Ai(Ai)' although 'Human' is a child of 'Ai'
    player_new = new Ai(player);
#endif
    break;

  case Player::AI_DUMMY:
    if (dynamic_cast<Aiconfig const*>(&player) != NULL)
      player_new = new AiDummy(player, dynamic_cast<Aiconfig const&>(player));
    else
      player_new = new AiDummy(player);
    break;

  case Player::AI_RANDOM:
    if (dynamic_cast<Aiconfig const*>(&player) != NULL)
      player_new = new AiRandom(player, dynamic_cast<Aiconfig const&>(player));
    else
      player_new = new AiRandom(player);
    break;

  case Player::NETWORK:
    DEBUG_ASSERTION(false,
                    "Player::create(type, player):\n"
                    "  type == NETWORK");
    break;
  } // switch(type)

  return player_new;
} // Player* Player::create(Type type, Player player)

/**
 ** returns a player, created by the infos of the file
 **
 ** @param	filename	file with the player info
 **
 ** @return	pointer to a new player of the given type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Player*
Player::create(string const& filename)
{
  ifstream istr(filename.c_str());
  if (istr.fail())
    return NULL;

#ifdef WORKAROUND
  // test whether the player file is empty
  // Note: if the human player (player 0) cannot be loaded,
  // he is replaced by an ai.
  istr.peek();
  if (istr.eof()) {
    cerr << "could not load player from file '" << filename << "', "
      << "creating new player" << endl;
    return Player::create(AI);
  }
#endif

  return Player::create(istr);
} // Player* Player::create(string filename)

/**
 ** returns a player, created by the infos of the stream
 **
 ** @param     istr   stream with the player info
 **
 ** @return    pointer to a new player of the given type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Player*
Player::create(istream& istr)
{
  Player* player = new Player(UNSET);

  Config config;
  istr >> config;

  int depth = 0;
  Type type_before = Player::UNSET;

  if ((config.name == "{")
      && config.value.empty()) {
    depth += 1;
    istr >> config;
  }
  while (istr.good()) {
#ifndef OUTDATED
    // DK: 2005-01-21
    // The player is not included in '{ }', so check, whether the next line
    // is 'Name = ...'
    while ( (istr.peek() == '\n')
           || (istr.peek() == '\r'))
      istr.get();
    if ((istr.peek() == 'N')
        && (depth == 0))
      break;
#endif

    if (istr.eof())
      break;

    if (istr.fail()) {
#ifndef RELEASE
      cerr << "reading player failed" << endl;
#endif
      return NULL;
    }

    // finished with the config file
    if ((config.name == "") && (config.value == ""))
      continue;

    if ((config.name == "{")
        && config.value.empty())
      depth += 1;

    if ((config.name == "}")
        && config.value.empty()) {
      depth -= 1;
      // finished with the player
      if (depth == -1)
        istr.putback('}');
      if (depth <= 0)
        break;
    }

    if (!player->read(config, istr))
      cerr << "Unknown player line '" << config.name << " = " << config.value
        << '\'' << endl;

    // if the type is set, change the underlying player
    if (player->type() != type_before) {
      Player* player2 = NULL;
      switch(player->type()) {
      case UNSET:
        DEBUG_ASSERTION(false,
                        "Player::create(istr):\n"
                        "  player type is 'UNSET'");
      case HUMAN:
        player2 = Player::create(HUMAN, *player);
        break;
      case AI:
        player2 = Player::create(AI, *player);
        break;
      case AI_DUMMY:
        player2 = Player::create(AI_DUMMY, *player);
        break;
      case AI_RANDOM:
        player2 = Player::create(AI_RANDOM, *player);
        break;
      case NETWORK:
        DEBUG_ASSERTION(false,
                        "Player* Player::create(istream)\n"
                        "  type is 'NETWORK'");
        break;
      } // switch(player->type())
      delete player;
      player = player2;
      type_before = player->type();
    } // if (player->type() != UNSET)

    istr >> config;
  } // while (istr.good())

  return player;
} // Player* Player::create(istream& istr)

/**
 ** constructor
 **
 ** @param       type   type of the player
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
Player::Player(Type const& type)  :
  game_(NULL),
  type_(type),
  name_(),
  voice_(),
  color_(""),
  team_(TEAM::NOTEAM),
  hand_(),
  sorted_hand_(this->hand_),
  reservation_(),
  trickpile_(),
  db_( new PlayersDb )
{ }

/**
 ** copy constructor
 **
 ** @param       player  player to copy
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.6.4
 **/
Player::Player(Player const& player) :
  game_(player.game_),
  type_(Player::UNSET),
  name_(player.name_),
  voice_(player.voice_),
  color_(player.color()),
  team_(player.team_),
  hand_(*this, player.hand_),
  sorted_hand_(player.sorted_hand_, this->hand_),
  reservation_(player.reservation_),
  trickpile_(player.trickpile_),
  db_( (player.db_ == NULL)
      ? NULL
      : new PlayersDb(player.db()) ) // *** DK must use a copy!
{ }


/**
 ** destructor
 **
 ** @param       -
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.5.3
 **/
Player::~Player()
{
  delete this->db_;

  return ;
} // Player::~Player()

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param	-
 **
 ** @return	wether the self-check was successful (no error)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 ** @todo	check the hand
 ** @todo	check the sorted hand
 **
 **/
bool
Player::self_check() const
{
  DEBUG_ASSERTION(this->db_,
                  "Player::self_check()\n"
                  "  no database");
  if (this->game_) {
    DEBUG_ASSERTION((this->game_->no(*this) != UINT_MAX),
                    "Player::self_check()\n"
                    "  player not in game: " << this->name_);


    // ToDo
    // check the hand
    // check the sorted hand


    for (TrickPile::const_iterator t = this->trickpile_.begin();
         t != this->trickpile_.end();
         ++t) {
      if (this->game_ != &(*t)->game()) {
        cerr << "  " << this->game_ << " != " << &(*t)->game() << endl;
        cerr << "  trickpile anz: " << this->trickpile_.size() << endl;
        cerr << "  Player: " << this->game().no(*this) << endl;
        cerr << "  Trick: " << *t << endl;
        if (*t)
          cerr << "  Trick: " << **t << endl;
        DEBUG_ASSERTION((this->game_ == &(*t)->game()),
                        "Player::self_check()\n"
                        "  Trick " << (*t)->no() << " has not the same game");
      }
    } // for (t \in this->trickpile_)

    this->hand().self_check();
  } // if (this->game_)

  return true;
} // virtual bool Player::self_check() const

/**
 **
 ** -> result
 **
 ** @param	player	player to compare with
 **
 ** @return	whether the players are the equal (value)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 ** @todo	compare values
 ** @todo	comparing sorted hands
 ** @todo	comparing database
 **
 **/
bool
Player::is_equal(Player const& player) const
{
  if (this->type() != player.type())
    return false;
  if (this->name_ != player.name_)
    return false;
  if (this->voice_ != player.voice_)
    return false;

  if (this->game_ != player.game_)
    return false;

  if (this->hand_ != player.hand_)
    return false;
#ifdef POSTPONED
  if (this->sorted_hand_ != player.sorted_hand_)
    return false;
#endif
  if (this->trickpile_ != player.trickpile_)
    return false;
#ifdef POSTPONED
  if (*(this->db_) != *(player.db_))
    return false;
#endif

  switch(this->type()) {
  case UNSET:
    return true;
  case HUMAN:
  case AI:
  case AI_DUMMY:
  case AI_RANDOM:
  case NETWORK:
    // ToDo: compare with values
    return (this == &player);
  } // switch (this->type())

  DEBUG_ASSERTION(false,
                  "Player::is_equal(player):\n"
                  "  behind the 'case' block");

  return true;
} // bool Player::is_equal(Player const& player) const

/**
 ** clone the player
 **
 ** @param       -
 **
 ** @return      pointer of a clone
 **		NULL if the player could get cloned
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.2
 **/
Player*
Player::clone() const
{  
  return new Player(*this);
} // virtual Player* Player::clone() const

/**
 **
 ** copy operator
 **
 ** @param       player  player to be copied
 **
 ** @return      copy of player
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Player&
Player::operator=(const Player& player)
{  
  // copy the values
  this->game_ = player.game_;
  this->type_ = player.type_;
  this->name_ = player.name_;
  this->voice_ = player.voice_;
  this->color_ = player.color_;
  this->team_ = player.team_;
  this->hand_ = player.hand_;
  this->hand_.set_player(*this);
  this->sorted_hand_ = SortedHand(player.sorted_hand_, this->hand_);
  this->trickpile_ = player.trickpile_;
  this->db_        = ((player.db_ == NULL)
                      ? NULL
                      : new PlayersDb(*(player.db_))); // *** DK must use a copy!

  return *this;
} // Player& Player::operator=(const Player& player)

/**
 **
 ** -> result
 **
 ** @param	player1		player 1 to compare
 ** @param	player2	 player 2 to compare
 **
 ** @return	whether the players are equal (they lie in the same memory)
 **
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 ** @todo	compare with values
 **
 **/
bool
operator==(Player const& player1, Player const& player2)
{
  //return player1.is_equal(player2);
  return (&player1 == &player2);
} // bool operator==(Player const& player1, Player const& player2)

/**
 **
 ** -> result
 **
 ** @param       player1 player 1 to compare
 ** @param       player2 player 2 to compare
 **
 ** @return      whether the players are not equal
 **              (they don't lie in the same memory)
 **
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
bool
operator!=(Player const& player1, Player const& player2)
{
  return !(player1 == player2);
} // bool operator!=(Player const& player1, Player const& player2)

/**
 **
 ** save the player in the file 'filename'
 **
 ** @param	filename    file the player shall be saved
 **
 ** @return	whether the saving was sucessful
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **
 **/
bool
Player::save(string const& filename) const
{
  string filename_tmp = filename + ".tmp";
  ofstream ostr(filename_tmp.c_str());
  if (!ostr.good()) {
    ::ui->information(::translation("Error::Player%sname%::save: Error opening temporary file %stmpfile%. Aborting saving.",
                                    this->name(), filename_tmp), INFORMATION::PROBLEM);
    return false;
  }

  ostr << "# FreeDoko player (" << *::version << ")\n"
    << '\n';
  this->write(ostr);

  if (!ostr.good()) {
    ::ui->information(::translation("Error::Player%sname%::save: Error saving in temporary file %stmpfile%. Keeping temporary file (for bug tracking).",
                                    this->name(), filename_tmp), INFORMATION::PROBLEM);
    return false;
  }
  ostr.close();

#ifdef WINDOWS
  unlink(filename.c_str());
#endif
  if (rename(filename_tmp.c_str(), filename.c_str())) {
    ::ui->information(::translation("Error::Player%sname%::save: Could not rename temporary file %stmpfile% to requested file %sfile%. Keeping temporary file.",
                                    this->name(), filename_tmp, filename), INFORMATION::PROBLEM);
    return false;
  }

  return true;
} // virtual bool Player::save(string const& filename) const

/**
 **
 ** write the player in the stream
 **
 ** @param       ostr    output stream
 **
 ** @return      output stream
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
ostream&
Player::write(ostream& ostr) const
{
  ostr << "name = " << this->name_value() << '\n';
  if (this->name_value().empty())
    ostr << "# name = " << this->name() << '\n';
  ostr << "voice = " << this->voice() << '\n'
    << "type = " << this->type() << '\n'
    << "color = " << this->color() << '\n';
  if (Player::write_database) {
    ostr << '\n'
      << "database\n"
      << "{\n"
      << this->db()
      << "}\n";
  } // if (Player::write_database)

  return ostr;
} // virtual ostream& Player::write(ostream& ostr) const

/**
 **
 ** output of the player
 **
 ** @param       ostr    output stream
 ** @param       player  player to be put in the stream
 **
 ** @return      output stream
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
ostream&
operator<<(ostream& ostr, const Player& player)
{
  player.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, const Player& player)

/**
 ** write the player in the stream in short form (name and type)
 **
 ** @param       ostr    output stream
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
void
Player::write_short(ostream& ostr) const
{
  ostr << this->name_value() << " -- "
    << this->type();
  return ;
} // void Player::write_short(ostream& ostr) const

/**
 **
 ** read the config of the player
 **
 ** @param	config		configuration to read
 ** @param	istr		input stream
 **
 ** @return	whether the configuration was valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
bool
Player::read(Config const& config, istream& istr)
{
  if (config.separator) {
#ifndef OUTDATED
    // 0.7.3
    if ((config.name == "name")
        ||(config.name == "Name"))
#else
      if (config.name == "name")
#endif
      {
        this->set_name(config.value);
        return true;
      } else if (config.name == "voice") {
        this->set_voice(config.value);
        return true;
      } else if (config.name == "color") {
        this->set_color(config.value);
        return true;
#ifndef OUTDATED
        // 0.7.3
      } else if ((config.name == "type")
                 ||(config.name == "Type")) {
#else
      } else if (config.name == "Type") {
#endif
        if (this->type() != UNSET) {
          cerr << "Type already set, ignoring further one" << endl;
          return true;
        }
        if (config.value == ::name(HUMAN)) {
          this->set_type(HUMAN);
        } else if (config.value == ::name(AI)) {
          this->set_type(AI);
        } else if (config.value == ::name(AI_DUMMY)) {
          this->set_type(AI_DUMMY);
        } else if (config.value == ::name(AI_RANDOM)) {
          this->set_type(AI_RANDOM);
        } else if (config.value == ::name(NETWORK)) {
#ifdef WORKAROUND
          this->set_type(AI);
#else
          DEBUG_ASSERTION(false,
                          "Player::read(config, istr):\n"
                          "  player type 'NETWORK' cannot be loaded");
#endif
        } else { // if (config.value == 'type')
          DEBUG_ASSERTION(false,
                          "Player::read(config, istr):\n"
                          "  player type '" << config.value << "' not known");
        }
        return true;
        // I know the type, so I can create the player
      }
  } else { // if (config.separator)
#ifndef OUTDATED
    // 0.7.3
    if ( (config.name == "database")
        || (config.name == "Database") )
#else
      if (config.name == "Database")
#endif
      {
        this->db_->read(istr);
        return true;
      }
  } // if (config.separator)

  return false;
} // virtual bool Player::read(Config config, istream& istr)

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      game the player is playing
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Game&
Player::game()
{
  DEBUG_ASSERTION(this->game_,
                  "Player::game(): 'game_' is a NULL-pointer\n"
                  "  this = " << this);

  return *(this->game_);
} // Game& Player::game()

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      game the player is playing
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Game const&
Player::game() const
{
  DEBUG_ASSERTION(this->game_ != NULL,
                  "Player::game(): 'game_' is a NULL-pointer\n"
                  "  this = " << this);

  return *(this->game_);
} // Game const& Player::game() const

/**
 **
 ** set the game
 **
 ** @param       game    new game
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.6.7
 **
 **/
void
Player::set_game(Game& game)
{
  this->game_ = &game;

  for (TrickPile::iterator t = this->trickpile_.begin();
       t != this->trickpile_.end();
       ++t)
    *t = &game.trick((*t)->no());

  return ;
} // void Player::set_game(Game& game)

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the number of the player in the game, UINT_MAX if not in a game
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
unsigned
Player::no() const
{
  if (this->game_ == NULL)
    return UINT_MAX;
  return this->game_->no(*this);
} // unsigned Player::no() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      name of the player
 **              if it is empty, take a default
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **/
string const
Player::name() const
{
  if (this->name_.empty())
    return Player::default_name(this->type(), ::party.no(*this));

  return this->name_;
} // string const Player::name() const

/**
 ** sets the name to the default one
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Player::set_default_name()
{
  this->set_name(Player::default_name(this->type(), ::party.no(*this)));
} // void Player::set_default_name()

/**
 ** sets the name
 **
 ** @param       name    new name
 **
 ** @return      name
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
void
Player::set_name(string const& name)
{
  this->name_ = name;

  if (::ui != NULL) {
    // check that this is no virtual player
    if (   (   !this->game_
            && !(::game_status & GAMESTATUS::GAME)
            && (::party.no(*this) != UINT_MAX) )
        || (   this->game_
            && !this->game_->isvirtual()) )
      // || (this->game_->no(*this) != UINT_MAX) )
      ::ui->name_changed(*this);
  }

  return ;
} // void Player::set_name(string name)

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      value of the name of the player
 **
 ** @author      Diether Knof
 **
 ** @version     0.5.3
 **
 **/
string const&
Player::name_value() const
{
  return this->name_;
} // string const& Player::name_value() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      voice of the player
 **              if it is empty, take a default
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
string const
Player::voice() const
{
  if (this->voice_.empty())
    return Player::default_voice(this->type(), ::party.no(*this), this->name());

  return this->voice_;
} // string const Player::voice() const

/**
 ** sets the voice to the default one
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Player::set_default_voice()
{
  this->set_voice(Player::default_voice(this->type(), ::party.no(*this),
                                        this->name()));
  return ;
} // void Player::set_default_voice()

/**
 ** sets the voice
 **
 ** @param       voice    new voice
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
void
Player::set_voice(string const& voice)
{
  this->voice_ = voice;
  ::ui->voice_changed(*this);

  return ;
} // void Player::set_voice(string voice)

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      value of the name of the player
 **
 ** @author      Diether Knof
 **
 ** @version     0.5.3
 **
 **/
string const&
Player::voice_value() const
{
  return this->voice_;
} // string const& Player::voice_value() const

/**
 ** -> result
 **
 ** @param     type       the type
 ** @param     playerno   the number of the player
 **
 ** @return    default name of the type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Player::default_name(Type const& type, unsigned const playerno)
{
  switch(type) {
  case HUMAN:
    return ::setting(Setting::NAME);
  default:
    switch (playerno) {
    case 0:
      return "Mara";
    case 1:
      return "Gerd";
    case 2:
      return "Erika";
    case 3:
      return "Sven";
    case 4:
      return "Tina";
    case 5:
      return "Alex";
    case UINT_MAX:
      return "";
    } // switch(playerno)
  } // switch(this->type())

  return ::name(type);
} // string Player::default_name(Type type, unsigned playerno)

/**
 ** -> result
 **
 ** @param     type       the type
 ** @param     playerno   the number of the player
 ** @param     name       the name of the player
 **
 ** @return    default voice of the type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **
 ** @todo      take male/female according to the name
 **/
string
Player::default_voice(Type const& type,
                      unsigned const playerno,
                      string const& name)
{
  switch(type) {
  case HUMAN:
#ifdef POSTPONED
    // todo: add check, that the voice exists
    return ::setting(Setting::NAME);
#endif
  default:
    switch (playerno) {
    case 0:
      return ("female/mbrola");
    case 1:
      return ("male/mbrola");
    case 2:
      return ("female/mbrola");
    case 3:
      return ("male/mbrola");
    case 4:
      return ("female/mbrola");
    case 5:
      return ("man/mbrola");
    case UINT_MAX:
      return "";
    } // switch(playerno)
  } // switch(this->type())

  return ::name(type);
} // string Player::default_voice(Type type, unsigned playerno, string name)

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the database
 **
 ** @author      Borg Enders
 **
 ** @version     0.6.2
 **
 **/
PlayersDb const&
Player::db() const
{
  DEBUG_ASSERTION(this->db_,
                  "Playe::db():\n"
                  "  'this->db_' == NULL");

  return *(this->db_);
} // PlayersDb const& Player::db() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the database
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.6
 **
 **/
PlayersDb&
Player::db()
{
  DEBUG_ASSERTION(this->db_,
                  "Playe::db():\n"
                  "  'this->db_' == NULL");

  return *(this->db_);
} // PlayersDb& Player::db()

/**
 ** -> result
 **
 ** @param     game_type   gametype to check
 **
 ** @return    whether the gametype is a duty solo for this player
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.7
 **/
bool
Player::is_remaining_duty_solo(GameType const game_type) const
{
  if (!GAMETYPE::is_real_solo(game_type))
    return false;
  if (this->remaining_duty_free_soli())
    return true;
  if (GAMETYPE::is_picture_solo(game_type)
      && this->remaining_duty_picture_soli())
    return true;
  if (GAMETYPE::is_color_solo(game_type)
      && this->remaining_duty_color_soli())
    return true;
  return false;
} // bool Player::is_remaining_duty_solo(GameType game_type) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the remaining duty solo games to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.7
 **/
unsigned
Player::remaining_duty_soli() const
{
  DEBUG_ASSERTION(this->game_,
                  "Player::remaining_duty_soli()\n"
                  "not in game");
  return this->game().party().remaining_duty_soli(*this);
} // unsigned Player::remaining_duty_soli() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the remaining duty solo games to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.7
 **
 **/
unsigned
Player::remaining_duty_color_soli() const
{
  DEBUG_ASSERTION(this->game_,
                  "Player::remaining_duty_color_soli()\n"
                  "not in game");
  return this->game().party().remaining_duty_color_soli(*this);
} // unsigned Player::remaining_duty_color_soli() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the remaining duty solo games to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.7
 **
 **/
unsigned
Player::remaining_duty_picture_soli() const
{
  DEBUG_ASSERTION(this->game_,
                  "Player::remaining_duty_picture_soli()\n"
                  "not in game");
  return this->game().party().remaining_duty_picture_soli(*this);
} // unsigned Player::remaining_duty_picture_soli() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the remaining duty solo games to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.7
 **
 **/
unsigned
Player::remaining_duty_free_soli() const
{
  DEBUG_ASSERTION(this->game_,
                  "Player::remaining_duty_free_soli()\n"
                  "not in game");
  return this->game().party().remaining_duty_free_soli(*this);
} // unsigned Player::remaining_duty_free_soli() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      hand
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Hand const&
Player::hand() const
{
  return this->hand_;
} // Hand const& Player::hand() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      hand
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Hand&
Player::hand()
{
  return this->hand_;
} // Hand& Player::hand()


/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      sorted hand
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.4
 **
 **/
SortedHand const&
Player::sorted_hand() const
{
  return this->sorted_hand_;
} // SortedHand const& Player::sorted_hand() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      hand
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.4
 **
 **/
SortedHand&
Player::sorted_hand()
{
  return this->sorted_hand_;
} // SortedHand& Player::hand()


/**
 ** sort the hand
 ** only if the player is in a real game
 **
 ** @param       -
 **
 ** @return      sorted hand
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.5.4
 **/
Hand const&
Player::hand_sort()
{
  if (::setting(Setting::CARDS_ORDER).sorted()
      && (this->game_ != NULL)
      && !(this->game().isvirtual())
      && (this->game().no(*this) != UINT_MAX)) {
    bool const changed = this->sorted_hand().sort();
    if (changed)
      ::ui->hand_changed(*this);
  } // if (::setting(Setting::CARDS_ORDER).sorted())

  return this->hand();
} // Hand const& Player::hand_sort()

/**
 **
 ** mixes the hand
 **
 ** @param       -
 **
 ** @return      mixed hand
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.5.4
 **
 **/
Hand const&
Player::hand_mix()
{
  if (!::setting(Setting::CARDS_ORDER).sorted()) {
    this->sorted_hand().mix();
    if ((this->game_ != NULL)
        && (!this->game().isvirtual()))
      ::ui->hand_changed(*this);
  } // if (!::setting(Setting::CARDS_ORDER).sorted())

  return this->hand();
} // Hand const& Player::hand_mix()


/**
 **
 ** set the hand
 **
 ** @param       hand    new hand
 **
 ** @return      player with hand h
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.6.7
 **
 **/
Hand const&
Player::set_hand(Hand const& hand)
{
  this->hand_ = hand;
  this->hand_.set_player(*this);

  if (   this->game_
      && !this->game().isvirtual()) {
    // In virtual games the hand is not shown, so it does not have to be sorted.
    if (::setting(Setting::CARDS_ORDER).sorted())
      this->hand_sort();
    else
      this->hand_mix();
  }

  if (::game_status == GAMESTATUS::GAME_INIT)
    this->set_team(  (this->hand().numberofclubqueens() > 0)
                   ? TEAM::RE
                   : this->hand().contains(Card::UNKNOWN)
                   ? TEAM::UNKNOWN
                   : TEAM::CONTRA );

  return this->hand();
} // Hand const& Player::set_hand(Hand const& hand)


/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      whether the player is re
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
bool
Player::isre() const
{
  return (this->team() == TEAM::RE);
} // bool Player::isre() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      whether the player has swines
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.5
 **
 **/
bool
Player::has_swines() const
{
  if (this->game().swines_announced()
      && (this == this->game().swines_owner())) {
    if ((::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
        && (*this == this->game().soloplayer()))
      return true;
    // search a swine in all cards of the hand
    for (unsigned c = 0; c < hand().cardsnumber_all(); c++)
      if (hand().card_all(c).isswine())
        return true;
  } // if (this->game().swines_announced())

  return false;
} // bool Player::has_swines() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      whether the player has hyperswines
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.5
 **
 **/
bool
Player::has_hyperswines() const
{
  if (this->game().hyperswines_announced()
      && (this == this->game().hyperswines_owner())) {
    // search a hyperswine in all cards of the hand
    for (unsigned c = 0; c < hand().cardsnumber_all(); c++)
      if (hand().card_all(c).ishyperswine())
        return true;
  } // if (this->game().hyperswines_announced())

  return false;
} // bool Player::has_hyperswines() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the reservation of the player
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
Reservation const&
Player::reservation() const
{
  return this->reservation_;
} // Reservation const& Player::reservation() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the reservation of the player
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
Reservation&
Player::reservation()
{
  return this->reservation_;
} // Reservation& Player::reservation()

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the trickpile of the player
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.4
 **
 **/
TrickPile const&
Player::trickpile() const
{
  return this->trickpile_;
} // TrickPile const& Player::trickpile() const

/**
 ** update the teaminfo
 **
 ** @param       -
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.4
 **/
void
Player::teaminfo_update()
{ }

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      announcment of this player (taken from 'game')
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Announcement const&
Player::announcement() const
{
  return this->game().announcement_of_player(*this).announcement;
} // Announcement const& Player::announcement() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      announcement of this player (taken from 'game')
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Announcement
Player::next_announcement() const
{
  if (!::is_real(this->team()))
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  Announcement const& last_announcement
    = (this->game().rule()(Rule::KNOCKING)
       ? this->game().announcement_of_player(*this).announcement
       : this->game().announcement_of_team(this->team()).announcement);

  Announcement next_announcement = ANNOUNCEMENT::NOANNOUNCEMENT;

  switch (last_announcement) {
  case ANNOUNCEMENT::NOANNOUNCEMENT:
    next_announcement = ANNOUNCEMENT::NO120;
    break;
  case ANNOUNCEMENT::NO120:
  case ANNOUNCEMENT::NO90:
  case ANNOUNCEMENT::NO60:
  case ANNOUNCEMENT::NO30:
    next_announcement = static_cast<Announcement>(last_announcement + 1);
    break;
  case ANNOUNCEMENT::NO0:
  default:
    next_announcement = ANNOUNCEMENT::NOANNOUNCEMENT;
    break;
  } // switch(last_announcement)

  return this->game().announcement_valid(next_announcement, *this);
} // Announcement Player::next_announcement() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the number of tricks the player has (taken from 'game')
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.0
 **
 **/
unsigned
Player::number_of_tricks() const
{
  DEBUG_ASSERTION((this->game_ != NULL),
                  "Player::number_of_tricks():\n"
                  "  'this->game' == NULL");

  return this->game().numberoftricks_of_player(*this);
} // unsigned Player::number_of_tricks() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the points of the tricks the player has (taken from 'game')
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.0
 **
 **/
unsigned
Player::points_in_trickpile() const
{
  DEBUG_ASSERTION((this->game_ != NULL),
                  "Player::points_in_trickpile():\n"
                  "  'this->game' == NULL");

  return this->game().points_of_player(*this);
} // unsigned Player::points_in_trickpile() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      the number of cards the player still has to play in this game
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.9
 **
 **/
unsigned
Player::cards_to_play() const
{
  DEBUG_ASSERTION((this->game_ != NULL),
                  "Player::cards_to_play():\n"
                  "  'this->game' == NULL");

  if (this->game().tricks().empty())
    return this->game().trickno();

  if (this->game().trick_current().cardno_of_player(*this)
      < this->game().trick_current().actcardno())
    return (this->game().trickno() - this->game().real_trick_current_no() - 1);
  else
    return (this->game().trickno() - this->game().real_trick_current_no());
} // unsigned Player::cards_to_play() const

/**
 ** the rule has changed
 **
 ** @param       type        rule type to have changed
 ** @param       old_value   old value of the rule
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.8
 **/
void
Player::rule_changed(int const type, void const* const old_value)
{ return ; }

/**
 ** the game 'game' is opened
 **
 ** @param       game    opened game
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
void
Player::game_open(Game& game)
{
  this->game_ = &game;
  this->hand_ = Hand();
  this->team_ = TEAM::UNKNOWN;
  this->reservation_ = Reservation();

  return ;
} // void Player::game_open(Game& game)

/**
 ** the game 'game' is closed
 **
 ** @param       game    closed game
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.13
 **/
void
Player::game_close(Game const& game)
{
  // check whether the player is playing in a game
  if (this->game_ == NULL)
    return;

  DEBUG_ASSERTION((this->game_ == &game)
                  || (::game_status == GAMESTATUS::PARTY_LOADED),
                  "Player::game_close(game):\n"
                  "  wrong game is closed: " << this->game_ << " instead of " << &game);

  this->trickpile_.clear();
  this->game_ = NULL;

  return ;
} // void Player::game_close(Game const& game)

/**
 **
 ** the player can make an announcement
 **
 ** @param       -
 **
 ** @return      announcement of the player
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
Announcement
Player::announcement_request() const
{
  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement Player::announcement_request() const

/**
 **
 ** the given player has made the given announcement
 **
 ** @param       announcement    the announcement
 ** @param       player          the player, who has made the announcement
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
void
Player::announcement_made(Announcement const& announcement,
                          Player const& player)
{ }

/**
 ** 'player' shifts 'cardno' cards
 **
 ** @param      player  the player who shifts the cards
 ** @param      cardno  the number of cards that are shifted
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **/
void
Player::poverty_shift(Player const& player, unsigned const cardno)
{  }

/**
 ** 'player' denied to take the shifted cards
 **
 ** @param      player  the player who has denied to take the cards
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **/
void
Player::poverty_take_denied(Player const& player)
{  }

/**
 ** all players have denied to take the cards
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **/
void
Player::poverty_take_denied_by_all()
{ }

/**
 ** 'player' accepts to take the shifted cards
 ** and returns 'cardno' cards with 'trumpno' trumps
 **
 ** @param      player  the player who has accepted to take the cards
 ** @param      cardno  number of cards that are given back
 ** @param      trumpno number of trumps of the cards
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **/
void
Player::poverty_take_accepted(Player const& player,
                              unsigned const cardno, unsigned const trumpno)
{  }

/**
 **
 ** returns which cards the player shifts
 **
 ** @param      -
 **
 ** @return     the cards that are to be shifted
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **
 **/
HandCards
Player::poverty_shift()
{ 
  cerr << "call of Player::poverty shift()"
    << endl;

  return HandCards();
} // HandCards Player::poverty_shift(Player const& player)

/**
 **
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param      cardno  the number of shifted cards
 **
 ** @return     whether to accept the cards
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **
 **/
bool
Player::poverty_take_accept(unsigned const cardno)
{ 
  cerr << "call of Player::poverty_take_accept(cardno)"
    << endl;

  return false;
} // void Player::poverty_take_accept(unsigned const cardno)

/**
 **
 ** changes the cards from the poverty-player
 **
 ** @param      cards   the cards that are given to the player
 **
 ** @return     the cards that are returned to the poverty-player
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **
 **/
HandCards
Player::poverty_cards_change(HandCards const& cards)
{ 
  cerr << "call of Player::poverty_cards_change(cards)"
    << endl;

  return HandCards();
} // HandCards Player::poverty_cards_change(HandCards const& cards)

/**
 ** gets the cards from the partner and add them to the hand
 **
 ** @param	cards	the cards that are given to the player
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5
 **/
void
Player::poverty_cards_get_back(HandCards const& cards)
{ 
  this->sorted_hand().add(cards);
  this->hand_sort();

  return ;
} // void Player::poverty_cards_get_back(HandCards const& cards)

/**
 ** the marriage partner has found a bride
 ** if the bride is the bridegroom, the bridegroom must play a solo
 **
 ** @param	bridegroom	the player with the marriage
 ** @param	bride		the bride
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
void
Player::marriage(Player const& bridegroom, Player const& bride)
{ }

/**
 ** genscher: -> result
 **
 ** @param	-
 **
 ** @return	pointer to the new teammate,
 **		NULL if the genscher is not to be announced
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
Player const*
Player::genscher_partner()
{ 
  cerr << "call of Player::genscher_partner()"
    << endl;

  return NULL;
} // Player const* Player::genscher_partner()

/**
 **
 ** genscher: Information of the new team
 **
 ** @param	player	genscher player
 ** @param	partner	partner of the gensher player
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
void
Player::genscher(Player const& player,
                 Player const& partner)
{ }

/**
 **
 ** a new game has started - init the player:
 ** empty the hand,
 ** set the teaminfo to the teaminfo of the game
 **
 ** @param       game    the game
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
void
Player::new_game(Game& game)
{ 
  this->game_ = &game;
  this->hand_ = Hand();

  return ;
} // void Player::new_game()

/**
 ** the player makes his reservation
 **
 ** @param       duty_solo	whether the player must play a duty solo
 **
 ** @return      reservation of the player
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.2
 **
 ** @todo	duty_solo documentation
 **/
Reservation const&
Player::reservation_get(bool const duty_solo) 
{ 
  DEBUG_ASSERTION(false,
                  "Player::reservation_get():\n"
                  "  call of this function");

  return (this->reservation() = this->reservation_get_default());
} // Reservation Player::reservation_get(bool duty_solo = false)

/**
 **
 ** -> result
 ** that is swines and hyperswines, if the player has them,
 ** poverty (nines, kings, trump), marriage
 **
 ** @param	-
 **
 ** @return	default reservation of the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.0
 **
 **/
Reservation
Player::reservation_get_default() const
{ 
  Reservation reservation;

  if (this->hand().numberofclubqueens()
      == this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
    reservation.game_type = GAMETYPE::MARRIAGE;
    if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_FOREIGN))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_FOREIGN;
    else if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_COLOR))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_COLOR;
    else if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_TRUMP))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_TRUMP;
    else if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_CLUB))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_CLUB;
    else if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_SPADE))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_SPADE;
    else if (this->game().rule()(MARRIAGE_SELECTOR::FIRST_HEART))
      reservation.marriage_selector = MARRIAGE_SELECTOR::FIRST_HEART;
    else
      DEBUG_ASSERTION(false,
                      "Player::reservation_get_default():\n"
                      "  no marriage selector valid.\n"
                      "  Ruleset:"
                      << this->game().rule());
  } // if (possible marriage)

  reservation.swines
    = (   this->game().rule()(Rule::SWINES)
       && this->game().rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN)
       && this->game().swines_announcement_valid(*this));

  reservation.hyperswines
    = (   this->game().rule()(Rule::HYPERSWINES)
       && this->game().rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN)
       && this->game().hyperswines_announcement_valid(*this));


  if (this->hand().has_poverty()) {
    reservation.game_type = GAMETYPE::POVERTY;
    if (this->hand().numberoftrumps()
        > this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS))
      reservation.swines = false;
  }

  if (this->game().rule()(Rule::THROW_WHEN_FOX_HIGHEST_TRUMP)) {
    unsigned c;
    for (c = 0; c < this->hand().cardsnumber(); ++c)
      // check with diamond ten because of swines!
      if (Card(Card::DIAMOND, Card::TEN).less(this->hand().card(c))
          && !this->hand().card(c).istrumpace())
        break;

    if (c == this->hand().cardsnumber())
      reservation.game_type = GAMETYPE::FOX_HIGHEST_TRUMP;
  } // test fox highest trump

  if (this->game().rule()(Rule::THROW_WITH_RICHNESS)
      && (this->hand().points()
          >= this->game().rule()(Rule::MIN_RICHNESS_FOR_THROWING)))
    reservation.game_type = GAMETYPE::THROWN_RICHNESS;

  if (this->game().rule()(Rule::THROW_WITH_NINES_AND_KINGS)
      && (this->hand().numberofnines() + this->hand().numberofkings()
          >= this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_NINES_AND_KINGS)))
    reservation.game_type = GAMETYPE::THROWN_NINES_AND_KINGS;

  if (this->game().rule()(Rule::THROW_WITH_KINGS)
      && (this->hand().numberofkings()
          >= this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_KINGS)))
    reservation.game_type = GAMETYPE::THROWN_KINGS;

  if (this->game().rule()(Rule::THROW_WITH_NINES)
      && (this->hand().numberofnines()
          >= this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_NINES)))
    reservation.game_type = GAMETYPE::THROWN_NINES;

  return reservation;
} // Reservation Player::reservation_get_default() const

/**
 **
 ** the game is started
 **
 ** @param       -
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.6.6
 **
 **/
void
Player::game_start() 
{ 
  // last time for swines announcements

  { // update the team
    if (::is_real(this->game().teaminfo(*this))) {
      // all solo games:
      this->set_team(this->game().teaminfo(*this));
    } else if (this->game().type() == GAMETYPE::MARRIAGE) {
      // undetermined marriage
      this->set_team(TEAM::MAYBE_CONTRA);

    } else {
      // a normal game
      this->set_team((this->hand().numberofclubqueens() > 0)
                     ? TEAM::RE
                     : this->hand().contains(Card::UNKNOWN)
                     ? TEAM::UNKNOWN
                     : TEAM::CONTRA);
    }
  } // update the team

  return ;
} // void Player::game_start()

/**
 **
 ** the trick is opened
 **
 ** @param      trick	opened trick
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.9
 **
 **/
void
Player::trick_open(Trick const& trick)
{  }

/**
 **
 ** the player plays a card
 **
 ** @param       -
 **
 ** @return      card of the player
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
HandCard
Player::card_get()
{ 
  DEBUG_ASSERTION(false,
                  "Player::card_get():\n"
                  "  call of this function");

  return this->hand().card(0);
} // HandCard Player::card_get()

/**
 **
 ** 'player' has played the card 'card'
 **
 ** @param       card    played card
 **
 ** @return      -
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **
 **/
void
Player::card_played(HandCard const& card)
{ }

/**
 **
 ** the trick is full
 **
 ** @param      trick   full trick
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.4.5
 **
 **/
void
Player::trick_full(Trick const& trick)
{  }

/**
 **
 ** the trick is taken
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.4
 **
 **/
void
Player::trick_taken()
{  }

/**
 **
 ** move the trick in the trickpile
 **
 ** @param      trick   trick
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.7
 **
 **/
void
Player::move_in_trickpile(Trick const& trick)
{ 
  DEBUG_ASSERTION((&trick.game() == this->game_),
                  "Player::move_in_trickpile(trick)"
                  "  trick.game != this->game");

  this->trickpile_.push_back(&trick);

  return ;
} // void Player::move_in_trickpile(Trick const& trick)

/**
 ** check a swines announcement at the game start
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.6
 **/
void
Player::check_swines_announcement_at_game_start()
{  }

/**
 **
 ** 'player' has announce swines
 **
 ** @param      player	player that has announced the swines
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.6
 **
 **/
void
Player::swines_announced(Player const& player)
{  }

/**
 **
 ** 'player' has announce hyperswines
 **
 ** @param      player	player that has announced the hyperswines
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.6.6
 **
 **/
void
Player::hyperswines_announced(Player const& player)
{  }

/**
 ** evaluation of game for playersDb
 **
 ** @param      game_summary	game summary
 **
 ** @return     -
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.7.2
 **/
void
Player::evaluate(GameSummary const& game_summary)
{
  this->db().evaluate(*this, game_summary);

  return ;
} // void Player::evaluate(GameSummary game_summary)

/**
 ** -> result
 **
 ** @param       type    the type
 **
 ** @return      name of the type
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.2
 **/
string
name(Player::Type const& type)
{
  switch(type) {
  case Player::UNSET:
    return "player type unset";
  case Player::HUMAN:
    return "human";
  case Player::AI:
    return "ai";
  case Player::AI_DUMMY:
    return "dummy ai";
  case Player::AI_RANDOM:
    return "random ai";
  case Player::NETWORK:
    return "network";
  } // switch(type)

  return "";
} // string name(Player::Type type)

/**
 ** output of the name of 'type'
 **
 ** @param       ostr    output stream
 ** @param       type    player type
 **
 ** @return      output stream
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.2
 **/
ostream&
operator<<(ostream& ostr, Player::Type const& type)
{
  ostr << name(type);

  return ostr;
} // ostream operator<<(ostream& ostr, Player::Type type);
