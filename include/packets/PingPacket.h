//
// Created by fogoz on 03/05/2025.
//

#ifndef PINGPACKET_H
#define PINGPACKET_H
#include <memory>

#include "BasePacket.h"


class PingPacket : public BasePacket {
    uint64_t unique_id;
public:
    explicit PingPacket(uint64_t unique_id = UINT64_MAX)
        : unique_id(unique_id) {
    }
    DECLARE_CALLBACKS(PingPacket)

    const packet_id_type getPacketID() const override;

    const packet_size_type packetToBuffer(packet_raw_type&) const override;

    static std::shared_ptr<PingPacket> create(const packet_raw_type& vector);
    CALL_CALLBACKS(PingPacket)
};



#endif //PINGPACKET_H
