//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/PongPacket.h"

DEFINE_CALLBACKS(PongPacket)
uint64_t PongPacket::getUniqueID() const {
    return unique_id;
}

const packet_size_type PongPacket::packetToBuffer(packet_raw_type & vector) const {
    return packet_utility::write(vector, unique_id);
}
