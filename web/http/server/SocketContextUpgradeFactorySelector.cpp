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

#include "SocketContextUpgradeFactorySelector.h"

#include "log/Logger.h"
#include "web/http/http_utils.h"
#include "web/http/server/Request.h"
#include "web/http/server/Response.h"
#include "web/http/server/SocketContextUpgradeFactory.h"
#include "web/http/server/SocketContextUpgradeFactoryInterface.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cxxabi.h>
#include <dlfcn.h>
#include <type_traits> // for add_const<>::type

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::http::server {

    SocketContextUpgradeFactorySelector* SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector = nullptr;

    SocketContextUpgradeFactorySelector* SocketContextUpgradeFactorySelector::instance() {
        if (socketContextUpgradeFactorySelector == nullptr) {
            socketContextUpgradeFactorySelector = new SocketContextUpgradeFactorySelector();
        }

        return socketContextUpgradeFactorySelector;
    }

    web::http::server::SocketContextUpgradeFactory* SocketContextUpgradeFactorySelector::select(web::http::server::Request& req,
                                                                                                web::http::server::Response& res) {
        web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory = nullptr;

        std::string upgradeContextNames = req.header("upgrade");

        while (!upgradeContextNames.empty() && socketContextUpgradeFactory == nullptr) {
            std::string upgradeContextName;
            std::string upgradeContextPriority;

            std::tie(upgradeContextName, upgradeContextNames) = httputils::str_split(upgradeContextNames, ',');
            std::tie(upgradeContextName, upgradeContextPriority) = httputils::str_split(upgradeContextName, '/');
            httputils::to_lower(upgradeContextName);

            if (socketContextUpgradePlugins.contains(upgradeContextName)) {
                socketContextUpgradeFactory = socketContextUpgradePlugins[upgradeContextName].socketContextUpgradeFactory;
            } else {
                socketContextUpgradeFactory = load(upgradeContextName);
            }
        }

        if (socketContextUpgradeFactory != nullptr) {
            socketContextUpgradeFactory->prepare(req, res);
        }

        return socketContextUpgradeFactory;
    }

    web::http::server::SocketContextUpgradeFactory* SocketContextUpgradeFactorySelector::load(const std::string& socketContextName) {
        VLOG(0) << "UpgradeSocketContext loading: " << socketContextName;

        web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory = nullptr;

        std::string socketContextLibraryPath = "/usr/local/lib/snode.c/web/http/upgrade/server/lib" + socketContextName + ".so";

        void* handle = dlopen(socketContextLibraryPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);

        if (handle != nullptr) {
            SocketContextUpgradeFactoryInterface* (*plugin)() =
                reinterpret_cast<SocketContextUpgradeFactoryInterface* (*) ()>(dlsym(handle, "plugin"));

            // Only allow signed plugins? Architecture for x509-certs for plugins?
            /*
                        const std::type_info& pluginTypeId = typeid(plugin());
                        const std::type_info& expectedTypeId = typeid(SocketContextUpgradeFactoryInterface*);
                        std::string pluginType = abi::__cxa_demangle(pluginTypeId.name(), 0, 0, &status);
                        std::string expectedType = abi::__cxa_demangle(expectedTypeId.name(), 0, 0, &status);
            */

            if (plugin != nullptr) {
                SocketContextUpgradeFactoryInterface* socketContextUpgradeFactoryInterface = plugin();

                if (socketContextUpgradeFactoryInterface != nullptr) {
                    socketContextUpgradeFactory = socketContextUpgradeFactoryInterface->create();

                    delete socketContextUpgradeFactoryInterface;

                    if (socketContextUpgradeFactory != nullptr) {
                        if (SocketContextUpgradeFactorySelector::instance()->add(socketContextUpgradeFactory, handle)) {
                            VLOG(0) << "UpgradeSocketContext loaded successfully: " << socketContextName;
                        } else {
                            socketContextUpgradeFactory->destroy();
                            socketContextUpgradeFactory = nullptr;
                            dlclose(handle);
                            VLOG(0) << "UpgradeSocketContext already existing. Not using: " << socketContextName;
                        }
                    } else {
                        dlclose(handle);
                        VLOG(0) << "SocketContextUpgradeFactorySelector not created (maybe to little memory?): "
                                << socketContextLibraryPath;
                    }
                } else {
                    dlclose(handle);
                    VLOG(0) << "SocketContextUpgradeInterface not created (maybe to little memory?): " << socketContextLibraryPath;
                }
            } else {
                dlclose(handle);
                VLOG(0) << "Not a Plugin \"" << socketContextLibraryPath;
            }
        } else {
            VLOG(0) << "Error dlopen: " << dlerror();
        }

        return socketContextUpgradeFactory;
    }

    void SocketContextUpgradeFactorySelector::unload() {
        for (const auto& [name, socketContextPlugin] : socketContextUpgradePlugins) {
            socketContextPlugin.socketContextUpgradeFactory->destroy();
            if (socketContextPlugin.handle != nullptr) {
                dlclose(socketContextPlugin.handle);
            }
        }

        delete this;
    }

    bool SocketContextUpgradeFactorySelector::add(web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory) {
        return add(socketContextUpgradeFactory, nullptr);
    }

    bool SocketContextUpgradeFactorySelector::add(web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory,
                                                  void* handle) {
        bool success = false;

        if (socketContextUpgradeFactory != nullptr) {
            SocketContextPlugin socketContextPlugin = {.socketContextUpgradeFactory = socketContextUpgradeFactory, .handle = handle};

            if (socketContextUpgradeFactory->role() == SocketContextUpgradeFactory::Role::SERVER) {
                std::tie(std::ignore, success) =
                    socketContextUpgradePlugins.insert({socketContextUpgradeFactory->name(), socketContextPlugin});
            }
        }

        return success;
    }

} // namespace web::http::server
