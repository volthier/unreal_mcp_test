# VoltStriker Character Design

> ⚠️ **Decisão aberta nº 1 do `Docs/PloidrekRPG_GDD_v3.md`:** o VoltStriker é **um chassi Runner com nome próprio**, um
> N.E.R.V. específico, ou apenas um **asset de protótipo**? Isso define se o mesh/skeleton atual é reaproveitado como um
> dos 10 chassis (e sob qual nome) ou arquivado.

**Codename:** VoltStriker  
**Theme:** Electric Plasma Combat Robot  
**Height:** 1.55 m  
**Engine:** Unreal Engine 5.8  

## Identity

Original IP-safe combat robot for a third-person action / future MMORPG prototype. **Style-inspired, original IP** — design language borrows *readability principles* from classic action platform shooters (compact proportions, rounded armor readability, expressive helmet, arm-cannon silhouette) without copying Mega Man, Mega Man X, or Capcom characters. No forehead gem, no human face/skin, no X-Buster replica; VoltStriker uses a full robotic energy visor, plasma chest core, and unique helmet/ear sensors.

## Visual Pillars

- Compact athletic mechanical body (no organic skin)
- Rounded futuristic helmet with energy visor + mechanical ears
- Blue plasma reactor chest core
- Large readable shoulder armor
- Left robotic hand / right integrated modular plasma cannon
- Fast-runner legs with heavy booster boots
- Anime-inspired, colorful, high-contrast silhouette

## Materials

| Slot | Role |
|------|------|
| Armor Blue | Primary plating |
| Armor Cyan | Accent / energy trim |
| Armor White | Shoulder / hand highlights |
| Armor Dark | Boots / barrel |
| Plasma | Core, visor glow, boosters, muzzle |
| Visor | Helmet energy glass |

## LOD Budgets

| LOD | Triangles |
|-----|-----------|
| LOD0 | 35,000 |
| LOD1 | 18,000 |
| LOD2 | 9,000 |
| LOD3 | 3,000 |

## Camera

Third-person follow: spring arm ~320 cm, socket offset (0, 55, 75), default pitch **-20°** looking down at the character (outside mesh, collision probe on).

## Content Paths

- Art source: `Art/VoltStriker/`
- Game assets: `/Game/VoltStriker/`
- Mesh: `/Game/VoltStriker/Mesh/SKM_VoltStriker`
