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

#include "receiver.h"
#include "sender.h"

#include "parser/parser.h"

#include "../server.h"

#include "../../party/party.h"
#include "../../game/gameplay_action.h"
#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../misc/lounge.h"
#include "../../utils/string.h"

#include <cstring>

namespace Network {
  namespace DokoLounge {
    /*
<<neuesKonto>>Der Name "FreeDoko" ist leider schon vergeben.
Bitte ein Passwort mit mindestens 4 Buchsteben/Zahlen wählen.<</neuesKonto>>

Mit falscher Version angemeldet
<<deinName>><</deinName>><<fuxbauchat>>Bitte lade dir von unserer Homepage (www.doko-lounge.de) eine aktuelle Version herunter. Mit dieser Version kannst du nicht mehr spielen.<</fuxbauchat>><<offline>><</offline>>


Anzahl der Spieler hat sich geändert
<<LobbySpieler>><<anz>>3<</anz>><<User1>>MacHolstein<</User1>><<UserIcon1>>face4.jpg<</UserIcon1>><<User2>>Diether<</User2>><<UserIcon2>>tux.jpg<</UserIcon2>><<User3>>FreeDoko<</User3>><<UserIcon3>>fux1.jpg<</UserIcon3>><</LobbySpieler>>

Spieler ist einem Tisch beigetreten
<<Tischart>><<Tischanz>>5<</Tischanz>><<1>>Sonderregeln<</1>><<2>>Lerntisch<</2>><<3>>Sonderregeln<</3>><<4>>DDV<</4>><<5>>Sonderregeln<</5>><</Tischart>>
< <<alleTische>>(TischSpieler01)Diether,frei,frei,frei||(TischSpieler02)frei,frei,frei,frei||(TischSpieler03)frei,frei,frei,frei||(TischSpieler04)frei,frei,frei,frei||(TischSpieler05)frei,frei,frei,frei||<</alleTische>><<Tischart>><<Tischanz>>5<</Tischanz>><<1>>Sonderregeln<</1>><<2>>Lerntisch<</2>><<3>>Sonderregeln<</3>><<4>>DDV<</4>><<5>>Sonderregeln<</5>><</Tischart>><<alleTische>>(TischSpieler01)Diether,frei,frei,frei||(TischSpieler02)frei,frei,frei,frei||(TischSpieler03)frei,frei,frei,frei||(TischSpieler04)frei,frei,frei,frei||(TischSpieler05)frei,frei,frei,frei||<</alleTische>>


zuschauen

unknown keyword: TischErlaubt -- 3
unknown keyword: alleSpielerAmTisch -- <<spieler1>>FreeDoko<</spieler1>><<spieler2>>frei<</spieler2>><<spieler3>>frei<</spieler3>><<spieler4>>frei<</spieler4>>
unknown keyword: NeuePunkte -- <<FreeDoko>>0<</FreeDoko>><<frei>>0<</frei>><<frei>>0<</frei>><<frei>>0<</frei>>
unknown keyword: KartenAufTisch -- 00-00-00-00-
<<gastverlassen>>Diether<</gastverlassen>>


unknown keyword: Karten -- 00-00-00-00-00-00-00-00-00-00-00-00-
unknown keyword: KartenAufTisch -- 00-00-00-00-
unknown keyword: ersteKarte -- 0
unknown keyword: SpielerIstDran -- frei
unknown keyword: NeuePunkte -- <<Diether>>6<</Diether>><<frei>>0<</frei>><<frei>>0<</frei>><<frei>>0<</frei>>
unknown keyword: zuschauer -- FreeDoko(0) schaut Diether zu
unknown keyword: Karten -- 35-14-42-17-47-32-20-26-34-01-09-28-
receiver.cpp#223  TischChat: Diether hat gegeben.
unknown keyword: Meldung -- Diether ist gesund.
unknown keyword: AnsageCount -- 12
unknown keyword: sonderregeln -- 
unknown keyword: regelinfo1 -- <<tischinfo>>Sonderregeln<</tischinfo>><<ohneSupersau>>1<</ohneSupersau>><<ersted>>1<</ersted>><<skir>>0<</skir>><<armut>>1<</armut>><<oNeunen>>0<</oNeunen>>
<<IchWillTischVerlassen>>Diether<</IchWillTischVerlassen>>

unknown keyword: deinIcon -- tux.jpg


*/

    /**
     ** constructor
     **
     ** @param	interpreter	corresponding interpreter
     **
     ** @return	-
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.12
     **/
    Interpreter::Receiver::Receiver(Interpreter& interpreter) :
      Connection::Interpreter::Receiver(interpreter),
      pending_line(),
      parser(new Parser(interpreter))
    { }

    /**
     ** destructor
     **
     ** @param	-
     **
     ** @return	-
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.1
     **/
    Interpreter::Receiver::~Receiver()
    { }

    /**
     ** -> result
     **
     ** @param		-
     **
     ** @return		the DokoLounge interpreter
     **
     ** @author		Diether Knof
     **
     ** @version	0.7.1
     **/
    Interpreter::Sender&
      Interpreter::Receiver::sender()
      {
        return dynamic_cast<Interpreter::Sender&>(this->interpreter().sender());
      } // Sender& Interpreter::Receiver::sender()

    /**
     ** a text has been received
     **
     ** @param     text   received text
     **
     ** @return    whether the text was accepted (or saved for later)
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    bool
      Interpreter::Receiver::received(string const& text)
      {
        if (text.empty())
          return false;
        //CLOG << text << endl;
        // add the text to the one already gotten
        this->pending_line += DK::Utils::String::latin1_to_utf8(text);

        this->get_next_command();
        return true;
      } // bool Interpreter::Receiver::received(string const& text)

    /**
     ** get the next command (from the pending line)
     **
     ** @param     -
     **
     ** @return    whether a new command was found
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Interpreter::Receiver::get_next_command()
      {
#ifdef OUTDATED
        if (this->parser->blocking()) {
          return false;
        }
#endif
        if (this->pending_line == "EOF") {
          this->pending_line = "";
          this->parser->receive_command("EOF", "");
        }
        string keyword;
        string entry;
        while (Parser::strip_next_part(this->pending_line, keyword, entry)) {
          //CLOG << keyword << endl;
          { // hack
            // <<alert>><</alert>>...<<alerttext>><</alerttext>>
            if (keyword == "alert") {
              this->pending_line = "<<alerttext>>" + this->pending_line;
            }
            if (keyword == "alerttext") {
              entry += "<</alerttext>>";
            }
          } // hack

#ifdef OUTDATED
          this->parser->interpret_command(keyword, entry);
          if (this->parser->blocking())
            break;
#endif
          this->parser->receive_command(keyword, entry);
          //CLOG << this->pending_line << endl;

        } // while (Parser::strip_next_part(this->pending_line, keyword, entry))
        return !keyword.empty();
      } // bool Interpreter::Receiver::get_next_command()

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
