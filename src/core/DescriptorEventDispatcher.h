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

#ifndef CORE_DESCRIPTOREVENTDISPATCHER_H
#define CORE_DESCRIPTOREVENTDISPATCHER_H

#include "core/EventDispatcher.h"
#include "core/system/select.h" // IWYU pragma: keep

namespace core {
    class EventReceiver;
} // namespace core

namespace utils {
    class Timeval;
} // namespace utils

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <list>
#include <map>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core {

    class DescriptorEventDispatcher {
    public:
        DescriptorEventDispatcher() = default;

        DescriptorEventDispatcher(const DescriptorEventDispatcher&) = delete;
        DescriptorEventDispatcher& operator=(const DescriptorEventDispatcher&) = delete;

    private:
        class EventReceiverList : public std::list<EventReceiver*> {
        public:
            using std::list<EventReceiver*>::begin;
            using std::list<EventReceiver*>::end;
            using std::list<EventReceiver*>::front;

            bool contains(EventReceiver* descriptorEventReceiver) const;
        };

    public:
        void enable(EventReceiver* eventReceiver);
        void disable(EventReceiver* eventReceiver);
        void suspend(EventReceiver* eventReceiver);
        void resume(EventReceiver* eventReceiver);

        unsigned long getEventCounter() const;

        fd_set& getFdSet();

        int getMaxFd() const;

        utils::Timeval getNextTimeout(const utils::Timeval& currentTime) const;

        void observeEnabledEvents();
        void dispatchActiveEvents(const utils::Timeval& currentTime);
        void unobserveDisabledEvents(const utils::Timeval& currentTime);
        void terminateObservedEvents();

    private:
        std::map<int, EventReceiverList> enabledEventReceiver;
        std::map<int, EventReceiverList> observedEventReceiver;
        std::map<int, EventReceiverList> disabledEventReceiver;

        EventDispatcher::FdSet fdSet;

        unsigned long eventCounter = 0;
    };
} // namespace core

#endif // CORE_DESCRIPTOREVENTDISPATCHER_H