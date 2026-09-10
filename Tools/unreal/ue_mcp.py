#!/usr/bin/env python3
"""Minimal UE engine MCP client (streamable HTTP on :8000).

Usage:
    python3 Tools/unreal/ue_mcp.py list_toolsets
    python3 Tools/unreal/ue_mcp.py call <tool_name> '<json args>'
"""
from __future__ import annotations

import json
import sys
import urllib.request

URL = "http://127.0.0.1:8000/mcp"


def rpc(payload, session=None):
    data = json.dumps(payload).encode()
    headers = {"Content-Type": "application/json", "Accept": "application/json, text/event-stream"}
    if session:
        headers["Mcp-Session-Id"] = session
    req = urllib.request.Request(URL, data=data, headers=headers)
    with urllib.request.urlopen(req, timeout=120) as r:
        sid = r.headers.get("Mcp-Session-Id")
        body = r.read().decode()
        try:
            return json.loads(body), sid
        except Exception:
            for line in body.splitlines():
                if line.startswith("data:"):
                    return json.loads(line[5:].strip()), sid
            return {"raw": body[:500]}, sid


def main():
    args = sys.argv[1:]
    init, sid = rpc({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {
        "protocolVersion": "2025-11-25", "capabilities": {},
        "clientInfo": {"name": "agent", "version": "1"}}})
    if not args:
        print(init.get("result", {}))
        return 0
    op = args[0]
    if op == "list_toolsets":
        res, _ = rpc({"jsonrpc": "2.0", "id": 2, "method": "tools/call",
                      "params": {"name": "list_toolsets", "arguments": {}}}, session=sid)
        _print(res)
    elif op == "call":
        fulltool, raw_args = args[1], args[2]
        if "." in fulltool:
            toolset_name, tool_name = fulltool.rsplit(".", 1)
        else:
            toolset_name, tool_name = "", fulltool
        reqargs = {"toolset_name": toolset_name, "tool_name": tool_name, "arguments": json.loads(raw_args)}
        res, _ = rpc({"jsonrpc": "2.0", "id": 2, "method": "tools/call",
                      "params": {"name": "call_tool", "arguments": reqargs}}, session=sid)
        _print(res)
    else:
        print("unknown op", op)


def _print(res):
    if "error" in res:
        print(json.dumps(res["error"], indent=1)[:1200])
        return
    for c in res.get("result", {}).get("content", []):
        print(c.get("text", ""))
    # also print structuredResult if present
    sr = res.get("result", {}).get("structuredResult")
    if sr is not None:
        print(json.dumps(sr, indent=1)[:1500])


if __name__ == "__main__":
    main()
