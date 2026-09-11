# Exceções registradas — o que ainda é script, e por quê

> **Regra:** o caminho padrão é o MCP. Um script só existe onde o MCP **não cobre** — e então ele fica aqui,
> com motivo, risco e gatilho de revisão. Esta lista foi **refeita** depois de levantar o servidor: a versão
> anterior estava errada e listava como exceção coisas que o MCP sempre teve (import, material, cena, salvar).

## 1. Exceções que permanecem

| ID | Script | Por que ainda é script | Gatilho de revisão |
|---|---|---|---|
| `EXC-001` | `Tools/comfy/*.py` (geração de imagem e de 3D) | o MCP do Comfy embrulha o CLI e falha em `run_workflow` neste ambiente; a API HTTP do ComfyUI foi o caminho que funcionou | reavaliar quando o `run_workflow` do `comfy-mcp` executar sem `ComfyCliError` |
| `EXC-002` | `Tools/Blender/preparar_para_ue.py`, `inspect_and_render.py` | hoje rodam por CLI. **Devem migrar** para `execute_blender_code` do MCP do Blender (mesmo código, porta padrão) — ficam aqui só até a migração | migrar na tarefa `MCP-005`; depois desta data não deve sobrar nenhuma |
| `EXC-005` | `Tools/unreal/importar_texturas_faltantes.py` — **importar textura** | **Medido nesta sessão:** o `TextureTools.import_file` do MCP descarta estes PNGs com *"Import of ... produced no assets. The file format may not be supported or the file may be invalid"*, mesmo com o arquivo sendo PNG válido (conferido com `file`: PNG 1600x896, 8-bit RGB). Tentei duas vezes, com e sem asset existente no destino, e falhou nas duas. O mesmo arquivo entra sem problema pelo `unreal.AssetImportTask` do script de engine — que também CONFIRMA cada asset com `does_asset_exist` (a lição da rodada 13, quando duas texturas nunca importaram e ninguém percebeu) | revisar quando o MCP importar textura com o mesmo arquivo — aí a exceção cai e o kit visual passa a entrar inteiro pelo MCP |
| `EXC-004` | `Tools/unreal/criar_pcg_floresta.py` (a criar) — configurar a **lista de malhas do `Static Mesh Spawner`** de um grafo de PCG | **Medido nesta sessão:** o MCP **não escreve** as entradas do seletor de malha. Tentei quatro caminhos, todos devolveram `false` e a lista continuou vazia: `UpdateNode` com `meshSelectorParameters`; `ObjectTools.set_properties` no objeto seletor instanciado com `mesh_entries`; com `meshEntries`; e com a malha como string em vez de referência. O que o MCP **consegue**: criar o grafo, adicionar nós, ligar pinos, dar densidade ao amostrador, instanciar o volume e executar. Só a lista de malhas fica de fora. **Precedente:** a mesma operação foi feita com sucesso por Python de engine na rodada 11 (`criar_pcg_cidade.py`), mutando o objeto instanciado que o getter devolve | revisar quando o `ObjectTools.set_properties` passar a escrever estruturas de objeto instanciadas — aí a exceção cai e o `PCG_FlorestaKardys` (que já está criado em disco, com amostrador e spawner ligados) passa a ser configurável só pelo MCP |
| `EXC-003` | `Tools/unreal/consolidar_modulo.sh` | resolve um artefato de build **do lado do sistema de arquivos** (manifesto × dylib), fora do alcance do editor: é o que destrava o próprio MCP quando o módulo não carrega | revisar se o UBT deixar de escrever iterações com o editor aberto |

## 2. O que deixou de ser exceção (e passa a ser MCP)

| Antes (script) | Agora (MCP) |
|---|---|
| `importar_predios.py`, `importar_casulo.py`, `importar_texturas_faltantes.py`, `importar_ui.py`, `importar_cidade_assets.py` | `StaticMeshTools.import_file`, `TextureTools.import_file`, `SkeletalMeshTools.import_file` + `AssetTools.save_assets` |
| `criar_materiais_nucleo.py`, `refazer_aura.py`, `criar_aura_material.py`, `create_accent_material.py` | `MaterialTools.create_material` + `MaterialInstanceTools.create` + `set_vector_parameter` |
| `criar_mundo.py`, `criar_cidade.py`, `vitrine_dos_nucleos.py`, `montar_cena.py` | `SceneTools.add_to_scene_from_asset` + `ActorTools.set_actor_transform` + `ObjectTools.set_properties` |
| `medir_malha.py`, `medir_cidade.py` | `ActorTools.get_actor_bounds` e `StaticMeshTools.get_bounds` |
| `import_datatables.py` | `DataTableTools.import_file` |
| testes por linha de comando (`-ExecCmds=Automation RunTests`) | `AutomationTestToolset.RunTests` / `RunTestsByFilter` / `GetTestResults` |
| recompilar C++ fechando o editor | `LiveCodingToolset.CompileLiveCoding` |
| `Tools/unreal/mcp_direto.py` e os outros 16 clientes MCP em Python | **removidos** — cliente próprio é proibido |

## 3. Como declarar uma exceção nova

1. Provar que o MCP não cobre: `list_toolsets` + `describe_toolset` (a prova fica no PR).
2. Registrar aqui com ID (`EXC-00N`), motivo, risco de compatibilidade e gatilho de revisão.
3. O script recebe o ID no cabeçalho, para ser rastreável por `grep EXC-`.
4. Quando o MCP ganhar a capacidade, a exceção **cai** e o script é removido no mesmo PR.

## 4. Nota sobre a era anterior (para não repetir)

Este projeto acumulou **17 scripts que falavam JSON-RPC de MCP por conta própria**. Eles escondiam a
capacidade real do servidor (59 toolsets!), não renovavam sessão, e faziam parecer que faltavam ferramentas
que sempre existiram. **Nenhum deles deve voltar.**
