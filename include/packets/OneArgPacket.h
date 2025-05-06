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
    name (data_type instance) : name(){ \
        this->instance = instance;\
    }\
    DECLARE_CALLBACKS(name)\
    const data_type get##instance(){ return this->instance;}\
    data_type& get##instance##Ref(){return this->instance;}\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        if(!packet_utility_v2::read(result->instance, current, it_end)) return nullptr;\
        return result;\
    }\
    CALL_CALLBACKS(name)\
};

ONE_ARG_PACKET_LIST
#undef PACKET


#endif //ONEARGPACKET_H
