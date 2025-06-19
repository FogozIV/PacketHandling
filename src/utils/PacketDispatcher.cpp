//
// Created by fogoz on 06/05/2025.
//

#include <utils/PacketDispatcher.h>


void PacketDispatcher::dispatch(const std::shared_ptr<IPacket>& packet) {
    auto cbs = callbacks[static_cast<PacketType>(packet->getPacketID())];
    for (auto it = cbs.begin(); it != cbs.end(); ) {
        if (it->second(packet)) {
            it=cbs.erase(it);
        }else {
            ++it;
        }
    }
}

void PacketDispatcher::removeCallback(PacketType packetType, CallbackID id) {
    auto it = callbacks.find(packetType);
    if (it != callbacks.end()) {
        it->second.erase(id);
    }
}
