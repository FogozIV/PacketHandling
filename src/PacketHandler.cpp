//
// Created by fogoz on 03/05/2025.
//

#include "PacketHandler.h"
#ifdef __WIN32__
#include <iostream>
#endif
packet_raw_type PacketHandler::getBuffer() const {
    return buffer;
}

void PacketHandler::receiveData(const uint8_t *data, size_t size) {
    buffer.insert(buffer.end(), data, data + size);
}

void PacketHandler::receiveData(const std::vector<uint8_t> &data) {
    buffer.insert(buffer.end(), data.begin(), data.end());
}
std::tuple<CheckStatus, std::shared_ptr<IPacket>> PacketHandler::checkPacket(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET) {
    if (buffer.size() < sizeof(packet_size_type)) {
        return std::make_tuple(WAITING_LENGTH, nullptr);
    }
    uint16_t packetLength = -1;
    auto a = buffer.begin();;
    if (!packet_utility_v2::read(packetLength, a, buffer.end()) ||packetLength > buffer.size()) {
        return std::make_tuple(WAITING_DATA, nullptr);
    }
    if (packetLength < 6) {
        shiftBuffer(packetLength); //We think that it's corrupted data and discard it
        return {PACKET_TOO_SMALL, nullptr};
    }
    uint16_t packetId = -1;
    if (!packet_utility_v2::read(packetId, a, buffer.end()) || packetId >= std::size(packetConstructors)) {
        shiftBuffer(packetLength);
        return {BAD_PACKET_ID, nullptr};
    }
    uint32_t crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(buffer.data(), packetLength-4);
    uint32_t crc_received = -1;
    auto e = buffer.end();
    auto crc_e = std::prev(e, 4);
    if (!packet_utility_v2::read(crc_received, crc_e, buffer.end())) {
        shiftBuffer(packetLength);
        return {CRC_ISSUE, nullptr};
    }
    if (crc != crc_received) {
        shiftBuffer(packetLength);
        return std::make_tuple(BAD_CRC, nullptr);
    }
    std::shared_ptr<IPacket> packet = packetConstructors[packetId](a, buffer.end());
    if (packet == nullptr) {
        shiftBuffer(packetLength);
        return std::make_tuple(NULL_PTR_RETURN, nullptr);
    }
    packet->executeCallbacks(ARG_NAME_CHECK_PACKET);
    shiftBuffer(packetLength);
    return std::make_tuple(EXECUTED_PACKET, packet);

}

std::vector<uint8_t> PacketHandler::createPacket(std::shared_ptr<IPacket> packet) {
    return createPacket(*packet);
}


packet_raw_type PacketHandler::createPacket(const IPacket &packet) {
    packet_raw_type result;
    result.emplace_back(0xFF);
    result.emplace_back(0xFF);
    auto back_inserter = std::back_inserter(result);
    packet_utility_v2::write(back_inserter, packet.getPacketID(), result.end());
    packet.packetToBuffer(result); //considered linear operation (only forward motion in vector)
    packet_size_type packetLength = result.size() + 4; //length + CRC
    if (result.size() + 4 > (1ULL << (sizeof(packet_size_type) * 8 )) - 1) {
        return {};
    }
    auto b = result.begin();
    packet_utility_v2::write(b, packetLength, result.end());
    auto crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(result);
    packet_utility_v2::write(back_inserter, crc, result.end());
    return result;
}
