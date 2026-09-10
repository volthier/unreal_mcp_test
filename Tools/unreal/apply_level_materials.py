#!/usr/bin/env python3
"""Apply colored materials to the Mega Man X level cube actors (by height)."""
import json, urllib.request
URL="http://127.0.0.1:8000/mcp"
def rpc(p, session=None):
    h={"Content-Type":"application/json","Accept":"application/json, text/event-stream"}
    if session: h["Mcp-Session-Id"]=session
    req=urllib.request.Request(URL,data=json.dumps(p).encode(),headers=h)
    with urllib.request.urlopen(req,timeout=60) as r:
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
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"m","version":"1"}}})
    sms="editor_toolset.toolsets.scene.SceneTools"; at="editor_toolset.toolsets.actor.ActorTools"; ot="editor_toolset.toolsets.object.ObjectTools"
    actors=call(session=sid,ts=sms,tool="find_actors",args={"name":"","tag":"","collision_channels":[],"actor_type":{"refPath":"/Script/Engine.StaticMeshActor"}})
    items=actors.get("returnValue",[])
    print("StaticMeshActors encontrados:", len(items))
    for act in items:
        apath=act.get("refPath","")
        comps=call(session=sid,ts=at,tool="get_components",args={"actor":{"refPath":apath}})
        cpath=None
        for c in comps.get("returnValue",[]):
            if "StaticMeshComponent" in c.get("refPath",""):
                cpath=c["refPath"]; break
        if not cpath: continue
        b=call(session=sid,ts=at,tool="get_actor_bounds",args={"actor":{"refPath":apath}})
        bb=b.get("returnValue",{})
        zt=bb.get("max",{}).get("z",0); zb=bb.get("min",{}).get("z",0); h=zt-zb
        # pick material by height/extent
        if h>350: mat="M_MMX_Gray"      # tall walls/pillars
        elif zt<150: mat="M_MMX_Orange" # ground (low, top near 0)
        else: mat="M_MMX_Blue"          # platforms
        ok=call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":cpath},"values":json.dumps({"overrideMaterials":[{"refPath":f"/Game/Level/{mat}.{mat}"}]})})
        print(f"  {apath[-20:]} -> {mat} (zt={zt:.0f},h={h:.0f}) ok={ok.get('returnValue')}")
if __name__=="__main__": main()
