#!/usr/bin/env python3
"""Generate a 3D asset from a reference image via comfy-mcp (Hunyuan3D 2.1).

Usage:
  python3 Tools/comfy/generate_asset.py <image_path> <workflow_path> <out_dir> [--no-upload]

Flow (all through the comfy-mcp stdio server):
  upload_file(image) -> run_workflow(wait=False) -> job(action="wait") -> fetch_outputs
"""
from __future__ import annotations

import asyncio
import json
import os
import sys

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

COMFY_MCP = "/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"
COMFY_BIN = "/Users/volthier/.venvs/comfy-mcp/bin/comfy"


def text_of(resp) -> str:
    return "\n".join(c.text for c in resp.content if hasattr(c, "text"))


async def call(session, tool: str, args: dict) -> str:
    resp = await session.call_tool(tool, args)
    return text_of(resp)


def prompt_id_from(text: str) -> str:
    try:
        obj = json.loads(text)
        for holder in (obj, obj.get("data", {})):
            if isinstance(holder, dict):
                for k in ("prompt_id", "id", "promptId"):
                    if isinstance(holder.get(k), str):
                        return holder[k]
    except json.JSONDecodeError:
        pass
    import re
    m = re.search(r"[0-9a-f]{8}-[0-9a-f-]{27,}", text)
    return m.group(0) if m else ""


async def main() -> int:
    if len(sys.argv) < 4:
        print(__doc__)
        return 2
    image, workflow, out_dir = sys.argv[1], sys.argv[2], sys.argv[3]
    do_upload = "--no-upload" not in sys.argv
    os.makedirs(out_dir, exist_ok=True)

    params = StdioServerParameters(
        command=COMFY_MCP, args=[], env={**os.environ, "COMFY_BIN": COMFY_BIN}
    )
    async with stdio_client(params) as (r, w):
        async with ClientSession(r, w) as s:
            await s.initialize()

            if do_upload:
                print(f"upload_file({image})", flush=True)
                print((await call(s, "upload_file", {"paths": [image], "overwrite": True}))[:300], flush=True)

            print(f"run_workflow({workflow}) wait=False", flush=True)
            submit = await call(s, "run_workflow", {"workflow_path": workflow, "wait": False})
            print(submit[:400], flush=True)
            pid = prompt_id_from(submit)
            if not pid:
                print("ERROR: no prompt_id", flush=True)
                return 1
            print(f"PROMPT_ID={pid}", flush=True)

            print(f"job(action=wait) {pid}", flush=True)
            res = await call(s, "job", {"action": "wait", "prompt_id": pid, "timeout_seconds": 1800})
            print(res[-600:], flush=True)

            print(f"fetch_outputs -> {out_dir}", flush=True)
            print((await call(s, "fetch_outputs", {"prompt_id": pid, "out_dir": out_dir}))[:600], flush=True)
            return 0


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
