//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/OneArgPacket.h"
#define PACKET(name, e_name, data_type, instance)\
DEFINE_CALLBACKS(name)\
const packet_size_type name::packetToBuffer(packet_raw_type & vector) const {\
    packet_utility::write(vector, instance);\
return vector.size();\
}

ONE_ARG_PACKET_LIST

#undef PACKET