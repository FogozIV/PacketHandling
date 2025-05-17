//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/OneArgPacket.h"
#define PACKET(name, e_name, data_type, instance)\
const packet_size_type name::packetToBuffer(packet_raw_type & vector) const {\
     auto iterator = std::back_inserter(vector); \
     packet_utility_v2::write(iterator, instance, vector.end());\
     return vector.size();\
}

ONE_ARG_PACKET_LIST

#undef PACKET