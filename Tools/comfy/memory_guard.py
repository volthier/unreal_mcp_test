#!/usr/bin/env python3
"""Memory guard for heavy ComfyUI generation runs (macOS, stdlib only).

Checks that enough unified memory is available BEFORE launching a large
model (e.g. Hunyuan3D 6.9 GB) so the resident LM Studio LLM + agent stay safe.

Usage:
    python3 Tools/comfy/memory_guard.py --need 15          # require >= 15 GB available
    python3 Tools/comfy/memory_guard.py --need 15 --json   # machine-readable output

Exit codes:
    0 = enough memory, proceed with the generation
    1 = not enough memory, ABORT (do not launch the workflow)
    2 = error reading system stats

"Available" is approximated as free + inactive + purgeable pages. On macOS
unified memory, inactive and purgeable pages are reclaimable by the kernel on
demand, so this is a conservative-but-useful estimate of what a new process
can actually use before pressure hits the resident LLM.

Recommended thresholds (64 GB machine, LM Studio LLM resident):
    Hunyuan3D 2.1 (model 6.9 GB + VAE/activations)   -> --need 15
    z_image_turbo bf16 (11 GB)                        -> --need 20
    ltx-2.5 int8 video (20 GB)                        -> --need 28
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys


def _vm_stat() -> str:
    return subprocess.run(
        ["vm_stat"], capture_output=True, text=True, check=True
    ).stdout


def memory_stats() -> dict:
    vm = _vm_stat()

    m = re.search(r"page size of (\d+)", vm)
    if not m:
        raise RuntimeError("could not parse page size from vm_stat")
    page_size = int(m.group(1))

    total_bytes = int(subprocess.run(
        ["sysctl", "-n", "hw.memsize"], capture_output=True, text=True, check=True
    ).stdout.strip())

    def pages(key: str) -> int:
        mm = re.search(rf"{re.escape(key)}:\s+(\d+)", vm)
        return int(mm.group(1)) if mm else 0

    free = pages("Pages free")
    inactive = pages("Pages inactive")
    purgeable = pages("Pages purgeable")
    wired = pages("Wired down")
    compressed = pages("Pages occupied by compressor")

    def gb(n_pages: int) -> float:
        return round(n_pages * page_size / (1024 ** 3), 2)

    available_bytes = (free + inactive + purgeable) * page_size
    return {
        "page_size": page_size,
        "total_gb": round(total_bytes / (1024 ** 3), 2),
        "available_gb": round(available_bytes / (1024 ** 3), 2),
        "free_gb": gb(free),
        "inactive_gb": gb(inactive),
        "purgeable_gb": gb(purgeable),
        "wired_gb": gb(wired),
        "compressed_gb": gb(compressed),
    }


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Abort (exit 1) if available unified memory < --need GB."
    )
    ap.add_argument("--need", type=float, required=True,
                    help="Minimum GB of available memory required")
    ap.add_argument("--json", action="store_true", help="Emit JSON output")
    args = ap.parse_args()

    try:
        stats = memory_stats()
    except Exception as exc:  # noqa: BLE001 - report any stat failure as exit 2
        print(f"memory_guard: failed to read system memory stats: {exc}", file=sys.stderr)
        return 2

    ok = stats["available_gb"] >= args.need
    if args.json:
        out = dict(stats, need_gb=args.need, ok=ok)
        print(json.dumps(out))
    else:
        verdict = "OK - proceed" if ok else "NOT ENOUGH - ABORT generation"
        print(
            f"memory_guard: available {stats['available_gb']} GB "
            f"(free {stats['free_gb']} + inactive {stats['inactive_gb']} "
            f"+ purgeable {stats['purgeable_gb']}) / total {stats['total_gb']} GB; "
            f"need {args.need} GB -> {verdict}"
        )
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
