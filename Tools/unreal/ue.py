#!/usr/bin/env python3
"""Generic UE MCP caller. Usage:
  python3 Tools/unreal/ue.py '<toolset_full_name>' '<tool_name>' '<json_args>'
Example:
  python3 Tools/unreal/ue.py 'editor_toolset.toolsets.asset.AssetTools' 'find_assets' '{"folder_path":"/Game/Level"}'
"""
from __future__ import annotations
import json, sys
sys.path.insert(0, __file__.rsplit('/', 1)[0])
import ue_mcp

def call(toolset_name, tool_name, args):
    init, sid = ue_mcp.rpc({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {
        "protocolVersion": "2025-11-25", "capabilities": {},
        "clientInfo": {"name": "agent", "version": "1"}}})
    res, _ = ue_mcp.rpc({"jsonrpc": "2.0", "id": 2, "method": "tools/call",
                         "params": {"name": "call_tool",
                                    "arguments": {"toolset_name": toolset_name,
                                                  "tool_name": tool_name,
                                                  "arguments": args}}}, session=sid)
    if "error" in res:
        print("ERROR", json.dumps(res["error"], indent=1)[:1500])
        return None
    for c in res.get("result", {}).get("content", []):
        print(c.get("text", ""))
    sr = res.get("result", {}).get("structuredResult")
    if sr is not None:
        print(json.dumps(sr, indent=1)[:2500])
    return res

if __name__ == "__main__":
    ts, tn, a = sys.argv[1], sys.argv[2], json.loads(sys.argv[3])
    call(ts, tn, a)
