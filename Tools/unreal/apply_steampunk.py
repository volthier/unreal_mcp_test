#!/usr/bin/env python3
"""Apply steampunk materials to the level: brass on geometry, city on backdrop."""
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
def set_mat(session, apath, mat):
    comps=call(session=session,ts="editor_toolset.toolsets.actor.ActorTools",tool="get_components",args={"actor":{"refPath":apath}})
    cpath=next((c["refPath"] for c in comps.get("returnValue",[]) if "StaticMeshComponent" in c.get("refPath","")),"")
    if cpath:
        call(session=session,ts="editor_toolset.toolsets.object.ObjectTools",tool="set_properties",
             args={"instance":{"refPath":cpath},"values":json.dumps({"overrideMaterials":[{"refPath":f"/Game/Level/{mat}.{mat}"}]})})
        return True
    return False
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"m","version":"1"}}})
    sms="editor_toolset.toolsets.scene.SceneTools"
    # city backdrop -> M_Steam_City
    acts=call(session=sid,ts=sms,tool="find_actors",args={"name":"City_Backdrop","tag":"","collision_channels":[]})
    for a in acts.get("returnValue",[]):
        ok=set_mat(sid, a["refPath"], "M_Steam_City"); print(f"City_Backdrop -> M_Steam_City ok={ok}")
    # level geometry (all StaticMeshActors) -> M_Steam_Brass
    acts=call(session=sid,ts=sms,tool="find_actors",args={"name":"","tag":"","collision_channels":[],"actor_type":{"refPath":"/Script/Engine.StaticMeshActor"}})
    n=0
    for a in acts.get("returnValue",[]):
        p=a.get("refPath","")
        if "City_Backdrop" in p: continue
        if set_mat(sid,p,"M_Steam_Brass"): n+=1
    print(f"geometria -> M_Steam_Brass ({n} atores)")
if __name__=="__main__": main()
