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

#include "net/EventLoop.h" // for EventLoop

#include "log/Logger.h" // for Logger
#include "net/system/select.h"
#include "net/system/signal.h"
#include "net/system/time.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <algorithm> // for min, max
#include <cerrno>    // for EINTR, errno
#include <cstdlib>   // for exit
#include <string>    // for string, to_string

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#define MAX_READ_INACTIVITY 60
#define MAX_WRITE_INACTIVITY 60
#define MAX_OUTOFBAND_INACTIVITY 60

namespace net {

    static std::string getTickCounterAsString(const el::LogMessage*) {
        std::string tick = std::to_string(EventLoop::instance().getTickCounter());

        if (tick.length() < 10) {
            tick.insert(0, 10 - tick.length(), '0');
        }

        return tick;
    }

    EventLoop EventLoop::eventLoop;

    bool EventLoop::initialized = false;
    bool EventLoop::running = false;
    bool EventLoop::stopped = true;
    int EventLoop::stopsig = 0;

    TickStatus EventLoop::_tick(struct timeval timeOut) {
        TickStatus tickStatus = TickStatus::SUCCESS;

        struct timeval nextTimeout = readEventDispatcher.observeEnabledEvents();
        nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

        nextTimeout = writeEventDispatcher.observeEnabledEvents();
        nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

        nextTimeout = exceptionalConditionEventDispatcher.observeEnabledEvents();
        nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

        nextTimeout = timerEventDispatcher.getNextTimeout();
        nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

        int maxFd = readEventDispatcher.getMaxFd();
        maxFd = std::max(writeEventDispatcher.getMaxFd(), maxFd);
        maxFd = std::max(exceptionalConditionEventDispatcher.getMaxFd(), maxFd);

        if (maxFd >= 0 || !timerEventDispatcher.empty()) {
            nextInactivityTimeout = std::max(nextInactivityTimeout, {0, 0});
            nextInactivityTimeout = std::min(nextInactivityTimeout, timeOut);

            int counter = net::system::select(maxFd + 1,
                                              &readEventDispatcher.getFdSet(),
                                              &writeEventDispatcher.getFdSet(),
                                              &exceptionalConditionEventDispatcher.getFdSet(),
                                              &nextInactivityTimeout);

            if (counter >= 0) {
                tickCounter++;

                timerEventDispatcher.dispatch();

                struct timeval currentTime = {net::system::time(nullptr), 0};
                nextInactivityTimeout = {LONG_MAX, 0};

                nextTimeout = readEventDispatcher.dispatchActiveEvents(currentTime);
                nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

                nextTimeout = writeEventDispatcher.dispatchActiveEvents(currentTime);
                nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);

                nextTimeout = exceptionalConditionEventDispatcher.dispatchActiveEvents(currentTime);
                nextInactivityTimeout = std::min(nextTimeout, nextInactivityTimeout);
            } else if (errno != EINTR) {
                PLOG(ERROR) << "select";
                tickStatus = TickStatus::SELECT_ERROR;
            }
        } else {
            tickStatus = TickStatus::NO_OBSERVER;
        }

        readEventDispatcher.unobserveDisabledEvents();
        writeEventDispatcher.unobserveDisabledEvents();
        exceptionalConditionEventDispatcher.unobserveDisabledEvents();

        readEventDispatcher.releaseUnobservedEvents();
        writeEventDispatcher.releaseUnobservedEvents();
        exceptionalConditionEventDispatcher.releaseUnobservedEvents();

        return tickStatus;
    }

    void EventLoop::_free() {
        readEventDispatcher.observeEnabledEvents();
        writeEventDispatcher.observeEnabledEvents();
        exceptionalConditionEventDispatcher.observeEnabledEvents();

        readEventDispatcher.disableObservedEvents();
        writeEventDispatcher.disableObservedEvents();
        exceptionalConditionEventDispatcher.disableObservedEvents();

        readEventDispatcher.unobserveDisabledEvents();
        writeEventDispatcher.unobserveDisabledEvents();
        exceptionalConditionEventDispatcher.unobserveDisabledEvents();

        readEventDispatcher.releaseUnobservedEvents();
        writeEventDispatcher.releaseUnobservedEvents();
        exceptionalConditionEventDispatcher.releaseUnobservedEvents();

        timerEventDispatcher.cancelAll();
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
    void EventLoop::init(int argc, char* argv[]) {
        Logger::init(argc, argv);

        Logger::setCustomFormatSpec("%tick", net::getTickCounterAsString);

        EventLoop::initialized = true;
    }

    int EventLoop::start(struct timeval timeOut) {
        if (!initialized) {
            PLOG(ERROR) << "snode.c not initialized. Use EventLoop::init(argc, argv) before EventLoop::start().";
            exit(1);
        }

        stopped = false;

        if (!running) {
            running = true;

            sighandler_t oldSigPipeHandler = net::system::signal(SIGPIPE, SIG_IGN);
            sighandler_t oldSigQuitHandler = net::system::signal(SIGQUIT, EventLoop::stoponsig);
            sighandler_t oldSigHubHandler = net::system::signal(SIGHUP, EventLoop::stoponsig);
            sighandler_t oldSigIntHandler = net::system::signal(SIGINT, EventLoop::stoponsig);
            sighandler_t oldSigTermHandler = net::system::signal(SIGTERM, EventLoop::stoponsig);
            sighandler_t oldSigAbrtHandler = net::system::signal(SIGABRT, EventLoop::stoponsig);

            while (!stopped) {
                if (eventLoop._tick(timeOut) != TickStatus::SUCCESS) {
                    stopped = true;
                }
            };

            net::system::signal(SIGPIPE, oldSigPipeHandler);
            net::system::signal(SIGQUIT, oldSigQuitHandler);
            net::system::signal(SIGHUP, oldSigHubHandler);
            net::system::signal(SIGINT, oldSigIntHandler);
            net::system::signal(SIGTERM, oldSigTermHandler);
            net::system::signal(SIGABRT, oldSigAbrtHandler);

            running = false;
        }

        eventLoop._free();

        int returnReason = 0;

        if (stopsig != 0) {
            returnReason = -1;
        }

        return returnReason;
    }

    TickStatus EventLoop::tick(struct timeval timeOut) {
        if (!initialized) {
            PLOG(ERROR) << "snode.c not initialized. Use EventLoop::init(argc, argv) before EventLoop::start().";
            exit(1);
        }

        TickStatus tickStatus;

        sighandler_t oldSigPipeHandler = net::system::signal(SIGPIPE, SIG_IGN);

        tickStatus = eventLoop._tick(timeOut);

        net::system::signal(SIGPIPE, oldSigPipeHandler);

        return tickStatus;
    }

    void EventLoop::free() {
        eventLoop._free();
    }

    void EventLoop::stop() {
        stopped = true;
    }

    void EventLoop::stoponsig(int sig) {
        stopsig = sig;
        stop();
    }

    unsigned long EventLoop::getEventCounter() {
        return readEventDispatcher.getEventCounter() + writeEventDispatcher.getEventCounter() +
               exceptionalConditionEventDispatcher.getEventCounter();
    }

} // namespace net
