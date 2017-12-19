/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Settings as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Settings, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Settings for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Settings
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

#include "filemenu.h"

#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <gtkmm/separatormenuitem.h>

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	signal_slot	the slot for the change signal
   **
   ** @return	-
   **/
  FileMenu::FileMenu(sigc::slot1<void, string> signal_slot) :
    Menu(),
    directories_(),
    signal_slot_(signal_slot)
  {
    this->realize();
    this->signal_show().connect(sigc::mem_fun(*this, &FileMenu::update));
    return ;
  } // FileMenu::FileMenu(sigc::slot1<void, string> signal_slot)

  /**
   ** constructor
   **
   ** @param	signal_slot	the slot for the change signal
   ** @param	directory	the directory to search from
   **
   ** @return	-
   **/
  FileMenu::FileMenu(sigc::slot1<void, string> signal_slot,
                     string const& directory) :
    Menu(),
    directories_(1, directory),
    signal_slot_(signal_slot)
  {
    this->realize();
    this->signal_show().connect(sigc::mem_fun(*this, &FileMenu::update));
    return ;
  } // FileMenu::FileMenu(sigc::slot1<void, string> signal_slot, string const& directory)

  /**
   ** constructor
   **
   ** @param	signal_slot	the slot for the change signal
   ** @param	directories	the directories to search from
   **
   ** @return	-
   **/
  FileMenu::FileMenu(sigc::slot1<void, string> signal_slot,
                     vector<string> const& directories) :
    Menu(),
    directories_(directories),
    signal_slot_(signal_slot)
  {
    this->realize();
    this->signal_show().connect(sigc::mem_fun(*this, &FileMenu::update));
    return ;
  } // FileMenu::FileMenu(sigc::slot1<void, string> signal_slot, vector<string> const& directories)

  /**
   ** destruktor
   **
   ** @param	-
   **
   ** @return	-
   **/
  FileMenu::~FileMenu()
  { }

  /**
   ** clear the directory list
   **
   ** @param	-
   **
   ** @return	-
   **/
  void
    FileMenu::clear_directories()
    {
      this->directories_.clear();

      return ;
    } // void FileMenu::clear_directories()

  /**
   ** sets the directory
   **
   ** @param	directory	new directory
   **
   ** @return	-
   **/
  void
    FileMenu::set_directory(string const& directory)
    {
      this->directories_.clear();
      this->add_directory(directory);

      return ;
    } // void FileMenu::set_directory(string const& directory)

  /**
   ** add the directory to the list
   **
   ** @param	directory	new directory
   **
   ** @return	-
   **/
  void
    FileMenu::add_directory(string const& directory)
    {
      this->directories_.push_back(directory);
      //this->update();

      return ;
    } // void FileMenu::add_directory(string const& directory)

  /**
   ** sets the directories
   **
   ** @param	directories	new directories
   **
   ** @return	-
   **/
  void
    FileMenu::set_directories(vector<string> const& directories)
    {
      this->directories_ = directories;

      return ;
    } // void FileMenu::set_directories(vector<string> const& directories)

  /**
   ** sets the signal slot
   **
   ** @param	signal_slot	the signal slot
   **
   ** @return	-
   **/
  void
    FileMenu::set_signal_slot(sigc::slot1<void, string> signal_slot)
    {
      this->signal_slot_ = signal_slot;

      return ;
    } // void FileMenu::set_signal_slot(sigc::slot1<void, string> signal_slot)

  /**
   ** shows the menu
   **
   ** @param	-
   **
   ** @return	-
   **/
  void
    FileMenu::show()
    {
      this->popup(0, 0);
      this->update();

      return ;
    } // void FileMenu::show()

  /**
   ** updates the menu (recreates it)
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @todo	transform the filename into utf8
   **/
  void
    FileMenu::update()
    {
      { // remove all childs
        for (auto& child : this->get_children())
          this->remove(*child);
      } // remove all childs

      this->show_all();

      auto const subdirname
        = static_cast<string*>(this->get_data("subdirname"));

      bool first_directory = true;
      for (auto const& directory : this->directories_) {
        if (!Glib::file_test(directory, Glib::FILE_TEST_EXISTS))
          continue;

        if (!first_directory)
          this->add(*(Gtk::manage(new Gtk::SeparatorMenuItem())));
        else
          first_directory = false;

        Glib::Dir dir(directory);

        for (auto const& filename : dir) {
#ifndef RELEASE
          if (filename == ".svn")
            continue;
#endif
          auto item = Gtk::manage(new Gtk::MenuItem(filename));

          if (this->accept(directory + "/" + filename)) {
            dynamic_cast<Gtk::Label*>(item->get_child())->set_text(this->item_label((subdirname ? *subdirname : ""), filename));
            if (subdirname)
              item->signal_activate().connect(sigc::bind<string>(this->signal_slot_, *subdirname + "/" + filename));
            else
              item->signal_activate().connect(sigc::bind<string>(this->signal_slot_, filename));

            this->add(*item);
          } else { // if !(this->accept(directory + "/" + filename))
            if (Glib::file_test(directory + "/" + filename,
                                Glib::FILE_TEST_IS_DIR)) {
#if 0
              FileMenu* subdir = Gtk::manage(new FileMenu(this->signal_slot_,
                                                          directory
                                                          + "/" + filename));
#endif
              auto subdir = Gtk::manage(this->create_new(directory
                                               + "/" + filename));
              if (subdirname)
                subdir->set_data("subdirname", new string(*subdirname
                                                          + "/" + filename));
              else
                subdir->set_data("subdirname", new string(filename));

              item->set_submenu(*subdir);
              this->add(*item);
              //item->show();

            } // if (Glib::file_test(directory + "/" + filename, Glib::FILE_TEST_IS_DIR))
          } // if !(this->accept(directory + "/" + filename))
        } // for (auto filename : dir)

        dir.close();
      } for (auto directory : this->directories_)

      if (this->get_children().size() == 0) {
        // There is no child, so add a stop image.
        auto stop
          = Gtk::manage(new Gtk::MenuItem(*Gtk::manage(new Gtk::Image(Gtk::Stock::STOP, Gtk::ICON_SIZE_MENU))));
        stop->set_sensitive(false);
        this->add(*stop);
      } // if (this->get_children().size() == 1)

      this->show_all();

      Gtk::Main::iteration(false);

      return ;
    } // void FileMenu::update()

  /**
   ** -> result
   **
   ** @param	directory	the directory
   **
   ** @return	a new file menu with the same settings
   **
   ** @todo     use smart pointer
   **/
  FileMenu*
    FileMenu::create_new(string const& directory) const
    {
      return (new FileMenu(this->signal_slot_, directory));
    } // virtual bool FileMenu::create_new(string directory) const

  /**
   ** -> result
   **
   ** @param	filename	filename to test
   **
   ** @return	whether the filename is accepted for the menu
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  bool
    FileMenu::accept(string const& filename) const
    {
      return Glib::file_test(filename, Glib::FILE_TEST_IS_REGULAR);
    } // virtual bool FileMenu::accept(string filename) const

  /**
   ** -> result
   **
   ** @param	subdirname	the name of the subdir
   ** @param	filename	the filename
   **
   ** @return	label of the menu item
   **/
  string
    FileMenu::item_label(string const& subdirname, string const& filename) const
    {
      return filename;
    } // virtual string FileMenu::item_label(string subdirname, string filename) const

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
