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

#ifndef CORE_SOCKET_STREAM_SOCKETWRITER_H
#define CORE_SOCKET_STREAM_SOCKETWRITER_H

#include "core/WriteEventReceiver.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cerrno>
#include <cstddef> // for std::size_t
#include <functional>
#include <sys/types.h>
#include <vector>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#ifndef MAX_SEND_JUNKSIZE
#define MAX_SEND_JUNKSIZE 16384
#endif

#ifndef MAX_SHUTDOWN_TIMEOUT
#define MAX_SHUTDOWN_TIMEOUT 1
#endif

namespace core::socket::stream {

    template <typename SocketT>
    class SocketWriter
        : public WriteEventReceiver
        , virtual public SocketT {
        SocketWriter() = delete;

    public:
        using Socket = SocketT;

    protected:
        explicit SocketWriter(const std::function<void(int)>& onError)
            : onError(onError) {
        }

        virtual ~SocketWriter() = default;

        void sendToPeer(const char* junk, std::size_t junkLen) {
            writeBuffer.insert(writeBuffer.end(), junk, junk + junkLen);

            if (isSuspended()) {
                resume();
            }
        }

    private:
        virtual ssize_t write(const char* junk, std::size_t junkLen) = 0;

        void writeEvent() override {
            doWrite();
        }

        virtual void doShutdown() {
            Socket::shutdown(Socket::shutdown::WR);
            resume();
            shutdownTriggered = true;
            // disable(); // Normally this should be sufficient - but google-chrome
        }

    protected:
        void shutdown() {
            if (!shutdownTriggered) {
                if (isSuspended()) {
                    doShutdown();
                    markShutdown = false;
                } else {
                    markShutdown = true;
                }
                setTimeout(MAX_SHUTDOWN_TIMEOUT);
            }
        }

        void terminate() override {
            shutdown();
        }

        void doWrite() {
            errno = 0;

            std::size_t writeLen = (writeBuffer.size() < MAX_SEND_JUNKSIZE) ? writeBuffer.size() : MAX_SEND_JUNKSIZE;

            ssize_t retWrite = -1;
            if (!shutdownTriggered) {
                retWrite = write(writeBuffer.data(), writeLen);
            }

            if (retWrite >= 0) {
                writeBuffer.erase(writeBuffer.begin(), writeBuffer.begin() + retWrite);
            } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                disable();
                onError(errno);
            }

            if (writeBuffer.empty()) {
                suspend();
                if (markShutdown) {
                    shutdown();
                }
            }
        }

    private:
        std::function<void(int)> onError;

        std::vector<char> writeBuffer;

        bool markShutdown = false;

        bool shutdownTriggered = false;
    };

} // namespace core::socket::stream

#endif // CORE_SOCKET_STREAM_SOCKETWRITER_H