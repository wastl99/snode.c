/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021 Volker Christian <me@vchrist.at>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "web/websocket/server/SubProtocol.h"

#include "web/websocket/server/ChannelManager.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::websocket::server {

    SubProtocol::SubProtocol(const std::string& name)
        : web::websocket::SubProtocol<web::websocket::server::SocketContext>(name) {
        ChannelManager::instance()->subscribe(this);
        // subscribe(name, this);
        // this->channel = name;
    }

    SubProtocol::~SubProtocol() {
        ChannelManager::instance()->unsubscribe(this);
        // unsubscribe(channel, this);
    }

    void SubProtocol::subscribe(const std::string& channel) {
        ChannelManager::instance()->subscribe(channel, this);
        // subscibe(channel, this);
        // unsubscribe(this->channel, this);
        // this->channel = channel;
    }

    void SubProtocol::sendBroadcast(const std::string& message) {
        ChannelManager::instance()->sendBroadcast(channel, message);
    }

    void SubProtocol::sendBroadcast(const char* message, std::size_t messageLength) {
        ChannelManager::instance()->sendBroadcast(channel, message, messageLength);
    }

    void SubProtocol::sendBroadcastStart(const char* message, std::size_t messageLength) {
        ChannelManager::instance()->sendBroadcastStart(channel, message, messageLength);
    }

    void SubProtocol::sendBroadcastStart(const std::string& message) {
        ChannelManager::instance()->sendBroadcastStart(channel, message);
    }

    void SubProtocol::sendBroadcastFrame(const char* message, std::size_t messageLength) {
        ChannelManager::instance()->sendBroadcastFrame(channel, message, messageLength);
    }

    void SubProtocol::sendBroadcastFrame(const std::string& message) {
        ChannelManager::instance()->sendBroadcastFrame(channel, message.data(), message.length());
    }

    void SubProtocol::sendBroadcastEnd(const char* message, std::size_t messageLength) {
        ChannelManager::instance()->sendBroadcastEnd(channel, message, messageLength);
    }

    void SubProtocol::sendBroadcastEnd(const std::string& message) {
        ChannelManager::instance()->sendBroadcastEnd(channel, message.data(), message.length());
    }

    void SubProtocol::forEachClient(const std::function<void(SubProtocol*)>& sendToClient) {
        ChannelManager::instance()->forEachClient(channel, sendToClient);
    }

} // namespace web::websocket::server
