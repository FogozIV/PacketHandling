//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/PingPacket.h"

DEFINE_CALLBACKS(PingPacket)
uint64_t PingPacket::getUniqueID() const {
    return unique_id;
}

const packet_size_type PingPacket::packetToBuffer(packet_raw_type& vector) const {
    packet_utility::write(vector, getPacketID());
    return packet_utility::write(vector, unique_id);
}