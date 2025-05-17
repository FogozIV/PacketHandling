//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#include <algorithm>
#include <functional>
#include <memory>
#include <tuple>

#include "BasePacket.h"
#include "utils/CRC.h"
#include "packets/PacketDefinition.h"
#include "packets/NoContentPacket.h"
#include "packets/OneArgPacket.h"
#include "iterator"

#define CHECK_STATUS_STRINGS \
    CHK_STATUS(WAITING_LENGTH) \
    CHK_STATUS(WAITING_DATA) \
    CHK_STATUS(BAD_CRC) \
    CHK_STATUS(EXECUTED_PACKET)\
    CHK_STATUS(BAD_PACKET_ID) \
    CHK_STATUS(PACKET_TOO_SMALL) \
    CHK_STATUS(NULL_PTR_RETURN)\
    CHK_STATUS(CRC_ISSUE) \
    CHK_STATUS(UNABLE_TO_READ_MAGIC)\
    CHK_STATUS(BAD_MAGIC_AND_NOT_FOUND)\
    CHK_STATUS(BAD_MAGIC_AND_FOUND)\
    CHK_STATUS(FOUND_NEW_PACKET)


#define CHK_STATUS(name) name,
enum CheckStatus {
    CHECK_STATUS_STRINGS
};
#undef CHK_STATUS

#define SEARCH_STATUS_STRINGS \
    SRCH_STATUS(NOTHING)\
    SRCH_STATUS(NO_MAGIC_FOUND)\
    SRCH_STATUS(UNKNOWN_ERROR_READING_MAGIC)\
    SRCH_STATUS(POSSIBLE_PACKET_FOUND_LENGTH_TOO_SMALL)\
    SRCH_STATUS(BAD_PACKET_ID_LOOK_UP)\
    SRCH_STATUS(UNABLE_TO_READ_CRC_LOOK_UP)\
    SRCH_STATUS(BAD_CRC_LOOK_UP)\
    SRCH_STATUS(GOOD_PACKET_FOUND)
#define SRCH_STATUS(name) name,
enum SearchStatus{
    SEARCH_STATUS_STRINGS
};
#undef SRCH_STATUS


constexpr uint64_t PACKET_MAGIC = 0xFEEDFACECAFEBEEF;

#undef PACKET
#define PACKET(name, e_name, ...) name::create,

inline std::vector<std::function<std::shared_ptr<IPacket>(packet_raw_type::iterator& current, packet_raw_type::iterator it_end)>> packetConstructors{
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST
};
#undef PACKET
class PacketHandler {
    packet_raw_type buffer;
    void shiftBuffer(packet_size_type size) {
        std::move(buffer.begin() + size, buffer.end(), buffer.begin());
        buffer.resize(buffer.size() - size);
    }
    template<typename Iterator>
    void shiftBuffer(Iterator last_iterator) {
        auto new_size = std::distance(last_iterator, buffer.end());
        std::move(last_iterator, buffer.end(), buffer.begin());
        buffer.resize(new_size);
    }

    auto findMagic(packet_raw_type::iterator it) {
        uint64_t magic = packet_utility_v2::process(PACKET_MAGIC);
        auto magic_bytes = (uint8_t*)(&magic);
        return std::search(it, buffer.end(), magic_bytes, magic_bytes+sizeof(PACKET_MAGIC));
    }

    bool shiftBufferToMagic() {
        if (buffer.begin() == buffer.end()) {
            return false;
        }
        auto it = findMagic(buffer.begin() + 1);
        bool ret = true;
        if (it == buffer.end()) {
            uint8_t shift_amount = std::min(buffer.size(), sizeof(PACKET_MAGIC) - 1);
            it = std::prev(it, shift_amount);
            ret = false;
        }
        shiftBuffer(it);
        return ret;
    }


public:
    [[nodiscard]] packet_raw_type getBuffer() const;

    packet_raw_type& getBufferRef();

    void receiveData(const uint8_t* data, size_t size);

    void receiveData(const std::vector<uint8_t>& data);

    //[PACKET_MAGIC][LENGTH][ID][...PAYLOAD...][CRC]
    std::tuple<CheckStatus, std::shared_ptr<IPacket>> checkPacket();

    std::tuple<SearchStatus, packet_raw_type::iterator> searchPacket(packet_raw_type::iterator it_begin, SearchStatus search= NOTHING);

    std::vector<uint8_t> createPacket(std::shared_ptr<IPacket> packet);

    std::vector<uint8_t> createPacket(const IPacket& packet);
};



#endif //PACKETHANDLER_H
