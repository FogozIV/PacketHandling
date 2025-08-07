//
// Created by fogoz on 03/05/2025.
//

#ifndef BASEPACKET_H
#define BASEPACKET_H
#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include "packets/PacketDefinition.h"
#include <utils/NetworkEndianness.h>
#ifdef TEENSY41
#include "Teensy41_AsyncTCP.hpp"
#endif
typedef uint16_t packet_id_type;
typedef uint16_t packet_size_type;
typedef int16_t bidirectional_offset_type;
typedef std::vector<uint8_t> packet_raw_type;





#define PACKET(name, enum_value, ...) enum_value,
enum PacketType {
    PACKETS
};
#undef PACKET
class IPacket {
protected:
    PacketType type;
public:
     packet_id_type getPacketID() const {
        return type;
    }

    virtual const packet_size_type packetToBuffer(packet_raw_type& vector) const = 0;
};
template<typename Derived>
class BasePacket : public std::enable_shared_from_this<Derived>, public IPacket{
public:
    virtual ~BasePacket() = default;

    std::shared_ptr<Derived> getShared() {
        return this->shared_from_this();
    }

};

#endif //BASEPACKET_H
