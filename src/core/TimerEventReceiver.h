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

#ifndef CORE_TIMEREVENTRECEIVER_H
#define CORE_TIMEREVENTRECEIVER_H

#include "utils/Timeval.h" // IWYU pragma: export

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core {

    class TimerEventReceiver {
    public:
        TimerEventReceiver(const TimerEventReceiver&) = delete;
        TimerEventReceiver& operator=(const TimerEventReceiver&) = delete;

        TimerEventReceiver(const utils::Timeval& delay, const void* arg)
            : absoluteTimeout(utils::Timeval::currentTime() + delay)
            , delay(delay)
            , arg(arg) {
        }

        virtual utils::Timeval getTimeout() const = 0;

    private:
        virtual bool dispatchEvent() = 0;
        virtual void unobservedEvent() = 0;

        virtual bool operator<(const TimerEventReceiver& timerEventReceiver) const = 0;

    protected:
        void update() {
            absoluteTimeout += delay;
        }

        utils::Timeval absoluteTimeout;
        utils::Timeval delay;

        const void* arg;

        friend class TimerEventDispatcher;
    };

} // namespace core

#endif // CORE_TIMEREVENTRECEIVER_H
