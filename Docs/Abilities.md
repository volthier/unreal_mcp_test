# VoltStriker Abilities (GAS)

Gameplay Tags live in `Config/DefaultGameplayTags.ini`.

| Ability | Class | Tag | Behavior |
|---------|-------|-----|----------|
| Basic Shot | `UGA_BasicShot` | `Ability.Combat.BasicShot` | Tap fire bolt |
| Charge Shot L1–L3 | `UGA_ChargeShot` | `Ability.Combat.ChargeShot` | Hold ≥0.2 / 1.4 / 2.4 s |
| Rapid Shot | `UGA_RapidShot` | `Ability.Combat.RapidShot` | 3-burst |
| Dash Shot | `UGA_DashShot` | `Ability.Combat.DashShot` | Launch + shot |
| Jump Shot | `UGA_JumpShot` | `Ability.Combat.JumpShot` | Aerial only bonus |

## Attribute Set

`UVoltStrikerAttributeSet`: Health, MaxHealth, Energy, MaxEnergy, AttackPower, ChargeLevel, MoveSpeed, Damage (meta).

## Input Mapping

- Fire press → start charge visuals
- Fire release < 0.2 s → Basic Shot
- Fire release ≥ 0.2 s → Charge Shot (level from hold time)
- Dash → Dash Shot
- Jump + Fire (via Jump Shot ability when airborne)

## Projectiles

- `AVoltStrikerProjectile` — basic / rapid / dash / jump
- `AVoltStrikerChargeProjectile` — piercing AoE sphere
