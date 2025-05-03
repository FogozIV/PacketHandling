//
// Created by fogoz on 03/05/2025.
//

#ifndef ONEARGPACKET_H
#define ONEARGPACKET_H
#include "PacketDefinition.h"
#include "BasePacket.h"
#include "utils/PacketUtility.h"
#undef PACKET
#define PACKET(name, e_name, data_type, instance) \
class name : public BasePacket<name> {\
    data_type instance; \
public:\
    explicit name() {\
        type = PacketType::e_name;\
    }\
    name (data_type instance){ \
        this->instance = instance;\
    }\
    DECLARE_CALLBACKS(name)\
    const data_type get##instance(){ return this->instance;}\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(const packet_raw_type& vector){\
    auto result = std::make_shared<name>();\
    packet_size_type offset = 0;\
    offset = packet_utility::read(result->instance, vector, offset);\
    return result;\
    }\
    CALL_CALLBACKS(name)\
};

ONE_ARG_PACKET_LIST
#undef PACKET


#endif //ONEARGPACKET_H
