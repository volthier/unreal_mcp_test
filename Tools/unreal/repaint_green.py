#!/usr/bin/env python3
"""Repaint level materials to the Mega Man X Highway green/teal palette."""
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
    ot="editor_toolset.toolsets.object.ObjectTools"; mt="editor_toolset.toolsets.material.MaterialTools"
    # material -> highway green palette (r,g,b)
    palette={
      "M_MMX_Orange": (0.22,0.45,0.32),  # highway/road green
      "M_MMX_Blue":   (0.18,0.42,0.36),  # platform teal-green
      "M_MMX_Gray":   (0.12,0.14,0.20),  # walls dark blue-gray
      "M_MMX_Lane":   (0.98,0.92,0.40),  # lane marks yellow (keep)
    }
    for name,rgb in palette.items():
        mat=f"/Game/Level/{name}.{name}"
        exp=call(session=sid,ts=mt,tool="get_expressions",args={"material_or_function":{"refPath":mat}})
        eps=exp.get("returnValue",[])
        if eps:
            epath=eps[0].get("refPath")
            val={"ParameterName":"Color","DefaultValue":{"R":rgb[0],"G":rgb[1],"B":rgb[2],"A":1.0}}
            ok=call(session=sid,ts=ot,tool="set_properties",args={"instance":{"refPath":epath},"values":json.dumps(val)})
            call(session=sid,ts=mt,tool="recompile",args={"material_or_function":{"refPath":mat}})
            print(f"{name} -> {rgb} ok={ok.get('returnValue')}")
if __name__=="__main__": main()
