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

#include "IntervalTimerStopable.h"

#include "core/TimerEventReceiver.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core::timer {

    IntervalTimerStopable::IntervalTimerStopable(const std::function<void(const void*, const std::function<void()>&)>& dispatcher,
                                                 const utils::Timeval& timeout,
                                                 const void* arg,
                                                 const std::string& name)
        : core::TimerEventReceiver(name, timeout)
        , dispatcher(dispatcher)
        , arg(arg) {
    }

    void IntervalTimerStopable::dispatchEvent() {
        bool stop = false;
        dispatcher(arg, [&stop]() -> void {
            stop = true;
        });
        if (stop) {
            cancel();
        } else {
            update();
        }
    }

    void IntervalTimerStopable::unobservedEvent() {
        delete this;
    }

} // namespace core::timer