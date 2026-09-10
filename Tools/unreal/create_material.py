#!/usr/bin/env python3
"""Create a solid-color material via UE MCP: create_material + VectorParameter -> BaseColor."""
import json, sys, urllib.request
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
    name=sys.argv[1]; rgb=json.loads(sys.argv[2])  # [r,g,b]
    folder="/Game/Level"
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"m","version":"1"}}})
    mt="editor_toolset.toolsets.material.MaterialTools"; ot="editor_toolset.toolsets.object.ObjectTools"
    mat=call(session=sid,ts=mt,tool="create_material",args={"folder_path":folder,"asset_name":name})
    mpath=mat.get("returnValue",{}).get("refPath")
    if not mpath: print(f"create failed {mat}"); return
    print("material:", mpath)
    exp=call(session=sid,ts=mt,tool="add_expression",args={"material_or_function":{"refPath":mpath},"expression_class":{"refPath":"/Script/Engine.MaterialExpressionVectorParameter"}})
    epath=exp.get("returnValue",{}).get("refPath")
    if epath:
        val={"ParameterName":"Color","DefaultValue":{"R":rgb[0],"G":rgb[1],"B":rgb[2],"A":1.0}}
        call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":epath},"values":json.dumps(val)})
        call(session=sid,ts=mt,tool="connect_to_output",args={"material":{"refPath":mpath},"expression":{"refPath":epath},"output_name":"RGB","material_property":"MP_BaseColor"})
        call(session=sid,ts=mt,tool="recompile",args={"material_or_function":{"refPath":mpath}})
        print(f"{name} colorido + compilado")
if __name__=="__main__": main()
