# Pipeline de Assets 3D via MCP — Unreal / Blender / ComfyUI

> Este documento descreve o pipeline **end-to-end** para gerar assets 3D com os três motores conectados por MCP, e o **protocolo de memória** para que a geração não derrube o agente (LLM residente no LM Studio) nem o ComfyUI.

## 1. Stack MCP (estado atual)

| Servidor | Cliente | Transporte / endereço | Ferramentas | Status |
|----------|---------|-----------------------|-------------|--------|
| **Unreal** | `unreal-mcp` | HTTP `http://127.0.0.1:8000/mcp` | Toolsets da engine (Editor, PCG, UMG, GAS, Asset, Blueprint, …) — descoberta via `list_toolset`/`call_tool` | ✅ auto-start configurado |
| **Blender** | `blender` | stdio `uvx --python 3.11 blender-mcp`; socket `localhost:9876` | `execute_blender_code`, import/export, etc. | ✅ addon v1.2 no Blender 5.2 |
| **ComfyUI** | `comfy-mcp` | stdio `~/.venvs/comfy-mcp/bin/comfy-mcp`; API `http://127.0.0.1:8188` | `server_info`, `run_workflow`, `job`, `fetch_outputs`, `validate_workflow`, `search_models`, `nodes`, `system_stats`, `free_memory`, `launch/stop_comfyui`, … | ✅ instalado e testado |

### Configuração (`config/` e `.mcp.json`)

`.mcp.json` (já atualizado):
```json
{
  "mcpServers": {
    "unreal-mcp": { "type": "http", "url": "http://127.0.0.1:8000/mcp" },
    "blender": {
      "command": "/Users/volthier/.local/bin/uvx",
      "args": ["--python", "3.11", "blender-mcp"],
      "env": { "UV_PYTHON_PREFERENCE": "only-managed", "BLENDER_HOST": "localhost", "BLENDER_PORT": "9876" }
    },
    "comfy-mcp": {
      "command": "/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp",
      "env": { "COMFY_BIN": "/Users/volthier/.venvs/comfy-mcp/bin/comfy" }
    }
  }
}
```

### Unreal — MCP da engine (plugin embutido Epic)
- Plugin `ModelContextProtocol` (Experimental) habilitado no `.uproject`.
- Porta **8000**, path **`/mcp`**, `bAutoStartServer=True` (config em `Config/DefaultEditorPerProjectUserSettings.ini` + `Saved/Config/MacEditor/EditorPerProjectUserSettings.ini`).
- Observação: o plugin de terceiros `UnrealMCP` (IvanMurzak) foi **removido do projeto** — estava desabilitado no `.uproject`, não era citado em nenhum `Config/` e o `.mcp.json` nunca apontou para ele. A engine **5.8 já traz a MCP oficial** (`ModelContextProtocol` + `MCPClientToolset` + Toolsets), que é a que roda na porta **8000**. Se algum dia for preciso, é plugin público: baixar de novo e habilitar no `.uproject`.

### Blender — MCP
- Addon **Blender MCP v1.2** em `~/Library/Application Support/Blender/5.2/scripts/addons/addon.py`.
- **Para usar**: abrir Blender → sidebar (N) → aba **BlenderMCP** → *Connect to Server* (sobe socket 9876).

### ComfyUI — MCP (configuração local)
> Fonte oficial: `comfy.org/mcp` + `docs.comfy.org/agent-tools/mcp` (pacote `comfy-mcp` 0.10.0, `comfy-cli` 1.19.0).
- venv: `~/.venvs/comfy-mcp` (`uv venv --python 3.11`; `uv pip install "comfy-cli>=1.14.0" comfy-mcp`).
- **Workspace apontado** (via `comfy set-default`) para: `/Users/volthier/ComfyUI-Installs/ComfyUI/ComfyUI` — o repo interno (o diretório externo `.../ComfyUI-Installs/ComfyUI` **não** é reconhecido como workspace; verificado).
- Instância rodando: **v0.34.2** em `http://127.0.0.1:8188` (Apple M2 Max, 64 GB unificados).
- **Não usar `comfy launch` enquanto o Comfy Desktop estiver rodando** (conflito de porta; um controlador por vez).

---

## 2. Protocolo de Memória (não derrubar o agente)

**Cenário (64 GB unificados):** o LLM do agente fica **residente** no LM Studio; o ComfyUI carrega o modelo de geração (Hunyuan3D **6,9 GB** + VAE/ativações ≈ 10–20 GB em uso). A memória é **unificada** (CPU/GPU), então a pressão é compartilhada.

### Regras
1. **Liberar modelo após o uso** — rodar o ComfyUI com `--cache-none` (modelos saem da RAM ao final de cada job). Aplicar nos launchArgs do Comfy Desktop (ex.: `--enable-manager --cache-none`) ou via `comfy launch --cache-none`.
2. **Um job por vez** — usar `run_workflow(wait=False)` + `job(action="wait")` (ou `wait=True` no v0.10, com atenção ao timeout de 110 s). Nunca fila paralela de gerações grandes.
3. **Guard pré-execução** — antes de um job pesado, rodar o guard:
   ```
   python3 Tools/comfy/memory_guard.py --need 15
   ```
   (exit 0 = OK; exit 1 = ABORT). Regra: `free ≥ modelo + 8 GB` de folga. Com a LM Studio em carga, medimos **~20–51 GB** disponíveis — folga suficiente para Hunyuan3D, mas **não** para vídeo grande (ver tabela).
4. **Ciclo econômico** — `launch_comfyui` sob demanda → gera → `fetch_outputs` → `stop_comfyui` se a próxima etapa for LLM-pesada (memória volta ao baseline). Também existe `free_memory` (descarrega modelos sem parar o servidor).
5. **Fallback** — se o modelo não couber (ex.: vídeo LTX 20 GB), usar **Comfy Cloud** (`https://cloud.comfy.org/mcp`, OAuth, 0 GB local). Requer assinatura Comfy Cloud.

### Tamanhos dos modelos instalados (para dimensionar o guard)
| Modelo | Arquivo | Tamanho | `--need` sugerido |
|--------|---------|---------|-------------------|
| Hunyuan3D 2.1 | `hunyuan_3d_v2.1.safetensors` | 6,9 GB | **15 GB** |
| z_image_turbo bf16 | `z_image_turbo_bf16.safetensors` | 11 GB | **20 GB** |
| ltx-2.5 (vídeo, int8) | `ltx-2.5-22b-*.safetensors` | 20 GB | **28 GB** |
| stable-audio | `stable-audio-open-1.0.safetensors` | 4,5 GB | 12 GB |

> ⚠️ A doc oficial do Comfy avisa: em Mac, os modelos open-weight atuais (LTX, MiniMax) não rodam em velocidade prática no Apple GPU. Para 3D (Hunyuan3D 6,9 GB) o M2 Max aguenta; para vídeo/LLM image, considere Comfy Cloud.

---

## 3. Pipeline 3D end-to-end

```
[Referência de arte] ──ComfyUI MCP──> GLB ──Blender MCP──> FBX ──Unreal MCP──> Content/ (Import + BP)
```

### Passo 1 — Gerar com ComfyUI (Hunyuan3D 2.1)
1. Check do ambiente: `server_info()` → `system_stats()` (confirma ComfyUI local em 8188).
2. Conferir modelo: `search_models()` (ex.: `hunyuan_3d_v2.1.safetensors`).
3. **Guard de memória**: `python3 Tools/comfy/memory_guard.py --need 15` → precisa exit 0.
4. **Validar workflow**: `validate_workflow(workflow_path)` → `{"valid": true}`.
5. **Rodar**: `run_workflow(workflow_path, wait=False)` → `prompt_id` → `job(action="wait", prompt_id)`.
6. **Buscar saída**: `fetch_outputs(prompt_id, out_dir)` → `.glb`.

Workflow pronto: `Tools/comfy/workflows/hunyuan3d_test.json` (formato API; usa `sample_maverik.jpg` como entrada). 

> ⚠️ **Cuidado com os IDs no JSON de workflow**: as referências de input devem ser **strings** (`["9", 0]`), não inteiros (`[9, 0]`) — chaves e referências do mesmo tipo. Referências int com chaves string causam `KeyError` no `/prompt` (`prompt_outputs_failed_validation`).

### Passo 2 — Refinar no Blender (via Blender MCP)
- Abrir o `.glb` gerado em `Art/generated/hunyuan3d_test/`.
- **Escala**: Blender trabalha em metros; no Blender MCP, manter unidade e exportar FBX com `global_scale=1.0`, `apply_unit_scale=True`, forward `-Y`, up `Z` (ver `Docs/ImportPipeline.md`).
- **Sockets**: adicionar `weapon_r`, `MuzzleFlash`, `ProjectileSpawn`, `FX` em bone `weapon_r` (não exportar empties `SOCKET_*` como ossos).
- Exportar **FBX** para `Art/<nome>/`.

### Passo 3 — Importar no Unreal (via Unreal MCP)
- `SkeletalMeshTools.import_file(folder_path=/Game/<destino>, asset_name=..., source_file=<fbx>, import_materials=true, import_textures=true, import_animations=true, create_physics_asset=true)`.
- Aplicar escala do componente 100 (em `BP_PlayerRobot`), altura ≈ 155 cm.
- Validar com screenshot do viewport (toolset de screenshot da engine).
- **Convenções**: prefixos `SKM_`/`T_`/`M_`/`MI_`; LODs per `Docs/CharacterDesign.md`; textura **2K** no jogo (4K como fonte em `Art/`).

---

## 4. Arquivos auxiliares em `Tools/comfy/`

| Arquivo | Uso |
|---------|-----|
| `memory_guard.py` | Guard de RAM pré-geração (`--need <GB>`) |
| `mcp_smoke_test.py` | Teste MCP: list_tools + server_info + system_stats + validate_workflow; `--run` executa a geração |
| `wait_and_fetch.py` | Aguarda `prompt_id` via `/history` e baixa os outputs via `/view` |
| `workflows/hunyuan3d_test.json` | Workflow API (imagem→GLB) Hunyuan3D 2.1 |

## 5. Checklist de validação
- [x] `comfy-mcp` expõe 39 tools (smoke test OK).
- [x] `server_info`/`system_stats` confirmam ComfyUI 0.34.2 em 127.0.0.1:8188.
- [x] `validate_workflow` → `valid: true` (1 warning de tipo `MESH` vs `FILE_3D_GLB`, inofensivo).
- [x] Workflow aceito no `/prompt` (validado após correção dos IDs string).
- [ ] GLB gerado no passo 3D (em andamento: `Art/generated/hunyuan3d_test/`).
- [ ] `memory_guard --need 15` → passou (memória disponível medida).
- [ ] `unreal-mcp` em 8000 ao abrir o editor (auto-start).
- [ ] `blender` MCP `tools/list` com add-on conectado.
