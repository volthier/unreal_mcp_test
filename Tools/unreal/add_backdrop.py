#!/usr/bin/env python3
"""Add a Mega Man X dusk backdrop (sky plane, city silhouette, sun) behind the play area."""
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
def xform(x,y,z,sx,sy,sz):
    return {"location":{"x":x,"y":y,"z":z},"rotation":{"pitch":0,"yaw":0,"roll":0},"scale":{"x":sx,"y":sy,"z":sz}}
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"b","version":"1"}}})
    sms="editor_toolset.toolsets.scene.SceneTools"; at="editor_toolset.toolsets.actor.ActorTools"; ot="editor_toolset.toolsets.object.ObjectTools"
    def add(name,x,y,z,sx,sy,sz,mat):
        args={"asset_path":"/Engine/BasicShapes/Cube.Cube","name":name,"xform":xform(x,y,z,sx,sy,sz)}
        a=call(session=sid,ts=sms,tool="add_to_scene_from_asset",args=args)
        apath=a.get("returnValue",{}).get("refPath","")
        if not apath: print(f"[{name}] add failed {a}"); return
        comps=call(session=sid,ts=at,tool="get_components",args={"actor":{"refPath":apath}})
        cpath=next((c["refPath"] for c in comps.get("returnValue",[]) if "StaticMeshComponent" in c.get("refPath","")),"")
        if cpath:
            call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":cpath},"values":json.dumps({"overrideMaterials":[{"refPath":f"/Game/Level/{mat}.{mat}"}]})})
        print(f"[{name}] OK {apath[-24:]}")
    # SKY: huge flat slab far behind (depth -Y)
    add("Sky", 400,-4200,400, 120,10,80, "M_MMX_Sky")
    # SUN: glowing orange disc on horizon
    add("Sun", -200,-4180,520, 30,0.5,30, "M_MMX_Sun")
    # CITY: row of dark silhouette buildings in front of the sky
    for i,(bx,bh) in enumerate([(-1400,90),(-1000,140),(-600,110),(-200,170),(300,100),(700,150),(1200,120),(1700,180),(2200,95),(2500,140)]):
        add(f"City_{i}", bx,-4050,bh/2.0, 8,3,bh/100.0, "M_MMX_City")
if __name__=="__main__": main()
