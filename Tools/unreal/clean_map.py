#!/usr/bin/env python3
"""Clean the map: remove StaticMeshActors + PCG volumes, keep the environment."""
import json, urllib.request
URL="http://127.0.0.1:8000/mcp"
def rpc(p, session=None):
    h={"Content-Type":"application/json","Accept":"application/json, text/event-stream"}
    if session: h["Mcp-Session-Id"]=session
    req=urllib.request.Request(URL,data=json.dumps(p).encode(),headers=h)
    with urllib.request.urlopen(req,timeout=90) as r:
        sid=r.headers.get("Mcp-Session-Id"); b=r.read().decode()
        try: return json.loads(b),sid
        except Exception:
            for line in b.splitlines():
                if line.startswith("data:"): return json.loads(line[5:].strip()),sid
            return {"raw":b[:300]},sid
def call(session, ts, tool, args):
    res,_=rpc({"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"call_tool","arguments":{"toolset_name":ts,"tool_name":tool,"arguments":args}}},session=session)
    for c in res.get("result",{}).get("content",[]):
        try: return json.loads(c.get("text","null"))
        except: return c.get("text","")
    return res
def rv_list(r):
    if isinstance(r,dict):
        v=r.get("returnValue")
        return v if isinstance(v,list) else []
    return []
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"c","version":"1"}}})
    sms="editor_toolset.toolsets.scene.SceneTools"; removed=0
    for atype in ["/Script/Engine.StaticMeshActor"]:
        acts=call(session=sid,ts=sms,tool="find_actors",args={"name":"","tag":"","collision_channels":[],"actor_type":{"refPath":atype}})
        for a in rv_list(acts):
            r=call(session=sid,ts=sms,tool="remove_from_scene",args={"actor":{"refPath":a["refPath"]}})
            if r.get("returnValue"): removed+=1
    acts=call(session=sid,ts=sms,tool="find_actors",args={"name":"PCG","tag":"","collision_channels":[]})
    for a in rv_list(acts):
        p=a.get("refPath","")
        if "PCGVolume" in p or "PCGWorld" in p:
            r=call(session=sid,ts=sms,tool="remove_from_scene",args={"actor":{"refPath":p}})
            if r.get("returnValue"): removed+=1
    print("removidos:", removed)
    call(session=sid,ts="editor_toolset.toolsets.asset.AssetTools",tool="save_assets",args={"asset_paths":["/Game/NewMap"]})
if __name__=="__main__": main()
