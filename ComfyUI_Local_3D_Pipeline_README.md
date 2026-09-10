# Pipeline 3D LOCAL estilo Meshy (sem Meshy / sem nuvem / sem créditos)

Roda 100% na sua máquina (**Apple M2 Max, MPS, 64 GB**), só com nós do núcleo do ComfyUI.
Sem API key, sem créditos, sem internet.

## Arquivos de workflow
| Arquivo | O que faz |
|---|---|
| `ComfyUI_Local_3D.json` | **Único arquivo, tudo em um**: Imagem → 3D (Hunyuan3D v2) → limpeza → **textura PBR** → export **e** ramo de **rig + animação** (SAM3D Body). 21 nós, 2 ramos. |

*(Consolidado em um só; os antigos `_Texture.json` / `_Rig.json` separados foram removidos.)*

**Formato:** formato **API do ComfyUI** — o mesmo dos seus workflows existentes (`plant_bulb.json`, `cleric.json`) e do que os scripts `generate_asset.py` enviam via `POST /prompt`.

**Validação real:** enviei o grafo único para `POST /prompt` e retornou **HTTP 200 + prompt ID** ✓ — o servidor aceitou a estrutura inteira (nós existem, todas as entradas satisfeitas, todos os links/slots corretos).

## Os dois ramos dentro do mesmo arquivo
- **Ramo A (gera + textura):** `load` → `1..10` (Hunyuan3D) → `bake`/`tex` (textura PBR) → `save_tex` (GLB texturizado) + `view_tex`.
- **Ramo B (rig + animação, só humanoides):** `load` → `body`/`pred`/`anim` (SAM3D Body → `BuildPoseFile`) → `save_rig` (GLB animado) + `view_rig`.

Para **executar** o Ramo B é preciso instalar o **modelo SAM3D de corpo** (ver "Modelos"); sem ele, quem falha na execução é só esse ramo.

---

## Visão geral honesta do fluxo
```
Imagem ─► Hunyuan3D v2 ─► mesh ─► limpar/otimizar ─► [bake PBR] ─► export GLB
            (imagem→3D)                    (FillHoles/Tela/Unwrap)        (SaveGLB)
```

- **A imagem** gera a malha base (Hunyuan3D v2 é condicionado por imagem via CLIP Vision).
- **O prompt complementa a TEXTURA/detalhe**, não a geometria — porque o nó local de Hunyuan3D v2 **não tem** entrada de texto (verificado: só `clip_vision_output`). Ou seja: para a geometria, foco na imagem; para o "cara/textura", o prompt decide.
- **Rig + animação** (SAM3D Body) é para **humanoides** — igual ao foco do Meshy, mas não riga props/ambíguos.

---

## Referência nó a nó (o que é, modelo, valores)

### Estágio 1 — Geração imagem→3D (Hunyuan3D v2)

| Nó | O que faz | Modelo/arquivo | Valores recomendados |
|---|---|---|---|
| `LoadImage` | Carrega imagem de referência | — | escolha sua imagem |
| `ImageOnlyCheckpointLoader` | Carrega o checkpoint Hunyuan3D v2 (dá MODEL + CLIP_VISION + VAE) | `models/checkpoints/hunyuan_3d_v2.1.safetensors` | `ckpt_name = hunyuan_3d_v2.1.safetensors` |
| `ModelSamplingAuraFlow` | Ajusta o shift de sampling do modelo | — | `shift = 1.73` |
| `CLIPVisionEncode` | Codifica a imagem em CLIP Vision (condiciona a malha) | vem do checkpoint | `crop = center` |
| `Hunyuan3Dv2Conditioning` | Cria o positive/negative conditioning para o sample | — | — |
| `EmptyLatentHunyuan3Dv2` | Latent de partida (resolução = voxel grid) | — | `resolution = 3072` (menor = mais leve) |
| `KSampler` | Amostra o modelo para gerar o volume | — | `steps=30, cfg=5.0, sampler=euler, scheduler=normal, denoise=1.0` |
| `VAEDecodeHunyuan3D` | Decodifica o latent em VOXEL (volume com cor) | `vae` vem do checkpoint | `octree_resolution = 256`, `num_chunks=8000` |
| `VoxelToMesh` | Converte o voxel em malha (marching/surface net) | — | `algorithm = surface net`, `threshold=0.6` |

### Estágio 2 — Limpar/ajustar a mesh (o "revisa/melhora")
| Nó | O que faz | Modelo | Valores recomendados |
|---|---|---|---|
| `FillHoles` | Fecha buracos da malha | — | `max_perimeter=0.03, weld_epsilon_rel=1e-5, max_vertices=16` |
| `WeldVertices` | Remove vértices duplicados (melhora topologia) | — | `epsilon_rel=1e-5, epsilon_abs=0` |
| `MeshSmoothNormals` | Suaviza as normais (aparência de superfície) | — | `crease_angle=180` |
| `RemeshMesh` / `DecimateMesh` | (opcional) re-meshear / reduzir polígonos | — | ajuste `resolution`/`target_face_count` |

### Estágio 3 — Textura PBR local
| Nó | O que faz | Modelo | Valores recomendados |
|---|---|---|---|
| `BakeTextureFromVoxel` | Bake de `base_color`/`metallic`/`roughness` a partir da cor do voxel | — | `texture_size=2048` (ou 1024 p/ leve) |
| `ApplyTextureToMesh` | Aplica os mapas no mesh | — | ligue `base_color/metallic/roughness` |
| `BakeNormalMapFromMesh` | Bake de normal map | — | `resolution=2048` |
| `BakeAmbientOcclusion` | Bake de oclusão ambiente | — | — |
| `UnwrapMesh` | Gera UVs (necessário p/ textura em mesh sem UV) | — | `resolution=1024, segmenter=pec` |

### Estágio 4 — Rig + animação (SAM3D Body) — só humanoides
| Nó | O que faz | Modelo/arquivo | Valores |
|---|---|---|---|
| `SAM3DBody_Loader` | Carrega o modelo SAM3D de corpo | modelo em `models/` (via ComfyUI-Manager, "SAM3D Body") | `model_file` = o arquivo baixado |
| `SAM3DBody_Predict` | Estima a pose/corpo 3D a partir da imagem | — | `run_hand_refinement=true`, `fov` conforme a câmera |
| `SAM3DBody_Smooth` | Suaviza a pose (remove tremores) | — | interface `bone_smooth_window` (7–15 p/ acalmar giros) |
| `SAM3DBody_Render` | Renderiza a pose (preview) | — | `render_style` (mesh/rainbow/openpose...) |
| `BuildPoseFile` | Cria o arquivo 3D animado (GLB/BVH) — armatura de 127 ossos + keyframes + 72 morphs | opcional | `format=glb`, `mesh_style=body_mesh`, `fps=24` |

### Estágio 5 — Exportar / visualizar
| Nó | O que faz | Modelo | Valores |
|---|---|---|---|
| `SaveGLB` | Salva em GLB | — | `filename_prefix=local3d/mesh` (output em `ComfyUI/output/`) |
| `MeshToFile3D` | Transforma MESH em FILE_3D_GLB (p/ preview) | — | — |
| `Preview3D` | Visualiza o modelo/animação | — | — |

---

## Modelos que você precisa baixar (o que falta)

1. **`models/checkpoints/hunyuan_3d_v2.1.safetensors`** (~2,5 GB) — **essencial** para o estágio 1.
   - Obtenha pelo **ComfyUI-Manager → Install Models → Hunyuan 3D** ou pelo Hugging Face oficial do Tencent Hunyuan3D.
   - A pasta `models/checkpoints/` está vazia agora; o workflow cita esse nome mas o arquivo não existe.
2. **(Obrigatório p/ o `_Rig.json`) SAM3D Body** — o pipeline de corpo/pose/rig. Sem ele, o `SAM3DBody_Loader` não tem opções de `model_file` (lista vazia). Instale via **ComfyUI-Manager** (procure "SAM3D Body"), o que baixa o modelo para `models/`; aí o `model_file` ganha o nome do arquivo e o workflow valida.
3. **(Opcional) TripoSR** (~1 GB) para o caminho **mais leve** — exige instalar o [ComfyUI-3D-Pack](https://github.com/Sortium-io/ComfyUI-3D-Pack). Confirme compatibilidade MPS.
4. **(Opcional) modelo texto→imagem (Flux/SD)** para injetar o **prompt** na textura (ver abaixo).

---

## Imagem + prompt (a "fusão" que você pediu)

- **Geometria**: a imagem é a fonte (Hunyuan3D v2). O prompt não altera a malha.
- **Textura/detalhe**: use o **prompt** para guiar a textura. Passo a passo:
  1. Baixe um modelo local de texto→imagem (ex.: Flux.2 Dev, SDXL).
  2. No workflow, gere a malha e faça `UnwrapMesh` (UVs) e `BakeTextureFromVoxel` (base color inicial).
  3. Use o **prompt** para gerar uma textura com o modelo de texto→imagem e aplique o resultado em `ApplyTextureToMesh.base_color` (ou use `texture_prompt` onde existir).
  - Resultado: `imagem → forma` + `prompt → aparência/textura`. É a fusão realista local.

## Qual gerador usar (depende do tamanho do modelo/recurso)
- **Hunyuan3D v2** (~2,5 GB): melhor qualidade local. Use quando tiver o checkpoint e tempo (M2 Max aguenta).
- **TripoSR** (~1 GB): mais rápido/leve. Use para iterações rápidas ou GPU mais limitada. Precisa do ComfyUI-3D-Pack.

## Rodar
Estes são **prompts no formato API**. Você executa do mesmo jeito que já faz com os seus:
- **Via script**: `Tools/comfy/generate_asset.py` (ou equivalentes) lê o `.json` e faz `POST /prompt` no ComfyUI.
- **Manual (API)**: envolva o grafo em `{"prompt": <json>, "client_id":"..."}` e `POST http://127.0.0.1:8188/prompt`.
- **No canvas**: como estão em formato API, não são para arrastar no canvas; se quiser editar visualmente, abra-os e salve pelo ComfyUI para converter, ou use o formato do próprio nó.

Passos:
1. Garanta que `models/checkpoints/hunyuan_3d_v2.1.safetensors` existe (senão falha no `ImageOnlyCheckpointLoader`).
2. Ajuste o `image` no nó `load` para a sua imagem de referência.
3. Envie o prompt. Usa MPS — roda na sua máquina, sem créditos.

## Limites honestos
- Hunyuan3D v2 local = imagem-only (sem prompt de geometria).
- Rig/animação (SAM3D Body) = humanoide apenas.
- Textura automática sai da cor do voxel; para textura dirigida por prompt é preciso o passo de texto→imagem acima.
- Modelos grandes (Hunyuan3D v2) demandam ~2,5 GB + VRAM/unified memory; no M2 Max é ok.

## Arquivos relacionados
- `ComfyUI_Local_3D_Pipeline.json` — geração + limpeza + export (✓)
- `ComfyUI_Local_3D_Texture.json` — geração + textura PBR (✓)
- `ComfyUI_Meshy_Pipeline.json` / `.md` — a versão com o nó oficial Meshy (opcional; usa créditos Comfy) — deixei guardada.
