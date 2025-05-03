//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/PingPacket.h"

#include "utils/PacketUtility.h"

const packet_id_type PingPacket::getPacketID() const {
    return 0x00;
}

const packet_size_type PingPacket::packetToBuffer(packet_raw_type& vector) const {
    packet_utility::write(vector, getPacketID());
    return packet_utility::write(vector, unique_id);
}

std::shared_ptr<PingPacket> PingPacket::create(const packet_raw_type& vector) {
    auto result = std::make_shared<PingPacket>();
    packet_size_type offset = 0;
    packet_utility::read(result->unique_id, vector, offset);
    return result;
}
