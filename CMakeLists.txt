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
install(TARGETS packet_handler
        LIBRARY DESTINATION packet_handler  # Places .pyd inside a package folder
)
install(FILES packet_handler/__init__.py DESTINATION packet_handler)

find_program(PYBIND11_STUBGEN_EXECUTABLE pybind11-stubgen)
message(STATUS "Using Python executable: ${Python3_EXECUTABLE}")
if (PYBIND11_STUBGEN_EXECUTABLE)
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:packet_handler>
            ${CMAKE_CURRENT_SOURCE_DIR}/packet_handler/packet_handler.pyd
            COMMENT "Copying .pyd to packet_handler/ for stub generation"
    )
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}
            ${Python3_EXECUTABLE} -m pybind11_stubgen packet_handler
            --output-dir ${CMAKE_CURRENT_BINARY_DIR}/generated_stubs
            COMMENT "Generating .pyi stub file for packet_handler"
    )
    # Cleanup copied .pyd
    add_custom_command(
            TARGET packet_handler
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove
            ${CMAKE_CURRENT_SOURCE_DIR}/packet_handler/packet_handler.pyd
            COMMENT "Cleaning up copied packet_handler.pyd"
    )

    # Install the generated .pyi file
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/generated_stubs/packet_handler/packet_handler.pyi
            DESTINATION packet_handler
    )
else()
    message(WARNING "pybind11-stubgen not found; .pyi stub will not be generated")
endif()
