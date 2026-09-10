#!/usr/bin/env python3
"""Add yellow lane markings along the road (highway look)."""
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
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"m","version":"1"}}})
    sms="editor_toolset.toolsets.scene.SceneTools"; at="editor_toolset.toolsets.actor.ActorTools"; ot="editor_toolset.toolsets.object.ObjectTools"
    m=call(session=sid,ts="editor_toolset.toolsets.material.MaterialTools",tool="create_material",args={"folder_path":"/Game/Level","asset_name":"M_MMX_Lane"})
    mpath=m.get("returnValue",{}).get("refPath")
    exp=call(session=sid,ts="editor_toolset.toolsets.material.MaterialTools",tool="add_expression",args={"material_or_function":{"refPath":mpath},"expression_class":{"refPath":"/Script/Engine.MaterialExpressionVectorParameter"}})
    epath=exp.get("returnValue",{}).get("refPath")
    call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":epath},"values":json.dumps({"ParameterName":"Color","DefaultValue":{"R":0.85,"G":0.75,"B":0.2,"A":1.0}})})
    call(session=sid,ts="editor_toolset.toolsets.material.MaterialTools",tool="connect_to_output",args={"material":{"refPath":mpath},"expression":{"refPath":epath},"output_name":"RGB","material_property":"MP_EmissiveColor"})
    call(session=sid,ts="editor_toolset.toolsets.material.MaterialTools",tool="recompile",args={"material_or_function":{"refPath":mpath}})
    # lane mark segments along the road (top of ground ~ Z=2)
    for i in range(-1900, 2600, 160):
        args={"asset_path":"/Engine/BasicShapes/Cube.Cube","name":"Lane","xform":{"location":{"x":i,"y":0,"z":2},"rotation":{"pitch":0,"yaw":0,"roll":0},"scale":{"x":1.2,"y":0.25,"z":0.05}}}
        a=call(session=sid,ts=sms,tool="add_to_scene_from_asset",args=args)
        apath=a.get("returnValue",{}).get("refPath","")
        comps=call(session=sid,ts=at,tool="get_components",args={"actor":{"refPath":apath}})
        cpath=next((c["refPath"] for c in comps.get("returnValue",[]) if "StaticMeshComponent" in c.get("refPath","")),"")
        if cpath:
            call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":cpath},"values":json.dumps({"overrideMaterials":[{"refPath":f"/Game/Level/M_MMX_Lane.M_MMX_Lane"}]})})
    print("lane marks added + M_MMX_Lane criado")
if __name__=="__main__": main()
