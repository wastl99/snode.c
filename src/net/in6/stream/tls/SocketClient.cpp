/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021, 2022 Volker Christian <me@vchrist.at>
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

#include "net/ClientSocket.hpp" // IWYU pragma: keep
#include "net/in6/stream/ClientSocket.hpp"
#include "net/in6/stream/tls/config/ConfigSocketClient.h" // IWYU pragma: keep

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif // DOXYGEN_SHOULD_SKIP_THIS

template class net::in6::stream::ClientSocket<net::in6::stream::tls::config::ConfigSocketClient>;
template class net::ClientSocket<net::in6::stream::tls::config::ConfigSocketClient, net::in6::stream::Socket>;