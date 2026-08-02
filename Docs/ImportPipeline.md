# Import Pipeline

## Blender → Unreal

1. Run `Tools/Blender/build_voltstriker.py` via Blender MCP (`execute_blender_code`).
2. Outputs:
   - `Art/VoltStriker/SKM_VoltStriker.fbx`
   - `Art/VoltStriker/VoltStriker.blend`
   - `Art/VoltStriker/Textures/T_VoltStriker_*.png` (2K game + 4K source)
3. Import via Unreal MCP:

```
SkeletalMeshTools.import_file(
  folder_path=/Game/VoltStriker/Mesh,
  asset_name=SKM_VoltStriker,
  source_file=<absolute fbx>,
  import_materials=true,
  import_textures=true,
  import_animations=true,
  create_physics_asset=true
)
```

4. Add **Skeletal Mesh Sockets** in Unreal (do **not** export empties as bones):
   - `weapon_r`, `MuzzleFlash`, `ProjectileSpawn`, `FX` on bone `weapon_r`
   - Exporting Blender empties named `SOCKET_*` creates unweighted bones and triggers "missing from bind pose" warnings.

## Scale

Blender builds in meters. `SkeletalMeshTools.import_file` currently keeps FBX numeric values as-is (no automatic m→cm). Use **mesh component scale 100** on `BP_PlayerRobot` / `VoltStrikerCharacter` so height ≈ 155 cm in Unreal. FBX export uses `global_scale=1.0`, `apply_unit_scale=True`, `-Y` forward, `Z` up.
