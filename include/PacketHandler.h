//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#include <functional>
#include <memory>

#include "BasePacket.h"
#include "packets/DataPacket.h"
#include "packets/PingPacket.h"
#include "packets/PongPacket.h"
#include "utils/PacketUtility.h"
#include "utils/CRC.h"
#include "packets/PacketDefinition.h"
#include "packets/NoContentPacket.h"
#include "packets/OneArgPacket.h"

enum CheckStatus {
    WAITING_LENGTH, WAITING_DATA, BAD_CRC, EXECUTED_PACKET, BAD_PACKET_ID, PACKET_TOO_SMALL, NULL_PTR_RETURN
};

#undef PACKET
#define PACKET(name, e_name, ...) name::create,

inline std::function<std::shared_ptr<IPacket>(std::vector<uint8_t>::iterator&, std::vector<uint8_t>::iterator)> packetConstructorV2[] = {
    PingPacket::create,
    PongPacket::create,
    DataPacket::create,
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST
};



class PacketHandler {
    packet_raw_type buffer;
    void shiftBuffer(packet_size_type size) {
        std::move(buffer.begin() + size, buffer.end(), buffer.begin());
        buffer.resize(buffer.size() - size);
    }

public:
    [[nodiscard]] packet_raw_type getBuffer() const;

    void receiveData(const uint8_t* data, size_t size);

    void receiveData(const std::vector<uint8_t>& data);

    std::tuple<CheckStatus, std::shared_ptr<IPacket>> checkPacket(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET);

    std::tuple<CheckStatus, std::shared_ptr<IPacket>> checkPacketV2(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET);

    std::vector<uint8_t> createPacket(std::shared_ptr<IPacket> packet);

    std::vector<uint8_t> createPacket(const IPacket& packet);

    std::vector<uint8_t> createPacketV2(const IPacket& packet);
};


#endif //PACKETHANDLER_H
