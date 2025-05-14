//
// Created by fogoz on 14/05/2025.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "PacketHandler.h"
#include "packets/NoContentPacket.h"
#include "packets/OneArgPacket.h"
namespace py = pybind11;

PYBIND11_MODULE(pybindings, m) {
    m.doc() = "Python bindings for PacketHandling";

    py::class_<IPacket, std::shared_ptr<IPacket>>(m, "IPacket");

#undef PACKET
#define PACKET(name, enum_value, ...) .value(#enum_value, PacketType::enum_value)
    py::enum_<PacketType>(m, "PacketType")
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST
    .export_values();
#undef PACKET

#define PACKET(name, e_name) \
    py::class_<name, IPacket, std::shared_ptr<name>>(m, #name)\
            .def(py::init<>())\
            .def("get_packet_id", &name::getPacketID)\
            .def("packet_to_buffer", [](const name &self) {\
                packet_raw_type buffer;\
                self.packetToBuffer(buffer);\
                return buffer; \
            });
    EMPTY_PACKET_LIST
    ONE_ARG_PACKET_LIST

#define CHK_STATUS(name) .value(#name, CheckStatus::name)
    py::enum_<CheckStatus>(m, "CheckStatus")
    CHECK_STATUS_STRINGS
    .export_values();
#undef CHK_STATUS

#define SRCH_STATUS(name) .value(#name, SearchStatus::name)
    py::enum_<SearchStatus>(m, "SearchStatus")
    SEARCH_STATUS_STRINGS
    .export_values();
#undef SRCH_STATUS

    py::class_<PacketHandler>(m, "PacketHandler")
    .def(py::init<>())
    .def("receive_data", py::overload_cast<const uint8_t*, size_t>(&PacketHandler::receiveData))
    .def("receive_data", py::overload_cast<const std::vector<uint8_t>&>(&PacketHandler::receiveData))
    .def("check_packet", &PacketHandler::checkPacket)
    .def("create_packet", py::overload_cast<std::shared_ptr<IPacket>>(&PacketHandler::createPacket));


}