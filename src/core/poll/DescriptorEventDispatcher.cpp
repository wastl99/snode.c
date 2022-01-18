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

#include "core/poll/DescriptorEventDispatcher.h"

#include "core/EventReceiver.h"
#include "core/poll/EventDispatcher.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <poll.h> // for pollfd, POLLNVAL

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core::poll {

    DescriptorEventDispatcher::DescriptorEventDispatcher(PollFds& pollFds, short events, short revents)
        : pollFds(pollFds)
        , events(events)
        , revents(revents) {
    }

    void DescriptorEventDispatcher::modAdd(EventReceiver* eventReceiver) {
        pollFds.add(pollEvents, eventReceiver, events);
    }

    void DescriptorEventDispatcher::modDel(EventReceiver* eventReceiver) {
        pollFds.del(pollEvents, eventReceiver, events);
    }

    void DescriptorEventDispatcher::modOn(EventReceiver* eventReceiver) {
        pollFds.modOn(pollEvents, eventReceiver, events);
    }

    void DescriptorEventDispatcher::modOff(EventReceiver* eventReceiver) {
        pollFds.modOff(pollEvents, eventReceiver, events);
    }

    int DescriptorEventDispatcher::getInterestCount() const {
        return static_cast<int>(observedEventReceiver.size());
    }

    void DescriptorEventDispatcher::dispatchActiveEvents(const utils::Timeval& currentTime) {
        pollfd* pollfds = pollFds.getEvents();

        std::unordered_map<int, PollFdIndex> pollFdsIndices = pollFds.getPollFdIndices();

        for (auto& [fd, eventReceiver] : pollEvents) { // cppcheck-suppress unassignedVariable
            pollfd& pollFd = pollfds[pollFdsIndices[fd].index];

            if ((pollFd.events & events) != 0 && (pollFd.revents & this->revents) != 0) {
                if (!eventReceiver->continueImmediately() && !eventReceiver->isSuspended()) {
                    eventCounter++;
                    eventReceiver->dispatch(currentTime);
                }
            }
        }
    }

    void core::poll::DescriptorEventDispatcher::finishTick() {
        pollFds.compress();
    }

} // namespace core::poll