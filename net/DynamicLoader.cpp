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

#include "DynamicLoader.h"

#include "log/Logger.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <dlfcn.h>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

//#define USE_SYNC

namespace net {

    DynamicLoader* DynamicLoader::dynamicLoader = nullptr;

    DynamicLoader* DynamicLoader::instance() {
        if (dynamicLoader == nullptr) {
            dynamicLoader = new DynamicLoader();
        }

        return dynamicLoader;
    }

    void* DynamicLoader::dlOpen(const std::string& libFile, int flags) {
        void* handle = dlopen(libFile.c_str(), flags);

        VLOG(0) << "dlOpen: " << handle << " : " << libFile.c_str();

#ifndef USE_SYNC
        instance()->loadedLibraries[handle] = libFile;
#endif

        return handle;
    }

    void DynamicLoader::dlClose(void* handle) {
        VLOG(0) << "dlClose: " << handle;
#ifdef USE_SYNC
        dlclose(handle);
#else
        VLOG(0) << "Register for late dlClose: " << handle << " : " << instance()->loadedLibraries[handle];
        if (instance()->loadedLibraries.contains(handle) && !instance()->registeredForUnload.contains(handle)) {
            instance()->registeredForUnload.insert(handle);
        }
#endif
    }

    void DynamicLoader::doRealDlClose([[maybe_unused]] void* handle) {
#ifndef USE_SYNC
        VLOG(0) << "DO doRealDlClose: " << handle << " : " << instance()->loadedLibraries[handle];
        dlclose(handle);
        instance()->loadedLibraries.erase(handle);
#endif
    }

    void DynamicLoader::doAllDlClose() {
#ifndef USE_SYNC
        VLOG(0) << "Do doRealDlClose for all: " << instance()->loadedLibraries.size();
        std::map<void*, std::string>::iterator it = instance()->loadedLibraries.begin();

        while (it != instance()->loadedLibraries.end()) {
            std::map<void*, std::string>::iterator tmpIt = it;
            ++it;
            doRealDlClose(tmpIt->first);
        }

        delete instance();
        DynamicLoader::dynamicLoader = nullptr;
#endif
    }

    void DynamicLoader::doAllDlClosedRealDlClose() {
#ifndef USE_SYNC
        VLOG(0) << "doAllDlClosedRealDlClose";
        for (void* handle : instance()->registeredForUnload) {
            doRealDlClose(handle);
        }

        instance()->registeredForUnload.clear();
#endif
    }

} // namespace net