//
// Created by fogoz on 03/05/2025.
//

#ifndef ONEARGPACKET_H
#define ONEARGPACKET_H
#include "PacketDefinition.h"
#include "BasePacket.h"
#include "utils/PacketUtility.h"
#undef PACKET
#define PACKET(name, e_name, data_type, instance) \
class name : public BasePacket<name> {\
    data_type instance; \
public:\
    static constexpr PacketType getPacketID(){ return PacketType::e_name;};\
    explicit name() {\
        type = name::getPacketID();\
    }\
    name (data_type instance) : name(){ \
        this->instance = instance;\
    }\
    DECLARE_CALLBACKS(name)\
    const data_type get##instance(){ return this->instance;}\
    data_type& get##instance##Ref(){return this->instance;}\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        if(!packet_utility_v2::read(result->instance, current, it_end)) return nullptr;\
        return result;\
    }\
    CALL_CALLBACKS(name)\
};

class PingPacket : public BasePacket<PingPacket> {
    uint32_t UniqueID;

public:
    static constexpr PacketType getPacketID() { return PacketType::PING; };
    explicit PingPacket() { type = PingPacket::getPacketID(); }
    PingPacket(uint32_t UniqueID) : PingPacket() { this->UniqueID = UniqueID; }
    static std::vector<std::function<bool(std::shared_ptr<PingPacket>)> > callbacks;
    const uint32_t getUniqueID() { return this->UniqueID; }
    uint32_t &getUniqueIDRef() { return this->UniqueID; }

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<PingPacket> create(packet_raw_type::iterator &current, packet_raw_type::iterator it_end) {
        auto result = std::make_shared<PingPacket>();
        if (!packet_utility_v2::read(result->UniqueID, current, it_end)) return nullptr;
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class PongPacket : public BasePacket<PongPacket> {
    uint32_t UniqueID;

public:
    static constexpr PacketType getPacketID() { return PacketType::PONG; };
    explicit PongPacket() { type = PongPacket::getPacketID(); }
    PongPacket(uint32_t UniqueID) : PongPacket() { this->UniqueID = UniqueID; }
    static std::vector<std::function<bool(std::shared_ptr<PongPacket>)> > callbacks;
    const uint32_t getUniqueID() { return this->UniqueID; }
    uint32_t &getUniqueIDRef() { return this->UniqueID; }

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<PongPacket> create(packet_raw_type::iterator &current, packet_raw_type::iterator it_end) {
        auto result = std::make_shared<PongPacket>();
        if (!packet_utility_v2::read(result->UniqueID, current, it_end)) return nullptr;
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class TestbitPacket : public BasePacket<TestbitPacket> {
    uint32_t value;

public:
    static constexpr PacketType getPacketID() { return PacketType::TEST_32BIT; };
    explicit TestbitPacket() { type = TestbitPacket::getPacketID(); }
    TestbitPacket(uint32_t value) : TestbitPacket() { this->value = value; }
    static std::vector<std::function<bool(std::shared_ptr<TestbitPacket>)> > callbacks;
    const uint32_t getvalue() { return this->value; }
    uint32_t &getvalueRef() { return this->value; }

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<TestbitPacket> create(packet_raw_type::iterator &current, packet_raw_type::iterator it_end) {
        auto result = std::make_shared<TestbitPacket>();
        if (!packet_utility_v2::read(result->value, current, it_end)) return nullptr;
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class DataPacket : public BasePacket<DataPacket> {
    std::vector<std::uint8_t> Data;

public:
    static constexpr PacketType getPacketID() { return PacketType::DATA; };
    explicit DataPacket() { type = DataPacket::getPacketID(); }
    DataPacket(std::vector<std::uint8_t> Data) : DataPacket() { this->Data = Data; }
    static std::vector<std::function<bool(std::shared_ptr<DataPacket>)> > callbacks;
    const std::vector<std::uint8_t> getData() { return this->Data; }
    std::vector<std::uint8_t> &getDataRef() { return this->Data; }

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<DataPacket> create(packet_raw_type::iterator &current, packet_raw_type::iterator it_end) {
        auto result = std::make_shared<DataPacket>();
        if (!packet_utility_v2::read(result->Data, current, it_end)) return nullptr;
        return result;
    }

    void executeCallbacks() override {
        for (auto it = callbacks.begin(); it != callbacks.end();) {
            bool result = (*it)(getShared());
            if (result) { it = callbacks.erase(it); } else { ++it; }
        }
    }
};

class ReceivedDataPacket : public BasePacket<ReceivedDataPacket> {
    uint32_t value;

public:
    static constexpr PacketType getPacketID() { return PacketType::RECEIVED_DATA; };
    explicit ReceivedDataPacket() { type = ReceivedDataPacket::getPacketID(); }
    ReceivedDataPacket(uint32_t value) : ReceivedDataPacket() { this->value = value; }
    static std::vector<std::function<bool(std::shared_ptr<ReceivedDataPacket>)> > callbacks;
    const uint32_t getvalue() { return this->value; }
    uint32_t &getvalueRef() { return this->value; }

    const packet_size_type packetToBuffer(packet_raw_type &) const override;

    static std::shared_ptr<ReceivedDataPacket> create(packet_raw_type::iterator &current,
                                                      packet_raw_type::iterator it_end) {
        auto result = std::make_shared<ReceivedDataPacket>();
        if (!packet_utility_v2::read(result->value, current, it_end)) return nullptr;
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


#endif //ONEARGPACKET_H
