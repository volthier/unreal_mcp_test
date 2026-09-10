#!/usr/bin/env python3
"""Generate isolated forest biome assets (single object, plain bg) via ComfyUI."""
import asyncio, os, re, sys
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client
COMFY_MCP="/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"; COMFY_BIN="/Users/volthier/.venvs/comfy-mcp/bin/comfy"
OUT="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/forest"
def text_of(r): return "\n".join(c.text for c in r.content if hasattr(c,"text"))
def pid_of(t):
    m=re.search(r'"prompt_id":\s*"([0-9a-f-]+)"', t); return m.group(1) if m else ""
STYLE=", single isolated game asset, plain dark background, photorealistic, biopunk, Mega Man X, high detail"
# isolated assets (name -> prompt). Each is ONE object, no scene.
ASSETS=[
 ("plant_fern","organic-synthetic fern plant, carbon fiber stem, circuit-board fronds, glowing nutrient veins"+STYLE),
 ("plant_bulb","glowing bioluminescent bulb plant, glass dome, fluorescent nutrient tube, biomechanical"+STYLE),
 ("plant_vine","hanging vine plant, electrical cables with moss colonizing them, biopunk greenhouse"+STYLE),
 ("plant_fungus","glowing biopunk mushroom cluster, metallic cap, biomechanical fungus"+STYLE),
 ("bush_small","small biomechanical bush, metal-leaved shrub, cyberpunk, isolated"+STYLE),
 ("bush_large","large biomechanical bush tree, dense circuit-leaves, tropical biopunk, isolated asset"+STYLE),
 ("soil_texture","seamless dark soil ground texture with roots and metal scraps, top-down, game texture"),
 ("forest_ground","patch of biomechanical forest ground, moss on metal plates, cyberpunk, game asset, top-down"+STYLE),
 ("mech_rabbit","biomechanical rabbit robot, chrome and rust, glowing eyes, wildlife, cyberpunk, isolated"+STYLE),
 ("mech_beetle","biomechanical beetle robot with titanium shell, glowing, insect wildlife, isolated asset"+STYLE),
]
async def main():
    only=sys.argv[1] if len(sys.argv)>1 else None
    os.makedirs(OUT, exist_ok=True)
    params=StdioServerParameters(command=COMFY_MCP,args=[],env={**os.environ,"COMFY_BIN":COMFY_BIN})
    async with stdio_client(params) as (r,w):
        async with ClientSession(r,w) as s:
            await s.initialize()
            for name,prompt in ASSETS:
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
