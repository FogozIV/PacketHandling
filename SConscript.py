print(">>> SConscript loaded from PacketHandling <<<")
import os
import subprocess
from pathlib import Path

Import("env")  # Import PlatformIO build environment
def generate_packets():
    lib_dir = Path(os.getcwd())
    packet_def = lib_dir / "include" / "packets" / "PacketList.packetdef"
    output_dir = lib_dir

    output_dir.mkdir(parents=True, exist_ok=True)

    print("[PacketGen] Generating packets...")
    import scripts.packet_gen as PG
    PG.main(packet_def, output_dir)

generate_packets()
