#ifndef OFFLINESOCKET_H
#define OFFLINESOCKET_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "cryptoTools/Network/IOService.h"
#include "cryptoTools/Network/Session.h"
#include "cryptoTools/Common/Log.h"
#include "cryptoTools/Common/Timer.h"
#include "ivory/Runtime/ShGc/ShGcRuntime.h"
#include "ivory/Runtime/sInt.h"
#include "ivory/Runtime/Party.h"
#include "cryptoTools/Crypto/PRNG.h"
#include "cryptoTools/Common/CLP.h"



namespace osuCrypto
{

class OfflineSocketSendOnly {
public:
    std::vector<uint8_t> &mBuffer;

    // WARNING: the caller needs to ensure that buffer is exists the full lifetime of OfflineSocketSendOnly
    explicit OfflineSocketSendOnly(std::vector<uint8_t> &buffer) : mBuffer(buffer) {}

    void send(const uint8_t * data, uint64_t size) {
        mBuffer.insert(mBuffer.end(), data, data+size);
    }

    void recv(uint8_t * data, uint64_t size) {
        throw new std::runtime_error("OfflineSocketSendOnly cannot receive data.");
    }

    template<typename F>
    void asyncCancel(F&& fn) {
        throw new std::runtime_error("asyncCancel not supported");
    }
};

class OfflineSocketRecvOnly {
public:
    size_t mPos;
    std::__1::vector<u8> mBuffer;

    // TODO: lots of copies for nothing, we should improve on this
    explicit OfflineSocketRecvOnly(std::__1::vector<u8> buffer) : mPos(0), mBuffer(buffer){}

    void send(const u8* data, u64 size) {
        throw new std::runtime_error("OfflineSocketSendOnly cannot receive data.");
    }

    void recv(u8* data, u64 size) {
        if(mPos + size > mBuffer.size()) {
            throw new std::runtime_error(
                    "Reading more than allowed: \n"
                    "  current pos:  " + std::to_string(mPos) +
                    "\n"
                    "  size to read: " + std::to_string(size) +
                    "\n"
                    "  total size:   " + std::to_string(mBuffer.size()) +
                    "\n"
                    );
        }
        std::copy(mBuffer.begin() + mPos, mBuffer.begin() + mPos + size, data);
        mPos += size;
    }

    template<typename F>
    void asyncCancel(F&& fn) {
        throw new std::runtime_error("asyncCancel not supported");
    }
};
}


#endif //OFFLINESOCKET_H
