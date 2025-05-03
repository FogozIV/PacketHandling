//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETDEFINITION_H
#define PACKETDEFINITION_H
#include <cstdint>
#define COMMA_MACRO(...) __VA_OPT__(,)##__VA_ARGS__
#ifdef TEENSY41
    #define ARG_CHECK_PACKET AsyncClient*
    #define ARG_NAME_CHECK_PACKET client
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

#define ONE_ARG_PACKET_LIST \
    PACKET(TestbitPacket, TEST_32BIT, uint32_t, value)




#endif //PACKETDEFINITION_H
