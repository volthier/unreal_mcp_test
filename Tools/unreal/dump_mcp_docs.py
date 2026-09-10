#!/usr/bin/env python3
"""Dump every UE MCP toolset + its tools + descriptions into a readable table."""
import json, urllib.request, re
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
def call(session, name, args):
    res,_=rpc({"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":name,"arguments":args}},session=session)
    for c in res.get("result",{}).get("content",[]):
        return c.get("text","")
    return json.dumps(res)
def main():
    init,sid=rpc({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-11-25","capabilities":{},"clientInfo":{"name":"docs","version":"1"}}})
    ts_list=call(session=sid,name="list_toolsets",args={})
    # parse toolset names (lines like "- Name.Subname: desc")
    items=[]
    for line in ts_list.splitlines():
        m=re.match(r"-\s*([A-Za-z0-9_.]+\.[A-Za-z0-9_.]+):\s*(.*)", line.strip())
        if m: items.append((m.group(1), m.group(2)))
    print(f"# UE MCP — {len(items)} toolsets\n")
    for toolset, desc in items:
        try:
            d=call(session=sid,name="describe_toolset",args={"toolset_name":toolset})
            data=json.loads(d)
            tools=data.get("tools",[])
            print(f"## {toolset}  ({len(tools)} tools)")
            print(f"> {desc[:120]}")
            for t in tools:
                tname=t["name"].split(".")[-1]
                td=t.get("description","").strip().split("\n")[0]
                print(f"- `{tname}` — {td[:110]}")
            print()
        except Exception as e:
            print(f"## {toolset}\n> ERR {e}\n")
if __name__=="__main__": main()
