//
// Created by fogoz on 03/05/2025.
//

#include "PacketHandler.h"

packet_raw_type PacketHandler::getBuffer() const {
    return buffer;
}

void PacketHandler::receiveData(const uint8_t *data, size_t size) {
    buffer.insert(buffer.end(), data, data + size);
}

void PacketHandler::receiveData(const std::vector<uint8_t> &data) {
    buffer.insert(buffer.end(), data.begin(), data.end());
}

std::tuple<CheckStatus, std::shared_ptr<IPacket>> PacketHandler::checkPacket() {
    if (buffer.size() < sizeof(packet_size_type)) {
        return std::make_tuple(WAITING_LENGTH, nullptr);
    }
    uint16_t packetLength;
    packet_size_type offset = packet_utility::read(packetLength, buffer, 0);
    if (packetLength > buffer.size()) {
        return std::make_tuple(WAITING_DATA, nullptr);
    }
    if (packetLength < 6) {
        shiftBuffer(packetLength);
        return {PACKET_TOO_SMALL, nullptr};
    }
    uint16_t packetId;
    offset = packet_utility::read(packetId, buffer, offset);
    if (packetId >= packetConstructors.size()) {
        return {BAD_PACKET_ID, nullptr};
    }
    uint32_t crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(buffer.data(), packetLength-4);
    uint32_t crc_received;
    packet_utility::read(crc_received, buffer, packetLength-4);
    if (crc != crc_received) {
        shiftBuffer(packetLength);
        return std::make_tuple(BAD_CRC, nullptr);
    }
    packet_raw_type packetData(buffer.begin() + offset, buffer.begin() + packetLength - 4 - sizeof(packet_size_type));
    std::shared_ptr<IPacket> packet = packetConstructors[packetId](packetData);
    if (packet == nullptr) {
        shiftBuffer(packetLength);
        return std::make_tuple(NULL_PTR_RETURN, nullptr);
    }
    packet->executeCallbacks();
    shiftBuffer(packetLength);
    return std::make_tuple(EXECUTED_PACKET, packet);
}

std::vector<uint8_t> PacketHandler::createPacket(std::shared_ptr<IPacket> packet) {
    return createPacket(*packet);
}

packet_raw_type PacketHandler::createPacket(const IPacket &packet) {
    packet_raw_type result;
    result.resize(sizeof(packet_size_type));
    packet_utility::write(result, packet.getPacketID());
    packet_size_type packetLength = packet.packetToBuffer(result);
    packetLength += 4; //crc
    packet_utility::write(result, packetLength, -result.size());
    uint32_t crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(result);
    packet_utility::write(result, crc);
    return result;
}
