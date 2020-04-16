#include <iostream>
#include <string.h>

#include "ConnectedSocket.h"
#include "SocketMultiplexer.h"
#include "ServerSocket.h"
#include "FileReader.h"


ConnectedSocket::ConnectedSocket(int csFd, ServerSocket* ss) : Socket(csFd), serverSocket(ss) {
}


ConnectedSocket::~ConnectedSocket() {
}


InetAddress& ConnectedSocket::getRemoteAddress() {
    return remoteAddress;
}


void ConnectedSocket::setRemoteAddress(const InetAddress& remoteAddress) {
    this->remoteAddress = remoteAddress;
}


void ConnectedSocket::write(const char* buffer, int size) {
    writeBuffer.append(buffer, size);
    SocketMultiplexer::instance().getWriteManager().manageSocket(this);
}


void ConnectedSocket::write(const std::string& junk) {
    writeBuffer += junk;
    SocketMultiplexer::instance().getWriteManager().manageSocket(this);
}


void ConnectedSocket::writeLn(const std::string& junk) {
    writeBuffer += junk + "\r\n";
    SocketMultiplexer::instance().getWriteManager().manageSocket(this);
}


void ConnectedSocket::sendFile(const std::string& file) {
    FileReader::read(file,
                     [&] (unsigned char* data, int length) -> void {
                         if (length > 0) {
                             this->write((char*) data, length);
                         } else {
                             this->close();
                         }
                     },
                     [&] (int err) -> void {
                         std::cout << "Error: " << strerror(err) << std::endl;
                         this->close();
                     });
}


void ConnectedSocket::close() {
    SocketMultiplexer::instance().getReadManager().unmanageSocket(this);
}


void ConnectedSocket::clearReadBuffer() {
    readBuffer.clear();
}


void ConnectedSocket::writeEvent() {
    ssize_t ret = ::send(this->getFd(), writeBuffer.c_str(), (writeBuffer.size() < 4096) ? writeBuffer.size() : 4096, 0);
    
    if (ret >= 0) {
        writeBuffer.erase(0, ret);
        if (writeBuffer.empty()) {
            SocketMultiplexer::instance().getWriteManager().unmanageSocket(this);
        }
    } else {
        SocketMultiplexer::instance().getWriteManager().unmanageSocket(this);
    }
}
