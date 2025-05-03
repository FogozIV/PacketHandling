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
#ifdef TEENSY41
#include "Teensy41_AsyncTCP.hpp"
#endif
typedef uint16_t packet_id_type;
typedef uint16_t packet_size_type;
typedef int16_t bidirectional_offset_type;
typedef std::vector<uint8_t> packet_raw_type;

#ifndef htons
inline uint16_t htons(uint16_t hostshort) {
    uint32_t data = 42;
    //LSB little-endian
    if (*((uint8_t*)&data) == 42) {
        uint8_t* ptr = (uint8_t*)&hostshort;
        std::swap(ptr[0], ptr[1]);
        return hostshort;
    }
    // MSB BIG ENDIAN
    return hostshort;
}
#define htonl(x) ((1==htons(1)) ? (x) : ((uint16_t)htons((x) & 0xFFFF) << 16) | htons((x) >> 16))
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)
#endif
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

enum PacketType {
    PING,
    PONG,
    DATA,
    EMPTY_PACKET_LIST
};

class IPacket {
protected:
    PacketType type;
public:
    virtual const packet_id_type getPacketID() const {
        return type;
    }

    virtual const packet_size_type packetToBuffer(packet_raw_type& vector) const = 0;

    virtual void executeCallbacks(ARG_CHECK_PACKET ) = 0;
};
#define DEFINE_CALLBACKS(name) std::vector<std::function<void(std::shared_ptr<name>COMMA_MACRO(ARG_CHECK_PACKET))>> name::callbacks;
#define DECLARE_CALLBACKS(name) static std::vector<std::function<void(std::shared_ptr<name>COMMA_MACRO(ARG_CHECK_PACKET))>> callbacks;
#define CALL_CALLBACKS(name) \
void executeCallbacks(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET) override {\
    for (auto a : callbacks) {\
        a(getShared()COMMA_MACRO(ARG_NAME_CHECK_PACKET));\
    }\
}
template<typename Derived>
class BasePacket : public std::enable_shared_from_this<Derived>, public IPacket{
public:
    virtual ~BasePacket() = default;

    std::shared_ptr<Derived> getShared() {
        return this->shared_from_this();
    }

};

#endif //BASEPACKET_H
