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

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#include "sender.h"

#include "../../misc/setting.h"
#include "../../misc/lounge.h"
#include "../../utils/string.h"

namespace Network {
  namespace DokoLounge {

#if 0
  \item "<<hilfe>>" + MeinName + "<</hilfe>>"
  \item "<<hilfechange>>" + s + "<</hilfechange>>"
  \item "<<bloglesen>>" + MeinName + "<</bloglesen>>"
  \item "<<blogsenden>>" + t + "<</blogsenden>>"
  \item "<<ganzepinnwand>>" + sendstr + "<</ganzepinnwand>>"
  \item "<<PinnwandRefresh>>" + MeinName + "<</PinnwandRefresh>>"
  \item "<<PinnwandSenden>>" + sendstr +  "<</PinnwandSenden>>"
  \item "<<GibRangliste>><<Name>>" + MeinName + "<</Name>><<Spiele>>" + Format(LetzteRangspiele) + "<</Spiele>><</GibRangliste>>"
  \item "<<GibRangliste2>><<Name>>" + MeinName + "<</Name>><<Spiele>>" + s + "<</Spiele>><</GibRangliste2>>"
  \item "<<angstgegner>><<name>>" + MeinName + "<</name>><<gegner>>" + g + "<</gegner>><</angstgegner>>"
  \item "<<neuerTisch>><<Name>>" + MeinName + "<</Name>><<TischArt>>" + helper + "<</TischArt>><</neuerTisch>>"
  <<maillesen>><<name>>Name<</name>><</maillesen>>

  <<offline>>
#endif

   /**
    ** create an account
    ** example: <<Anmelden>>Tux<</Anmelden>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::create_account()
     {
       if (::setting(Setting::DOKOLOUNGE_NAME) == "")
         ::setting.set(Setting::DOKOLOUNGE_NAME,
                             ::setting(Setting::NAME));
       if (::setting(Setting::DOKOLOUNGE_PASSWORD) == "")
         ::setting.set(Setting::DOKOLOUNGE_PASSWORD,
                             "Standardpasswort");
       this->send_command("neuesKonto",
                          this->command("Name", ::setting(Setting::DOKOLOUNGE_NAME))
                          + this->command("passwd", ::setting(Setting::DOKOLOUNGE_PASSWORD)));

       return ;
     } // void Interpreter::Sender::create_account()

   /**
    ** login
    ** example: <<Anmelden>>Tux<</Anmelden>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::login()
     {
       this->send_command("anmelden",
                          this->command("Name",
                                        ::setting(Setting::DOKOLOUNGE_NAME)
                          + "|"
                          + ::setting(Setting::DOKOLOUNGE_PASSWORD))
                          + this->command("MacAdresse", "")
                          //+ this->command("AppName", "FreeDoko " + static_cast<string>(*::version))
                          //+ this->command("version", "freedoko")
                          + this->command("version", Interpreter::version)
                         );
#if 0
          this->send(this->command("fuxbauchat",
                             "Diether: /newpassword Test1234"));
#endif

       return ;
     } // void Interpreter::Sender::login()

   /**
    ** logout
    ** example: <<Abmelden>>Tux<</Abmelden>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::logout()
     {
       this->send_name_command("Abmelden");
       ::lounge->logged_out();

       return ;
     } // void Interpreter::Sender::logout()

   /**
    ** refresh
    ** example: <<Refresh>>Tux<</Refresh>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::refresh()
     {
       this->send_name_command("Refresh");

       return ;
     } // void Interpreter::Sender::refresh()

   /**
    ** refresh all players (at the table)
    ** example: <<Refresh>>Tux<</Refresh>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::refresh_all_players()
     {
       if (::lounge->location() == Lounge::LOUNGE) {
         this->refresh();
         return;
       }
#ifdef POSTPONED
       Lounge::Table const& table = ::lounge->table();
       for (auto p : table.players) {
         this->send_command("Refresh", *p);
       }
#else
         this->refresh();
#endif

       return ;
     } // void Interpreter::Sender::refresh_all_players()

   /**
    ** send the icon of the player
    ** example: <<MeinBild>><<Name>>Tux<</Name>><<Bild>>tux.jpg<</Bild>><</MeinBild>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **
    ** @todo      take the icon from the settings
    ** @todo      call this function if the setting has changed
    **/
   void
     Interpreter::Sender::send_icon()
     {
       this->send_command("MeinBild",
                          this->command("Name", this->account())
                          + this->command("Bild", "tux.jpg"));
       return ;
     } // void Interpreter::Sender::send_icon()

   /**
    ** Klingel betätigen
    ** example: <<klingel>><</klingel>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::klingel()
     {
       this->send_simple_command("klingel");

       return ;
     } // void Interpreter::Sender::klingel()

   /**
    ** Hilfe aufrufen
    ** example: <<hilfe>>Tux<</hilfe>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::hilfe()
     {
       this->send_name_command("hilfe");

       return ;
     } // void Interpreter::Sender::hilfe()

   /**
    ** Blog aufrufen
    ** example: <<bloglesen>>Tux<</bloglesen>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::blog()
     {
       this->send_name_command("bloglesen");

       return ;
     } // void Interpreter::Sender::blog()

   /**
    ** Pinnwand aufrufen
    ** example: <<bloglesen>>Tux<</bloglesen>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::pinnwand()
     {
       this->send_name_command("PinnwandRefresh");

       return ;
     } // void Interpreter::Sender::pinnwand()

   /**
    ** eigene Pinnwand aufrufen
    ** example: <<maillesen>><<name>>Tux<</name>><</maillesen>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::messages()
     {
       this->send_command("maillesen",
                          this->name_command("name"));

       return ;
     } // void Interpreter::Sender::messages()

   /**
    ** Einen Tisch betreten
    ** example: <<TischBeitreten>>Tux(2)<</TischBeitreten>>
    **
    ** @param     table   Tischnummer
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::enter_table(int const table)
     {
       CLOG << "Tisch beitreten" << endl;
       this->send_command("TischBeitreten",
                          this->account() + "(" + DK::Utils::String::to_string(table + 1) + ")"); 

       return ;
     } // void Interpreter::Sender::enter_table(int const table)

   /**
    ** Tisch verlassen
    ** example: <<maillesen>><<name>>Tux<</name>><</maillesen>>
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::leave_table()
     {
       //this->send_command("maillesen",
       //this->name_command("name"));

       return ;
     } // void Interpreter::Sender::leave_table()

   /**
    ** Einen Spieler beobachten
    ** example: <<zuschauen>><<gast>>Tux<</gast>><<hostspieler>>Diether<</hostspieler>><</zuschauen>>
    **
    ** @param     name   name of the player to follow
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.13
    **/
   void
     Interpreter::Sender::follow_player(string const& name)
     {
       if (name == "frei")
         return ;
       CLOG << "zuschauen" << endl;
       CLOG << this->account() << endl;
       CLOG << name << endl;
       this->send_command("zuschauen",
                          this->command("gast", this->account())
                          + this->command("hostspieler", name)
                          + this->command("mitspielen", "zuschauen")); // oder "mitspielen"

       return ;
     } // void Interpreter::Sender::follow_player(string name)

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
