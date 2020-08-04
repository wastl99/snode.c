#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cstring>
#include <netdb.h>
#include <sys/socket.h> // for AF_INET

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "socket/InetAddress.h"

InetAddress::InetAddress() {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress(const std::string& ipOrHostname, uint16_t port) {
    struct hostent* he = gethostbyname(ipOrHostname.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
}

InetAddress::InetAddress(const std::string& ipOrHostname) {
    struct hostent* he = gethostbyname(ipOrHostname.c_str());
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
}

InetAddress::InetAddress(in_port_t port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress(const struct sockaddr_in& addr) {
    memcpy(&this->addr, &addr, sizeof(struct sockaddr_in));
}

InetAddress::InetAddress(const InetAddress& ina) {
    memcpy(&this->addr, &ina.addr, sizeof(struct sockaddr_in));
}

InetAddress& InetAddress::operator=(const InetAddress& ina) {
    if (this != &ina) {
        memcpy(&this->addr, &ina.addr, sizeof(struct sockaddr_in));
    }

    return *this;
}

in_port_t InetAddress::port() const {
    return (ntohs(addr.sin_port));
}

const struct sockaddr_in& InetAddress::getSockAddr() const {
    return this->addr;
}
