//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/NoContentPacket.h"

#define PACKET(name, e_name)\
const packet_size_type name::packetToBuffer(packet_raw_type & vector) const {\
    return vector.size();\
}
EMPTY_PACKET_LIST

#undef PACKET
