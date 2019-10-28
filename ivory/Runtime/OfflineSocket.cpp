#include <cryptoTools/Network/OfflineSocket.h>
#include <cryptoTools/Common/Defines.h>
#include <cryptoTools/Common/BitVector.h>

namespace osuCrypto {
    void OfflineSocket::send(const std::vector<block> data, u64 size)
    {
        auto size_block = size / sizeof(block);
        for (u64 i = 0; i < size_block; i++) {
            q_block.push_back(data[i]);
        }
    }

    void OfflineSocket::asyncSend(const std::vector<block> data)
    {
        send(data, (u64) data.size() * sizeof(block));
    }

    void asyncSendCopy(const u8* data, u64 size)
    {
        std::vector<block> data_copy(data, data + (size / sizeof(block)));
        asyncSend(data_copy, (u64) size); 
    }

    void recv(u8* data, u64 size) {
        auto size_block = size / sizeof(block);
        for (u64 i = 0; i < size_block; i++) {
            *(data+i * (sizeof(block) / sizeof(u8))) = q_block.front();
            q_block.pop_front();
        }
    }

    void recv(std::vector<block>& data) {
        recv((u8*) data.data(), data.size() * sizeof(block));
    }

    void recv(BitVector& data) {
        for (u64 i = 0; i < data.size(); i++) {
            data[i] = q_block.front();
            q_block.pop_front();
        }
    }

};
