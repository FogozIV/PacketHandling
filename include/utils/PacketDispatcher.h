//
// Created by fogoz on 06/05/2025.
//

#ifndef PACKETDISPATCHER_H
#define PACKETDISPATCHER_H
#include <functional>
#include <memory>

#include "BasePacket.h"


class PacketDispatcher {
    using GeneralCallback = std::function<bool(std::shared_ptr<IPacket>)>;
    std::unordered_map<PacketType, std::vector<GeneralCallback>> callbacks;
public:
    template<typename PacketT>
    void registerCallBack(std::function<bool(std::shared_ptr<PacketT>)> cb){
        static_assert(std::is_base_of<IPacket, PacketT>::value, "PacketT must be a subclass of IPacket");
        callbacks[PacketT::getPacketID()].push_back([cb](std::shared_ptr<IPacket>pckt) {
            return cb(std::static_pointer_cast<PacketT>(pckt));
        });
    }

    void dispatch(std::shared_ptr<IPacket> packet);
};


#endif //PACKETDISPATCHER_H
