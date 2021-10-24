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

#include "SocketContextUpgradeFactory.h"

#include "web/http/SocketContextUpgradeFactory.hpp"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::http::client {

    SocketContextUpgradeFactory::SocketContextUpgradeFactory()
        : web::http::SocketContextUpgradeFactory<Request, Response>(http::SocketContextUpgradeFactory<Request, Response>::Role::CLIENT) {
    }

} // namespace web::http::client

namespace web::http {
    template class web::http::SocketContextUpgradeFactory<web::http::client::Request, web::http::client::Response>;
}
