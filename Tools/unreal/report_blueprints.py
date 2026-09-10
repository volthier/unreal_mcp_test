import unreal

def parent_name(bp):
    for prop in ("ParentClass", "parent_class"):
        try:
            pc = bp.get_editor_property(prop)
            return pc.get_name() if pc else "ORFA"
        except Exception:
            continue
    return "(nao exposto)"

def fix(path, parent_path):
    try:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp is None:
            unreal.log_error("NAO CARREGOU: " + path); return
        pc = unreal.load_object(None, parent_path)
        if pc is None:
            unreal.log_error("CLASSE NAO EXISTE: " + parent_path); return
        antes = parent_name(bp)
        unreal.BlueprintEditorLibrary.reparent_blueprint(bp, pc)
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
        unreal.log("FIX " + path + " : " + antes + " -> " + parent_name(bp))
    except Exception as e:
        unreal.log_error("FALHA em " + path + " : " + str(e))

unreal.log("### RELATORIO DE BLUEPRINTS ###")
alvos = [
    "/Game/VoltStriker/Blueprints/BP_PlayerRobot",
    "/Game/VoltStriker/Blueprints/ABP_VoltStriker",
    "/Game/VoltStriker/Blueprints/BP_VoltStrikerGameMode",
    "/Game/VoltStriker/Blueprints/BP_Weapon",
    "/Game/VoltStriker/Blueprints/BP_Projectile",
    "/Game/VoltStriker/Blueprints/BP_ChargeProjectile",
    "/Game/VoltStriker/Blueprints/BP_CameraManager",
    "/Game/MegamanX/Blueprints/BP_MegamanX",
    "/Game/MegamanX/Blueprints/BP_MegamanXGameMode",
    "/Game/Characters/BP_MegaManX",
    "/Game/Characters/Cleric/BP_ClericPlayer",
]
for p in alvos:
    a = unreal.EditorAssetLibrary.load_asset(p)
    if a is None:
        unreal.log("   " + p + "  ->  (nao existe)")
    else:
        unreal.log("   " + p + "  ->  " + parent_name(a))

unreal.log("### CORRECAO DO LEGADO ###")
# BP_MegamanX tem CameraBoom/FollowCamera proprios e conflita com o C++ do Runner: volta para ACharacter
fix("/Game/MegamanX/Blueprints/BP_MegamanX", "/Script/Engine.Character")

unreal.log("### ESTADO FINAL DOS BPs DO JOGADOR ###")
for p in alvos[:7]:
    a = unreal.EditorAssetLibrary.load_asset(p)
    unreal.log("   " + p + "  ->  " + (parent_name(a) if a else "(nao existe)"))
unreal.log("### FIM ###")
