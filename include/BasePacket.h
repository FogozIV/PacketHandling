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
#define ntohs(x) htons(x)
#endif
#ifndef htonl
#define htonl(x) ((1==htons(1)) ? (x) : ((uint16_t)htons((x) & 0xFFFF) << 16) | htons((x) >> 16))
#define ntohl(x) htonl(x)
#endif
#ifndef htonll
#define htonll(x) ((1 == htons(1)) ? (x) : \
    (((uint64_t)htonl((uint32_t)((x) >> 32))) | \
    ((uint64_t)htonl((uint32_t)(x)) << 32)))
#define ntohll(x) htonll(x)
#endif
#define PACKET(name, enum_value, ...) enum_value,
enum PacketType {
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST
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

    virtual void executeCallbacks(ARG_CHECK_PACKET ) = 0;
};
#define DEFINE_CALLBACKS(name) std::vector<std::function<bool(std::shared_ptr<name>COMMA_MACRO(ARG_CHECK_PACKET))> > name::callbacks;
#define DECLARE_CALLBACKS(name) static std::vector<std::function<bool(std::shared_ptr<name>COMMA_MACRO(ARG_CHECK_PACKET))> > callbacks;
#define CALL_CALLBACKS(name) \
void executeCallbacks(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET) override {\
    for (auto it = callbacks.begin(); it != callbacks.end(); ) {\
        bool result = (*it)(getShared()COMMA_MACRO(ARG_NAME_CHECK_PACKET));\
        if(result){\
            it = callbacks.erase(it);            \
        }else{\
            ++it;\
        }\
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
