import unreal

def log(msg):
    unreal.log("### " + msg)

# O mapa NewMap sobrescreve o GameMode no WorldSettings e aponta para BP_VoltStrikerGameMode.
# Em vez de mexer no mapa (que exige o editor grafico), tornamos esse BP o GameMode de MENU:
# ele passa a abrir a tela de login, e o jogo usa AFGameMode pelo parametro de URL.
ORIGEM = "/Game/VoltStriker/Blueprints/BP_VoltStrikerGameMode"
DESTINO = "/Game/VoltStriker/Blueprints/BP_MenuGameMode"
NOVO_PAI = "/Script/PloidrekRPG.MenuGameMode"

if unreal.EditorAssetLibrary.does_asset_exist(DESTINO):
    log("ja renomeado: " + DESTINO)
    bp = unreal.EditorAssetLibrary.load_asset(DESTINO)
else:
    bp = unreal.EditorAssetLibrary.load_asset(ORIGEM)
    if bp is None:
        log("ERRO: nao carregou " + ORIGEM)
        bp = None
    else:
        if unreal.EditorAssetLibrary.rename_asset(ORIGEM, DESTINO):
            log("renomeado: " + ORIGEM + " -> " + DESTINO)
            bp = unreal.EditorAssetLibrary.load_asset(DESTINO)
        else:
            log("ERRO ao renomear")

if bp:
    pai = unreal.load_class(None, NOVO_PAI)
    if pai is None:
        log("ERRO: classe " + NOVO_PAI + " nao existe")
    else:
        unreal.BlueprintEditorLibrary.reparent_blueprint(bp, pai)
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(DESTINO, only_if_is_dirty=False)
        log("reparentado para MenuGameMode: " + DESTINO)

# relatorio final dos GameModes
for p in ("/Game/VoltStriker/Blueprints/BP_MenuGameMode", "/Game/MegamanX/Blueprints/BP_MegamanXGameMode"):
    a = unreal.EditorAssetLibrary.load_asset(p)
    if a:
        try:
            nome = a.get_editor_property("ParentClass").get_name()
        except Exception:
            nome = "(nao exposto)"
        log("BP " + p + " pai=" + str(nome))
log("FIM")
