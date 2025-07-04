import os
import sys
from pathlib import Path

def parse_line(line):
    tokens = line.strip().split()
    if not tokens or line.startswith("#"):
        return None
    name, enum = tokens[0], tokens[1]
    fields = []
    for pair in tokens[2:]:
        if pair.startswith('(') and pair.endswith(')'):
            type_name = pair[1:-1].split()
            if len(type_name) != 2:
                raise ValueError(f"Invalid field: {pair}")
            fields.append((type_name[0], type_name[1]))
    return name, enum, fields

def generate_packet_hpp(packets):
    result = ['#pragma once',
              '#include "BasePacket.h"',
              '#include "packets/PacketDefinition.h"',
              '#include "utils/PacketUtility.h"',
              '#include <memory>',
              '']
    for name, enum, fields in packets:
        result.append(f'class {name} : public BasePacket<{name}> {{')
        for t, n in fields:
            result.append(f'    {t} {n};')
        result.append('public:')
        result.append(f'    static constexpr PacketType getPacketID() {{ return PacketType::{enum}; }}')
        result.append(f'    explicit {name}() {{ type = getPacketID(); }}')
        if fields:
            args = ', '.join(f'{t} {n}_' for t, n in fields)
            inits = '\n        '.join(f'this->{n} = {n}_;' for _, n in fields)
            result.append(f'    {name}({args}) : {name}() {{\n        {inits}\n    }}')
        for t, n in fields:
            result.append(f'    const {t}& get{n.capitalize()}() const {{ return {n}; }}')
        result.append('    const packet_size_type packetToBuffer(packet_raw_type&) const override;')
        result.append('    static std::shared_ptr<{}> create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end);'.format(name))
        result.append('};\n')
    return '\n'.join(result)

def generate_packet_cpp(packets):
    result = ['#include "PacketsData.hpp"',
              '#include "utils/PacketUtility.h"',
              '']
    for name, enum, fields in packets:
        result.append(f'const packet_size_type {name}::packetToBuffer(packet_raw_type & vector) const {{')
        result.append('    auto iterator = std::back_inserter(vector);')
        for _, n in fields:
            result.append(f'    packet_utility_v2::write(iterator, {n}, vector.end());')
        result.append('    return vector.size();')
        result.append('}')
        result.append(f'std::shared_ptr<{name}> {name}::create(packet_raw_type::iterator& current, packet_raw_type::iterator it_end) {{')
        result.append(f'    auto result = std::make_shared<{name}>();')
        for _, n in fields:
            result.append(f'    if (!packet_utility_v2::read(result->{n}, current, it_end)) return nullptr;')
        result.append('    return result;')
        result.append('}\n')
    return '\n'.join(result)
def generate_packet_macro_header(packets):
    lines = ['#pragma once', '', '#define PACKETS_PYTHON_GENERATED \\']
    for name, enum, _ in packets:
        lines.append(f'    PACKET({name}, {enum}) \\')
    lines[-1] = lines[-1].rstrip(' \\')  # clean trailing backslash
    lines.append('\n')
    return '\n'.join(lines)
def main():
    infile = Path(sys.argv[1])
    outdir = Path(sys.argv[2])
    outdir.mkdir(parents=True, exist_ok=True)

    with open(infile) as f:
        lines = f.readlines()

    packets = [parse_line(line) for line in lines]
    packets = [p for p in packets if p]

    hpp_code = generate_packet_hpp(packets)
    cpp_code = generate_packet_cpp(packets)

    with open(outdir / "PacketsData.hpp", "w") as f:
        f.write(hpp_code)
    with open(outdir / "PacketsData.cpp", "w") as f:
        f.write(cpp_code)
    with open(outdir / "PacketMacros.hpp", "w") as f:
        f.write(generate_packet_macro_header(packets))

if __name__ == "__main__":
    main()