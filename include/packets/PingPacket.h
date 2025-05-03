//
// Created by fogoz on 03/05/2025.
//

#ifndef PINGPACKET_H
#define PINGPACKET_H
#include <memory>

#include "BasePacket.h"
#include "utils/PacketUtility.h"


class PingPacket : public BasePacket<PingPacket> {
    uint64_t unique_id;
public:
    explicit PingPacket(uint64_t unique_id = UINT64_MAX)
        : unique_id(unique_id){
        type = PacketType::PING;
    }
    DECLARE_CALLBACKS(PingPacket)

    [[nodiscard]] uint64_t getUniqueID() const;

    const packet_size_type packetToBuffer(packet_raw_type& buffer) const override;

    static std::shared_ptr<PingPacket> create(const packet_raw_type& vector){
        auto result = std::make_shared<PingPacket>();
        packet_size_type offset = 0;
        packet_utility::read(result->unique_id, vector, offset);
        return result;
    }
    CALL_CALLBACKS(PingPacket)
};



#endif //PINGPACKET_H
