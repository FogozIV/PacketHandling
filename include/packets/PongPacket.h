//
// Created by fogoz on 03/05/2025.
//

#ifndef PONGPACKET_H
#define PONGPACKET_H
#include <cstdint>
#include <memory>

#include "BasePacket.h"
#include "utils/PacketUtility.h"


class PongPacket : public BasePacket<PongPacket> {
    uint64_t unique_id;
public:
    explicit PongPacket(uint64_t unique_id = UINT64_MAX)
        : unique_id(unique_id) {
        type = PacketType::PONG;
    }
    DECLARE_CALLBACKS(PongPacket)


    [[nodiscard]] uint64_t getUniqueID() const;

    const packet_size_type packetToBuffer(packet_raw_type&) const override;

    static std::shared_ptr<PongPacket> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {
        auto result = std::make_shared<PongPacket>();
        if (!packet_utility_v2::read(result->unique_id, current, it_end)) return nullptr;
        return result;
    }

    CALL_CALLBACKS(PongPacket)
};



#endif //PONGPACKET_H
