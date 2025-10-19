cmake_minimum_required(VERSION 3.22)
project(PacketHandling)

set(CMAKE_CXX_STANDARD 20)

# --------------------------
# PACKET GENERATOR SETTINGS
# --------------------------

# Python required to run the generator
find_package(Python3 REQUIRED)

set(PACKET_DEF_FILE ${CMAKE_SOURCE_DIR}/include/packets/PacketList.packetdef)
set(GENERATED_DIR ${CMAKE_SOURCE_DIR})

# Run the generator to create Packets.cpp and Packets.hpp
add_custom_command(
        OUTPUT
        ${GENERATED_DIR}/src/packets/PacketsData.cpp
        ${GENERATED_DIR}/include/packets/PacketsData.hpp
        ${GENERATED_DIR}/include/packets/PacketMacros.hpp
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/packet_gen.py
        ${PACKET_DEF_FILE} ${GENERATED_DIR}
        DEPENDS ${PACKET_DEF_FILE} ${CMAKE_SOURCE_DIR}/scripts/packet_gen.py
        COMMENT "Generating packet classes from PacketList.packetdef"
)
add_custom_target(GeneratePackets
        DEPENDS
        ${GENERATED_DIR}/src/packets/PacketsData.cpp
        ${GENERATED_DIR}/include/packets/PacketsData.hpp
        ${GENERATED_DIR}/include/packets/PacketMacros.hpp
)
# --------------------------
# PYBIND11 + PACKET LOGIC
# --------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
add_subdirectory(extern/pybind11)

# Gather your source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# Include the generated files
list(APPEND SOURCES ${GENERATED_CPP})
list(APPEND HEADERS ${GENERATED_HPP})

# Create the static library
add_library(PacketHandling STATIC ${SOURCES} ${HEADERS})

add_dependencies(PacketHandling GeneratePackets)

# Include both your own headers and the generated folder
target_include_directories(PacketHandling
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GENERATED_DIR}
)

# Pybind11 module
pybind11_add_module(packet_handler bindings/packet_handler.cpp)

target_link_libraries(packet_handler PRIVATE PacketHandling)

target_include_directories(packet_handler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GENERATED_DIR}
)

# Windows .pyd setup
set_target_properties(packet_handler PROPERTIES OUTPUT_NAME "_packet_handler" SUFFIX ".pyd")

install(TARGETS packet_handler
        LIBRARY DESTINATION packet_handler
)
install(FILES packet_handler/__init__.py DESTINATION packet_handler)
target_link_options(packet_handler PRIVATE -static-libgcc -static-libstdc++)
# Stub generation
find_program(PYBIND11_STUBGEN_EXECUTABLE pybind11-stubgen)
message(STATUS "Using Python executable: ${Python3_EXECUTABLE}")
if (PYBIND11_STUBGEN_EXECUTABLE)
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:packet_handler>
            ${CMAKE_CURRENT_SOURCE_DIR}/packet_handler/_packet_handler.pyd
            COMMENT "Copying .pyd to packet_handler/ for stub generation"
    )
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/run_stubgen.py
            COMMENT "Generating .pyi stub file for packet_handler"
    )
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove
            ${CMAKE_CURRENT_SOURCE_DIR}/packet_handler/_packet_handler.pyd
            COMMENT "Cleaning up copied packet_handler.pyd"
    )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/generated_stubs/packet_handler/_packet_handler.pyi
            DESTINATION packet_handler
    )
else()
    message(WARNING "pybind11-stubgen not found; .pyi stub will not be generated")
endif()
