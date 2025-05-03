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
    WAITING_LENGTH, WAITING_DATA, BAD_CRC, EXECUTED_PACKET, BAD_PACKET_ID, PACKET_TOO_SMALL, NULL_PTR_RETURN
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
    void receiveData(const uint8_t* data, size_t size);

    void receiveData(const std::vector<uint8_t>& data);

    std::tuple<CheckStatus, std::shared_ptr<BasePacket>> checkPacket();

    std::vector<uint8_t> createPacket(std::shared_ptr<BasePacket> packet);
};



#endif //PACKETHANDLER_H
