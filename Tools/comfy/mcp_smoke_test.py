#!/usr/bin/env python3
"""End-to-end smoke test for the comfy-mcp server (stdio transport).

Must be run with the venv Python that has the ``mcp`` package installed:
    ~/.venvs/comfy-mcp/bin/python Tools/comfy/mcp_smoke_test.py            # tools + server_info + validate
    ~/.venvs/comfy-mcp/bin/python Tools/comfy/mcp_smoke_test.py --run     # + submit, wait, fetch_outputs

Exercises exactly the tools the agent uses (list_tools, server_info,
validate_workflow, system_stats, run_workflow, job, fetch_outputs).
"""
from __future__ import annotations

import asyncio
import json
import os
import sys

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

HERE = os.path.dirname(os.path.abspath(__file__))
COMFY_MCP = "/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"
COMFY_BIN = "/Users/volthier/.venvs/comfy-mcp/bin/comfy"
WORKFLOW = os.path.join(HERE, "workflows", "hunyuan3d_test.json")
PROJECT = "/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST"
OUT_DIR = os.path.join(PROJECT, "Art", "generated", "hunyuan3d_test")


def text_of(resp) -> str:
    """Concatenate text content blocks from an MCP tool result."""
    return "\n".join(
        c.text for c in resp.content if hasattr(c, "text")
    )


async def main() -> int:
    do_run = "--run" in sys.argv
    env = {**os.environ, "COMFY_BIN": COMFY_BIN}
    params = StdioServerParameters(command=COMFY_MCP, args=[], env=env)

    async with stdio_client(params) as (read_stream, write_stream):
        async with ClientSession(read_stream, write_stream) as session:
            await session.initialize()

            tools = await session.list_tools()
            names = sorted(t.name for t in tools.tools)
            print(f"TOOLS({len(names)}): {', '.join(names)}", flush=True)

            for tool, args in [
                ("server_info", {}),
                ("system_stats", {}),
                ("validate_workflow", {"workflow_path": WORKFLOW}),
            ]:
                try:
                    resp = await session.call_tool(tool, args)
                    print(f"\n=== {tool} ===", flush=True)
                    print(text_of(resp)[:1200], flush=True)
                except Exception as exc:  # noqa: BLE001
                    print(f"\n=== {tool} ERROR: {exc} ===", flush=True)

            if not do_run:
                print("\n(no --run; stopping here)", flush=True)
                return 0

            print("\n=== submitting run_workflow (wait=False) ===", flush=True)
            resp = await session.call_tool(
                "run_workflow", {"workflow_path": WORKFLOW, "wait": False}
            )
            submit = text_of(resp)
            print(submit[:800], flush=True)

            prompt_id = _extract_prompt_id(submit)
            if not prompt_id:
                print("\nERROR: could not extract prompt_id from submit response", flush=True)
                return 1
            print(f"\nPROMPT_ID={prompt_id}", flush=True)

            print(f"\n=== waiting for job {prompt_id} (up to 3600s) ===", flush=True)
            resp = await session.call_tool(
                "job",
                {"action": "wait", "prompt_id": prompt_id, "timeout_seconds": 3600},
            )
            print(text_of(resp)[:1500], flush=True)

            print(f"\n=== fetch_outputs -> {OUT_DIR} ===", flush=True)
            os.makedirs(OUT_DIR, exist_ok=True)
            resp = await session.call_tool(
                "fetch_outputs", {"prompt_id": prompt_id, "out_dir": OUT_DIR}
            )
            print(text_of(resp)[:1500], flush=True)

            return 0


def _extract_prompt_id(text: str) -> str:
    """Best-effort extraction of a prompt_id from the submit output."""
    try:
        # The tool returns an envelope-style JSON string.
        obj = json.loads(text)
        for key in ("prompt_id", "id", "promptId"):
            if isinstance(obj, dict) and isinstance(obj.get(key), str):
                return obj[key]
        # nested under data
        data = obj.get("data", {}) if isinstance(obj, dict) else {}
        for key in ("prompt_id", "id", "promptId"):
            if isinstance(data, dict) and isinstance(data.get(key), str):
                return data[key]
    except json.JSONDecodeError:
        pass
    import re
    m = re.search(r"[0-9a-f]{8}-[0-9a-f-]{27,}", text)
    return m.group(0) if m else ""


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
