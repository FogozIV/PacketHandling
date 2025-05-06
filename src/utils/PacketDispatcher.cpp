//
// Created by fogoz on 06/05/2025.
//

#include "../../include/utils/PacketDispatcher.h"

template<typename PacketT>
void PacketDispatcher::registerCallBack(std::function<bool(std::shared_ptr<PacketT>)> cb) {
    callbacks[PacketT::getPacketID()].push_back(cb);
}

void PacketDispatcher::dispatch(std::shared_ptr<IPacket> packet) {
    auto cbs = callbacks[static_cast<PacketType>(packet->getPacketID())];
    for (auto it = cbs.begin(); it != cbs.end(); ) {
        if (it->operator()(packet)) {
            it=cbs.erase(it);
        }else {
            ++it;
        }
    }
}
