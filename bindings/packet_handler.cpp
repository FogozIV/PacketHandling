//
// Created by fogoz on 14/05/2025.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include "PacketHandler.h"
#include "packets/NoContentPacket.h"
#include "packets/OneArgPacket.h"
#include "utils/PacketDispatcher.h"
namespace py = pybind11;

PYBIND11_MODULE(packet_handler, m) {
    m.doc() = "Python bindings for PacketHandling";

    py::class_<IPacket, std::shared_ptr<IPacket>>(m, "IPacket");

#undef PACKET
#define PACKET(name, enum_value, ...) .value(#enum_value, PacketType::enum_value)
    py::enum_<PacketType>(m, "PacketType")
    PACKETS
    .export_values();
#undef PACKET

#define PACKET(name, e_name, ...) \
py::class_<name, IPacket, std::shared_ptr<name>>(m, #name)\
.def(py::init<>())\
.def("get_packet_id", &name::getPacketID)\
.def("packet_to_buffer", [](const name &self) {\
packet_raw_type buffer;\
self.packetToBuffer(buffer);\
return buffer; \
});
    EMPTY_PACKET_LIST
#undef PACKET

#define PACKET(name, e_name, arg_type, arg_name) \
py::class_<name, IPacket, std::shared_ptr<name>>(m, #name)\
.def(py::init<>())\
.def("get_packet_id", &name::getPacketID)\
.def("get_"#arg_name, &name::get##arg_name)\
.def("packet_to_buffer", [](const name &self) {\
packet_raw_type buffer;\
self.packetToBuffer(buffer);\
return buffer; \
});
    ONE_ARG_PACKET_LIST
#undef PACKET

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

#define PACKET(name, e_name, ...) \
    .def("register_"#name"_callback", [](PacketDispatcher& self, py::function py_cb){\
        auto cpp_cb = [py_cb](std::shared_ptr<name> packet) -> bool { \
            py::gil_scoped_acquire acquire; \
            py::object result = py_cb(packet); \
            return !result.is_none() && result.cast<bool>();\
        };\
        return self.registerCallBack<name>(cpp_cb);\
    })


    py::class_<PacketDispatcher>(m, "PacketDispatcher")
    .def(py::init<>())
    PACKETS
    .def("remove_callback", &PacketDispatcher::removeCallback)
    .def("dispatch_packet", &PacketDispatcher::dispatch);

}