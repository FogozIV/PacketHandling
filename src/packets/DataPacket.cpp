//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/DataPacket.h"

#include "utils/PacketUtility.h"

DEFINE_CALLBACKS(DataPacket)
DataPacket::DataPacket() {
    type = PacketType::DATA;
}

DataPacket::DataPacket(std::vector<std::uint8_t> data): data(std::move(data)) {}

std::vector<std::uint8_t> DataPacket::getData() const {
    return data;
}

const packet_size_type DataPacket::packetToBuffer(packet_raw_type & buffer) const {
    packet_utility::write(buffer, getPacketID());
    return packet_utility::write(buffer, this->data);
}