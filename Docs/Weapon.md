# VoltStriker Weapon

Modular integrated plasma cannon attached to `weapon_r`.

## Modules

| Mesh | Purpose |
|------|---------|
| Barrel | Main emitter tube |
| Core | Plasma containment sphere |
| Cooling fins | Heat dissipation rings |
| Charging chamber | Mid-body charge volume |
| Muzzle | Exit collar |

## Sockets

| Socket | Use |
|--------|-----|
| `MuzzleFlash` | Niagara muzzle flash spawn |
| `ProjectileSpawn` | Projectile launch origin |
| `FX` | Charge particle attach |
| `weapon_r` | Character attachment |

## Classes

- C++: `AVoltStrikerWeapon`
- BP: `BP_Weapon` (subclass after compile)

## Charge Visuals

`SetChargeVisual(0..1)` drives:

- Emission strength (1 → 8)
- Plasma color intensify
- Charge Niagara user param `Charge`
- Optional charge loop audio (assign in BP)
