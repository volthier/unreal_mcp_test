#!/usr/bin/env python3
"""Generate steampunk Mega Man X textures via comfy-mcp, then fetch_outputs to disk."""
import asyncio, json, os, re, sys, shutil
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client
COMFY_MCP="/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"; COMFY_BIN="/Users/volthier/.venvs/comfy-mcp/bin/comfy"
OUT="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/steampunk"
def text_of(r): return "\n".join(c.text for c in r.content if hasattr(c,"text"))
def pid_of(t):
    m=re.search(r'"prompt_id":\s*"([0-9a-f-]+)"', t); return m.group(1) if m else ""
PROMPTS=[
 ("city_street","steampunk victorian megacity street, brass pipes and gears, foggy dusk, dark purple sky with glowing orange windows, high detail concept art"),
 ("brass_wall","seamless steampunk brass metal plate texture with rivets and scratches, dark copper, high detail game texture"),
 ("ice_cave","frozen ice cave, glowing blue crystals, steampunk metal ice structures, cold mist, concept art"),
 ("bio_forest","biomechanical forest, organic metal trees with brass leaves and glowing cyan veins, fog, dark, concept art"),
 ("steam_house","steampunk victorian house with copper roof, gears and pipes on walls, warm lit windows, concept art"),
 ("plants_gears","steampunk gears and copper pipes with small mechanical plants, dark industrial, high detail texture"),
]
async def main():
    only=sys.argv[1] if len(sys.argv)>1 else None
    os.makedirs(OUT, exist_ok=True)
    params=StdioServerParameters(command=COMFY_MCP,args=[],env={**os.environ,"COMFY_BIN":COMFY_BIN})
    async with stdio_client(params) as (r,w):
        async with ClientSession(r,w) as s:
            await s.initialize()
            for name,prompt in PROMPTS:
                if only and name!=only: continue
                print(f"== {name}: gerando...", flush=True)
                try:
                    resp=await s.call_tool("generate_image",{"prompt":prompt,"wait":True,"timeout_seconds":300})
                    t=text_of(resp); p=pid_of(t)
                    print(f"   prompt_id={p}", flush=True)
                    if p:
                        fr=await s.call_tool("fetch_outputs",{"prompt_id":p,"out_dir":OUT})
                        print(f"   {text_of(fr)[:300]}", flush=True)
                except Exception as e:
                    print(f"   ERRO {e}", flush=True)
asyncio.run(main())
