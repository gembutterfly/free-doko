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
#ifdef USE_NETWORK

#include "ui.h"

#include "network.h"
#include "network_log.h"

#include "party_settings.h"
#include "rules.h"

#include "../../network/listener.h"
#include "../../network/connection.h"


namespace UI_GTKMM_NS {

/**
 ** the listener 'listener' was opened
 **
 ** @param     listener   new listener
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_listener_new(::Network::Listener const& listener)
  {
    if (this->network)
      this->network->network_listener_new(listener);
    return ;
  }

/**
 ** the listener 'listener' was deleted
 **
 ** @param     listener   deleted listener
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_listener_deleted(::Network::Listener const& listener)
  {
    if (this->network)
      this->network->network_listener_deleted(listener);
    return ;
  }

/**
 ** the connection 'connection' was opened
 **
 ** @param     connection   new connection
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_connection_new(::Network::Connection const& connection)
  {
    if (this->network)
      this->network->network_connection_new(connection);
    if (this->party_settings)
      this->party_settings->sensitivity_update();
    return ;
  }

/**
 ** the connection 'connection' was changed
 **
 ** @param     connection   connection that was changed
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_connection_changed(::Network::Connection const& connection)
  {
    if (this->network)
      this->network->network_connection_changed(connection);
    if (this->party_settings)
      this->party_settings->sensitivity_update();
    return ;
  }

/**
 ** the connection 'connection' was deleted
 **
 ** @param     connection   deleted connection
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_connection_deleted(::Network::Connection const& connection)
  {
    if (this->network)
      this->network->network_connection_deleted(connection);
    if (this->party_settings)
      this->party_settings->sensitivity_update();
    return ;
  }

/**
 ** there was network traffic over the connection 'connection'
 **
 ** @param     connection   connection with the traffic
 ** @param     traffic_type   the traffic type
 ** @param     text   	send text
 **
 ** @return    -
 **/
void
  UI_GTKMM::network_traffic(::Network::Connection const& connection,
                            ::Network::TrafficType const traffic_type,
                            string const& text)
  {
    if (this->network_log)
      this->network_log->add_traffic(connection, traffic_type, text);
    if (this->network)
      this->network->traffic(connection, traffic_type, text);
    return ;
  }

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
