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

#ifndef EXPRESS_ROUTER_H
#define EXPRESS_ROUTER_H

#include "express/Next.h"      // IWYU pragma: export
#include "express/Request.h"   // IWYU pragma: export
#include "express/Response.h"  // IWYU pragma: export
#include "express/RootRoute.h" // IWYU pragma: export

namespace express {
    class Router;
} // namespace express

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <functional>
#include <memory>
#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#define MIDDLEWARE(req, res, next)                                                                                                         \
    ([[maybe_unused]] express::Request & (req), [[maybe_unused]] express::Response & (res), [[maybe_unused]] express::Next & (next))

#define APPLICATION(req, res) ([[maybe_unused]] express::Request & (req), [[maybe_unused]] express::Response & (res))

#define DECLARE_ROUTER_REQUESTMETHOD(METHOD)                                                                                               \
    Route& METHOD(const Router& router);                                                                                                   \
    Route& METHOD(const std::string& relativeMountPath, const Router& router);                                                             \
    Route& METHOD(const std::function<void(Request & req, Response & res)>& lambda);                                                       \
    Route& METHOD(const std::string& relativeMountPath, const std::function<void(Request & req, Response & res)>& lambda);                 \
    Route& METHOD(const std::function<void(Request & req, Response & res, Next & next)>& lambda);                                          \
    Route& METHOD(const std::string& relativeMountPath, const std::function<void(Request & req, Response & res, Next & next)>& lambda);    \
    template <typename... Lambdas>                                                                                                         \
    Route& METHOD(const std::function<void(Request & req, Response & res)>& lambda, Lambdas... lambdas);                                   \
    template <typename... Lambdas>                                                                                                         \
    Route& METHOD(                                                                                                                         \
        const std::string& relativeMountPath, const std::function<void(Request & req, Response & res)>& lambda, Lambdas... lambdas);       \
    template <typename... Lambdas>                                                                                                         \
    Route& METHOD(const std::function<void(Request & req, Response & res, Next & next)>& lambda, Lambdas... lambdas);                      \
    template <typename... Lambdas>                                                                                                         \
    Route& METHOD(const std::string& relativeMountPath,                                                                                    \
                  const std::function<void(Request & req, Response & res, Next & next)>& lambda,                                           \
                  Lambdas... lambdas);

namespace express {

    class Router /*: protected express::dispatcher::Route*/ {
    public:
        Router();

        DECLARE_ROUTER_REQUESTMETHOD(use)
        DECLARE_ROUTER_REQUESTMETHOD(all)
        DECLARE_ROUTER_REQUESTMETHOD(get)
        DECLARE_ROUTER_REQUESTMETHOD(put)
        DECLARE_ROUTER_REQUESTMETHOD(post)
        DECLARE_ROUTER_REQUESTMETHOD(del)
        DECLARE_ROUTER_REQUESTMETHOD(connect)
        DECLARE_ROUTER_REQUESTMETHOD(options)
        DECLARE_ROUTER_REQUESTMETHOD(trace)
        DECLARE_ROUTER_REQUESTMETHOD(patch)
        DECLARE_ROUTER_REQUESTMETHOD(head)

    protected:
        std::shared_ptr<RootRoute> rootRoute = nullptr;

        friend class Route;
        friend class RootRoute;
    };

} // namespace express

#include "express/Router.hpp" // IWYU pragma: export

#endif // EXPRESS_ROUTER_H
