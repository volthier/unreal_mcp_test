# Gameplay

## Defaults

- GameMode: `AVoltStrikerGameMode` (`Config/DefaultEngine.ini`)
- Pawn: `AVoltStrikerCharacter` → Blueprint `BP_PlayerRobot`
- Camera: SpringArm 320 → 280 while charging + force feedback
- ASC replication: Mixed
- Projectiles: replicated movement, authority damage, multicast impact FX

## Blueprints

| BP | Parent (current) | Notes |
|----|------------------|-------|
| BP_PlayerRobot | Character (interim) | Mesh assigned to SKM_VoltStriker. **Reparent to `VoltStrikerCharacter` after editor restart** to unlock GAS. |
| BP_Weapon | Actor (interim) | Reparent to `VoltStrikerWeapon` after restart |
| BP_Projectile | Actor (interim) | Reparent to `VoltStrikerProjectile` |
| BP_ChargeProjectile | Actor (interim) | Reparent to `VoltStrikerChargeProjectile` |
| BP_CameraManager | PlayerCameraManager | Reparent to `VoltStrikerCameraManager` optional |
| BP_VoltStrikerGameMode | GameModeBase | DefaultPawn = BP_PlayerRobot |

**Required once (blocks GAS / C++ camera pitch):**
1. Close and reopen the Unreal Editor so `libUnrealEditor-AI_MEGA_MAN_TEST.dylib` loads.
2. Confirm `VoltStrikerCharacter` appears in class picker.
3. Reparent `BP_PlayerRobot` → `VoltStrikerCharacter`.
4. Remove duplicate BP `CameraBoom` / `FollowCamera` if C++ components already exist (avoid double cameras).
5. Assign StartupAbilities + Weapon/Projectile classes on the BP defaults.
6. PIE: verify 3rd-person follow at ~−20° pitch, move/jump/land anims.

Until restart: mesh is on `BP_PlayerRobot` (scale ×100); boom length 320; full GAS needs C++ parent.

## Validation Checklist

- [ ] Spawn in NewMap
- [ ] Move / Jump
- [ ] Basic Shot
- [ ] Charge L1–L3
- [ ] Damage apply
- [ ] Listen-server replication
- [ ] Package project
