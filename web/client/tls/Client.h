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

#ifndef HTTP_CLIENT_TLS_CLIENT_H
#define HTTP_CLIENT_TLS_CLIENT_H

#include "net/socket/ip/tcp/ipv4/tls/SocketClient.h"
#include "net/socket/ip/tcp/ipv6/tls/SocketClient.h"
#include "web/client/Client.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::client::tls {

    template <typename Request = web::client::Request, typename Response = web::client::Response>
    class Client : public web::client::Client<net::socket::ip::tcp::ipv4::tls::SocketClient, Request, Response> {
    public:
        using web::client::Client<net::socket::ip::tcp::ipv4::tls::SocketClient, Request, Response>::Client;
    };
    /*
        Client(const std::function<void(net::socket::ip::tcp::ipv4::tls::SocketClient::SocketConnection*)>& onConnect,
               const std::function<void(net::socket::ip::tcp::ipv4::tls::SocketClient::SocketConnection*)>& onConnected,
               const std::function<void(Request&)>& onRequestBegin,
               const std::function<void(Response&)>& onResponse,
               const std::function<void(int, const std::string&)>& onResponseError,
               const std::function<void(net::socket::ip::tcp::ipv4::tls::SocketClient::SocketConnection*)>& onDisconnect,
               const std::map<std::string, std::any>& options = {{}})
            ->Client<web::client::Request, web::client::Response>;
    */
    template <typename Request = web::client::Request, typename Response = web::client::Response>
    class Client6 : public web::client::Client<net::socket::ip::tcp::ipv6::tls::SocketClient, Request, Response> {
    public:
        using web::client::Client<net::socket::ip::tcp::ipv6::tls::SocketClient, Request, Response>::Client;
    };
    /*
        Client6(const std::function<void(net::socket::ip::tcp::ipv6::tls::SocketClient::SocketConnection*)>& onConnect,
                const std::function<void(net::socket::ip::tcp::ipv6::tls::SocketClient::SocketConnection*)>& onConnected,
                const std::function<void(Request&)>& onRequestBegin,
                const std::function<void(Response&)>& onResponse,
                const std::function<void(int, const std::string&)>& onResponseError,
                const std::function<void(net::socket::ip::tcp::ipv6::tls::SocketClient::SocketConnection*)>& onDisconnect,
                const std::map<std::string, std::any>& options = {{}})
            ->Client6<web::client::Request, web::client::Response>;
    */
} // namespace web::client::tls

#endif // HTTP_CLIENT_TLS_CLIENT_H