/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020 Volker Christian <me@vchrist.at>
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

#ifndef HTTP_SERVER_LEGACY_SERVER_H
#define HTTP_SERVER_LEGACY_SERVER_H

#include "http/server/Server.h"
#include "net/socket/ip/tcp/ipv4/legacy/SocketServer.h"
#include "net/socket/ip/tcp/ipv6/legacy/SocketServer.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace http::server::legacy {

    template <typename Request = http::server::Request, typename Response = http::server::Response>
    class Server : public http::server::Server<net::socket::ip::tcp::ipv4::legacy::SocketServer, Request, Response> {
    public:
        using http::server::Server<net::socket::ip::tcp::ipv4::legacy::SocketServer, Request, Response>::Server;
    };

    Server(const std::function<void(net::socket::ip::tcp::ipv4::legacy::SocketServer::SocketConnection*)>& onConnect,
           const std::function<void(Request& req, Response& res)>& onRequestReady,
           const std::function<void(net::socket::ip::tcp::ipv4::legacy::SocketServer::SocketConnection*)>& onDisconnect,
           const std::map<std::string, std::any>& options = {{}})
        ->Server<http::server::Request, http::server::Response>;

    template <typename Request = http::server::Request, typename Response = http::server::Response>
    class Server6 : public http::server::Server<net::socket::ip::tcp ::ipv6::legacy::SocketServer, Request, Response> {
    public:
        using http::server::Server<net::socket::ip::tcp::ipv6::legacy::SocketServer, Request, Response>::Server;
    };

    Server6(const std::function<void(net::socket::ip::tcp::ipv6::legacy::SocketServer::SocketConnection*)>& onConnect,
            const std::function<void(Request& req, Response& res)>& onRequestReady,
            const std::function<void(net::socket::ip::tcp::ipv6::legacy::SocketServer::SocketConnection*)>& onDisconnect,
            const std::map<std::string, std::any>& options = {{}})
        ->Server6<http::server::Request, http::server::Response>;

} // namespace http::server::legacy

#endif // HTTP_SERVER_LEGACY_SERVER_H