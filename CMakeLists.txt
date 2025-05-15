cmake_minimum_required(VERSION 3.22)
project(PacketHandling)

set(CMAKE_CXX_STANDARD 20)

# Add pybind11 submodule or external directory
add_subdirectory(extern/pybind11)

# Set up the source files and headers for the PacketHandling library
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# Create the static library for PacketHandling
add_library(PacketHandling STATIC ${SOURCES} ${HEADERS})

# Specify the include directory for consumers of PacketHandling
target_include_directories(PacketHandling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the pybind11 bindings module
pybind11_add_module(packet_handler bindings/packet_handler.cpp)

# Link the PacketHandling library to the pybindings module
target_link_libraries(packet_handler PRIVATE PacketHandling)

# Specify the include directories for the pybindings module
target_include_directories(packet_handler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find Python and pybind11 (if not already done in `add_subdirectory`)
find_package(Python3 REQUIRED)

# Ensure proper output settings for the Python shared library (.pyd on Windows)
set_target_properties(packet_handler PROPERTIES PREFIX "" SUFFIX ".pyd")# For Windows .pyd