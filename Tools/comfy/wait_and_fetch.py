#!/usr/bin/env python3
"""Wait for a ComfyUI prompt to finish, then download its output files.

Usage: python3 Tools/comfy/wait_and_fetch.py <prompt_id> [out_dir]

Polls /history/<id>, and once the job is complete streams every output file
(glb/png/... ) into ``out_dir`` via /view. Stdlib only; writes inside the repo.
"""
from __future__ import annotations

import json
import os
import sys
import time
import urllib.request

BASE = "http://127.0.0.1:8188"
PROJECT = "/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST"
DEFAULT_OUT = os.path.join(PROJECT, "Art", "generated", "hunyuan3d_test")


def _get(url: str, timeout: int = 20) -> bytes:
    with urllib.request.urlopen(url, timeout=timeout) as r:
        return r.read()


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: wait_and_fetch.py <prompt_id> [out_dir]")
        return 2
    prompt_id = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_OUT
    os.makedirs(out_dir, exist_ok=True)

    status = "pending"
    deadline = time.time() + 3600  # 1h cap
    history = {}
    while time.time() < deadline:
        try:
            history = json.loads(_get(f"{BASE}/history/{prompt_id}").decode())
        except Exception as exc:  # noqa: BLE001
            print(f"poll error: {exc}", flush=True)
        st = history.get(prompt_id, {}).get("status", {})
        status = st.get("status_str", st.get("status", "unknown"))
        done = history.get(prompt_id, {}).get("status", {}).get("completed", False)
        print(f"[{time.strftime('%H:%M:%S')}] status={status} completed={done}", flush=True)
        if done:
            break
        time.sleep(15)

    if not done:
        print(f"TIMEOUT waiting for {prompt_id}", flush=True)
        return 1

    outputs = history.get(prompt_id, {}).get("outputs", {})
    saved = []
    for node_id, node_out in outputs.items():
        for key, val in (node_out or {}).items():
            items = val if isinstance(val, list) else [val]
            for item in items:
                if not isinstance(item, dict) or "filename" not in item:
                    continue
                fn = item["filename"]
                sub = item.get("subfolder", "")
                ftype = item.get("type", "output")
                # SaveGLB may put the glb under subfolder "mesh/..." — flatten to basename.
                safe_name = fn.replace("/", "_")
                dest = os.path.join(out_dir, safe_name)
                url = f"{BASE}/view?filename={urllib.parse.quote(fn)}&subfolder={urllib.parse.quote(sub)}&type={ftype}"
                data = _get(url)
                with open(dest, "wb") as fh:
                    fh.write(data)
                saved.append((safe_name, len(data)))
                print(f"saved {safe_name} ({len(data)} bytes) from node {node_id}/{key}", flush=True)

    print(f"DONE: {len(saved)} file(s) in {out_dir}", flush=True)
    for name, size in saved:
        print(f"  - {name} ({size} bytes)", flush=True)
    return 0


if __name__ == "__main__":
    import urllib.parse  # noqa: E402
    sys.exit(main())
