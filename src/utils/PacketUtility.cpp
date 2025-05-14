//
// Created by fogoz on 26/05/2024.
//

#include "../../include/utils/PacketUtility.h"
#ifdef __WIN32__
#include <iostream>
#endif


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



