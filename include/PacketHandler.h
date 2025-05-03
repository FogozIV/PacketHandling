//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#include <functional>
#include <memory>

#include "BasePacket.h"
#include "packets/PingPacket.h"
#include "packets/PongPacket.h"
#include "utils/PacketUtility.h"
#include "utils/CRC.h"

enum CheckStatus {
    WAITING_LENGTH, WAITING_DATA, BAD_CRC, EXECUTED_PACKET, BAD_PACKET_ID, PACKET_TOO_SMALL
};

inline std::vector<std::function<std::shared_ptr<BasePacket>(const packet_raw_type& vector)>> packetConstructors{
    PingPacket::create,
    PongPacket::create,

};
class PacketHandler {
    packet_raw_type buffer;
    void shiftBuffer(packet_size_type size) {
        std::move(buffer.begin() + size, buffer.end(), buffer.begin());
        buffer.resize(buffer.size() - size);
    }

public:
    void receiveData(const uint8_t* data, size_t size) {
        buffer.insert(buffer.end(), data, data + size);
    }
    std::tuple<CheckStatus, std::shared_ptr<BasePacket>> checkPacket() {
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
        uint32_t crc = algoCRC_32.computeCRC(buffer.data(), packetLength-4);
        uint32_t crc_received;
        packet_utility::read(crc_received, buffer, packetLength-4);
        if (crc != crc_received) {
            shiftBuffer(packetLength);
            return std::make_tuple(BAD_CRC, nullptr);
        }
        packet_raw_type packetData(buffer.begin() + offset, buffer.begin() + packetLength - 4 - sizeof(packet_size_type));
        std::shared_ptr<BasePacket> packet = packetConstructors[packetId](packetData);
        packet->executeCallbacks(packet);
        shiftBuffer(packetLength);
        return std::make_tuple(EXECUTED_PACKET, packet);
    }

    std::vector<uint8_t> createPacket(std::shared_ptr<BasePacket> packet) {
        packet_raw_type result;
        packet_size_type packetLength = packet->packetToBuffer(result);
        packetLength += sizeof(packet_size_type); //taille
        packetLength += 4; //crc
        packet_utility::write(result, packetLength, -result.size());
        uint32_t crc = algoCRC_32.computeCRC(result);
        packet_utility::write(result, crc);
        return result;
    }
};



#endif //PACKETHANDLER_H
