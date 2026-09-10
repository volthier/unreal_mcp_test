import unreal

PASTA = "/Game/Runners"
NOME = "M_RunnerAccent"
FULL = PASTA + "/" + NOME

if unreal.EditorAssetLibrary.does_asset_exist(FULL):
    unreal.log("ja existe: " + FULL)
else:
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    mat = ferramentas.create_asset(NOME, PASTA, unreal.Material, unreal.MaterialFactoryNew())

    # Overlay do corpo: unlit + translucido, so para tingir o manequim com a cor do chassi.
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", True)

    cor = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -600, -120)
    cor.set_editor_property("parameter_name", "AccentColor")
    cor.set_editor_property("default_value", unreal.LinearColor(0.2, 0.9, 1.0, 1.0))

    opac = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 140)
    opac.set_editor_property("parameter_name", "AccentOpacity")
    opac.set_editor_property("default_value", 0.35)

    mult = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionMultiply, -320, 0)
    unreal.MaterialEditingLibrary.connect_material_expressions(cor, "", mult, "A")
    unreal.MaterialEditingLibrary.connect_material_expressions(opac, "", mult, "B")

    unreal.MaterialEditingLibrary.connect_material_property(mult, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    unreal.MaterialEditingLibrary.connect_material_property(opac, "", unreal.MaterialProperty.MP_OPACITY)

    unreal.MaterialEditingLibrary.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(FULL, only_if_is_dirty=False)
    unreal.log("criado: " + FULL)
