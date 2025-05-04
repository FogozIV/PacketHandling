//
// Created by fogoz on 26/05/2024.
//

#include "../../include/utils/PacketUtility.h"
#ifdef __WIN32__
#include <iostream>
#endif

packet_size_type packet_utility::read(std::vector<std::string> &result, const packet_raw_type &packet,
    packet_size_type offset) {
    uint16_t size;
    offset = packet_utility::read(size, packet, offset);
    for (uint16_t i = 0; i < size; i++) {
        std::string str;
        offset = packet_utility::read(str, packet, offset);
        result.push_back(str);
    }
    return offset;
}

packet_size_type packet_utility::read(uint8_t &value, const packet_raw_type &packet, packet_size_type offset) {
    auto index = packet.begin();
    index += offset;
    value = *index;
    return offset + 1;
}

packet_size_type packet_utility::read(uint16_t &value, const packet_raw_type &packet, packet_size_type offset) {
    auto index = packet.begin();
    index += offset;
    memcpy(&value, index.base(), sizeof(value));
    value = ntohs(value);
    return offset + sizeof(value);
}

packet_size_type packet_utility::read(uint32_t &value, const packet_raw_type &packet, packet_size_type offset) {
    auto index = packet.begin();
    index += offset;
    memcpy(&value, index.base(), sizeof(value));
    value = ntohl(value);
    return offset + sizeof(value);
}

packet_size_type packet_utility::read(uint64_t &value, const packet_raw_type &packet, packet_size_type offset) {
    auto index = packet.begin();
    index += offset;
    memcpy(&value, index.base(), sizeof(value));
    value = ntohll(value);
    return offset + sizeof(value);
}

packet_size_type packet_utility::read(std::string &value, const packet_raw_type &packet, packet_size_type offset) {
    uint16_t size;
    offset = packet_utility::read(size, packet, offset);
    value.resize(size);
    std::copy((packet.begin() + offset), packet.begin() + size + offset, value.begin());
    return offset + size;
}

packet_size_type packet_utility::write(packet_raw_type &packet, const std::vector<std::string> &array) {
    write(packet, (uint16_t) array.size());
    for (auto &str : array) {
        write(packet, str);
    }
    return packet.size();
}

packet_size_type packet_utility::write(packet_raw_type &packet, uint8_t value, bidirectional_offset_type offset) {
    packet.insert(packet.end() + offset, value);
    return packet.size();
}

packet_size_type packet_utility::write(packet_raw_type &packet, uint16_t value, bidirectional_offset_type offset) {
    value = htons(value);
    packet.insert(packet.end() + offset, (uint8_t*)&value, ((uint8_t*)&value)  + sizeof(value));
    return packet.size();
}

packet_size_type packet_utility::write(packet_raw_type &packet, uint32_t value, bidirectional_offset_type offset) {
    value  = htonl(value);
    packet.insert(packet.end() + offset, (uint8_t*)&value, ((uint8_t*)&value) + sizeof(value));
    return packet.size();
}

packet_size_type packet_utility::write(packet_raw_type &packet, uint64_t value) {
    value  = htonll(value);
    std::copy(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(value), back_inserter(packet));
    return packet.size();
}

packet_size_type packet_utility::write(packet_raw_type &packet, const std::string value, bidirectional_offset_type offset) {
    write(packet, (uint16_t) value.size());
    packet.insert(packet.end() + offset, value.begin(), value.end());
    return packet.size();
}


template<typename Iterator>
bool packet_utility_v2::read(std::string &result, Iterator &current, Iterator end) {
    uint16_t size;
    if (!read(size, current, end))
        return false;
    if (std::distance(current, end) < static_cast<std::ptrdiff_t>(size)) {
        return false;
    }
    result.resize(size);
    for (uint16_t i = 0; i < size; i++) {
        result[i] = *current++;
    }
    return true;
}

template<typename Iterator, typename Iterator_end>
bool packet_utility_v2::write(Iterator &iterator, std::string value, Iterator_end end) {
    uint16_t str_size = value.size();
    if constexpr (!is_back_inserter<Iterator>::value) {
        if (std::distance(iterator, end) < static_cast<std::ptrdiff_t>(str_size + 2)) {
            return false;
        }
    }
    write(iterator, str_size, end);
    std::copy(value.begin(), value.end(), iterator);
    return true;
}



