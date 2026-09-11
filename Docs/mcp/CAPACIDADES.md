# Capacidades do MCP — o que é possível hoje e quando usar

> **Como este documento foi feito:** levantado do **próprio servidor**, com `list_toolsets` e
> `describe_toolset` contra o editor aberto (`127.0.0.1:8000/mcp`). **59 toolsets**. Não é de memória, e não é
> uma lista de desejos — cada ferramenta citada aqui existe no servidor agora.
>
> Isto existe porque a estimativa anterior (feita de cabeça, e depois por um cliente caseiro que só enxergava
> parte) concluiu que o MCP **não** tinha import, material, cena nem salvar. **Tem.** A conclusão errada custou
> rodadas e criou scripts que não deveriam existir.

---

## 1. O mapa que importa para este projeto

### 1.1 Assets: importar, achar, mover, salvar

| Capacidade | Ferramenta | Quando usar | Evidência |
|---|---|---|---|
| Importar malha estática (FBX/GLB) | `editor_toolset.toolsets.static_mesh.StaticMeshTools.import_file` | trazer do Blender | asset existe + `get_bounds` |
| Importar textura (PNG/TGA) | `editor_toolset.toolsets.texture.TextureTools.import_file` | arte de UI, textura de material | `get_size` |
| Importar malha esquelética | `editor_toolset.toolsets.skeletal_mesh.SkeletalMeshTools.import_file` | corpo/anim | `get_bone_names`, `get_bounds` |
| **Salvar assets** | `editor_toolset.toolsets.asset.AssetTools.save_assets` | **sempre** depois de criar/alterar | `is_dirty` = falso |
| Achar por pasta/tipo | `AssetTools.find_assets`, `list_folders` | inventário, conferência | lista |
| Mover / duplicar / renomear / apagar | `AssetTools.move`, `duplicate`, `delete` | higiene (tirar IP do Content inclui isto) | `exists` no novo caminho |
| Dependências e referenciadores | `AssetTools.get_dependencies`, `get_referencers` | **antes de apagar ou mover** | lista |
| Ler/escrever arquivo de projeto | `AssetTools.read_file`, `write_file` | CSVs de `Data/` | conteúdo |

### 1.2 Materiais e instâncias

| Capacidade | Ferramenta | Quando usar | Evidência |
|---|---|---|---|
| Criar material | `editor_toolset.toolsets.material.MaterialTools.create_material` | base de qualquer look | asset existe |
| Montar o grafo | `MaterialTools.add_expression`, `connect_expressions`, `connect_to_output`, `list_expression_classes` | montar o grafo por código | `get_expressions` |
| Recompilar | `MaterialTools.recompile` | depois de mexer | sem erro |
| **Criar instância** | `editor_toolset.toolsets.material_instance.MaterialInstanceTools.create` | as 11 `MI_Nucleo_*` e 11 `MI_Aura_*` | `list_parameters` |
| Definir pai e parâmetros | `MaterialInstanceTools.set_parent`, `set_vector_parameter`, `set_scalar_parameter`, `set_texture_parameter` | cor do chassi (`CoreColor`), brilho, névoa | `get_vector_parameter` devolve o esperado |

### 1.3 Cena, atores e o mapa

| Capacidade | Ferramenta | Quando usar | Evidência |
|---|---|---|---|
| Abrir/carregar nível | `editor_toolset.toolsets.scene.SceneTools.load_level`, `get_current_level` | trabalhar num mapa | nome do nível |
| **Pôr objeto na cena** | `SceneTools.add_to_scene_from_asset`, `add_to_scene_from_class` | montar cidade, mostradores, atores | `find_actors` |
| Achar atores | `SceneTools.find_actors`, `get_actors_in_folder` | conferir o que existe | lista |
| Transformar | `editor_toolset.toolsets.actor.ActorTools.set_actor_transform`, `get_actor_transform`, `look_at` | posicionar/orientar | transform lido de volta |
| **Medir** | `ActorTools.get_actor_bounds`, `StaticMeshTools.get_bounds` | a evidência de geometria | números em unidades |
| Componentes | `ActorTools.add_component`, `get_components`, `set_parent_component` | montar a vitrine | componentes |
| Etiqueta e tags | `ActorTools.set_label`, `get_label`, `add_tag`, `get_tags` | nomear o que se cria | rótulo |
| Propriedades | `editor_toolset.toolsets.object.ObjectTools.set_properties`, `get_properties`, `list_properties` | configurar qualquer objeto | valor lido de volta |
| Salvar ator | `SceneTools.save_actor` | persistir mudança de ator | — |

### 1.4 Dados (as tabelas do jogo)

| Capacidade | Ferramenta | Quando usar | Evidência |
|---|---|---|---|
| Criar DataTable | `editor_toolset.toolsets.data_table.DataTableTools.create` | `DT_Chassis`, `DT_Classes` | existe |
| Importar de CSV/JSON | `DataTableTools.import_file` | **`Data/*.csv` é a fonte** | `list_rows` |
| Ler/escrever linhas | `DataTableTools.get_rows`, `set_rows`, `add_rows`, `remove_rows`, `rename_rows` | balanceamento | linhas conferidas |
| Esquema | `DataTableTools.get_schema`, `search_row_structs` | garantir que a coluna existe (`CoreColor`) | schema |

### 1.5 Testes, compilação e diagnóstico

| Capacidade | Ferramenta | Quando usar | Evidência |
|---|---|---|---|
| **Rodar testes de automação** | `AutomationTestToolset.AutomationTestToolset.RunTests`, `RunTestsByFilter`, `GetTestResults`, `ListTests` | a DoD exige teste verde | resultado por teste |
| **Compilar C++ no editor aberto** | `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` | não querer fechar o editor | log da compilação |
| Ver a cena | `EditorToolset.EditorAppToolset.CaptureViewport`, `CaptureEditorImage`, `CaptureAssetImage` | prova visual | imagem |
| Jogar (PIE) | `EditorAppToolset.StartPIE`, `StopPIE`, `IsPIERunning` | validar a tela real | captura em PIE |
| Log do editor | `EditorToolset.LogsToolset` | diagnosticar | trecho de log |
| Câmera e seleção | `EditorAppToolset.SetCameraTransform`, `FocusOnActors`, `SelectActors`, `SelectAssets` | enquadrar e navegar | — |
| CVars | `EditorAppToolset.SearchCVars` | investigar configuração | valor |

### 1.6 UI, PCG, Niagara e GAS (as áreas do roadmap)

| Área | Toolsets | Para o quê |
|---|---|---|
| **UI (AAA)** | `UMGToolSet.UMGToolSet` (`CreateWidgetBlueprint`, `AddWidget`, `CompileWidgetBlueprint`, `BindToEventProperty`, `ListWidgetClasses`) · `MVVMToolset.MVVMToolset` · `SlateInspectorToolset.SlateInspectorToolset` | login/seleção/criação em UMG + MVVM |
| **Mundo procedural** | `PCGToolset.PCGToolset` (~30 ferramentas) · `PCGToolset.PCGSpatialToolset` · `WorldConditionsToolset` | ruas, rio, floresta, relevo |
| **FX** | `NiagaraToolsets.NiagaraToolset_System` (emissor, módulo, topologia) · `_Component` · `_Blueprint` · `_Assets` · `_Info` | a aura do cristal |
| **Combate (GAS)** | `GASToolsets.GameplayCueToolset` · `AttributeSetToolset` · `AbilitySystemInspectorToolset` | habilidades, atributos, cues |
| **Animação** | `animation_toolset.toolsets.*` (sequencer, control rig, keyframing) · `SequencerAnimMixer` | animação do corpo |
| **Outros** | `PhysicsToolsets` · `GameplayTagsToolset` · `DataRegistryToolset` · `GameFeaturesToolset` · `ConfigSettingsToolset` · `PluginToolset` · `ChaosClothAssetToolset` · `SemanticSearchToolset` · `state_tree`/`behavior_tree`/`conversation` | conforme a necessidade |

### 1.7 Orquestração em lote (o caminho padrão para muitas chamadas)

| Capacidade | Ferramenta | Como usar |
|---|---|---|
| Rodar código que **chama as ferramentas do MCP** | `editor_toolset.toolsets.programmatic.ProgrammaticToolset.execute_tool_script` | script Python curto com `run()` que orquestra chamadas (montar 20 atores, 11 instâncias, etc.). **Primeiro** chamar `get_execution_environment` |

> Isto substitui a maior parte dos scripts headless: o código roda **dentro** do padrão, com as APIs dos
> toolsets, e não por fora.

---

## 2. Índice completo dos 59 toolsets

**Editor e núcleo:** `EditorToolset.EditorAppToolset` · `EditorToolset.LogsToolset` · `ToolsetRegistry.AgentSkillToolset` ·
`editor_toolset.toolsets.actor.ActorTools` · `.asset.AssetTools` · `.blueprint.BlueprintTools` · `.curve_table.CurveTableTools` ·
`.data_asset.DataAssetTools` · `.data_table.DataTableTools` · `.material.MaterialTools` · `.material_instance.MaterialInstanceTools` ·
`.object.ObjectTools` · `.primitive.PrimitiveTools` · `.scene.SceneTools` · `.skeletal_mesh.SkeletalMeshTools` ·
`.static_mesh.StaticMeshTools` · `.string_table.StringTableTools` · `.programmatic.ProgrammaticToolset` · `.texture.TextureTools`

**Niagara:** `NiagaraToolset_Info`, `_Component`, `_Blueprint`, `_System`, `_Assets`, `GetAssetDiscoveryInfo`,
`FindNiagaraScripts`, `GetNiagaraScriptDigest`

**PCG:** `PCGToolset.PCGToolset`, `PCGToolset.PCGSpatialToolset`, `DataflowAgent.DataflowAgentToolset`

**GAS:** `GASToolsets.GameplayCueToolset`, `.AttributeSetToolset`, `.AbilitySystemInspectorToolset`

**UI e apresentação:** `UMGToolSet.UMGToolSet`, `MVVMToolset.MVVMToolset`, `SlateInspectorToolset.SlateInspectorToolset`

**Animação:** `animation_toolset.toolsets.controlrig.ControlRigTools`, `.sequencer.SequencerTools`,
`.keyframing.SequencerKeyframingTools`, `.controlrig_sequencer.SequencerControlRigTools`, `.outliner.SequencerOutlinerTools`,
`.conditions.SequencerConditionTools`, `.custom_bindings.SequencerCustomBindingTools`, `.import_export.SequencerImportExportTools`,
`sequencer_animmixer_toolset.toolsets.mixer.SequencerAnimMixerTools`

**IA e mundo:** `aimodule_toolset.toolsets.behavior_tree.BehaviorTreeTools`, `state_tree_toolset.toolsets.state_tree.StateTreeTools`,
`conversation_toolset.toolsets.conversation.ConversationTools`, `WorldConditionsToolset.WorldConditionTools`

**Infra e sistema:** `LiveCodingToolset.LiveCodingToolset`, `AutomationTestToolset.AutomationTestToolset`,
`GameFeaturesToolset.GameFeaturesToolset`, `PluginToolset.PluginToolset`, `ConfigSettingsToolset.ConfigSettingsToolset`,
`DataRegistryToolset.DataRegistryTools`, `GameplayTagsToolset.GameplayTagsToolset`, `ChaosClothAssetToolset.ChaosClothAssetToolset`,
`PhysicsToolsets.PhysicsAssetToolset`, `SemanticSearchToolset.SemanticSearchToolset`

---

## 3. Como se chama

1. `list_toolsets` — o índice. 2. `describe_toolset <nome>` — o schema de cada ferramenta (é daqui que saem
os nomes exatos e os argumentos). 3. `call_tool` com `toolset_name`, `tool_name` e `arguments`.
4. Para muitas chamadas: `ProgrammaticToolset.execute_tool_script`.

## 4. O que **não** está coberto (e por isso é exceção registrada)

Ver `EXCECOES.md`. Em resumo, hoje: **geração de imagem/3D com o ComfyUI** quando o caminho do MCP falha, e
nada mais de relevante — todo o resto do acervo tem ferramenta no MCP.

`Docs/mcp/CAPACIDADES.yml` é este mesmo conteúdo em formato legível por máquina, para bot.
