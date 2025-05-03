cmake_minimum_required(VERSION 3.22)
project(PacketHandling)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)
add_library(PacketHandling STATIC ${SOURCES} ${HEADERS})

# Specify the include directory for consumers
target_include_directories(PacketHandling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
