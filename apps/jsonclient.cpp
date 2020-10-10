/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020  Volker Christian <me@vchrist.at>
 * Json Middleware 2020 Marlene Mayr, Anna Moser, Matteo Prock, Eric Thalhammer
 * Github <MarleneMayr><moseranna><MatteoMatteoMatteo><peregrin-tuk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "EventLoop.h"
#include "Logger.h"
#include "ServerResponse.h"
#include "config.h" // just for this example app
#include "legacy/Client.h"
#include "tls/Client.h"

#include <cstring>
#include <openssl/x509v3.h>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

int main(int argc, char* argv[]) {
    net::EventLoop::init(argc, argv);

    http::legacy::Client jsonClient(
        [](http::legacy::Client::SocketConnection* socketConnection) -> void {
            VLOG(0) << "-- OnConnect";
            VLOG(0) << "     Server: " + socketConnection->getRemoteAddress().host() + "(" + socketConnection->getRemoteAddress().ip() +
                           "):" + std::to_string(socketConnection->getRemoteAddress().port());
            VLOG(0) << "     Client: " + socketConnection->getLocalAddress().host() + "(" + socketConnection->getLocalAddress().ip() +
                           "):" + std::to_string(socketConnection->getLocalAddress().port());
        },
        []([[maybe_unused]] const http::ServerRequest& clientRequest) -> void {
        },
        [](const http::ServerResponse& clientResponse) -> void {
            VLOG(0) << "-- OnResponse";
            VLOG(0) << "     Status:";
            VLOG(0) << "       " << clientResponse.httpVersion;
            VLOG(0) << "       " << clientResponse.statusCode;
            VLOG(0) << "       " << clientResponse.reason;

            VLOG(0) << "     Headers:";
            for (auto [field, value] : *clientResponse.headers) {
                VLOG(0) << "       " << field + " = " + value;
            }

            VLOG(0) << "     Cookies:";
            for (auto [name, cookie] : *clientResponse.cookies) {
                VLOG(0) << "       " + name + " = " + cookie.getValue();
                for (auto [option, value] : cookie.getOptions()) {
                    VLOG(0) << "         " + option + " = " + value;
                }
            }

            char* body = new char[clientResponse.contentLength + 1];
            memcpy(body, clientResponse.body, clientResponse.contentLength);
            body[clientResponse.contentLength] = 0;

            VLOG(1) << "     Body:\n----------- start body -----------\n" << body << "------------ end body ------------";

            delete[] body;
        },
        [](int status, const std::string& reason) -> void {
            VLOG(0) << "-- OnResponseError";
            VLOG(0) << "     Status: " << status;
            VLOG(0) << "     Reason: " << reason;
        },
        [](http::legacy::Client::SocketConnection* socketConnection) -> void {
            VLOG(0) << "-- OnDisconnect";
            VLOG(0) << "     Server: " + socketConnection->getRemoteAddress().host() + "(" + socketConnection->getRemoteAddress().ip() +
                           "):" + std::to_string(socketConnection->getRemoteAddress().port());
            VLOG(0) << "     Client: " + socketConnection->getLocalAddress().host() + "(" + socketConnection->getLocalAddress().ip() +
                           "):" + std::to_string(socketConnection->getLocalAddress().port());
        });

    jsonClient.post({{"host", "localhost"},
                     {"port", 8080},
                     {"path", "/index.html"},
                     {"body", "{\"userId\":1,\"schnitzel\":\"good\",\"hungry\":false}"}},
                    [](int err) -> void {
                        if (err != 0) {
                            PLOG(ERROR) << "OnError: " << err;
                        }
                    }); // Connection:keep-alive\r\n\r\n"

    return net::EventLoop::start();
}
