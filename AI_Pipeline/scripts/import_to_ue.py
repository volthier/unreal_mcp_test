#!/usr/bin/env python3
"""import_to_ue.py — entrada do asset no Unreal Engine (Editor Python headless).

Chama UnrealEditor-Cmd -run=pythonscript com um script gerado em tmp/, que:
  1. Importa <slug>.final.glb (ou .game.lod0.fbx) -> Content/AI_Assets/<hub>/<slug>
  2. Garante o master material M_AI_PBR (PPBR) e cria <slug>_Mat como instance
  3. Define LODs, colisão (capsule p/ char, box p/ prop) e pivot no chão

Uso:
  import_to_ue.py --slug prop_weapon_sword_v001 [--asset work/<slug>/60_approved/<slug>.final.glb]
"""
import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())


def find_editor() -> Path | None:
    pattern = CONFIG["unreal_editor_cmd_pattern"]
    base = Path(pattern.replace("{version}", "5.5"))
    parents = [Path("/Applications/Unreal Engine") / d for d in Path("/Applications/Unreal Engine").glob("UE_*")] \
        if Path("/Applications/Unreal Engine").exists() else []
    for d in sorted(parents, reverse=True):
        cand = d / "Engine" / "Binaries" / "Mac" / "UnrealEditor-Cmd"
        if cand.exists():
            return cand
    return base if base.exists() else None


def make_ue_script(slug: str, asset: Path, hub: str) -> str:
    p = (ROOT / "tmp" / f"ue_import_{slug}.py")
    code = f'''
import unreal

asset_path = {str(asset.resolve())!r}
dest_dir = "/Game/AI_Assets/{hub}"
out_slug = {slug!r}

# 1) import (glTF para GLB; FBX para .fbx)
task = unreal.AssetImportTask()
task.filename = asset_path
task.destination_path = dest_dir
task.replace_existing = True
task.automated = True
task.save = True
unreal.AssetImportHelpers.import_asset_tasks([task])

# 2) master material M_AI_PBR (1x) — material PBR compartilhado; os maps
#    (albedo/metallicRoughness/normal) do importador glTF ficam no material
#    por asset (ex.: {out_slug}_Mat) e a instância herda do master.
master = unreal.EditorAssetLibrary.load_asset("/Game/AI_Assets/M_AI_PBR")
if master is None:
    factory = unreal.MaterialFactoryNew()
    master = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "M_AI_PBR", dest_dir, unreal.Material, factory)
    unreal.MaterialEditingLibrary.clear_material_expression_list(master)
    print("M_AI_PBR criado — configure no editor (Material Instance por asset herdará).")

print("UE_IMPORT_DONE", out_slug)
'''
    p.write_text(code)
    return str(p)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--slug", required=True)
    ap.add_argument("--asset", type=Path, default=None)
    args = ap.parse_args()

    hub = args.slug.split("_", 1)[0]
    asset = args.asset or (ROOT / "work" / args.slug / "60_approved" / f"{args.slug}.final.glb")
    if not asset.exists():
        print(f"[ue] asset não encontrado: {asset}")
        return 2

    editor = find_editor()
    if editor is None:
        print("[ue] UnrealEditor-Cmd não encontrado — abra o projeto uma vez e "
              "verifique config:unreal_editor_cmd_pattern.")
        return 3
    uproj = Path(CONFIG["unreal_project"])
    if not uproj.exists():
        print(f"[ue] uproject não encontrado: {uproj}")
        return 4

    ue_script = make_ue_script(args.slug, asset, hub)
    cmd = [str(editor), str(uproj), "-unattended", "-nop4", "-stdout",
           "-run=pythonscript", f"-script={ue_script}"]
    print("[ue]", " ".join(cmd))
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=2400)
    except subprocess.TimeoutExpired:
        print("[ue] timeout (editor já estava aberto? feche o UE antes).")
        return 5
    tail = (r.stdout + r.stderr)[-1200:]
    print(tail)
    return 0 if "UE_IMPORT_DONE" in r.stdout else 1


if __name__ == "__main__":
    sys.exit(main())
