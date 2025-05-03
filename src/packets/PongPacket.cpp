//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/PongPacket.h"

#include "utils/PacketUtility.h"

inline std::shared_ptr<PongPacket> PongPacket::create(const packet_raw_type &vector) {
    std::shared_ptr<PongPacket> result = std::make_shared<PongPacket>();
    packet_size_type offset = 0;
    offset = packet_utility::read(result->unique_id, vector, offset);
    return result;
}

const packet_id_type PongPacket::getPacketID() const {
    return 0x01;
}

const packet_size_type PongPacket::packetToBuffer(packet_raw_type & vector) const {
    packet_utility::write(vector, getPacketID());
    return packet_utility::write(vector, unique_id);
}
