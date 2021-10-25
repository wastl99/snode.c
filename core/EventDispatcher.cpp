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

#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include "core/EventDispatcher.h"

#include "core/EventReceiver.h"
#include "log/Logger.h"    // for Writer, CWARNING, LOG
#include "utils/Timeval.h" // for operator-, operator<, operator>=

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <algorithm> // for min, find
#include <climits>
#include <iterator>    // for reverse_iterator
#include <type_traits> // for add_const<>::type
#include <utility>     // for tuple_element<>::type

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core {

    EventDispatcher::FdSet::FdSet() {
        zero();
    }

    void EventDispatcher::FdSet::set(int fd) {
        FD_SET(fd, &registered);
    }

    void EventDispatcher::FdSet::clr(int fd) {
        FD_CLR(fd, &registered);
        FD_CLR(fd, &active);
    }

    int EventDispatcher::FdSet::isSet(int fd) const {
        return FD_ISSET(fd, &active);
    }

    void EventDispatcher::FdSet::zero() {
        FD_ZERO(&registered);
        FD_ZERO(&active);
    }

    fd_set& EventDispatcher::FdSet::get() {
        active = registered;
        return active;
    }

    bool EventDispatcher::DescriptorEventReceiverList::contains(EventReceiver* eventReceiver) const {
        return std::find(begin(), end(), eventReceiver) != end();
    }

    void EventDispatcher::enable(EventReceiver* eventReceiver, int fd) {
        if (disabledEventReceiver[fd].contains(eventReceiver)) {
            // same tick as disable
            disabledEventReceiver[fd].remove(eventReceiver);
        } else if (!eventReceiver->isEnabled() &&
                   (!enabledEventReceiver.contains(fd) || !enabledEventReceiver[fd].contains(eventReceiver))) {
            // next tick as disable
            enabledEventReceiver[fd].push_back(eventReceiver);
            eventReceiver->enabled();
            if (unobservedEventReceiver.contains(eventReceiver)) {
                unobservedEventReceiver.remove(eventReceiver);
            }
        } else {
            LOG(WARNING) << "EventReceiver double enable";
        }
    }

    void EventDispatcher::disable(EventReceiver* eventReceiver, int fd) {
        if (enabledEventReceiver[fd].contains(eventReceiver)) {
            // same tick as enable
            enabledEventReceiver[fd].remove(eventReceiver);
            eventReceiver->disabled();
            if (eventReceiver->observationCounter == 0) {
                unobservedEventReceiver.push_back(eventReceiver);
            }
        } else if (eventReceiver->isEnabled() &&
                   (!disabledEventReceiver.contains(fd) || !disabledEventReceiver[fd].contains(eventReceiver))) {
            // next tick as enable
            disabledEventReceiver[fd].push_back(eventReceiver);
        } else {
            LOG(WARNING) << "EventReceiver double disable";
        }
    }

    void EventDispatcher::suspend(EventReceiver* eventReceiver, int fd) {
        if (!eventReceiver->isSuspended()) {
            eventReceiver->suspended();
            if (observedEventReceiver.contains(fd) && observedEventReceiver[fd].front() == eventReceiver) {
                fdSet.clr(fd);
            }
        } else {
            LOG(WARNING) << "EventReceiver double suspend";
        }
    }

    void EventDispatcher::resume(EventReceiver* eventReceiver, int fd) {
        if (eventReceiver->isSuspended()) {
            eventReceiver->resumed();
            if (observedEventReceiver.contains(fd) && observedEventReceiver[fd].front() == eventReceiver) {
                fdSet.set(fd);
            }
        } else {
            LOG(WARNING) << "EventReceiver double resume";
        }
    }

    unsigned long EventDispatcher::getEventCounter() const {
        return eventCounter;
    }

    int EventDispatcher::getMaxFd() const {
        int maxFd = -1;

        if (!observedEventReceiver.empty()) {
            maxFd = observedEventReceiver.rbegin()->first;
        }

        return maxFd;
    }

    fd_set& EventDispatcher::getFdSet() {
        return fdSet.get();
    }

    struct timeval EventDispatcher::observeEnabledEvents() {
        struct timeval nextTimeout = {LONG_MAX, 0};

        for (const auto& [fd, eventReceivers] : enabledEventReceiver) {
            for (EventReceiver* eventReceiver : eventReceivers) {
                observedEventReceiver[fd].push_front(eventReceiver);
                if (!eventReceiver->isSuspended()) {
                    fdSet.set(fd);
                    nextTimeout = std::min(nextTimeout, eventReceiver->getTimeout());
                } else {
                    fdSet.clr(fd);
                }
            }
        }
        enabledEventReceiver.clear();

        return nextTimeout;
    }

    struct timeval EventDispatcher::dispatchActiveEvents(struct timeval currentTime) {
        struct timeval nextInactivityTimeout {
            LONG_MAX, 0
        };

        for (const auto& [fd, eventReceivers] : observedEventReceiver) {
            EventReceiver* eventReceiver = eventReceivers.front();
            struct timeval maxInactivity = eventReceiver->getTimeout();
            if (fdSet.isSet(fd) || (eventReceiver->continueImmediately())) {
                eventCounter++;
                eventReceiver->dispatchEvent();
                eventReceiver->triggered(currentTime);
                if (!eventReceiver->isSuspended()) {
                    if (eventReceiver->continueImmediately()) {
                        nextInactivityTimeout = {0, 0};
                    } else {
                        nextInactivityTimeout = std::min(nextInactivityTimeout, maxInactivity);
                    }
                }
            } else {
                struct timeval inactivity = currentTime - eventReceiver->getLastTriggered();
                if (inactivity >= maxInactivity) {
                    eventReceiver->timeoutEvent();
                } else {
                    nextInactivityTimeout = std::min(maxInactivity - inactivity, nextInactivityTimeout);
                }
            }
        }

        return nextInactivityTimeout;
    }

    void EventDispatcher::unobserveDisabledEvents() {
        for (const auto& [fd, eventReceivers] : disabledEventReceiver) {
            for (EventReceiver* eventReceiver : eventReceivers) {
                observedEventReceiver[fd].remove(eventReceiver);
                if (observedEventReceiver[fd].empty() || observedEventReceiver[fd].front()->isSuspended()) {
                    if (observedEventReceiver[fd].empty()) {
                        observedEventReceiver.erase(fd);
                    }
                    fdSet.clr(fd);
                } else {
                    fdSet.set(fd);
                    observedEventReceiver[fd].front()->triggered();
                }
                eventReceiver->disabled();
                if (eventReceiver->observationCounter == 0) {
                    unobservedEventReceiver.push_back(eventReceiver);
                }
                eventReceiver->fd = -1;
            }
        }
        disabledEventReceiver.clear();
    }

    void EventDispatcher::releaseUnobservedEvents() {
        for (EventReceiver* eventReceiver : unobservedEventReceiver) {
            eventReceiver->unobserved();
        }
        unobservedEventReceiver.clear();
    }

    void EventDispatcher::disableObservedEvents() {
        for (const auto& [fd, eventReceivers] : observedEventReceiver) {
            for (EventReceiver* eventReceiver : eventReceivers) {
                disabledEventReceiver[fd].push_back(eventReceiver);
            }
        }
    }

} // namespace core

#endif // EVENTDISPATCHER_HPP