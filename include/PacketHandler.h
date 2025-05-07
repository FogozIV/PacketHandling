//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#include <functional>
#include <memory>

#include "BasePacket.h"
#include "utils/CRC.h"
#include "packets/PacketDefinition.h"
#include "packets/NoContentPacket.h"
#include "packets/OneArgPacket.h"

enum CheckStatus {
    WAITING_LENGTH,
    WAITING_DATA,
    BAD_CRC,
    EXECUTED_PACKET,
    BAD_PACKET_ID,
    PACKET_TOO_SMALL,
    NULL_PTR_RETURN,
    CRC_ISSUE,
    UNABLE_TO_READ_MAGIC,
    BAD_MAGIC_NOT_FOUND,
    BAD_MAGIC_FOUND,

};
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

public:
    [[nodiscard]] packet_raw_type getBuffer() const;

    void receiveData(const uint8_t* data, size_t size);

    void receiveData(const std::vector<uint8_t>& data);

    std::tuple<CheckStatus, std::shared_ptr<IPacket>> checkPacket(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET);

    std::vector<uint8_t> createPacket(std::shared_ptr<IPacket> packet);

    std::vector<uint8_t> createPacket(const IPacket& packet);
};



#endif //PACKETHANDLER_H
