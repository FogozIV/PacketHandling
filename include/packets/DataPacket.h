//
// Created by fogoz on 03/05/2025.
//

#ifndef DATAPACKET_H
#define DATAPACKET_H
#include "BasePacket.h"


class DataPacket : public BasePacket {
    std::vector<std::uint8_t> data;
public:
    explicit DataPacket();

    DataPacket(std::vector<std::uint8_t> data);

    DECLARE_CALLBACKS(DataPacket)

    const packet_id_type getPacketID() const override;

    const packet_size_type packetToBuffer(packet_raw_type& buffer) const override;

    static std::shared_ptr<DataPacket> create(const packet_raw_type& vector);
    CALL_CALLBACKS(DataPacket)
};



#endif //DATAPACKET_H
