# PacketHandling Library Architecture

## Overview

The PacketHandling library is a C++ framework designed for efficient network packet serialization, deserialization, and validation. It provides a type-safe, macro-driven approach to define and handle different packet types with built-in CRC validation and endianness handling.

## Core Architecture

### 1. Packet Structure

All packets follow a standardized binary format:

```
[MAGIC][LENGTH][PACKET_ID][PAYLOAD][CRC]
```

- **MAGIC**: 8-byte magic number (`0xFEEDFACECAFEBEEF`) for packet identification
- **LENGTH**: 2-byte packet length (including CRC)
- **PACKET_ID**: 2-byte packet type identifier
- **PAYLOAD**: Variable-length packet-specific data
- **CRC**: 4-byte CRC-32 checksum for data integrity

### 2. Class Hierarchy

```
IPacket (Interface)
    └── BasePacket<T> (Template Base)
        ├── NoContentPacket Classes
        └── OneArgPacket Classes
```

#### Base Classes

- **`IPacket`**: Abstract interface defining packet contract
  - `getPacketID()`: Returns packet type identifier
  - `packetToBuffer()`: Serializes packet to binary buffer

- **`BasePacket<T>`**: Template base class providing shared functionality
  - Inherits from `std::enable_shared_from_this<T>`
  - Provides `getShared()` method for safe shared pointer creation

### 3. Packet Types

The library supports two categories of packets defined through macros:

#### Empty Packets (NoContentPacket)
Packets without payload data:
- `StartFlashPacket`
- `AlreadyFlashingPacket`
- `IssueStartingFlashingPacket`
- `IssueFlashingPacket`
- `FlashingSoftwarePacket`

#### Single-Argument Packets (OneArgPacket)
Packets with one data field:
- `PingPacket` (uint32_t UniqueID)
- `PongPacket` (uint32_t UniqueID)
- `TestbitPacket` (uint32_t value)
- `DataPacket` (std::vector<std::uint8_t> Data)
- `ReceivedDataPacket` (uint32_t value)

## Macro System

### 1. Packet Definition Macros

The library uses a sophisticated macro system to generate packet classes automatically:

#### Packet Type Enumeration
```cpp
#define PACKET(name, enum_value, ...) enum_value,
enum PacketType {
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST
};
```

#### Empty Packet Generation
```cpp
#define PACKET(name, e_name) \
class name : public BasePacket<name> {\
public:\
    static constexpr PacketType getPacketID(){return PacketType::e_name;}\
    explicit name() {\
        type = name::getPacketID();\
    }\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        return result;\
    }\
};
```

#### Single-Argument Packet Generation
```cpp
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
    const data_type get##instance(){ return this->instance;}\
    data_type& get##instance##Ref(){return this->instance;}\
    const packet_size_type packetToBuffer(packet_raw_type&) const override;\
    static std::shared_ptr<name> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {\
        auto result = std::make_shared<name>();\
        if(!packet_utility_v2::read(result->instance, current, it_end)) return nullptr;\
        return result;\
    }\
};
```

### 2. Status Enumeration Macros

The library uses macros to generate status enums for packet processing:

#### Check Status
```cpp
#define CHECK_STATUS_STRINGS \
    CHK_STATUS(WAITING_LENGTH) \
    CHK_STATUS(WAITING_DATA) \
    CHK_STATUS(BAD_CRC) \
    CHK_STATUS(EXECUTED_PACKET)\
    CHK_STATUS(BAD_PACKET_ID) \
    CHK_STATUS(PACKET_TOO_SMALL) \
    CHK_STATUS(NULL_PTR_RETURN)\
    CHK_STATUS(CRC_ISSUE) \
    CHK_STATUS(UNABLE_TO_READ_MAGIC)\
    CHK_STATUS(BAD_MAGIC_AND_NOT_FOUND)\
    CHK_STATUS(BAD_MAGIC_AND_FOUND)\
    CHK_STATUS(FOUND_NEW_PACKET)
```

#### Search Status
```cpp
#define SEARCH_STATUS_STRINGS \
    SRCH_STATUS(NOTHING)\
    SRCH_STATUS(NO_MAGIC_FOUND)\
    SRCH_STATUS(UNKNOWN_ERROR_READING_MAGIC)\
    SRCH_STATUS(POSSIBLE_PACKET_FOUND_LENGTH_TOO_SMALL)\
    SRCH_STATUS(BAD_PACKET_ID_LOOK_UP)\
    SRCH_STATUS(UNABLE_TO_READ_CRC_LOOK_UP)\
    SRCH_STATUS(BAD_CRC_LOOK_UP)\
    SRCH_STATUS(GOOD_PACKET_FOUND)
```

## Packet Handler Core

### 1. Main Handler Class

The `PacketHandler` class manages packet processing with the following key responsibilities:

- **Buffer Management**: Maintains an internal buffer for incoming data
- **Packet Detection**: Searches for valid packets using magic number
- **Packet Validation**: Verifies CRC and packet integrity
- **Packet Construction**: Creates packet objects from binary data
- **Packet Serialization**: Converts packet objects to binary format

### 2. Key Methods

#### Data Reception
```cpp
void receiveData(const uint8_t* data, size_t size);
void receiveData(const std::vector<uint8_t>& data);
```

#### Packet Processing
```cpp
std::tuple<CheckStatus, std::shared_ptr<IPacket>> checkPacket();
std::tuple<SearchStatus, packet_raw_type::iterator> searchPacket(...);
```

#### Packet Creation
```cpp
std::vector<uint8_t> createPacket(std::shared_ptr<IPacket> packet);
std::vector<uint8_t> createPacket(const IPacket& packet);
```

## Utility Systems

### 1. Packet Utility (PacketUtility.h)

Provides type-safe serialization/deserialization for supported data types:

#### Supported Types
- **Integers**: int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t
- **Containers**: std::vector<T>, std::string
- **Endianness**: Automatic network byte order conversion

#### Key Functions
```cpp
template<typename T, typename Iterator>
bool read(T& data, Iterator& current, Iterator end);

template<typename T, typename Iterator, typename Iterator_end>
bool write(Iterator& iterator, T value, Iterator_end end);
```

### 2. CRC System (CRC.h)

Comprehensive CRC implementation supporting multiple algorithms:

#### Supported Algorithms
- CRC-8
- CRC-CCITT
- MODBUS
- CRC-16
- CRC-24
- CRC-32
- CRC-32-BZIP2
- CRC-64-Jones

#### Algorithm Definition Macro
```cpp
#define ALGO(NAME, size, POLYNOMIAL, INITIAL_VALUE, FINAL_XOR_VALUE, REVERSED_DATA, REVERSED_OUT, WIDTH)
```

## Packet Processing Flow

### 1. Reception Flow
1. Raw data received via `receiveData()`
2. Data appended to internal buffer
3. `checkPacket()` called to process available data

### 2. Packet Detection Flow
1. Search for magic number in buffer
2. Read packet length and validate
3. Read packet ID and validate against known types
4. Calculate and verify CRC
5. Create packet object using appropriate constructor

### 3. Packet Creation Flow
1. Start with magic number
2. Reserve space for length field
3. Write packet ID
4. Serialize payload data
5. Calculate and write CRC
6. Update length field with final size

## Error Handling

### 1. Status-Based Error Handling
The library uses comprehensive status enums to report processing results:

- **CheckStatus**: For packet validation and processing
- **SearchStatus**: For packet discovery and recovery

### 2. Recovery Mechanisms
- **Magic Number Search**: Automatically searches for valid packet boundaries
- **Buffer Shifting**: Removes invalid data to find valid packets
- **CRC Validation**: Ensures data integrity

## Platform Support

### 1. Endianness Handling
Automatic endianness detection and conversion:
```cpp
inline uint16_t htons(uint16_t hostshort) {
    uint32_t data = 42;
    if (*((uint8_t*)&data) == 42) {
        // Little-endian: swap bytes
        uint8_t* ptr = (uint8_t*)&hostshort;
        std::swap(ptr[0], ptr[1]);
        return hostshort;
    }
    return hostshort; // Big-endian: no swap needed
}
```

### 2. Platform-Specific Features
- **Teensy 4.1 Support**: Conditional compilation for embedded targets
- **Cross-Platform**: Works on Windows, Linux, and embedded systems

## Usage Patterns

### 1. Defining New Packets
To add a new packet type, modify `PacketDefinition.h`:

```cpp
// For empty packet
#define EMPTY_PACKET_LIST \
    PACKET(MyEmptyPacket, MY_EMPTY_PACKET)

// For single-argument packet
#define ONE_ARG_PACKET_LIST \
    PACKET(MyDataPacket, MY_DATA_PACKET, uint32_t, Value)
```

### 2. Using the Packet Handler
```cpp
PacketHandler handler;

// Receive data
handler.receiveData(data, size);

// Process packets
auto [status, packet] = handler.checkPacket();
if (status == EXECUTED_PACKET) {
    // Handle packet
    if (auto ping = std::dynamic_pointer_cast<PingPacket>(packet)) {
        uint32_t id = ping->getUniqueID();
    }
}

// Create packets
auto ping = std::make_shared<PingPacket>(12345);
auto data = handler.createPacket(ping);
```

## Performance Considerations

### 1. Memory Management
- Uses `std::shared_ptr` for automatic memory management
- Efficient buffer operations with `std::move`
- Minimal memory allocations during packet processing

### 2. Type Safety
- Compile-time type checking through templates
- Runtime type safety through dynamic casting
- Macro-generated code ensures consistency

### 3. CRC Performance
- Pre-computed lookup tables for fast CRC calculation
- Template specialization for optimal performance
- Support for multiple CRC algorithms

## Extensibility

The architecture is designed for easy extension:

1. **New Packet Types**: Add to macro definitions
2. **New Data Types**: Extend PacketUtility with new type support
3. **New CRC Algorithms**: Add to CRC macro system
4. **Platform Support**: Conditional compilation for new targets

This modular design ensures the library can adapt to various use cases while maintaining type safety and performance. 