# AI_Pipeline — Pipeline 100% local de assets 3D para o Unreal Engine (Apple Silicon)

Pipeline gratuito e 100% local (sem nuvem) que transforma **texto/imagem → mesh → textura PBR → rig → animação → import no Unreal**, pensado para o MMORPG de mundo aberto `AI_MEGA_MAN_TEST`.

## Stack garantido (o que este pipeline executa)

```
Texto ──► [MLX Flux | TripoFlux MLX] ──► Imagem
          [Hunyuan3D-2.1-mlx Stage 1 | trellis2-apple] ──► Mesh        ✅ garantido
          [Hunyuan3D-2.1-mlx Stage 2] ──► Textura PBR                  ✅ garantido
          [LLaMA-Mesh GGUF p/ props]  ──► Mesh procedural limpo       ✅ garantido (LM Studio)
          [Blender: Rigify + limpeza] ──► Rig                          ✅ garantido (nativo)
          [MediaPipe → UE Live Link | Motion Matching do UE] ──► Anim  ✅ garantido
```

**Etapa 1 — texto/imagem → 3D:** [Hunyuan3D-2.1-mlx](https://github.com/dgrauet/Hunyuan3D-2.1-mlx) (MLX; INT8 p/ 16 GB, FP16 p/ 32 GB+) · [trellis2-mlx](https://github.com/gtrg55/trellis2-mlx) / [ComfyUI-trellis2-apple](https://github.com/LeonardMeagher2/ComfyUI-trellis2-apple) (detalhe 1024³ em M4 Max, 512³ em Macs menores) · [LLaMA-Mesh GGUF](https://huggingface.co/bartowski/LLaMA-Mesh-GGUF) via LM Studio (props procedurais) · TripoFlux MLX (blockout via gaussian splat).
**Etapa 2 — textura PBR:** Hunyuan3D Stage 2 (albedo 4096² + metallic/roughness) usando a imagem/prompt original.
**Etapa 3 — rig:** Blender Rigify (humano/basic) via `rigify_rig.py` — **não existe modelo HF MLX/MPS de rig** (auditado); MagicArticulate fica como opção CPU lenta documentada.
**Etapa 4 — animação:** MediaPipe Pose (nativo macOS) → retarget em Blender → FBX com esqueleto próprio; Live Link fica como complemento facial; Motion Matching do UE para variação de clips já importados.

> ⚠️ Não-possíveis no Mac (trap documentado): UniRig/SkinTokens, HY-Motion sem CUDA, Arbor (Linux/NVIDIA), AccuRig (Windows-only), MediaPipe4U (Windows/Android). Se uma mensagem de erro citar CUDA — era isso.

## Materiais de cor e "o que fica em pedaços com cores"

- **Material do modelo** = textura **PBR** gerada na etapa 2 e embutida no GLB (`baseColor` + `metallicRoughness`). No UE, o importador glTF cria o material do asset e o pipeline garante um **Master `M_AI_PBR`** + **Material Instance por asset**, unificando escala/tiling.
- **"Fica em pedaços com cores" = 3D Gaussian Splatting** (pontos/partículas coloridos; saída `.splat/.ply` do TripoSplat MLX e VGGT). Papel aqui: **só blockout/preview** em `10_draft` — não entra no UE (não é mesh). O primo "cubinhos coloridos" = **voxel** (mesma regra). Texturas *vertex color* ("modo Color" de TRELLIS) também são rascunho: a etiqueta final sempre é PBR real do Stage 2.

## Estrutura

```
AI_Pipeline/
├── config/pipeline.json          # única config (rotas, budgets, LM Studio, UE)
├── scripts/                      # install.sh remove.sh detect_env.py gen_asset.py
│                                 # paint_asset.py cleanup_mesh.py rigify_rig.py
│                                 # mocap_capture.py mocap_retarget.py qc.py
│                                 # agent_loop.py critic_client.py import_to_ue.py
├── engines/                      # repositórios clonados (somente leitura)
├── weights/                      # pesos MLX + GGUFs
├── work/{slug}/                  # 00_input … 60_approved … 95_archive
│   └── manifests/{slug}.json     # fonte da verdade (estado, métricas, decisões)
├── logs/  · tmp/
└── .install_manifest.json        # criado pelo install.sh; lido pelo remove.sh
```

No Unreal: `Content/AI_Assets/{prop|char|creature|env}/{slug}` + `M_AI_PBR`.

## Convenções de nome

- **Slug:** `{hub}_{subcat}_{nome}_v{NNN}` — `prop_weapon_sword_v001`, `char_player_knight_v002`, `creature_beast_wolf_v001`, `env_ruin_tower_v003`. Novo ciclo = `v+1`; nunca sobrescreve.
- **Inputs:** `{slug}__prompt.md`, `{slug}__refNN.png`, `{slug}__maskNN.png`, `{slug}__mocap_{action}.json`.
- **Outputs:** `{slug}.draft.glb` · `{slug}.preview.splat` (só blockout) · `{slug}.pbr.glb` · `{slug}.clean.fbx` · `{slug}.rig.fbx` · `{slug}.anim_{walk|attack|idle|cast}_{NNN}.fbx` · `{slug}.game.lod0.fbx` · `{slug}.final.glb` · `{slug}.zip` (arquivo).

## Fluxo automático dos agentes

`agent_loop.py` roda a máquina de estados `plan → draft → textured → cleaned → rigged → animated → approved`, com **rodadas de crítica** (até `loop.max_fix_rounds`=3) onde o **Qwen3-VL local** (LM Studio) olha um turntable de 8 views e responde `{faltas:[{regiao,modo}], veredito}`, decidindo `inpaint | regenerate | mesh_edit | rig_fix`. 3 falhas de QC ⇒ `90_quarantine` (sem loop eterno). `--resume` retoma do último estado. Gate humano antes do import (desligue com `--auto`).

```
python3 scripts/agent_loop.py --slug prop_weapon_sword_v001 \
    --prompt "espada de ferro medieval, low-poly, p/ MMORPG" [--ref img.png]
```

## Performance e espaço

| Componente | Disco | RAM pico | Tempo (16 GB / 32 GB+) |
|---|---|---|---|
| Miniforge + envs | ~10 GB | — | — |
| Hunyuan3D-MLX pesos | 3 GB (INT8) / 5,7 GB (FP16) | ~6 / ~10 GB | ST1 ~2–5 min (estim.) · ST2 **~9 min** (verificado M2 Pro) |
| trellis2 pesos | ~15 GB | 512³ ≤ 64 GB · 1024³ M4 Max | 1024³: vários min |
| LM Studio + GGUFs (Qwen3-VL 8B Q4 + LLaMA-Mesh Q6) | ~12 GB | ~8 GB | crítica 10–20 s · prop LLaMA-Mesh 1–2 min |
| Blender 4.x | ~1,5 GB | ~4 GB | mesh 5–15 · rig 5–10 |
| MediaPipe | <0,5 GB | <1 GB | tempo real |
| **Total mínimo 30 GB · recomendado 70 GB livres** | | | |

`detect_env.py` escolhe sozinho: INT8@16 GB, FP16@32 GB+, trellis 512³ ≤ 64 GB, 1 geração por vez ≤ 32 GB (feche o ComfyUI durante gerações).

## Instalar / Remover

```bash
bash scripts/install.sh --dry-run    # ver o que será feito (default)
bash scripts/install.sh --yes        # executa  (--with-trellis <--comfyui-dir=…> · --smoke · --skip-llm)
bash scripts/remove.sh               # dry-run (default)
bash scripts/remove.sh --yes         # remove só o registrado (--keep-archive · --purge-ue)
```

- O install **aborta** se espaço livre < 40 GB, **não usa sudo**, não toca em `ComfyUI` (exceto o nó trellis2 com a flag), nem no projeto UE, nem no LM Studio (só adiciona GGUFs em `~/.lmstudio/models/ai-pipeline`).
- O remove só apaga o que está em `.install_manifest.json`, remove o bloco conda de `~/.zshrc` com backup `.bak`, e jamais mexe no `Content/` do UE sem `--purge-ue` (com confirmação "SIM").

## Comandos avulsos (etapa por etapa)

```bash
# 1+2: gerar e texturizar
python3 scripts/gen_asset.py  --slug prop_weapon_sword_v001 --prompt "espada…" [--ref img.png] [--route auto|hy3d|trellis|llamamesh]
python3 scripts/paint_asset.py --slug prop_weapon_sword_v001

# 3a/3b: limpar + rigar
"/Applications/Blender.app/Contents/MacOS/Blender" -b --python scripts/cleanup_mesh.py -- --slug prop_weapon_sword_v001 --render
"/Applications/Blender.app/Contents/MacOS/Blender" -b --python scripts/rigify_rig.py  -- --slug char_player_knight_v001 --kind human

# 4: capturar + retarget
python3 scripts/mocap_capture.py --action walk --seconds 10
"/Applications/Blender.app/Contents/MacOS/Blender" -b --python scripts/mocap_retarget.py -- --slug char_player_knight_v001 --action walk

# QC + import
python3 scripts/qc.py --asset work/<slug>/50_game/<slug>.game.lod0.fbx --hub prop
python3 scripts/import_to_ue.py --slug prop_weapon_sword_v001
```

## Aceitação (testes recomendados)

1. `install.sh --dry-run` e depois `--yes` → "INSTALL OK" sem erros no `logs/install.log`.
2. `gen_asset.py` gera `draft.glb` + `pbr.glb` de uma espada em ≤ 30 min (QC: tris ≤ 20k, tex ≥ 1024).
3. `agent_loop.py --auto` leva um `char` até `approved` e importa em `Content/AI_Assets/char/`.
4. `mocap_retarget.py` produz `anim_walk_001.fbx` com duração ≥ 1 s e > 15 ossos.
5. `remove.sh --dry-run` lista só o pipeline; remoção real não afeta ComfyUI/LM Studio/projeto.

## Licenças (revisar antes de publicar)

Hunyuan3D-2.1 (licença comunitária Tencent — verificar uso comercial) · LLaMA-Mesh (pesos Llama-3.1) · Qwen3-VL (Apache-2.0) · Blender (GPL) · TRELLIS.2 (MIT código, pesos Microsoft) · MediaPipe (Apache-2.0). Todos os pesos são baixados apenas na instalação; nenhuma API de IA em nuvem é chamada em nenhum momento do fluxo.
