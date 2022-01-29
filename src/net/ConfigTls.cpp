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

#include "net/ConfigTls.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "utils/CLI11.hpp"

#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace net {

    void ConfigTls::finish(CLI::App* serverSc) {
        tlsSc = serverSc->add_subcommand("tls");
        tlsSc->description("Options for SSL/TLS behoviour");
        tlsSc->configurable();

        tlsInitTimeoutOpt = tlsSc->add_option("-i,--init-timeout", initTimeout, "Read timeout");
        tlsInitTimeoutOpt->type_name("[sec]");
        tlsInitTimeoutOpt->default_val(2);
        tlsInitTimeoutOpt->configurable();

        tlsShutdownTimeoutOpt = tlsSc->add_option("-s,--shutdown-timeout", shutdownTimeout, "Write timeout");
        tlsShutdownTimeoutOpt->type_name("[sec]");
        tlsShutdownTimeoutOpt->default_val(2);
        tlsShutdownTimeoutOpt->configurable();
    }

    int ConfigTls::getShutdownTimeout() const {
        return shutdownTimeout;
    }

    int ConfigTls::getInitTimeout() const {
        return initTimeout;
    }

} // namespace net