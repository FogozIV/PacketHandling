//
// Created by fogoz on 26/05/2024.
//

#ifndef NETWORKPACKET_PACKETUTILITY_H
#define NETWORKPACKET_PACKETUTILITY_H

#include <vector>
#include <string>
#include <BasePacket.h>
#include <type_traits>
#include <string>
#include <iterator>

#ifdef __WIN32
#include<iostream>
#endif
namespace packet_utility_v2 {
    template <typename Iterator>
struct is_back_inserter : std::false_type {};

    template <typename Container>
    struct is_back_inserter<std::back_insert_iterator<Container>> : std::true_type {};

#define DO_NOTHING(x) x

    template<typename T>
    struct is_supported_integer : std::false_type {};
    #define RAPID \
    A(int8_t, DO_NOTHING)\
    A(int16_t, htons)\
    A(int32_t, htonl)\
    A(int64_t, htonll)\
    A(uint8_t, DO_NOTHING)\
    A(uint16_t, htons)\
    A(uint32_t, htonl)\
    A(uint64_t, htonll)

    #define A(type, ...) template<> struct is_supported_integer<type> : std::true_type {};

    RAPID
    #undef A

    #define A(type, processor)\
    inline type process(type data) {\
        return processor(data);\
    }

    RAPID
    #undef A

    #undef RAPID
    template<typename T>
    struct is_supported_number : is_supported_integer<T> {};

    template<typename T>
    struct is_supported_vector_arg : is_supported_number<T>{};
    template<>
    struct is_supported_vector_arg<std::string> : std::true_type {};

    //Work for all integers
    template<typename T, typename Iterator, typename = typename std::enable_if<is_supported_integer<T>::value>::type>
    bool read(T& data, Iterator& current, Iterator end){
        size_t dataSize = sizeof(T);
        if (std::distance(current, end) < static_cast<std::ptrdiff_t>(dataSize)) {
            return false;
        }
        auto* d = reinterpret_cast<uint8_t *>(&data);
        for (uint16_t i = 0; i < dataSize; i++) {
            d[i] = *current++;
        }
        data = process(data);
        return true;
    }

    template<typename T,  typename Iterator, typename = typename std::enable_if<is_supported_vector_arg<T>::value>::type>
    bool read(std::vector<T>& result, Iterator& current, Iterator end) {
        uint16_t size;
        if (!packet_utility_v2::read(size, current, end)) {
            return false;
        }
        for (uint16_t i = 0; i < size; i++) {
            T tmp;
            if (!packet_utility_v2::read(tmp, current, end))
                return false;
            result.push_back(std::move(tmp));
        }
        return true;
    }

    template<typename Iterator>
    bool read(std::string& result, Iterator& current, Iterator end);



    //Work for all integers
    template<typename T, typename Iterator, typename Iterator_end ,typename = typename std::enable_if<is_supported_integer<T>::value>::type>
    bool write(Iterator& iterator, T value, Iterator_end end){
        value = process(value);
        auto dataPtr = reinterpret_cast<const uint8_t*>(&value);
        if constexpr (is_back_inserter<Iterator>::value) {
            //For back inserter no check
        }else {
            //for normal iterator check
            if (std::distance(iterator, end) < static_cast<std::ptrdiff_t>(sizeof(T))) {
                return false;
            }
        }
        std::copy_n(dataPtr, sizeof(T), iterator);
        return true;
    }
    template<typename Iterator, typename Iterator_end>
    bool write(Iterator& iterator, std::string value, Iterator_end end);

    template<typename T,  typename Iterator, typename Iterator_end, typename = typename std::enable_if<is_supported_vector_arg<T>::value>::type>
    inline bool write(Iterator& iterator, std::vector<T> value, Iterator_end end) {
        if (!packet_utility_v2::write(iterator, (uint16_t)value.size(), end)) {
            return false;
        }
        for (size_t i = 0;i < value.size(); i++) {
            if (!packet_utility_v2::write(iterator, value[i], end)) {
                return false;
            }
        }
        return true;

    }
}


#endif //NETWORKPACKET_PACKETUTILITY_H
