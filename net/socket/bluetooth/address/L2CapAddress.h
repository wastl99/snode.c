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

#ifndef NET_SOCKET_BLUETOOTH_ADDRESS_L2CAPADDRESS_H
#define NET_SOCKET_BLUETOOTH_ADDRESS_L2CAPADDRESS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <bluetooth/bluetooth.h> // IWYU pragma: keep, for str2ba, ba2str, bdaddr_t
#include <bluetooth/l2cap.h>     // IWYU pragma: keep, for sockaddr_rc
#include <cstdint>               // for uint16_t
#include <exception>             // IWYU pragma: keep
#include <string>
// IWYU pragma: no_include <bits/exception.h>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "socket/SocketAddress.h"

namespace net::socket::bluetooth::address {

    class L2CapAddress : public SocketAddress<struct sockaddr_l2> {
    public:
        L2CapAddress();
        L2CapAddress(const L2CapAddress& bta);

        explicit L2CapAddress(const std::string& btAddress);
        L2CapAddress(const std::string& btAddress, uint16_t psm);
        explicit L2CapAddress(uint16_t psm);
        explicit L2CapAddress(const struct sockaddr_l2& addr);

        uint16_t psm() const;
        std::string address() const;

        L2CapAddress& operator=(const L2CapAddress& bta);

        const struct sockaddr_l2& getSockAddrRc() const;
    };

} // namespace net::socket::bluetooth::address

#endif // NET_SOCKET_BLUETOOTH_ADDRESS_L2CAPADDRESS_H