//
// Created by fogoz on 06/05/2025.
//

#ifndef PACKETDISPATCHER_H
#define PACKETDISPATCHER_H
#include <functional>
#include <memory>

#include "BasePacket.h"

typedef uint64_t CallbackID;

class PacketDispatcher {
    using GeneralCallback = std::function<bool(std::shared_ptr<IPacket>)>;
    std::unordered_map<PacketType, std::unordered_map<CallbackID, GeneralCallback>> callbacks;
public:
    template<typename PacketT>
    CallbackID registerCallBack(std::function<bool(std::shared_ptr<PacketT>)> cb){
        static_assert(std::is_base_of<IPacket, PacketT>::value, "PacketT must be a subclass of IPacket");
        static CallbackID id = 0;
        CallbackID callback_id = id++;
        callbacks[PacketT::getPacketID()][callback_id] = ([cb](std::shared_ptr<IPacket>pckt) {
            return cb(std::static_pointer_cast<PacketT>(pckt));
        });
        return callback_id;
    }

    void dispatch(const std::shared_ptr<IPacket>& packet);

    void removeCallback(PacketType packetType, CallbackID id);
};


#endif //PACKETDISPATCHER_H
