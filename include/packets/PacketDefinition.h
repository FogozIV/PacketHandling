//
// Created by fogoz on 03/05/2025.
//

#ifndef PACKETDEFINITION_H
#define PACKETDEFINITION_H
#include <cstdint>
//#define COMMA_MACRO(...) __VA_OPT__(,)##__VA_ARGS__
//FOR STUPID COMPILER LIKE THE VISUAL STUDIO ONE :|
#define HAS_ARGS(...)  HAS_ARGS_IMPL(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define HAS_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N

#define MAYBE_COMMA(...) \
IF(HAS_ARGS(__VA_ARGS__), COMMA, EMPTY)()

#define IF(cond, then, else) _IF(cond)(then, else)
#define _IF(cond) IF_##cond
#define IF_1(then, else) then
#define IF_0(then, else) else

#define COMMA() ,
#define EMPTY()

#define COMMA_MACRO(...) MAYBE_COMMA(__VA_ARGS__) __VA_ARGS__
#ifdef TEENSY41
    #define ARG_CHECK_PACKET AsyncClient*
    #define ARG_NAME_CHECK_PACKET client
#endif
#define EMPTY_PACKET_LIST \
    PACKET(StartFlashPacket, START_FLASH)\
    PACKET(AlreadyFlashingPacket, ALREADY_FLASHING)\
    PACKET(IssueStartingFlashingPacket, ISSUE_START_FLASH_PACKET)\
    PACKET(IssueFlashingPacket, ISSUE_FLASHING)\
    PACKET(FlashingSoftwarePacket, FLASHING_SOFTWARE)

#define ONE_ARG_PACKET_LIST \
    PACKET(PingPacket, PING, uint32_t, UniqueID) \
    PACKET(PongPacket, PONG, uint32_t, UniqueID) \
    PACKET(TestbitPacket, TEST_32BIT, uint32_t, value) \
    PACKET(DataPacket, DATA, std::vector<std::uint8_t>, Data)\
    PACKET(ReceivedDataPacket, RECEIVED_DATA, uint32_t, value)

#define PACKETS \
    EMPTY_PACKET_LIST\
    ONE_ARG_PACKET_LIST




#endif //PACKETDEFINITION_H
