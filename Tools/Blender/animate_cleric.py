"""Background Blender: create procedural animations for the cleric rig and save.

Opens the rigged CLERIC_BLEND, builds an Action per animation, pushes each to an
NLA track, and saves the file (FBX export happens separately).
"""
import bpy, sys
from math import radians

CLERIC_BLEND = sys.argv[sys.argv.index("--") + 1]
OUT = sys.argv[sys.argv.index("--") + 2]

bpy.ops.wm.open_mainfile(filepath=CLERIC_BLEND)
arm = bpy.data.objects.get("ClericRig")
scene = bpy.context.scene
scene.render.fps = 30
bpy.context.view_layer.objects.active = arm


def set_rot(bone, frame, x=0.0, y=0.0, z=0.0):
    pb = arm.pose.bones.get(bone)
    if not pb:
        return
    pb.rotation_mode = 'XYZ'
    pb.rotation_euler = (radians(x), radians(y), radians(z))
    pb.keyframe_insert("rotation_euler", frame=frame)


def set_loc(bone, frame, x=0.0, y=0.0, z=0.0):
    pb = arm.pose.bones.get(bone)
    if not pb:
        return
    pb.location = (x, y, z)
    pb.keyframe_insert("location", frame=frame)


def reset_all(frame=1):
    for pb in arm.pose.bones:
        pb.rotation_mode = 'XYZ'
        pb.rotation_euler = (0, 0, 0)
        pb.keyframe_insert("rotation_euler", frame=frame)


def new_action(name):
    arm.animation_data_create()
    act = bpy.data.actions.new(name)
    arm.animation_data.action = act
    return act


def make_action(name, frames):
    act = new_action(name)
    for f, fn in frames:
        fn(f)
    # push to NLA track (muted) so all actions export
    tr = arm.animation_data.nla_tracks.new()
    tr.name = name
    st = tr.strips.new(name, 1, act)
    st.name = name
    arm.animation_data.action = None
    return act


# ---------- IDLE (subtle breathing) ----------
def idle_build():
    make_action("Idle", [
        (1,  lambda f: (reset_all(f), set_rot("spine", f, 0, 0, 0), set_loc("pelvis", f, 0, 0, 0))[0]),
        (15, lambda f: (set_rot("chest", f, 3, 0, 0), set_rot("head", f, 2, 0, 0))[0]),
        (30, lambda f: (set_rot("chest", f, 0, 0, 0), set_rot("head", f, 0, 0, 0))[0]),
        (45, lambda f: (set_rot("chest", f, 3, 0, 0), set_rot("head", f, 2, 0, 0))[0]),
        (60, lambda f: reset_all(f)),
    ])

# ---------- WALK (2-step cycle) ----------
def walk_build():
    def key(f, thL, thR, caL, caR, uaL, uaR, spine, bob):
        set_rot("thigh_L", f, thL, 0, 0); set_rot("thigh_R", f, thR, 0, 0)
        set_rot("calf_L", f, caL, 0, 0); set_rot("calf_R", f, caR, 0, 0)
        set_rot("upperarm_L", f, uaL, 0, 0); set_rot("upperarm_R", f, uaR, 0, 0)
        set_rot("spine", f, spine, 0, 0)
        set_loc("pelvis", f, 0, 0, bob)
    make_action("Walk", [
        (1,  lambda f: (reset_all(f), key(f, 25, -25, 10, 40, -20, 20, 2, 0.0))[0]),
        (8,  lambda f: key(f, 0, 0, 30, 30, 0, 0, 4, -0.02)),
        (16, lambda f: key(f, -25, 25, 40, 10, 20, -20, 2, 0.0)),
        (24, lambda f: key(f, 0, 0, 30, 30, 0, 0, 4, -0.02)),
        (32, lambda f: (reset_all(f), key(f, 25, -25, 10, 40, -20, 20, 2, 0.0))[0]),
    ])

# ---------- RUN (faster, larger) ----------
def run_build():
    def key(f, thL, thR, caL, caR, uaL, uaR, spine, bob):
        set_rot("thigh_L", f, thL, 0, 0); set_rot("thigh_R", f, thR, 0, 0)
        set_rot("calf_L", f, caL, 0, 0); set_rot("calf_R", f, caR, 0, 0)
        set_rot("upperarm_L", f, uaL, 0, 0); set_rot("upperarm_R", f, uaR, 0, 0)
        set_rot("spine", f, spine, 0, 0)
        set_loc("pelvis", f, 0, 0, bob)
    make_action("Run", [
        (1,  lambda f: (reset_all(f), key(f, 45, -45, 20, 70, -40, 40, 8, 0.0))[0]),
        (6,  lambda f: key(f, 10, -10, 60, 60, 0, 0, 12, -0.05)),
        (11, lambda f: key(f, -45, 45, 70, 20, 40, -40, 8, 0.0)),
        (16, lambda f: key(f, 10, -10, 60, 60, 0, 0, 12, -0.05)),
        (21, lambda f: (reset_all(f), key(f, 45, -45, 20, 70, -40, 40, 8, 0.0))[0]),
    ])

# ---------- JUMP (crouch -> launch -> land) ----------
def jump_build():
    make_action("Jump", [
        (1,  lambda f: reset_all(f)),
        (5,  lambda f: (set_rot("thigh_L", f, 40, 0, 0), set_rot("thigh_R", f, 40, 0, 0),
                        set_rot("calf_L", f, 60, 0, 0), set_rot("calf_R", f, 60, 0, 0),
                        set_loc("pelvis", f, 0, 0, -0.08))[0]),
        (12, lambda f: (set_rot("thigh_L", f, -30, 0, 0), set_rot("thigh_R", f, -30, 0, 0),
                        set_rot("calf_L", f, 5, 0, 0), set_rot("calf_R", f, 5, 0, 0),
                        set_rot("upperarm_L", f, -60, 0, 0), set_rot("upperarm_R", f, -60, 0, 0),
                        set_loc("pelvis", f, 0, 0, 0.05))[0]),
        (24, lambda f: (set_rot("thigh_L", f, -10, 0, 0), set_rot("thigh_R", f, -10, 0, 0),
                        set_rot("calf_L", f, 30, 0, 0), set_rot("calf_R", f, 30, 0, 0),
                        set_rot("upperarm_L", f, -40, 0, 0), set_rot("upperarm_R", f, -40, 0, 0))[0]),
        (32, lambda f: (reset_all(f), set_rot("thigh_L", f, 40, 0, 0), set_rot("thigh_R", f, 40, 0, 0),
                        set_rot("calf_L", f, 60, 0, 0), set_rot("calf_R", f, 60, 0, 0),
                        set_loc("pelvis", f, 0, 0, -0.06))[0]),
        (40, lambda f: reset_all(f)),
    ])

# ---------- SHOOT (right arm forward + recoil) ----------
def shoot_build():
    make_action("Shoot", [
        (1,  lambda f: reset_all(f)),
        (3,  lambda f: (set_rot("upperarm_R", f, -80, 0, 0), set_rot("lowerarm_R", f, 10, 0, 0),
                        set_rot("chest", f, 0, 20, 0))[0]),
        (6,  lambda f: (set_rot("upperarm_R", f, -90, 0, 0), set_rot("lowerarm_R", f, 0, 0, 0),
                        set_rot("chest", f, 0, 25, 0))[0]),
        (9,  lambda f: (set_rot("upperarm_R", f, -75, 0, 0), set_rot("chest", f, 0, 18, 0))[0]),
        (14, lambda f: (set_rot("upperarm_R", f, -80, 0, 0), set_rot("chest", f, 0, 15, 0))[0]),
        (18, lambda f: reset_all(f)),
    ])

# ---------- WALL SLIDE (against wall, legs bent, arms up) ----------
def wallslide_build():
    make_action("WallSlide", [
        (1,  lambda f: reset_all(f)),
        (6,  lambda f: (set_rot("thigh_L", f, 50, 0, 0), set_rot("thigh_R", f, -50, 0, 0),
                        set_rot("calf_L", f, 70, 0, 0), set_rot("calf_R", f, 70, 0, 0),
                        set_rot("upperarm_L", f, -110, 0, 0), set_rot("upperarm_R", f, -110, 0, 0),
                        set_loc("pelvis", f, 0, 0, -0.1))[0]),
        (30, lambda f: (set_rot("thigh_L", f, 50, 0, 0), set_rot("thigh_R", f, -50, 0, 0),
                        set_rot("calf_L", f, 70, 0, 0), set_rot("calf_R", f, 70, 0, 0),
                        set_rot("upperarm_L", f, -110, 0, 0), set_rot("upperarm_R", f, -110, 0, 0),
                        set_loc("pelvis", f, 0, 0, 0.02))[0]),
        (40, lambda f: (set_rot("thigh_L", f, 50, 0, 0), set_rot("thigh_R", f, -50, 0, 0),
                        set_rot("calf_L", f, 70, 0, 0), set_rot("calf_R", f, 70, 0, 0),
                        set_rot("upperarm_L", f, -110, 0, 0), set_rot("upperarm_R", f, -110, 0, 0),
                        set_loc("pelvis", f, 0, 0, -0.06))[0]),
        (46, lambda f: reset_all(f)),
    ])

# ---------- WALL JUMP (push off) ----------
def walljump_build():
    make_action("WallJump", [
        (1,  lambda f: reset_all(f)),
        (4,  lambda f: (set_rot("thigh_L", f, 50, 0, 0), set_rot("thigh_R", f, -50, 0, 0),
                        set_rot("calf_L", f, 70, 0, 0), set_rot("calf_R", f, 70, 0, 0),
                        set_rot("upperarm_L", f, -110, 0, 0), set_rot("upperarm_R", f, -110, 0, 0),
                        set_loc("pelvis", f, 0, 0, -0.08))[0]),
        (9,  lambda f: (set_rot("thigh_L", f, -40, 0, 0), set_rot("thigh_R", f, -40, 0, 0),
                        set_rot("calf_L", f, 10, 0, 0), set_rot("calf_R", f, 10, 0, 0),
                        set_rot("upperarm_L", f, -30, 0, 0), set_rot("upperarm_R", f, -30, 0, 0),
                        set_loc("pelvis", f, 0, 0, 0.06))[0]),
        (16, lambda f: (set_rot("thigh_L", f, -20, 0, 0), set_rot("thigh_R", f, -20, 0, 0),
                        set_rot("calf_L", f, 40, 0, 0), set_rot("calf_R", f, 40, 0, 0))[0]),
        (22, lambda f: reset_all(f)),
    ])

idle_build(); walk_build(); run_build(); jump_build(); shoot_build(); wallslide_build(); walljump_build()

bpy.ops.wm.save_as_mainfile(filepath=OUT)
print(f"[anim] saved {OUT} with actions: {[a.name for a in bpy.data.actions]}")
print("[done]")
