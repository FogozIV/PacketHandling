//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETDEFINITION_H
#define PACKETDEFINITION_H

#define EMPTY_PACKET_LIST \
    PACKET(StartFlashPacket, START_FLASH)\
    PACKET(SendingFlashPacket, SENDING_FLASH)\
    PACKET(MovingFlashPacket, MOVING_FLASH)

#ifdef BASEPACKET_H
#define PACKET(name, enum_value, ...) enum_value,
#endif


#endif //PACKETDEFINITION_H
