//
// Created by fogoz on 03/05/2025.
//

#include "PacketHandler.h"

#include <utility>
#ifdef __WIN32__
#include <iostream>
#endif
packet_raw_type PacketHandler::getBuffer() const {
    return buffer;
}

packet_raw_type & PacketHandler::getBufferRef() {
    return buffer;
}

void PacketHandler::receiveData(const uint8_t *data, size_t size) {
    buffer.insert(buffer.end(), data, data + size);
}

void PacketHandler::receiveData(const std::vector<uint8_t> &data) {
    buffer.reserve(buffer.size() + data.size());
    buffer.insert(buffer.end(), data.begin(), data.end());
}

std::tuple<CheckStatus, std::shared_ptr<IPacket>> PacketHandler::checkPacket(ARG_CHECK_PACKET ARG_NAME_CHECK_PACKET) {
    uint16_t minimumPacketSize = sizeof(packet_size_type) + sizeof(PACKET_MAGIC) + sizeof(packet_id_type) + sizeof(uint32_t);
    if (buffer.size() < sizeof(packet_size_type) + sizeof(PACKET_MAGIC)) {
        return std::make_tuple(WAITING_LENGTH, nullptr);
    }
    uint16_t packetLength = -1;
    uint64_t magic = -1;
    auto a = buffer.begin();
    if (!packet_utility_v2::read(magic, a, buffer.end())){
        return std::make_tuple(UNABLE_TO_READ_MAGIC, nullptr);
    }
    if (magic != PACKET_MAGIC) {
        if (shiftBufferToMagic()) {
            return {BAD_MAGIC_AND_FOUND, nullptr};
        }
        return {BAD_MAGIC_AND_NOT_FOUND, nullptr};
    }

    if (!packet_utility_v2::read(packetLength, a, buffer.end()) ||packetLength > buffer.size()) {
        if (buffer.size() >= 2* minimumPacketSize) {
            auto [status, it] = searchPacket(a);
            if (status == GOOD_PACKET_FOUND) {
                shiftBuffer(it);
                return {FOUND_NEW_PACKET, nullptr};
            }
        }
        return std::make_tuple(WAITING_DATA, nullptr);
    }
    if (packetLength < minimumPacketSize) { //size of size, packet_magic, id, and crc which is the minimum length
        shiftBufferToMagic();
        return {PACKET_TOO_SMALL, nullptr};
    }
    uint16_t packetId = -1;
    if (!packet_utility_v2::read(packetId, a, buffer.end()) || packetId >= std::size(packetConstructors)) {
        shiftBufferToMagic();
        return {BAD_PACKET_ID, nullptr};
    }
    uint32_t crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(buffer.data(), packetLength-4);
    uint32_t crc_received = -1;
    auto crc_e = std::next(buffer.begin(), packetLength-4);
    if (!packet_utility_v2::read(crc_received, crc_e, buffer.end())) {
        shiftBufferToMagic();
        return {CRC_ISSUE, nullptr};
    }
    if (crc != crc_received) {
        shiftBufferToMagic();
        return std::make_tuple(BAD_CRC, nullptr);
    }
    std::shared_ptr<IPacket> packet = packetConstructors[packetId](a, buffer.end());
    if (packet == nullptr) {

        shiftBuffer(packetLength);
        return std::make_tuple(NULL_PTR_RETURN, nullptr);
    }
    packet->executeCallbacks(ARG_NAME_CHECK_PACKET);
    shiftBuffer(packetLength);
    return std::make_tuple(EXECUTED_PACKET, packet);
}

std::tuple<SearchStatus, packet_raw_type::iterator> PacketHandler::searchPacket(packet_raw_type::iterator it_begin, SearchStatus status) {
    uint16_t minimumPacketSize = sizeof(packet_size_type) + sizeof(PACKET_MAGIC) + sizeof(packet_id_type) + sizeof(uint32_t);
    if (it_begin == buffer.end()) {
        return {status == NOTHING ? NO_MAGIC_FOUND : status, it_begin};
    }
    auto magic_i = findMagic(it_begin);
    if (magic_i == buffer.end()) {
        return {status == NOTHING ? NO_MAGIC_FOUND : status, it_begin};
    }
    uint16_t packetLength = -1;
    uint64_t magic = -1;
    auto a = magic_i;
    auto begin = magic_i;
    if (!packet_utility_v2::read(magic, a, buffer.end())){
        return {UNKNOWN_ERROR_READING_MAGIC, it_begin};
    }
    if (magic != PACKET_MAGIC) {
        return {UNKNOWN_ERROR_READING_MAGIC, it_begin};
    }
    auto bufferSize = std::distance(magic_i, buffer.end());
    if (bufferSize< minimumPacketSize) {
        return {POSSIBLE_PACKET_FOUND_LENGTH_TOO_SMALL, magic_i};
    }
    if (!packet_utility_v2::read(packetLength, a, buffer.end()) ||packetLength > bufferSize) {
        return searchPacket(std::next(begin, 1), {POSSIBLE_PACKET_FOUND_LENGTH_TOO_SMALL});
    }

    uint16_t packetId = -1;
    if (!packet_utility_v2::read(packetId, a, buffer.end()) || packetId >= std::size(packetConstructors)) {
        return searchPacket(std::next(begin, 1), BAD_PACKET_ID_LOOK_UP);
    }
    auto end = std::next(std::as_const(magic_i), packetLength-4);

    uint32_t crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(magic_i, end);
    uint32_t crc_received = -1;
    auto crc_e = std::next(buffer.begin(), packetLength-4);
    if (!packet_utility_v2::read(crc_received, crc_e, buffer.end())) {
        return {UNABLE_TO_READ_CRC_LOOK_UP, it_begin};
    }
    if (crc != crc_received) {
        return searchPacket(std::next(begin, 1), BAD_CRC_LOOK_UP);
    }
    return {GOOD_PACKET_FOUND, magic_i};
}


std::vector<uint8_t> PacketHandler::createPacket(std::shared_ptr<IPacket> packet) {
    return createPacket(*packet);
}


packet_raw_type PacketHandler::createPacket(const IPacket &packet) {
    packet_raw_type result;
    auto b_i = std::back_inserter(result);
    packet_utility_v2::write(b_i, PACKET_MAGIC, result.end()); //magic number
    result.emplace_back(0xFF);
    result.emplace_back(0xFF);
    auto back_inserter = std::back_inserter(result);
    packet_utility_v2::write(back_inserter, packet.getPacketID(), result.end());
    packet.packetToBuffer(result); //considered linear operation (only forward motion in vector)
    packet_size_type packetLength = result.size() + 4; //length + CRC
    if (result.size() + 4 > (1ULL << (sizeof(packet_size_type) * 8 )) - 1) {
        return {};
    }
    auto b = result.begin();
    std::advance(b, sizeof(PACKET_MAGIC)); //enter in the packetLength area
    packet_utility_v2::write(b, packetLength, result.end());
    auto crc = CRC_PACKET_HANDLER::algoCRC_32.computeCRC(result);
    packet_utility_v2::write(back_inserter, crc, result.end());
    return result;
}
