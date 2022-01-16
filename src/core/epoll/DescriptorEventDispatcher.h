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

#ifndef CORE_EPOLL_DESCRIPTOREVENTDISPATCHER_H
#define CORE_EPOLL_DESCRIPTOREVENTDISPATCHER_H

#include "core/DescriptorEventDispatcher.h" // IWYU pragma: export

namespace core {
    class EventReceiver;
} // namespace core

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "utils/Timeval.h"

#include <cstdint>
#include <sys/epoll.h>
#include <vector>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core::epoll {

    class DescriptorEventDispatcher : public core::DescriptorEventDispatcher {
        DescriptorEventDispatcher(const DescriptorEventDispatcher&) = delete;
        DescriptorEventDispatcher& operator=(const DescriptorEventDispatcher&) = delete;

    private:
        class EPollEvents {
        public:
            explicit EPollEvents(uint32_t event);

        private:
            void mod(core::EventReceiver* eventReceiver, uint32_t events);

        public:
            void add(core::EventReceiver* eventReceiver);
            void del(core::EventReceiver* eventReceiver);

            void modOn(core::EventReceiver* eventReceiver);
            void modOff(core::EventReceiver* eventReceiver);

            void compress();

            int getEPFd() const;
            epoll_event* getEvents();

            int getMaxEvents() const;

            void printStats();

        private:
            int epfd;
            std::vector<epoll_event> ePollEvents;
            uint32_t interestCount;

            uint32_t events;
        };

        void modAdd(EventReceiver* eventReceiver) override;
        void modDel(EventReceiver* eventReceiver) override;
        void modOn(EventReceiver* eventReceiver) override;
        void modOff(EventReceiver* eventReceiver) override;

    public:
        explicit DescriptorEventDispatcher(uint32_t events);

        int getInterestCount() const;

        int getEPFd() const;

        void dispatchActiveEvents(const utils::Timeval& currentTime);
        void unobserveDisabledEvents(const utils::Timeval& currentTime);

    private:
        EPollEvents ePollEvents;
    };

} // namespace core::epoll

#endif // CORE_EPOLL_DESCRIPTOREVENTDISPATCHER_H
