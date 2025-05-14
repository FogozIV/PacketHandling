//
// Created by fogoz on 03/05/2025.
//

#ifndef NOCONTENTPACKET_H
#define NOCONTENTPACKET_H
#include <memory>
#include <functional>
#include <vector>
#include "BasePacket.h"
#include "PacketDefinition.h"
#undef PACKET
#define PACKET(name, e_name) \
class name : public BasePacket<name> {\
public:\
    static constexpr PacketType getPacketID(){return PacketType::e_name;}\
    explicit name() {\
        type = name::getPacketID();\
    }\
    DECLARE_CALLBACKS(name)\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        return result;\
    }\
    CALL_CALLBACKS(name)\
};

class StartFlashPacket : public BasePacket<StartFlashPacket> {
public:
    static constexpr PacketType getPacketID() { return PacketType::START_FLASH; }
    explicit StartFlashPacket() { type = StartFlashPacket::getPacketID(); }
    static std::vector<std::function<bool(std::shared_ptr<StartFlashPacket>)> > callbacks;

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<StartFlashPacket> create(packet_raw_type::iterator &current,
                                                    packet_raw_type::iterator it_end) {
        auto result = std::make_shared<StartFlashPacket>();
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class AlreadyFlashingPacket : public BasePacket<AlreadyFlashingPacket> {
public:
    static constexpr PacketType getPacketID() { return PacketType::ALREADY_FLASHING; }
    explicit AlreadyFlashingPacket() { type = AlreadyFlashingPacket::getPacketID(); }
    static std::vector<std::function<bool(std::shared_ptr<AlreadyFlashingPacket>)> > callbacks;

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<AlreadyFlashingPacket> create(packet_raw_type::iterator &current,
                                                         packet_raw_type::iterator it_end) {
        auto result = std::make_shared<AlreadyFlashingPacket>();
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class IssueStartingFlashingPacket : public BasePacket<IssueStartingFlashingPacket> {
public:
    static constexpr PacketType getPacketID() { return PacketType::ISSUE_START_FLASH_PACKET; }
    explicit IssueStartingFlashingPacket() { type = IssueStartingFlashingPacket::getPacketID(); }
    static std::vector<std::function<bool(std::shared_ptr<IssueStartingFlashingPacket>)> > callbacks;

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<IssueStartingFlashingPacket> create(packet_raw_type::iterator &current,
                                                               packet_raw_type::iterator it_end) {
        auto result = std::make_shared<IssueStartingFlashingPacket>();
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class IssueFlashingPacket : public BasePacket<IssueFlashingPacket> {
public:
    static constexpr PacketType getPacketID() { return PacketType::ISSUE_FLASHING; }
    explicit IssueFlashingPacket() { type = IssueFlashingPacket::getPacketID(); }
    static std::vector<std::function<bool(std::shared_ptr<IssueFlashingPacket>)> > callbacks;

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<IssueFlashingPacket> create(packet_raw_type::iterator &current,
                                                       packet_raw_type::iterator it_end) {
        auto result = std::make_shared<IssueFlashingPacket>();
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class FlashingSoftwarePacket : public BasePacket<FlashingSoftwarePacket> {
public:
    static constexpr PacketType getPacketID() { return PacketType::FLASHING_SOFTWARE; }
    explicit FlashingSoftwarePacket() { type = FlashingSoftwarePacket::getPacketID(); }
    static std::vector<std::function<bool(std::shared_ptr<FlashingSoftwarePacket>)> > callbacks;

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<FlashingSoftwarePacket> create(packet_raw_type::iterator &current,
                                                          packet_raw_type::iterator it_end) {
        auto result = std::make_shared<FlashingSoftwarePacket>();
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};
#undef PACKET


#endif //NOCONTENTPACKET_H
