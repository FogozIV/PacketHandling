//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/DataPacket.h"

#include "utils/PacketUtility.h"

DataPacket::DataPacket() {
}

DataPacket::DataPacket(std::vector<std::uint8_t> data): data(std::move(data)) {}

const packet_id_type DataPacket::getPacketID() const {
    return 0x02;
}

const packet_size_type DataPacket::packetToBuffer(packet_raw_type & buffer) const {
    packet_utility::write(buffer, getPacketID());
    return packet_utility::write(buffer, this->data);
}

std::shared_ptr<DataPacket> DataPacket::create(const packet_raw_type &vector) {
    std::shared_ptr<DataPacket> result = std::make_shared<DataPacket>();
    packet_size_type offset = 0;
    packet_utility::read(result->data, vector, offset);
    return result;
}
