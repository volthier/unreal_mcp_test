#!/usr/bin/env python3
"""Create a material using a texture (TextureSample -> EmissiveColor)."""
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
    matname=sys.argv[1]; tex=sys.argv[2]
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"m","version":"1"}}})
    mt="editor_toolset.toolsets.material.MaterialTools"; ot="editor_toolset.toolsets.object.ObjectTools"
    mat=call(session=sid,ts=mt,tool="create_material",args={"folder_path":"/Game/Level","asset_name":matname})
    mpath=mat.get("returnValue",{}).get("refPath")
    exp=call(session=sid,ts=mt,tool="add_expression",args={"material_or_function":{"refPath":mpath},"expression_class":{"refPath":"/Script/Engine.MaterialExpressionTextureSample"}})
    epath=exp.get("returnValue",{}).get("refPath")
    if not epath: print("no tex expr", exp); return
    # set the texture on the sample
    call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":epath},"values":json.dumps({"texture":{"refPath":tex}})})
    # connect RGB -> EmissiveColor
    call(session=sid,ts=mt,tool="connect_to_output",args={"material":{"refPath":mpath},"expression":{"refPath":epath},"output_name":"RGB","material_property":"MP_EmissiveColor"})
    call(session=sid,ts=mt,tool="recompile",args={"material_or_function":{"refPath":mpath}})
    print(f"{matname} criado com textura {tex}")
if __name__=="__main__": main()
