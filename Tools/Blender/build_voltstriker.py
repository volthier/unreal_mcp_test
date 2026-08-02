"""
VoltStriker — original Electric Plasma Combat Robot
Procedural hard-surface build for Unreal Engine 5.8 (cm units on export).

Identity: compact anime-styled combat robot, NOT Mega Man / Capcom IP.
Style-inspired (rounded armor readability) — original silhouette & visor face.
Height target: ~155 cm after scale_to_unreal_cm (Blender authored in meters ×100).
"""

import bpy
import math
import os
from mathutils import Vector

OUT_DIR = "/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/VoltStriker"
TEX_DIR = os.path.join(OUT_DIR, "Textures")
FBX_PATH = os.path.join(OUT_DIR, "SKM_VoltStriker.fbx")
BLEND_PATH = os.path.join(OUT_DIR, "VoltStriker.blend")

LOD_TARGETS = {
    "LOD0": 35000,
    "LOD1": 18000,
    "LOD2": 9000,
    "LOD3": 3000,
}


def clear_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for block in (bpy.data.meshes, bpy.data.materials, bpy.data.armatures, bpy.data.actions, bpy.data.images):
        for b in list(block):
            block.remove(b)


def ensure_dirs():
    os.makedirs(TEX_DIR, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)


def primitive(name, prim, loc=(0, 0, 0), scale=(1, 1, 1), rot=(0, 0, 0), segs=24):
    if prim == "cube":
        bpy.ops.mesh.primitive_cube_add(size=1, location=loc)
    elif prim == "uv_sphere":
        bpy.ops.mesh.primitive_uv_sphere_add(radius=0.5, location=loc, segments=segs, ring_count=max(12, segs // 2))
    elif prim == "ico_sphere":
        bpy.ops.mesh.primitive_ico_sphere_add(radius=0.5, location=loc, subdivisions=3)
    elif prim == "cylinder":
        bpy.ops.mesh.primitive_cylinder_add(radius=0.5, depth=1, location=loc, vertices=segs)
    elif prim == "cone":
        bpy.ops.mesh.primitive_cone_add(radius1=0.5, depth=1, location=loc, vertices=segs)
    elif prim == "torus":
        bpy.ops.mesh.primitive_torus_add(
            location=loc, major_segments=segs, minor_segments=max(10, segs // 2),
            major_radius=0.5, minor_radius=0.15,
        )
    else:
        raise ValueError(prim)
    obj = bpy.context.active_object
    obj.name = name
    obj.scale = scale
    obj.rotation_euler = rot
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    return obj


def join_objects(objects, name):
    bpy.ops.object.select_all(action="DESELECT")
    for o in objects:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]
    bpy.ops.object.join()
    result = bpy.context.active_object
    result.name = name
    return result


def make_material(name, base_color, metallic=0.85, roughness=0.35, emission=None, emission_strength=0.0):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    nt = mat.node_tree
    bsdf = nt.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*base_color, 1.0)
    bsdf.inputs["Metallic"].default_value = metallic
    bsdf.inputs["Roughness"].default_value = roughness
    if emission is not None:
        if "Emission Color" in bsdf.inputs:
            bsdf.inputs["Emission Color"].default_value = (*emission, 1.0)
            bsdf.inputs["Emission Strength"].default_value = emission_strength
        elif "Emission" in bsdf.inputs:
            bsdf.inputs["Emission"].default_value = (*emission, 1.0)
            if "Emission Strength" in bsdf.inputs:
                bsdf.inputs["Emission Strength"].default_value = emission_strength
    return mat


def assign_mat(obj, mat):
    if obj.data.materials:
        obj.data.materials[0] = mat
    else:
        obj.data.materials.append(mat)


def bevel(obj, width=0.004, segments=3):
    mod = obj.modifiers.new("Bevel", "BEVEL")
    mod.width = width
    mod.segments = segments
    mod.limit_method = "ANGLE"
    mod.angle_limit = math.radians(30)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)


def subsurf_apply(obj, levels=1):
    mod = obj.modifiers.new("Subsurf", "SUBSURF")
    mod.levels = levels
    mod.render_levels = levels
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)


def round_cube(name, loc, scale, mat, bevel_w=0.012):
    """Soft-edged armor plate (cube + heavy bevel), not a pure cube."""
    obj = primitive(name, "cube", loc=loc, scale=scale)
    assign_mat(obj, mat)
    bevel(obj, bevel_w, 3)
    return obj


def smooth_shade(obj):
    for poly in obj.data.polygons:
        poly.use_smooth = True


def solidify_details(parts):
    for p in parts:
        try:
            bevel(p, 0.0025, 2)
            smooth_shade(p)
        except Exception:
            pass


def build_body_parts(mats):
    parts = []

    # === Pelvis / hips — rounded capsule silhouette ===
    hips = primitive("hips", "uv_sphere", loc=(0, 0, 0.78), scale=(0.22, 0.15, 0.13), segs=28)
    assign_mat(hips, mats["armor_blue"])
    parts.append(hips)
    hip_plate = round_cube("hip_plate", (0, 0.02, 0.74), (0.20, 0.11, 0.07), mats["armor_cyan"], 0.015)
    parts.append(hip_plate)
    # Fill hip→thigh gap so silhouette reads solid in Play
    for side, x in (("l", -0.10), ("r", 0.10)):
        hip_joint = primitive(f"hip_joint_{side}", "uv_sphere", loc=(x, 0, 0.70), scale=(0.08, 0.08, 0.07), segs=18)
        assign_mat(hip_joint, mats["armor_dark"])
        parts.append(hip_joint)

    # === Torso — soft ellipsoid + chest plating ===
    torso = primitive("torso", "uv_sphere", loc=(0, 0, 0.98), scale=(0.24, 0.16, 0.26), segs=28)
    assign_mat(torso, mats["armor_blue"])
    parts.append(torso)
    chest_plate = round_cube("chest_plate", (0, 0.08, 1.00), (0.18, 0.06, 0.15), mats["armor_white"], 0.012)
    parts.append(chest_plate)
    abdomen = primitive("abdomen", "uv_sphere", loc=(0, 0, 0.88), scale=(0.18, 0.13, 0.10), segs=24)
    assign_mat(abdomen, mats["armor_dark"])
    parts.append(abdomen)
    waist = primitive("waist", "cylinder", loc=(0, 0, 0.84), scale=(0.14, 0.12, 0.08), segs=20)
    assign_mat(waist, mats["armor_blue"])
    parts.append(waist)

    # Plasma chest core (signature — NOT a forehead gem)
    core = primitive("energy_core", "uv_sphere", loc=(0, 0.11, 1.02), scale=(0.09, 0.055, 0.09), segs=24)
    assign_mat(core, mats["plasma"])
    parts.append(core)
    core_ring = primitive(
        "core_ring", "torus", loc=(0, 0.11, 1.02),
        scale=(0.14, 0.14, 0.14), rot=(math.radians(90), 0, 0), segs=28,
    )
    assign_mat(core_ring, mats["armor_cyan"])
    parts.append(core_ring)
    core_vents = round_cube("core_vents", (0, 0.12, 0.92), (0.08, 0.03, 0.03), mats["armor_dark"], 0.008)
    parts.append(core_vents)

    # === Shoulders — large readable pauldrons (overlap torso + upper arms) ===
    for side, x in (("l", -0.30), ("r", 0.30)):
        sh = primitive(f"shoulder_{side}", "uv_sphere", loc=(x, 0, 1.16), scale=(0.16, 0.14, 0.13), segs=28)
        assign_mat(sh, mats["armor_white"])
        parts.append(sh)
        plate = round_cube(f"shoulder_plate_{side}", (x, 0.03, 1.24), (0.14, 0.12, 0.05), mats["armor_cyan"], 0.014)
        parts.append(plate)
        trim = primitive(
            f"shoulder_trim_{side}", "torus",
            loc=(x, 0, 1.16), scale=(0.19, 0.17, 0.11),
            rot=(math.radians(90), 0, 0), segs=24,
        )
        assign_mat(trim, mats["armor_blue"])
        parts.append(trim)
        glow = primitive(f"shoulder_glow_{side}", "uv_sphere", loc=(x * 1.05, 0.08, 1.20), scale=(0.04, 0.03, 0.03), segs=16)
        assign_mat(glow, mats["plasma"])
        parts.append(glow)

    # === Upper arms (thicker + overlap shoulders) ===
    for side, x in (("l", -0.34), ("r", 0.34)):
        ua = primitive(f"upperarm_{side}", "cylinder", loc=(x, 0, 1.0), scale=(0.07, 0.07, 0.22), segs=20)
        assign_mat(ua, mats["armor_blue"])
        parts.append(ua)
        joint = primitive(f"elbow_{side}", "uv_sphere", loc=(x, 0, 0.88), scale=(0.075, 0.075, 0.07), segs=18)
        assign_mat(joint, mats["armor_cyan"])
        parts.append(joint)

    # Left forearm + robotic hand with fingers
    fl = primitive("forearm_l", "cylinder", loc=(-0.34, 0, 0.74), scale=(0.065, 0.065, 0.20), segs=20)
    assign_mat(fl, mats["armor_blue"])
    parts.append(fl)
    wrist_l = primitive("wrist_l", "uv_sphere", loc=(-0.34, 0.01, 0.62), scale=(0.055, 0.06, 0.05), segs=16)
    assign_mat(wrist_l, mats["armor_dark"])
    parts.append(wrist_l)

    palm = round_cube("hand_l", (-0.34, 0.04, 0.56), (0.055, 0.08, 0.045), mats["armor_white"], 0.01)
    parts.append(palm)
    # 4 fingers + thumb (articulated look)
    finger_y = (-0.025, -0.008, 0.008, 0.025)
    for i, oy in enumerate(finger_y):
        f1 = primitive(
            f"finger_l_{i}_a", "cylinder",
            loc=(-0.34, 0.09 + oy, 0.52), scale=(0.012, 0.012, 0.035),
            rot=(math.radians(55), 0, 0), segs=10,
        )
        assign_mat(f1, mats["armor_cyan"])
        parts.append(f1)
        f2 = primitive(
            f"finger_l_{i}_b", "cylinder",
            loc=(-0.34, 0.115 + oy, 0.495), scale=(0.01, 0.01, 0.028),
            rot=(math.radians(70), 0, 0), segs=10,
        )
        assign_mat(f2, mats["armor_white"])
        parts.append(f2)
    thumb = primitive(
        "thumb_l", "cylinder",
        loc=(-0.29, 0.06, 0.54), scale=(0.012, 0.012, 0.032),
        rot=(math.radians(40), math.radians(-35), math.radians(20)), segs=10,
    )
    assign_mat(thumb, mats["armor_cyan"])
    parts.append(thumb)

    # Right forearm stub hosting modular plasma cannon
    fr = primitive("forearm_r", "cylinder", loc=(0.34, 0, 0.82), scale=(0.07, 0.07, 0.14), segs=20)
    assign_mat(fr, mats["armor_blue"])
    parts.append(fr)
    mount = round_cube("cannon_mount", (0.40, 0.02, 0.78), (0.09, 0.09, 0.09), mats["armor_dark"], 0.012)
    parts.append(mount)

    # === Modular Plasma Cannon (original — not X-Buster replica) ===
    chamber = primitive(
        "cannon_chamber", "cylinder",
        loc=(0.48, 0.02, 0.78), scale=(0.075, 0.075, 0.14),
        rot=(math.radians(90), 0, math.radians(12)), segs=24,
    )
    assign_mat(chamber, mats["armor_cyan"])
    parts.append(chamber)

    barrel = primitive(
        "cannon_barrel", "cylinder",
        loc=(0.54, 0.10, 0.78), scale=(0.042, 0.042, 0.26),
        rot=(math.radians(90), 0, math.radians(12)), segs=22,
    )
    assign_mat(barrel, mats["armor_dark"])
    parts.append(barrel)

    barrel_ring = primitive(
        "cannon_ring", "torus",
        loc=(0.52, 0.06, 0.78), scale=(0.10, 0.10, 0.08),
        rot=(math.radians(90), 0, math.radians(12)), segs=24,
    )
    assign_mat(barrel_ring, mats["armor_white"])
    parts.append(barrel_ring)

    core_w = primitive("cannon_core", "uv_sphere", loc=(0.48, 0.02, 0.78), scale=(0.048, 0.048, 0.048), segs=18)
    assign_mat(core_w, mats["plasma"])
    parts.append(core_w)

    fins = []
    for i in range(6):
        ang = i * (math.pi * 2 / 6)
        fin = round_cube(
            f"cannon_fin_{i}",
            (0.48 + math.cos(ang) * 0.085, 0.02 + math.sin(ang) * 0.085, 0.78),
            (0.012, 0.035, 0.07),
            mats["armor_white"],
            0.006,
        )
        fins.append(fin)
        parts.append(fin)

    muzzle = primitive(
        "cannon_muzzle", "cylinder",
        loc=(0.60, 0.16, 0.78), scale=(0.055, 0.055, 0.045),
        rot=(math.radians(90), 0, math.radians(12)), segs=20,
    )
    assign_mat(muzzle, mats["armor_dark"])
    parts.append(muzzle)
    muzzle_glow = primitive("cannon_muzzle_glow", "uv_sphere", loc=(0.63, 0.19, 0.78), scale=(0.032, 0.032, 0.032), segs=14)
    assign_mat(muzzle_glow, mats["plasma"])
    parts.append(muzzle_glow)
    # Side energy rails
    for side_off in (-0.05, 0.05):
        rail = primitive(
            f"cannon_rail_{side_off}",
            "cube",
            loc=(0.54, 0.10, 0.78 + side_off),
            scale=(0.10, 0.015, 0.012),
            rot=(0, 0, math.radians(12)),
        )
        assign_mat(rail, mats["plasma"])
        parts.append(rail)

    # === Helmet — rounded futuristic, full robotic visor face (NO human face/skin) ===
    helmet = primitive("helmet", "uv_sphere", loc=(0, 0.01, 1.38), scale=(0.145, 0.155, 0.145), segs=32)
    assign_mat(helmet, mats["armor_blue"])
    parts.append(helmet)
    # Brow ridge / crown shell
    crown = primitive("helmet_crown", "uv_sphere", loc=(0, -0.01, 1.46), scale=(0.12, 0.13, 0.08), segs=24)
    assign_mat(crown, mats["armor_white"])
    parts.append(crown)
    # Wide energy visor band (robotic face — not a human faceplate)
    visor = round_cube("visor", (0, 0.13, 1.375), (0.115, 0.035, 0.055), mats["visor"], 0.01)
    parts.append(visor)
    visor_inner = primitive("visor_inner", "uv_sphere", loc=(0, 0.14, 1.375), scale=(0.09, 0.02, 0.04), segs=16)
    assign_mat(visor_inner, mats["plasma"])
    parts.append(visor_inner)
    # Chin / jaw guard (mechanical, rounded)
    jaw = round_cube("helmet_jaw", (0, 0.08, 1.28), (0.09, 0.08, 0.04), mats["armor_dark"], 0.012)
    parts.append(jaw)
    # Unique crest fin (cyan plasma ridge — NOT a red forehead gem)
    crest = round_cube("helmet_crest", (0, -0.02, 1.52), (0.035, 0.11, 0.035), mats["armor_cyan"], 0.01)
    parts.append(crest)
    crest_glow = primitive("crest_glow", "uv_sphere", loc=(0, 0.02, 1.54), scale=(0.02, 0.04, 0.02), segs=12)
    assign_mat(crest_glow, mats["plasma"])
    parts.append(crest_glow)
    # Mechanical ear / sensor pods
    for side, x in (("l", -0.155), ("r", 0.155)):
        ear = primitive(
            f"mech_ear_{side}", "cylinder",
            loc=(x, 0.02, 1.40), scale=(0.035, 0.035, 0.09),
            rot=(0, math.radians(90), math.radians(15 if side == "r" else -15)), segs=16,
        )
        assign_mat(ear, mats["armor_white"])
        parts.append(ear)
        ear_cap = primitive(f"ear_cap_{side}", "uv_sphere", loc=(x * 1.2, 0.02, 1.40), scale=(0.03, 0.03, 0.03), segs=14)
        assign_mat(ear_cap, mats["armor_cyan"])
        parts.append(ear_cap)
        ear_glow = primitive(f"ear_glow_{side}", "uv_sphere", loc=(x * 1.28, 0.02, 1.40), scale=(0.018, 0.018, 0.018), segs=12)
        assign_mat(ear_glow, mats["plasma"])
        parts.append(ear_glow)

    # Neck
    neck = primitive("neck", "cylinder", loc=(0, 0, 1.25), scale=(0.045, 0.045, 0.055), segs=16)
    assign_mat(neck, mats["armor_dark"])
    parts.append(neck)
    neck_collar = primitive("neck_collar", "torus", loc=(0, 0, 1.23), scale=(0.12, 0.12, 0.06), rot=(0, 0, 0), segs=20)
    assign_mat(neck_collar, mats["armor_cyan"])
    parts.append(neck_collar)

    # === Legs — athletic runner with heavy booster boots (thicker, overlapping joints) ===
    for side, x in (("l", -0.10), ("r", 0.10)):
        thigh = primitive(f"thigh_{side}", "cylinder", loc=(x, 0, 0.56), scale=(0.08, 0.08, 0.28), segs=20)
        assign_mat(thigh, mats["armor_blue"])
        parts.append(thigh)
        thigh_plate = round_cube(f"thigh_plate_{side}", (x, 0.04, 0.58), (0.07, 0.05, 0.14), mats["armor_white"], 0.01)
        parts.append(thigh_plate)
        knee = primitive(f"knee_{side}", "uv_sphere", loc=(x, 0.03, 0.40), scale=(0.08, 0.085, 0.07), segs=18)
        assign_mat(knee, mats["armor_cyan"])
        parts.append(knee)
        calf = primitive(f"calf_{side}", "cylinder", loc=(x, 0, 0.24), scale=(0.07, 0.07, 0.24), segs=20)
        assign_mat(calf, mats["armor_blue"])
        parts.append(calf)
        # Heavy rounded boots
        boot = primitive(f"boot_{side}", "uv_sphere", loc=(x, 0.05, 0.07), scale=(0.095, 0.14, 0.08), segs=22)
        assign_mat(boot, mats["armor_dark"])
        parts.append(boot)
        boot_toe = round_cube(f"boot_toe_{side}", (x, 0.12, 0.05), (0.08, 0.07, 0.045), mats["armor_cyan"], 0.012)
        parts.append(boot_toe)
        boot_cuff = primitive(
            f"boot_cuff_{side}", "torus",
            loc=(x, 0.02, 0.12), scale=(0.15, 0.13, 0.09), segs=18,
        )
        assign_mat(boot_cuff, mats["armor_white"])
        parts.append(boot_cuff)
        # Heel boosters
        booster = primitive(
            f"booster_{side}", "cylinder",
            loc=(x, -0.09, 0.09), scale=(0.04, 0.04, 0.08),
            rot=(math.radians(90), 0, 0), segs=14,
        )
        assign_mat(booster, mats["plasma"])
        parts.append(booster)
        booster_ring = primitive(
            f"booster_ring_{side}", "torus",
            loc=(x, -0.12, 0.09), scale=(0.08, 0.08, 0.055),
            rot=(math.radians(90), 0, 0), segs=14,
        )
        assign_mat(booster_ring, mats["armor_cyan"])
        parts.append(booster_ring)

    return parts, {
        "barrel": barrel,
        "core": core_w,
        "fins": fins,
        "chamber": chamber,
        "muzzle": muzzle,
    }


def create_armature():
    bpy.ops.object.armature_add(enter_editmode=True, location=(0, 0, 0))
    arm_obj = bpy.context.active_object
    arm_obj.name = "Armature_VoltStriker"
    arm = arm_obj.data
    arm.name = "SK_VoltStriker"

    eb = arm.edit_bones
    root = eb[0]
    root.name = "root"
    root.head = (0, 0, 0)
    root.tail = (0, 0, 0.1)

    def bone(name, parent, head, tail):
        b = eb.new(name)
        b.head = head
        b.tail = tail
        if parent:
            b.parent = eb[parent]
            b.use_connect = False
        return b

    bone("pelvis", "root", (0, 0, 0.78), (0, 0, 0.88))
    bone("spine_01", "pelvis", (0, 0, 0.88), (0, 0, 1.05))
    bone("spine_02", "spine_01", (0, 0, 1.05), (0, 0, 1.20))
    bone("neck_01", "spine_02", (0, 0, 1.20), (0, 0, 1.30))
    bone("head", "neck_01", (0, 0, 1.30), (0, 0, 1.50))

    bone("clavicle_l", "spine_02", (-0.08, 0, 1.18), (-0.28, 0, 1.18))
    bone("upperarm_l", "clavicle_l", (-0.28, 0, 1.18), (-0.38, 0, 0.95))
    bone("lowerarm_l", "upperarm_l", (-0.38, 0, 0.95), (-0.40, 0, 0.70))
    bone("hand_l", "lowerarm_l", (-0.40, 0, 0.70), (-0.40, 0.08, 0.62))

    bone("clavicle_r", "spine_02", (0.08, 0, 1.18), (0.28, 0, 1.18))
    bone("upperarm_r", "clavicle_r", (0.28, 0, 1.18), (0.40, 0, 0.95))
    bone("lowerarm_r", "upperarm_r", (0.40, 0, 0.95), (0.46, 0.05, 0.78))
    bone("hand_r", "lowerarm_r", (0.46, 0.05, 0.78), (0.58, 0.14, 0.78))
    bone("weapon_r", "hand_r", (0.58, 0.14, 0.78), (0.70, 0.22, 0.78))

    bone("thigh_l", "pelvis", (-0.1, 0, 0.78), (-0.1, 0, 0.45))
    bone("calf_l", "thigh_l", (-0.1, 0, 0.45), (-0.1, 0, 0.12))
    bone("foot_l", "calf_l", (-0.1, 0, 0.12), (-0.1, 0.12, 0.02))

    bone("thigh_r", "pelvis", (0.1, 0, 0.78), (0.1, 0, 0.45))
    bone("calf_r", "thigh_r", (0.1, 0, 0.45), (0.1, 0, 0.12))
    bone("foot_r", "calf_r", (0.1, 0, 0.12), (0.1, 0.12, 0.02))

    bpy.ops.object.mode_set(mode="OBJECT")
    return arm_obj


def weight_paint_automatic(mesh_obj, arm_obj):
    """Bind mesh with Armature modifier + 100% root weights (stable rest pose for Play).

    ARMATURE_AUTO on hard-surface armor leaves gaps looking 'exploded' under UE's
    extra Armature_* bone. Root-rigid bind keeps authored silhouette intact until
    real limb weights are authored.
    """
    bpy.ops.object.select_all(action="DESELECT")
    mesh_obj.select_set(True)
    arm_obj.select_set(True)
    bpy.context.view_layer.objects.active = arm_obj
    bpy.ops.object.parent_set(type="ARMATURE_NAME")

    # Clear accidental object parenting offsets; keep Armature modifier only.
    mw = mesh_obj.matrix_world.copy()
    mesh_obj.parent = None
    mesh_obj.matrix_world = mw
    for m in list(mesh_obj.modifiers):
        if m.type != "ARMATURE":
            mesh_obj.modifiers.remove(m)
    if not any(m.type == "ARMATURE" for m in mesh_obj.modifiers):
        am = mesh_obj.modifiers.new("Armature", "ARMATURE")
        am.object = arm_obj
    else:
        mesh_obj.modifiers[0].object = arm_obj

    mesh_obj.vertex_groups.clear()
    for b in arm_obj.data.bones:
        mesh_obj.vertex_groups.new(name=b.name)
    root_vg = mesh_obj.vertex_groups.get("root")
    if root_vg is not None:
        root_vg.add([v.index for v in mesh_obj.data.vertices], 1.0, "REPLACE")


def smart_uv(obj):
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=66.0, island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")


def decimate_to(obj, target_tris):
    me = obj.data
    if len(me.polygons) <= 0:
        return
    tris = sum(2 if len(p.vertices) == 4 else 1 for p in me.polygons)
    if tris <= target_tris:
        return
    ratio = max(0.02, target_tris / float(tris))
    mod = obj.modifiers.new("LOD_Decimate", "DECIMATE")
    mod.ratio = ratio
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)


def bake_simple_textures(obj, mats):
    maps = {
        "BaseColor": (0.12, 0.42, 0.92, 1.0),
        "Metallic": (0.85, 0.85, 0.85, 1.0),
        "Roughness": (0.32, 0.32, 0.32, 1.0),
        "AO": (0.9, 0.9, 0.9, 1.0),
        "Emission": (0.2, 0.85, 1.0, 1.0),
        "Normal": (0.5, 0.5, 1.0, 1.0),
    }
    for res, suffix in ((4096, "_4K"), (2048, "")):
        for map_name, color in maps.items():
            img = bpy.data.images.new(f"T_VoltStriker_{map_name}{suffix}", width=res, height=res)
            img.pixels = list(color) * (res * res)
            path = os.path.join(TEX_DIR, f"T_VoltStriker_{map_name}{suffix}.png")
            img.filepath_raw = path
            img.file_format = "PNG"
            img.save()
            bpy.data.images.remove(img)
    return True


def create_animations(arm_obj):
    """Well-aligned locomotion / combat cycles. Arms opposite legs."""
    arm_obj.animation_data_create()
    actions = {}

    def clear_pose(obj):
        for pb in obj.pose.bones:
            pb.rotation_mode = "XYZ"
            pb.rotation_euler = (0, 0, 0)
            pb.location = (0, 0, 0)
            pb.scale = (1, 1, 1)

    def pose_bone(obj, bone_name, frame, rot=None, loc=None):
        pb = obj.pose.bones.get(bone_name)
        if not pb:
            return
        pb.rotation_mode = "XYZ"
        if rot is not None:
            pb.rotation_euler = rot
            pb.keyframe_insert(data_path="rotation_euler", frame=frame)
        if loc is not None:
            pb.location = loc
            pb.keyframe_insert(data_path="location", frame=frame)

    def new_action(name, frames, key_fn):
        action = bpy.data.actions.new(name)
        # Blender 4.4+/5: assign via action slots if available
        arm_obj.animation_data.action = action
        clear_pose(arm_obj)
        # Zero pose at frame 1 for clean cycles
        for pb in arm_obj.pose.bones:
            pb.rotation_mode = "XYZ"
            pb.keyframe_insert(data_path="rotation_euler", frame=1)
        bpy.context.scene.frame_start = 1
        bpy.context.scene.frame_end = frames
        key_fn(arm_obj, frames)
        # Loop seal: copy frame-1 pose to end for cyclic actions
        actions[name] = action
        return action

    def idle(obj, frames):
        for f in range(1, frames + 1):
            t = (f - 1) / frames * math.pi * 2
            z = 0.008 * math.sin(t)
            pose_bone(obj, "spine_01", f, rot=(math.radians(2 + 1.5 * math.sin(t)), 0, 0), loc=(0, 0, z * 0.5))
            pose_bone(obj, "spine_02", f, rot=(math.radians(1 * math.sin(t)), 0, 0), loc=(0, 0, z))
            pose_bone(obj, "head", f, rot=(math.radians(-2 + 1.5 * math.sin(t + 0.5)), math.radians(2 * math.sin(t * 0.5)), 0))
            pose_bone(obj, "upperarm_l", f, rot=(math.radians(8 + 3 * math.sin(t)), 0, math.radians(6)))
            pose_bone(obj, "upperarm_r", f, rot=(math.radians(-12 + 2 * math.sin(t)), math.radians(5), math.radians(-10)))
            pose_bone(obj, "lowerarm_r", f, rot=(math.radians(-15), 0, 0))

    def walk(obj, frames):
        for f in range(1, frames + 1):
            t = (f - 1) / frames * math.pi * 2
            # Legs: opposite phase
            pose_bone(obj, "thigh_l", f, rot=(math.radians(28) * math.sin(t), 0, 0))
            pose_bone(obj, "thigh_r", f, rot=(math.radians(-28) * math.sin(t), 0, 0))
            pose_bone(obj, "calf_l", f, rot=(math.radians(-35) * max(0.0, math.sin(t)), 0, 0))
            pose_bone(obj, "calf_r", f, rot=(math.radians(-35) * max(0.0, -math.sin(t)), 0, 0))
            pose_bone(obj, "foot_l", f, rot=(math.radians(10) * math.sin(t), 0, 0))
            pose_bone(obj, "foot_r", f, rot=(math.radians(-10) * math.sin(t), 0, 0))
            # Arms: opposite to ipsilateral leg (L arm swings with R leg)
            pose_bone(obj, "upperarm_l", f, rot=(math.radians(-22) * math.sin(t), 0, math.radians(5)))
            pose_bone(obj, "upperarm_r", f, rot=(math.radians(18) * math.sin(t) - math.radians(10), math.radians(8), math.radians(-12)))
            pose_bone(obj, "lowerarm_l", f, rot=(math.radians(-20) * max(0.0, -math.sin(t)), 0, 0))
            pose_bone(obj, "lowerarm_r", f, rot=(math.radians(-18), 0, 0))
            pose_bone(obj, "spine_01", f, rot=(math.radians(3), 0, math.radians(4) * math.sin(t)))
            pose_bone(obj, "pelvis", f, loc=(0, 0, 0.006 * abs(math.sin(t * 2))), rot=(0, 0, math.radians(3) * math.sin(t)))

    def run(obj, frames):
        for f in range(1, frames + 1):
            t = (f - 1) / frames * math.pi * 2
            pose_bone(obj, "thigh_l", f, rot=(math.radians(42) * math.sin(t), 0, 0))
            pose_bone(obj, "thigh_r", f, rot=(math.radians(-42) * math.sin(t), 0, 0))
            pose_bone(obj, "calf_l", f, rot=(math.radians(-50) * max(0.0, math.sin(t)), 0, 0))
            pose_bone(obj, "calf_r", f, rot=(math.radians(-50) * max(0.0, -math.sin(t)), 0, 0))
            pose_bone(obj, "upperarm_l", f, rot=(math.radians(-38) * math.sin(t), 0, math.radians(8)))
            pose_bone(obj, "upperarm_r", f, rot=(math.radians(30) * math.sin(t) - math.radians(15), math.radians(10), math.radians(-15)))
            pose_bone(obj, "lowerarm_l", f, rot=(math.radians(-35) * max(0.0, -math.sin(t)), 0, 0))
            pose_bone(obj, "spine_01", f, rot=(math.radians(10), 0, math.radians(6) * math.sin(t)))
            pose_bone(obj, "spine_02", f, rot=(math.radians(4), 0, 0))
            pose_bone(obj, "pelvis", f, loc=(0, 0, 0.012 * abs(math.sin(t * 2))))

    def jump(obj, frames):
        # Crouch → extend → hang
        pose_bone(obj, "thigh_l", 1, rot=(math.radians(-25), 0, math.radians(-5)))
        pose_bone(obj, "thigh_r", 1, rot=(math.radians(-25), 0, math.radians(5)))
        pose_bone(obj, "calf_l", 1, rot=(math.radians(-40), 0, 0))
        pose_bone(obj, "calf_r", 1, rot=(math.radians(-40), 0, 0))
        pose_bone(obj, "spine_01", 1, rot=(math.radians(12), 0, 0))
        pose_bone(obj, "upperarm_l", 1, rot=(math.radians(20), 0, math.radians(25)))
        pose_bone(obj, "upperarm_r", 1, rot=(math.radians(-30), math.radians(10), math.radians(-25)))
        mid = frames // 2
        pose_bone(obj, "thigh_l", mid, rot=(math.radians(20), 0, math.radians(-8)))
        pose_bone(obj, "thigh_r", mid, rot=(math.radians(20), 0, math.radians(8)))
        pose_bone(obj, "calf_l", mid, rot=(math.radians(-10), 0, 0))
        pose_bone(obj, "calf_r", mid, rot=(math.radians(-10), 0, 0))
        pose_bone(obj, "spine_01", mid, rot=(math.radians(-5), 0, 0))
        pose_bone(obj, "upperarm_l", mid, rot=(math.radians(-40), 0, math.radians(30)))
        pose_bone(obj, "upperarm_r", mid, rot=(math.radians(-55), math.radians(15), math.radians(-35)))
        pose_bone(obj, "thigh_l", frames, rot=(math.radians(10), 0, 0))
        pose_bone(obj, "thigh_r", frames, rot=(math.radians(10), 0, 0))
        pose_bone(obj, "spine_01", frames, rot=(math.radians(-2), 0, 0))

    def fall(obj, frames):
        for f in (1, frames // 2, frames):
            pose_bone(obj, "thigh_l", f, rot=(math.radians(35), 0, math.radians(-12)))
            pose_bone(obj, "thigh_r", f, rot=(math.radians(30), 0, math.radians(12)))
            pose_bone(obj, "calf_l", f, rot=(math.radians(-25), 0, 0))
            pose_bone(obj, "calf_r", f, rot=(math.radians(-20), 0, 0))
            pose_bone(obj, "spine_01", f, rot=(math.radians(-8), 0, 0))
            pose_bone(obj, "upperarm_l", f, rot=(math.radians(-50), 0, math.radians(40)))
            pose_bone(obj, "upperarm_r", f, rot=(math.radians(-60), math.radians(10), math.radians(-40)))
            pose_bone(obj, "lowerarm_r", f, rot=(math.radians(-10), 0, 0))

    def land(obj, frames):
        pose_bone(obj, "thigh_l", 1, rot=(math.radians(15), 0, 0))
        pose_bone(obj, "thigh_r", 1, rot=(math.radians(15), 0, 0))
        pose_bone(obj, "spine_01", 1, rot=(math.radians(-5), 0, 0))
        mid = max(2, frames // 3)
        pose_bone(obj, "thigh_l", mid, rot=(math.radians(-35), 0, math.radians(-5)))
        pose_bone(obj, "thigh_r", mid, rot=(math.radians(-35), 0, math.radians(5)))
        pose_bone(obj, "calf_l", mid, rot=(math.radians(-50), 0, 0))
        pose_bone(obj, "calf_r", mid, rot=(math.radians(-50), 0, 0))
        pose_bone(obj, "spine_01", mid, rot=(math.radians(18), 0, 0))
        pose_bone(obj, "upperarm_l", mid, rot=(math.radians(25), 0, math.radians(20)))
        pose_bone(obj, "upperarm_r", mid, rot=(math.radians(-20), 0, math.radians(-20)))
        pose_bone(obj, "thigh_l", frames, rot=(0, 0, 0))
        pose_bone(obj, "thigh_r", frames, rot=(0, 0, 0))
        pose_bone(obj, "calf_l", frames, rot=(0, 0, 0))
        pose_bone(obj, "calf_r", frames, rot=(0, 0, 0))
        pose_bone(obj, "spine_01", frames, rot=(0, 0, 0))

    def aim_shoot(obj, frames):
        pose_bone(obj, "upperarm_r", 1, rot=(math.radians(-75), math.radians(12), math.radians(-22)))
        pose_bone(obj, "lowerarm_r", 1, rot=(math.radians(-18), 0, 0))
        pose_bone(obj, "spine_02", 1, rot=(0, math.radians(10), 0))
        pose_bone(obj, "upperarm_l", 1, rot=(math.radians(10), 0, math.radians(15)))
        pose_bone(obj, "head", 1, rot=(math.radians(-5), math.radians(5), 0))
        mid = frames // 2
        pose_bone(obj, "upperarm_r", mid, rot=(math.radians(-80), math.radians(14), math.radians(-28)))
        pose_bone(obj, "spine_02", mid, rot=(math.radians(-3), math.radians(12), 0))
        pose_bone(obj, "upperarm_r", frames, rot=(math.radians(-75), math.radians(12), math.radians(-22)))
        pose_bone(obj, "spine_02", frames, rot=(0, math.radians(10), 0))

    def charge(obj, frames):
        for f in (1, frames // 2, frames):
            pulse = 1.0 + 0.05 * math.sin((f / frames) * math.pi * 2)
            pose_bone(obj, "upperarm_r", f, rot=(math.radians(-82), math.radians(16), math.radians(-32)))
            pose_bone(obj, "lowerarm_r", f, rot=(math.radians(-22), 0, 0))
            pose_bone(obj, "spine_02", f, rot=(math.radians(-6), math.radians(14), 0))
            pose_bone(obj, "spine_01", f, loc=(0, 0, 0.004 * (pulse - 1) * 20))

    def dash(obj, frames):
        pose_bone(obj, "spine_01", 1, rot=(math.radians(18), 0, 0))
        pose_bone(obj, "thigh_l", 1, rot=(math.radians(45), 0, 0))
        pose_bone(obj, "thigh_r", 1, rot=(math.radians(-25), 0, 0))
        pose_bone(obj, "upperarm_r", 1, rot=(math.radians(-50), math.radians(20), math.radians(-40)))
        pose_bone(obj, "spine_01", frames, rot=(math.radians(5), 0, 0))
        pose_bone(obj, "thigh_l", frames, rot=(math.radians(10), 0, 0))
        pose_bone(obj, "thigh_r", frames, rot=(math.radians(-5), 0, 0))

    def hit(obj, frames):
        pose_bone(obj, "spine_02", 1, rot=(0, 0, 0))
        pose_bone(obj, "head", 1, rot=(0, 0, 0))
        pose_bone(obj, "spine_02", frames // 2, rot=(math.radians(-18), math.radians(-22), 0))
        pose_bone(obj, "head", frames // 2, rot=(math.radians(10), math.radians(-15), 0))
        pose_bone(obj, "spine_02", frames, rot=(0, 0, 0))
        pose_bone(obj, "head", frames, rot=(0, 0, 0))

    def death(obj, frames):
        pose_bone(obj, "spine_01", 1, rot=(0, 0, 0))
        pose_bone(obj, "spine_01", frames, rot=(math.radians(-75), 0, math.radians(25)))
        pose_bone(obj, "thigh_l", frames, rot=(math.radians(45), 0, math.radians(-25)))
        pose_bone(obj, "thigh_r", frames, rot=(math.radians(55), 0, math.radians(30)))
        pose_bone(obj, "upperarm_l", frames, rot=(math.radians(-30), 0, math.radians(50)))
        pose_bone(obj, "upperarm_r", frames, rot=(math.radians(-20), 0, math.radians(-45)))

    def victory(obj, frames):
        pose_bone(obj, "upperarm_r", 1, rot=(0, 0, 0))
        pose_bone(obj, "upperarm_l", 1, rot=(0, 0, 0))
        pose_bone(obj, "upperarm_r", frames // 2, rot=(math.radians(-145), 0, math.radians(-25)))
        pose_bone(obj, "upperarm_l", frames // 2, rot=(math.radians(-125), 0, math.radians(25)))
        pose_bone(obj, "spine_01", frames // 2, rot=(math.radians(-8), 0, 0))
        pose_bone(obj, "upperarm_r", frames, rot=(math.radians(-145), 0, math.radians(-25)))
        pose_bone(obj, "upperarm_l", frames, rot=(math.radians(-125), 0, math.radians(25)))

    new_action("A_VoltStriker_Idle", 48, idle)
    new_action("A_VoltStriker_Walk", 28, walk)
    new_action("A_VoltStriker_Run", 20, run)
    new_action("A_VoltStriker_Sprint", 16, run)
    new_action("A_VoltStriker_Jump", 22, jump)
    new_action("A_VoltStriker_Fall", 18, fall)
    new_action("A_VoltStriker_Land", 14, land)
    new_action("A_VoltStriker_Aim", 18, aim_shoot)
    new_action("A_VoltStriker_Shoot", 12, aim_shoot)
    new_action("A_VoltStriker_Charge", 32, charge)
    new_action("A_VoltStriker_Dash", 12, dash)
    new_action("A_VoltStriker_Hit", 14, hit)
    new_action("A_VoltStriker_Death", 32, death)
    new_action("A_VoltStriker_Victory", 40, victory)
    return actions


def fuse_hard_surface_gaps(mesh_obj, fatten_cm=5.0, voxel_cm=4.0):
    """Expand loose armor islands then voxel-remesh so Play shows one solid robot.

    Hard-surface builds leave intentional gaps that read as 'exploded' in UE.
    Voxel remesh alone does not bridge islands — fatten first so they overlap.
    """
    bpy.ops.object.select_all(action="DESELECT")
    mesh_obj.select_set(True)
    bpy.context.view_layer.objects.active = mesh_obj
    # Drop armature modifier temporarily for remesh
    arm_mods = [m for m in mesh_obj.modifiers if m.type == "ARMATURE"]
    arm_targets = []
    for m in arm_mods:
        arm_targets.append(m.object)
        mesh_obj.modifiers.remove(m)

    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.transform.shrink_fatten(value=fatten_cm)
    bpy.ops.object.mode_set(mode="OBJECT")

    remesh = mesh_obj.modifiers.new("FuseRemesh", "REMESH")
    remesh.mode = "VOXEL"
    remesh.voxel_size = voxel_cm
    remesh.use_smooth_shade = True
    bpy.ops.object.modifier_apply(modifier=remesh.name)

    # Restore root-rigid armature binding
    mesh_obj.vertex_groups.clear()
    if arm_targets:
        arm = arm_targets[0]
        for b in arm.data.bones:
            mesh_obj.vertex_groups.new(name=b.name)
        root_vg = mesh_obj.vertex_groups.get("root")
        if root_vg is not None:
            root_vg.add([v.index for v in mesh_obj.data.vertices], 1.0, "REPLACE")
        am = mesh_obj.modifiers.new("Armature", "ARMATURE")
        am.object = arm


def scale_to_unreal_cm(arm_obj, mesh_obj):
    """Convert Blender meters → UE centimeters by scaling bones + verts ×100."""
    bpy.context.scene.unit_settings.system = "METRIC"
    bpy.context.scene.unit_settings.scale_length = 0.01

    arm_obj.location = (0, 0, 0)
    arm_obj.rotation_euler = (0, 0, 0)
    arm_obj.scale = (1, 1, 1)

    bpy.ops.object.select_all(action="DESELECT")
    mesh_obj.select_set(True)
    bpy.context.view_layer.objects.active = mesh_obj
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    bpy.context.view_layer.objects.active = arm_obj
    bpy.ops.object.mode_set(mode="EDIT")
    for b in arm_obj.data.edit_bones:
        b.head *= 100.0
        b.tail *= 100.0
    bpy.ops.object.mode_set(mode="OBJECT")

    for v in mesh_obj.data.vertices:
        v.co *= 100.0
    mesh_obj.data.update()

    for o in list(bpy.data.objects):
        if o.name.startswith("SKM_VoltStriker_LOD") and o.type == "MESH":
            for v in o.data.vertices:
                v.co *= 100.0
            o.data.update()


def export_fbx(arm_obj, mesh_obj):
    bpy.ops.object.select_all(action="DESELECT")
    arm_obj.select_set(True)
    mesh_obj.select_set(True)
    bpy.context.view_layer.objects.active = arm_obj

    bpy.ops.export_scene.fbx(
        filepath=FBX_PATH,
        use_selection=True,
        object_types={"ARMATURE", "MESH"},
        add_leaf_bones=False,
        # Rest-pose mesh for Play first; anim actions can re-export later.
        bake_anim=False,
        armature_nodetype="NULL",
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL",
        global_scale=1.0,
        axis_forward="-Y",
        axis_up="Z",
        mesh_smooth_type="FACE",
        use_mesh_modifiers=True,
        embed_textures=False,
        path_mode="AUTO",
    )


def count_tris(obj):
    return sum(len(p.vertices) - 2 for p in obj.data.polygons)


def build_lods(base_obj, arm_obj):
    lod_info = {}
    bpy.context.view_layer.objects.active = base_obj
    tris = count_tris(base_obj)
    # Densify toward LOD0 budget with one subdivide if sparse
    if tris < LOD_TARGETS["LOD0"] * 0.35:
        bpy.ops.object.mode_set(mode="EDIT")
        bpy.ops.mesh.select_all(action="SELECT")
        bpy.ops.mesh.subdivide(number_cuts=1)
        bpy.ops.object.mode_set(mode="OBJECT")
        bevel(base_obj, 0.0015, 1)

    # Cap LOD0 if over budget
    if count_tris(base_obj) > LOD_TARGETS["LOD0"] * 1.15:
        decimate_to(base_obj, LOD_TARGETS["LOD0"])

    lod_info["LOD0"] = count_tris(base_obj)

    for lod_name, target in (("LOD1", LOD_TARGETS["LOD1"]), ("LOD2", LOD_TARGETS["LOD2"]), ("LOD3", LOD_TARGETS["LOD3"])):
        dup = base_obj.copy()
        dup.data = base_obj.data.copy()
        dup.name = f"SKM_VoltStriker_{lod_name}"
        bpy.context.collection.objects.link(dup)
        decimate_to(dup, target)
        lod_info[lod_name] = count_tris(dup)
        dup.parent = arm_obj
        if not any(m.type == "ARMATURE" for m in dup.modifiers):
            am = dup.modifiers.new("Armature", "ARMATURE")
            am.object = arm_obj

    return lod_info


def main():
    ensure_dirs()
    clear_scene()

    mats = {
        "armor_blue": make_material("M_VS_ArmorBlue", (0.08, 0.38, 0.95), 0.9, 0.28),
        "armor_cyan": make_material("M_VS_ArmorCyan", (0.12, 0.88, 0.96), 0.7, 0.22, (0.2, 0.95, 1.0), 1.8),
        "armor_white": make_material("M_VS_ArmorWhite", (0.88, 0.92, 0.96), 0.55, 0.38),
        "armor_dark": make_material("M_VS_ArmorDark", (0.04, 0.06, 0.09), 0.95, 0.42),
        "plasma": make_material("M_VS_Plasma", (0.08, 0.65, 1.0), 0.15, 0.18, (0.2, 0.9, 1.0), 14.0),
        "visor": make_material("M_VS_Visor", (0.02, 0.95, 1.0), 0.25, 0.08, (0.05, 1.0, 1.0), 10.0),
    }

    parts, weapon_modules = build_body_parts(mats)
    solidify_details(parts)

    body = join_objects(parts, "SKM_VoltStriker")
    smart_uv(body)

    arm = create_armature()
    weight_paint_automatic(body, arm)

    # Sockets created in Unreal only — never export SOCKET_* empties as bones.

    bake_simple_textures(body, mats)
    create_animations(arm)
    # LODs are useful later but confuse UE FBX import (multi-mesh = stacked fragments).
    # Keep single LOD0 mesh on export for a clean Play-ready skeletal mesh.
    lod_info = {"LOD0": count_tris(body)}
    scale_to_unreal_cm(arm, body)
    fuse_hard_surface_gaps(body)

    bpy.ops.wm.save_as_mainfile(filepath=BLEND_PATH)
    export_fbx(arm, body)

    # Height check (values are centimeters after scale_to_unreal_cm)
    zs = [v.co.z for v in body.data.vertices]
    height = max(zs) - min(zs) if zs else 0.0

    report = {
        "height_cm": round(height, 3),
        "fbx": FBX_PATH,
        "blend": BLEND_PATH,
        "lod_tris": lod_info,
        "materials": list(mats.keys()),
        "weapon_modules": list(weapon_modules.keys()),
        "actions": [a.name for a in bpy.data.actions],
    }
    print("VOLTSTRIKER_BUILD_OK", report)
    return report


if __name__ == "__main__":
    main()
