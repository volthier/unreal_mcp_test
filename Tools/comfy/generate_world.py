#!/usr/bin/env python3
"""Generate Mega Man X world concept assets via ComfyUI → fetch_outputs."""
import asyncio, json, os, re, sys
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client
COMFY_MCP="/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"; COMFY_BIN="/Users/volthier/.venvs/comfy-mcp/bin/comfy"
OUT="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/world"
def text_of(r): return "\n".join(c.text for c in r.content if hasattr(c,"text"))
def pid_of(t):
    m=re.search(r'"prompt_id":\s*"([0-9a-f-]+)"', t); return m.group(1) if m else ""
CONCEPTS=[
 ("city_building","towering vertical sci-fi skyscraper reaching to the clouds, dark solar panel facade, polished metal alloy, worn holographic billboards flickering old ads, high tech futuristic city, Mega Man X style, concept art"),
 ("street_neon","suspended magnetic highway between skyscrapers, neon blue green orange signs, dark synthetic asphalt, steam vents, futuristic cyberpunk street, Mega Man X, cinematic"),
 ("factory","brutalist automated factory, riveted steel walls, glowing plasma generators, liquid metal vats, robotic arms on conveyor belts, heavy industrial, Mega Man X, dark moody"),
 ("reploid","humanoid reploid robot with expressive face and articulate joints, spherical joints, shoulder pistons, modular armor in blue red yellow, polished chrome, Mega Man X character concept"),
 ("maverick","corrupted battle-damaged robot with glowing red eyes, rusty weathered armor, scratches and battle damage, menacing, Mega Man X maverick concept"),
 ("mech_scorpion","biomechanical scorpion robot with laser tail, chrome carapace, menacing, cyberpunk wildlife, Mega Man X, concept art"),
 ("bio_tree","organic-synthetic tree, carbon fiber trunk, circuit-board printed leaves, glowing nutrient tubes, greenhouse controlled biosphere, biopunk, Mega Man X, concept"),
]
async def main():
    only=sys.argv[1] if len(sys.argv)>1 else None
    os.makedirs(OUT, exist_ok=True)
    params=StdioServerParameters(command=COMFY_MCP,args=[],env={**os.environ,"COMFY_BIN":COMFY_BIN})
    async with stdio_client(params) as (r,w):
        async with ClientSession(r,w) as s:
            await s.initialize()
            for name,prompt in CONCEPTS:
                if only and name!=only: continue
                print(f"== {name}", flush=True)
                try:
                    resp=await s.call_tool("generate_image",{"prompt":prompt,"wait":True,"timeout_seconds":300})
                    p=pid_of(text_of(resp))
                    if p:
                        await s.call_tool("fetch_outputs",{"prompt_id":p,"out_dir":OUT})
                        print(f"   -> {p}", flush=True)
                except Exception as e: print(f"   ERRO {e}", flush=True)
asyncio.run(main())
