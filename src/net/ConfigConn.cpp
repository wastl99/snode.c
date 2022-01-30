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

#include "net/ConfigConn.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "utils/CLI11.hpp"

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace net {

    ConfigConn::ConfigConn(CLI::App* baseSc) {
        connectionSc = baseSc->add_subcommand("conn");
        connectionSc->description("Options for established client connections");
        connectionSc->configurable();

        connectionSc->group("Sub-Options (use -h,--help on them)");

        readTimeoutOpt = connectionSc->add_option("-r,--read-timeout", readTimeout, "Read timeout");
        readTimeoutOpt->type_name("[sec]");
        readTimeoutOpt->default_val(60);
        readTimeoutOpt->configurable();

        writeTimeoutOpt = connectionSc->add_option("-w,--write-timeout", writeTimeout, "Write timeout");
        writeTimeoutOpt->type_name("[sec]");
        writeTimeoutOpt->default_val(60);
        writeTimeoutOpt->configurable();

        readBlockSizeOpt = connectionSc->add_option("--read-block-size", readBlockSize, "Read block size");
        readBlockSizeOpt->type_name("[bytes]");
        readBlockSizeOpt->default_val(4096);
        readBlockSizeOpt->configurable();

        writeBlockSizeOpt = connectionSc->add_option("--write-block-size", writeBlockSize, "Write block size");
        writeBlockSizeOpt->type_name("[bytes]");
        writeBlockSizeOpt->default_val(4096);
        writeBlockSizeOpt->configurable();

        terminateTimeoutOpt = connectionSc->add_option("-t,--terminate-timeout", terminateTimeout, "Terminate timeout");
        terminateTimeoutOpt->type_name("[sec]");
        terminateTimeoutOpt->default_val(1);
        terminateTimeoutOpt->configurable();
    }

    const utils::Timeval& ConfigConn::getReadTimeout() const {
        return readTimeout;
    }

    const utils::Timeval& ConfigConn::getWriteTimeout() const {
        return writeTimeout;
    }

    std::size_t ConfigConn::getReadBlockSize() const {
        return readBlockSize;
    }

    std::size_t ConfigConn::getWriteBlockSize() const {
        return writeBlockSize;
    }

    const utils::Timeval& ConfigConn::getTerminateTimeout() const {
        return terminateTimeout;
    }

} // namespace net