//
// Created by fogoz on 26/05/2024.
//

#ifndef NETWORKPACKET_PACKETUTILITY_H
#define NETWORKPACKET_PACKETUTILITY_H

#include <vector>
#include <string>
#include <set>
#include "../BasePacket.h"
#include <cstring>
#include <type_traits>

namespace packet_utility{

    packet_size_type read(std::vector<std::string>& result, const packet_raw_type& packet, packet_size_type offset);

    packet_size_type read(uint8_t& value, const packet_raw_type& packet, packet_size_type offset);

    packet_size_type read(uint16_t& value, const packet_raw_type& packet, packet_size_type offset);

    packet_size_type read(uint32_t& value, const packet_raw_type& packet, packet_size_type offset);

    packet_size_type read(uint64_t& value, const packet_raw_type& packet, packet_size_type offset);

    packet_size_type read(std::string& value, const packet_raw_type& packet, packet_size_type offset);
    template<typename T, typename std::enable_if<
            std::is_same<uint8_t, T>::value ||
            std::is_same<uint16_t, T>::value ||
            std::is_same<uint32_t, T>::value ||
            std::is_same<uint64_t, T>::value, int>::type = 0>
    inline packet_size_type read(std::vector<T>& value, const packet_raw_type& packet, packet_size_type offset){
        uint16_t size;
        offset = packet_utility::read(size, packet, offset);
        value.resize(size);
        for (uint16_t i = 0; i < size; i++) {
            T tmp;
            offset = packet_utility::read(tmp, packet, offset);
            value[i] = tmp;
        }
        return offset;
    }


    packet_size_type write(packet_raw_type& packet, const std::vector<std::string>& array);

    packet_size_type write(packet_raw_type& packet, uint8_t value, bidirectional_offset_type offset = 0);

    packet_size_type write(packet_raw_type& packet, uint16_t value, bidirectional_offset_type offset = 0);

    packet_size_type write(packet_raw_type& packet, uint32_t value, bidirectional_offset_type offset = 0);

    packet_size_type write(packet_raw_type& packet, uint64_t value, bidirectional_offset_type offset = 0);

    packet_size_type write(packet_raw_type& packet, std::string value, bidirectional_offset_type offset = 0);


    template<typename T, typename std::enable_if<std::is_same<uint8_t, T>::value || std::is_same<uint16_t, T>::value ||
    std::is_same<uint32_t, T>::value || std::is_same<uint64_t, T>::value, int>::type = 0>
    inline packet_size_type packet_utility::write(packet_raw_type &packet, const std::vector<T>& value ,bidirectional_offset_type offset = 0){
        uint16_t size = value.size();
        packet_utility::write(packet, size);
        for (auto &tmp : value) {
            packet_utility::write(packet, tmp);
        }
        return packet.size();
    }


}


#endif //NETWORKPACKET_PACKETUTILITY_H
