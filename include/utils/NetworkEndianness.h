//
// Created by fogoz on 07/08/2025.
//

#ifndef PACKETHANDLING_NETWORKENDIANNESS_H
#define PACKETHANDLING_NETWORKENDIANNESS_H
#include <cstdint>
#include <type_traits>

constexpr bool is_little_endian() {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#else
    union {
        uint16_t i;
        uint8_t c[2];
    } u = {1};
    return u.c[0] == 1;
#endif
}

inline uint16_t byte_swap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

inline uint32_t byte_swap32(uint32_t x) {
    return (x >> 24) |
           ((x >> 8) & 0x0000FF00) |
           ((x << 8) & 0x00FF0000) |
           (x << 24);
}

inline uint64_t byte_swap64(uint64_t x) {
    return (static_cast<uint64_t>(byte_swap32(x & 0xFFFFFFFF)) << 32) | byte_swap32(x >> 32);
}

// Generic byte swap for signed/unsigned
template <typename T>
inline T byte_swap(T value) {
    using U = std::make_unsigned_t<T>;
    U raw = static_cast<U>(value);
    if constexpr (sizeof(T) == 2)
        return static_cast<T>(byte_swap16(static_cast<uint16_t>(raw)));
    else if constexpr (sizeof(T) == 4)
        return static_cast<T>(byte_swap32(static_cast<uint32_t>(raw)));
    else if constexpr (sizeof(T) == 8)
        return static_cast<T>(byte_swap64(static_cast<uint64_t>(raw)));
    else
        static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "Unsupported size for byte_swap");
}

// Generic hton / ntoh
template <typename T>
inline T host_to_network(T value) {
    return is_little_endian() ? byte_swap(value) : value;
}

template <typename T>
inline T network_to_host(T value) {
    return is_little_endian() ? byte_swap(value) : value;
}

// Macros for C-style fallback compatibility
#ifndef htons
#define htons(x) host_to_network(x)
#define ntohs(x) network_to_host(x)
#endif

#ifndef htonl
#define htonl(x) host_to_network(x)
#define ntohl(x) network_to_host(x)
#endif

#ifndef htonll
#define htonll(x) host_to_network(x)
#define ntohll(x) network_to_host(x)
#endif



#endif //PACKETHANDLING_NETWORKENDIANNESS_H