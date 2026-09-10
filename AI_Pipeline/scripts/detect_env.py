#!/usr/bin/env python3
"""detect_env.py — identifica chip/RAM/macOS/espaço e escolhe quantização e
resoluções adequadas (Int8@16GB, FP16@32GB+, trellis 512³ <= 64GB).

Uso:
  detect_env.py --json          # saída JSON (usada por install.sh / agent_loop.py)
  detect_env.py                 # saída legível
"""
import json
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG_PATH = ROOT / "config" / "pipeline.json"


def _sysctl(name: str) -> str:
    try:
        out = subprocess.run(["/usr/sbin/sysctl", "-n", name], capture_output=True, text=True)
        return out.stdout.strip()
    except Exception:
        return ""


def detect() -> dict:
    is_arm = platform.machine() == "arm64"
    brand = _sysctl("machdep.cpu.brand_string") or _sysctl("machdep.cpu.core_count") or "unknown"
    ram_gb = 0
    if is_arm:
        try:
            ram_gb = (int(_sysctl("hw.memsize")) or 0) // (1024 ** 3)
        except Exception:
            ram_gb = 0

    disk_free_gb = 0
    try:
        disk_free_gb = shutil.disk_usage("/").free // (1024 ** 3)
    except Exception:
        pass

    quant = "int8" if ram_gb < 32 else "fp16"
    trellis_res = "512" if ram_gb <= 64 else "1024"
    tripoflux_quant = "4bit" if ram_gb <= 32 else "8bit"

    info = {
        "arch": platform.machine(),
        "is_apple_silicon": is_arm,
        "chip": brand,
        "ram_gb": ram_gb,
        "disk_free_gb": disk_free_gb,
        "macos": platform.mac_ver()[0],
        "hunyuan_quant": quant,
        "trellis_resolution": trellis_res,
        "tripoflux_quant": tripoflux_quant,
        "recommended": {
            "warn_close_comfyui": ram_gb <= 24,
            "concurrent_generations": 1 if ram_gb <= 32 else 2,
            "min_disk_free_gb": 40,
            "recommended_disk_free_gb": 70,
        },
    }
    return info


def main() -> int:
    info = detect()
    if "--json" in sys.argv:
        print(json.dumps(info, indent=2))
        return 0

    if not info["is_apple_silicon"]:
        print("ERRO: este pipeline exige Apple Silicon (arm64). Detecção: %s" % info["arch"])
        return 1

    print("Arch   : %s (%s)" % (info["arch"], info["chip"]))
    print("macOS  : %s" % info["macos"])
    print("RAM    : %s GB" % info["ram_gb"])
    print("Disco  : %s GB livres" % info["disk_free_gb"])
    print("Hunyan : %s | trellis: %s³ | tripoflux: %s" % (
        info["hunyuan_quant"], info["trellis_resolution"], info["tripoflux_quant"]))
    if disk_too_low(info):
        print("AVISO: espaço livre abaixo do recomendado (%s GB)." % info["recommended"]["min_disk_free_gb"])
        return 2
    return 0


def disk_too_low(info: dict) -> bool:
    return info["disk_free_gb"] < info["recommended"]["min_disk_free_gb"]


if __name__ == "__main__":
    sys.exit(main())
