//
// Created by fogoz on 03/05/2025.
//

#ifndef NOCONTENTPACKET_H
#define NOCONTENTPACKET_H
#include <memory>
#include <functional>
#include <vector>
#include "BasePacket.h"
#include "PacketDefinition.h"
#undef PACKET
#define PACKET(name, e_name) \
class name : public BasePacket<name> {\
public:\
    static constexpr PacketType getPacketID(){return PacketType::e_name;}\
    explicit name() {\
        type = name::getPacketID();\
    }\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        return result;\
    }\
};
EMPTY_PACKET_LIST
#undef PACKET


#endif //NOCONTENTPACKET_H
