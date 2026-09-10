#!/usr/bin/env python3
"""Build a Mega Man X style side-scroller level in the open UE map via MCP."""
import json
import sys
import urllib.request

URL = "http://127.0.0.1:8000/mcp"


def rpc(payload, session=None):
    headers = {"Content-Type": "application/json", "Accept": "application/json, text/event-stream"}
    if session:
        headers["Mcp-Session-Id"] = session
    req = urllib.request.Request(URL, data=json.dumps(payload).encode(), headers=headers)
    with urllib.request.urlopen(req, timeout=90) as r:
        sid = r.headers.get("Mcp-Session-Id")
        body = r.read().decode()
        try:
            return json.loads(body), sid
        except Exception:
            for line in body.splitlines():
                if line.startswith("data:"):
                    return json.loads(line[5:].strip()), sid
            return {"raw": body[:300]}, sid


def call(session, toolset, tool, args):
    res, _ = rpc({"jsonrpc": "2.0", "id": 3, "method": "tools/call", "params": {
        "name": "call_tool", "arguments": {"toolset_name": toolset, "tool_name": tool, "arguments": args}}},
        session=session)
    if "error" in res:
        return {"__err": res["error"]}
    for c in res.get("result", {}).get("content", []):
        try:
            return json.loads(c.get("text", "null"))
        except Exception:
            return c.get("text", "")
    return res


def xform(x, y, z, sx, sy, sz):
    return {"location": {"x": x, "y": y, "z": z},
            "rotation": {"pitch": 0, "yaw": 0, "roll": 0},
            "scale": {"x": sx, "y": sy, "z": sz}}


# (name, center_x, center_z, scale_x, scale_y, scale_z)
LAYOUT = [
    # ground: long slab, top surface ~ Z=0
    ("Ground",      400, -100, 64, 6, 2),
    # raised platforms (side-scroller stepping)
    ("Platform1",  -700, 150, 10, 6, 1),
    ("Platform2",  -250, 320, 8,  6, 1),
    ("Platform3",   250, 500, 8,  6, 1),
    ("Platform4",   900, 300, 10, 6, 1),
    ("Platform5",  1500, 520, 8,  6, 1),
    ("Platform6",  2100, 250, 12, 6, 1),
    # vertical pillars / walls
    ("Wall1",     -1100, 350, 2, 6, 8),
    ("Wall2",      2600, 400, 2, 6, 9),
    ("Pillar1",     600,  250, 2, 6, 4),
    ("Pillar2",    1800,  450, 2, 6, 6),
    # a few short boxes as obstacles
    ("Box1",        0,    50,  3, 6, 1),
    ("Box2",       1300,  50,  3, 6, 1),
]


def main():
    init, sid = rpc({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {
        "protocolVersion": "2025-11-25", "capabilities": {},
        "clientInfo": {"name": "levelbuilder", "version": "1"}}})
    ts = "editor_toolset.toolsets.scene.SceneTools"
    for name, cx, cz, sx, sy, sz in LAYOUT:
        args = {"asset_path": "/Engine/BasicShapes/Cube.Cube", "name": name,
                "xform": xform(cx, 0, cz, sx, sy, sz)}
        r = call(session=sid, toolset=ts, tool="add_to_scene_from_asset", args=args)
        if isinstance(r, dict) and "__err" in r:
            print(f"[{name}] ERROR: {r['__err']}")
        else:
            print(f"[{name}] OK -> {r.get('returnValue',{}).get('refPath','')[:60]}")
    # set the viewport camera to a side view (Mega Man X) via editor app toolset
    print("done")


if __name__ == "__main__":
    main()
