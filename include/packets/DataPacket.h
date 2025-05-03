//
// Created by fogoz on 03/05/2025.
//

#ifndef DATAPACKET_H
#define DATAPACKET_H
#include "BasePacket.h"
#include "utils/PacketUtility.h"


class DataPacket : public BasePacket<DataPacket> {
    std::vector<std::uint8_t> data;
public:
    explicit DataPacket();

    DataPacket(std::vector<std::uint8_t> data);

    DECLARE_CALLBACKS(DataPacket)

    std::vector<std::uint8_t> getData() const;

    const packet_size_type packetToBuffer(packet_raw_type& buffer) const override;

    static std::shared_ptr<DataPacket> create(const packet_raw_type& vector){
        std::shared_ptr<DataPacket> result = std::make_shared<DataPacket>();
        packet_size_type offset = 0;
        packet_utility::read(result->data, vector, offset);
        return result;
    }
    CALL_CALLBACKS(DataPacket)
};



#endif //DATAPACKET_H
