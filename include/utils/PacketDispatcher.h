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
    void registerCallBack(std::function<bool(std::shared_ptr<PacketT>)> cb);

    void dispatch(std::shared_ptr<IPacket> packet);
};


#endif //PACKETDISPATCHER_H
