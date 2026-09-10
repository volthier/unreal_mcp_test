import unreal

# Reparenta os Blueprints que apontavam para as classes antigas (VoltStriker*),
# agora renomeadas para Runner*. Reporta antes e depois.
MAP = [
    ("/Game/VoltStriker/Blueprints/BP_PlayerRobot",           "/Script/PloidrekRPG.RunnerCharacter"),
    ("/Game/VoltStriker/Blueprints/BP_Weapon",                "/Script/PloidrekRPG.WeaponBase"),
    ("/Game/VoltStriker/Blueprints/BP_Projectile",            "/Script/PloidrekRPG.ProjectileBolt"),
    ("/Game/VoltStriker/Blueprints/BP_ChargeProjectile",      "/Script/PloidrekRPG.ProjectileCharge"),
    ("/Game/VoltStriker/Blueprints/BP_CameraManager",         "/Script/PloidrekRPG.RunnerCameraManager"),
    ("/Game/VoltStriker/Blueprints/BP_VoltStrikerGameMode",   "/Script/PloidrekRPG.AFGameMode"),
    ("/Game/VoltStriker/Blueprints/ABP_VoltStriker",          "/Script/PloidrekRPG.RunnerAnimInstance"),
    ("/Game/MegamanX/Blueprints/BP_MegamanXGameMode",         "/Script/PloidrekRPG.AFGameMode"),
    ("/Game/MegamanX/Blueprints/BP_MegamanX",                 "/Script/PloidrekRPG.RunnerCharacter"),
]

renamed = []
for path, parent_path in MAP:
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if bp is None:
        unreal.log_error("NAO CARREGOU: " + path)
        continue
    parent_class = unreal.load_object(None, parent_path)
    if parent_class is None:
        unreal.log_error("CLASSE NAO EXISTE: " + parent_path)
        continue
    try:
        atual = bp.get_editor_property("parent_class")
    except Exception as e:
        atual = None
    nome_atual = atual.get_name() if atual else "NENHUMA (orfa)"
    if atual is parent_class:
        unreal.log("JA OK  " + path + "  (pai: " + nome_atual + ")")
        continue
    unreal.BlueprintEditorLibrary.reparent_blueprint(bp, parent_class)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    renamed.append(path)
    unreal.log("REPARENTADO  " + path + "  " + nome_atual + " -> " + parent_class.get_name())

# tambem lista os BPs restantes de Characters para eu saber o que sobrou
unreal.log("== BPs em /Game/Characters ==")
for p in unreal.EditorAssetLibrary.list_assets("/Game/Characters", recursive=True, include_folder=False):
    if p.endswith("_C") == False and "/BP_" in p or "/ABP_" in p:
        a = unreal.EditorAssetLibrary.load_asset(p)
        if isinstance(a, unreal.Blueprint):
            try:
                pc = a.get_editor_property("parent_class")
                unreal.log("   " + p + " -> " + (pc.get_name() if pc else "ORFA"))
            except Exception:
                unreal.log("   " + p + " -> (erro ao ler pai)")

unreal.log("TOTAL REPARENTADOS: " + str(len(renamed)))
