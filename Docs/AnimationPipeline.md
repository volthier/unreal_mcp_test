# Animation Pipeline

Animations authored in Blender on `Armature_VoltStriker` and baked into FBX.

| Action | Frames | Purpose |
|--------|--------|---------|
| Idle | 40 | Breathing idle |
| Walk | 24 | Locomotion |
| Run | 18 | Combat run |
| Sprint | 14 | Fast run |
| Jump / Fall / Land | 20/16/12 | Aerial |
| Aim / Shoot | 16/12 | Fire poses |
| Charge | 30 | Hold charge |
| Dash | 10 | Dash burst |
| Hit / Death / Victory | 12/30/40 | Reactions |

Imported as `/Game/VoltStriker/Mesh/SKM_VoltStriker_Anim_*`.

Wire into `ABP_VoltStriker` (AnimBP) with locomotion blendspace + upper-body fire slot after C++/BP character is ready.

Control Rig: create `CR_VoltStriker` from skeleton via ControlRigTools once editor has compiled module.
