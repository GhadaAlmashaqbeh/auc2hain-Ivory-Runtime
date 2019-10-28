#include <cryptoTools/Common/Defines.h>
#include <deque>


namespace osuCrypto {
    class OfflineSocket {
        public:
            std::deque<u8> q_u8;
            std::deque<block> q_block;

        OfflineSocket() {}

        ~OfflineSocket() {}

        void send(const std::vector<block> data, u64 size);

        void asyncSend(const std::vector<block> data);

        void asyncSendCopy(const u8* data, u64 size); 

        void recv(u8* data, u64 size);

        void recv(u8* data);

        // std::deque<block>& get_q() {
        //     return q;
        // }

        // void set_other_q(std::deque<block>* other_q) {
        //     q_other = other_q;
        // }
    
    };
}