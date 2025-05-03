//
// Created by fogoz on 03/05/2025.
//

#ifndef NOCONTENTPACKET_H
#define NOCONTENTPACKET_H
#include "BasePacket.h"
#include "PacketDefinition.h"
#undef PACKET
#define PACKET(name, e_name) \
class name : public BasePacket<name> {\
public:\
    explicit name() {\
        type = PacketType::e_name;\
    }\
    DECLARE_CALLBACKS(name)\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(const packet_raw_type& vector){\
        auto result = std::make_shared<name>();\
        return result;\
    }\
    CALL_CALLBACKS(name)\
};

EMPTY_PACKET_LIST
#undef PACKET


#endif //NOCONTENTPACKET_H
