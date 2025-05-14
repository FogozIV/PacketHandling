//
// Created by fogoz on 03/05/2025.
//

#include "../../include/packets/NoContentPacket.h"

#define PACKET(name, e_name)\
DEFINE_CALLBACKS(name)\
const packet_size_type name::packetToBuffer(packet_raw_type & vector) const {\
    return vector.size();\
}
std::vector<std::function<bool(std::shared_ptr<StartFlashPacket>)> > StartFlashPacket::callbacks;
const packet_size_type StartFlashPacket::packetToBuffer(packet_raw_type &vector) const { return vector.size(); }
std::vector<std::function<bool(std::shared_ptr<AlreadyFlashingPacket>)> > AlreadyFlashingPacket::callbacks;
const packet_size_type AlreadyFlashingPacket::packetToBuffer(packet_raw_type &vector) const { return vector.size(); }
std::vector<std::function<bool(std::shared_ptr<IssueStartingFlashingPacket>)> > IssueStartingFlashingPacket::callbacks;

const packet_size_type IssueStartingFlashingPacket::packetToBuffer(packet_raw_type &vector) const {
    return vector.size();
}

std::vector<std::function<bool(std::shared_ptr<IssueFlashingPacket>)> > IssueFlashingPacket::callbacks;
const packet_size_type IssueFlashingPacket::packetToBuffer(packet_raw_type &vector) const { return vector.size(); }
std::vector<std::function<bool(std::shared_ptr<FlashingSoftwarePacket>)> > FlashingSoftwarePacket::callbacks;
const packet_size_type FlashingSoftwarePacket::packetToBuffer(packet_raw_type &vector) const { return vector.size(); }

#undef PACKET
