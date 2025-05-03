//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETDEFINITION_H
#define PACKETDEFINITION_H
#define COMMA_MACRO(...) __VA_OPT__(,)##__VA_ARGS__
#ifndef INCLUDER_CHECK_PACKET
#define INCLUDER_CHECK_PACKET
#endif

#ifndef ARG_CHECK_PACKET
#define ARG_CHECK_PACKET
#endif

#ifndef ARG_NAME_CHECK_PACKET
#define ARG_NAME_CHECK_PACKET
#endif
#define EMPTY_PACKET_LIST \
    PACKET(StartFlashPacket, START_FLASH)\
    PACKET(SendingFlashPacket, SENDING_FLASH)\
    PACKET(MovingFlashPacket, MOVING_FLASH)

#ifdef BASEPACKET_H
#define PACKET(name, enum_value, ...) enum_value,
#endif


#endif //PACKETDEFINITION_H
