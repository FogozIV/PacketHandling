import os
import sys
import runpy
from pathlib import Path
import shutil


#os.add_dll_directory(r"C:\Users\fogoz\AppData\Local\Programs\CLion Nova\bin\mingw\bin")
def detect_mingw_bin():
    gpp = shutil.which("g++")
    if not gpp:
        return None
    gpp_path = Path(gpp).resolve()
    return gpp_path.parent if (gpp_path.parent / "libstdc++-6.dll").exists() else None

if sys.platform == "win32":
    mingw_path = detect_mingw_bin()
    if mingw_path:
        print(f"[*] Adding MinGW DLL path: {mingw_path}")
        os.add_dll_directory(str(mingw_path))
    else:
        print("[!] Could not detect MinGW DLL path; stubgen may fail.")

project_root = Path(__file__).parent.resolve()  # <-- keep this a Path object
os.environ["PYTHONPATH"] = str(project_root)
build_dir = Path(os.environ.get("SKBUILD_BUILD_DIR", Path.cwd())).resolve()
stub_output_dir = build_dir / "generated_stubs" / "packet_handler"
stub_output_dir.mkdir(parents=True, exist_ok=True)
sys.argv = [
    "pybind11_stubgen",  # fake script name
    "packet_handler",
    "--output-dir", str(build_dir)
]

# Run pybind11_stubgen as if it was run from CLI
runpy.run_module("pybind11_stubgen", run_name="__main__")