import os
import sys
import runpy
import shutil
from pathlib import Path


def detect_mingw_bin():
    import shutil
    gpp = shutil.which("g++")
    if not gpp:
        return None
    gpp_path = Path(gpp).resolve()
    return gpp_path.parent if (gpp_path.parent / "libstdc++-6.dll").exists() else None


# --- 1. Add MinGW DLL path if needed (for Windows) ---
if sys.platform == "win32":
    mingw_path = detect_mingw_bin()
    if mingw_path:
        os.add_dll_directory(str(mingw_path))
        print(f"[*] Added MinGW DLL path: {mingw_path}")
    else:
        print("[!] Could not detect MinGW DLL path; stubgen may fail.")

# --- 2. Find the build directory ---
build_dir = Path(os.environ.get("SKBUILD_BUILD_DIR", Path.cwd())).resolve()
print(f"[*] Using build dir: {build_dir}")

# --- 3. Copy packet_handler package to build dir ---
project_root = Path(__file__).resolve().parent.parent
pkg_src = project_root / "packet_handler"
pkg_dst = build_dir / "packet_handler"

if not pkg_dst.exists():
    shutil.copytree(pkg_src, pkg_dst)
    print(f"[*] Copied package folder to temp build dir: {pkg_dst}")
else:
    print(f"[*] Package folder already exists at: {pkg_dst}")

# --- 4. Add to sys.path so import works ---
sys.path.insert(0, str(build_dir))
print(f"[*] Added to sys.path: {build_dir}")
debug = os.environ.get("SKDEBUG")
if debug:
    print(f"\n[*] Listing build directory contents at: {build_dir}")
    for root, dirs, files in os.walk(build_dir):
        rel_root = Path(root).relative_to(build_dir)
        print(f"\n📁 {rel_root if rel_root != Path('.') else './'}")
        for d in dirs:
            print(f"  📂 {d}/")
        for f in files:
            print(f"  📄 {f}")

# --- 5. Run pybind11-stubgen normally ---
sys.argv = [
    "pybind11_stubgen",
    "packet_handler",
    "--output-dir", str(build_dir / "generated_stubs")
]

runpy.run_module("pybind11_stubgen", run_name="__main__")
