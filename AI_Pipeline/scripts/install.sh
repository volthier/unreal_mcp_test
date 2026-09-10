#!/usr/bin/env bash
# ============================================================================
# install.sh — instalação segura do pipeline AI3D 100% local (Apple Silicon)
# Raiz: <projeto>/AI_Pipeline  (este arquivo fica em AI_Pipeline/scripts/)
#
# Uso:
#   bash scripts/install.sh --dry-run        # (default) só mostra o que faria
#   bash scripts/install.sh --yes            # executa de fato
#   bash scripts/install.sh --yes --with-trellis    # + nó trellis2 no ComfyUI
#   bash scripts/install.sh --yes --smoke    # + teste pós-instalação
#   bash scripts/install.sh --yes --skip-llm # não baixa GGUFs / não instala LM Studio
#
# Segurança: sem sudo; nada fora de ~/miniforge3, AI_Pipeline/, weights/,
# ~/.lmstudio/models/ai-pipeline/ e /Applications/Blender.app. Registra tudo
# em .install_manifest.json (obrigatório p/ remove.sh).
# ============================================================================
set -Eeuo pipefail

PIPELINE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CONFIG="$PIPELINE_ROOT/config/pipeline.json"
MANIFEST="$PIPELINE_ROOT/.install_manifest.json"
LOG="$PIPELINE_ROOT/logs/install.log"
PY=$(command -v python3 || true)

DRY=1; WITH_TRELLIS=0; SMOKE=0; SKIP_LLM=0; YES=0
for arg in "$@"; do
  case "$arg" in
    --dry-run) DRY=1 ;;
    --yes) DRY=0; YES=1 ;;
    --with-trellis) WITH_TRELLIS=1 ;;
    --smoke) SMOKE=1 ;;
    --skip-llm) SKIP_LLM=1 ;;
    --comfyui-dir=*) COMFYUI_DIR="${arg#*=}" ;;
  esac
done

log() { echo "[install] $*" | tee -a "$LOG"; }
run() { # executa com dry-run (só imprime) ou de verdade
  if [ "$DRY" -eq 1 ]; then
    log "(dry-run) $*"
  else
    "$@"
  fi
}

mkdir -p "$PIPELINE_ROOT"/{logs,tmp,engines,weights,work/manifests}
: > "$LOG"

# ---------------------------------------------------------------------------
log "=== AI3D Pipeline — instalação ==="
log "raiz: $PIPELINE_ROOT"

# 0) pré-checagens ----------------------------------------------------------
if [ "$(uname -m)" != "arm64" ]; then
  log "ERRO: este pipeline exige Apple Silicon (arm64)."
  exit 1
fi
sw_vers -productVersion >/dev/null 2>&1 || { log "ERRO: macOS não detectado."; exit 1; }
[ "$PY" ] || { log "ERRO: python3 ausente (instale o Xcode Command Line Tools)."; exit 1; }

FREE_GB=$("$PY" - <<'EOF'
import shutil
print(shutil.disk_usage("/").free // (1024**3))
EOF
)
if [ "$FREE_GB" -lt 40 ]; then
  log "ERRO: espaço livre < 40 GB (tem ${FREE_GB} GB). Libere disco ou mova AI_Pipeline."
  exit 1
fi
if [ "$FREE_GB" -lt 70 ]; then
  log "AVISO: recomendado ≥ 70 GB livres (tem ${FREE_GB} GB). Pode seguir se estiver ok."
fi

# 1) Miniforge --------------------------------------------------------------
MINIFORGE="$HOME/miniforge3"
if [ ! -x "$MINIFORGE/bin/conda" ]; then
  log "instalando Miniforge em $MINIFORGE …"
  INSTALLER="$PIPELINE_ROOT/tmp/Miniforge3-MacOSX-arm64.sh"
  URL="https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-MacOSX-arm64.sh"
  run curl -fL -o "$INSTALLER" "$URL"
  run sh -c "echo 'verify o SHA256 em github.com/conda-forge/miniforge/releases e confirme:'"
  run sh -c "shasum -a 256 \"$INSTALLER\""
  if [ "$DRY" -eq 1 ]; then
    log "(dry-run) não baixou o instalador"
  else
    if [ "$YES" -eq 1 ]; then
      bash "$INSTALLER" -b -p "$MINIFORGE"
    else
      log "pausado: rode 'bash scripts/install.sh --yes' para confirmar a instalação do Miniforge."
      exit 0
    fi
  fi
fi
CONDA="$MINIFORGE/bin/conda"
[ -x "$CONDA" ] || { log "ERRO: conda não disponível em $CONDA."; exit 1; }

# 2) env hy3d-mlx (Hunyuan3D-2.1-mlx + MediaPipe) ---------------------------
if ! "$CONDA" env list | grep -q "hy3d-mlx"; then
  run "$CONDA" create -n hy3d-mlx python=3.11 -y
fi
run "$CONDA" run -n hy3d-mlx python -m pip install --upgrade pip setuptools wheel
run "$CONDA" run -n hy3d-mlx python -m pip install \
  mlx mlx-arsenal safetensors Pillow trimesh scikit-image PyMCubes scipy \
  huggingface_hub xatlas opencv-python mediapipe

# 3) Hunyuan3D-2.1-mlx ------------------------------------------------------
HY3D="$PIPELINE_ROOT/engines/hunyuan3d-2.1-mlx"
if [ ! -d "$HY3D" ]; then
  run git clone --depth 1 https://github.com/dgrauet/Hunyuan3D-2.1-mlx.git "$HY3D"
fi
run "$CONDA" run -n hy3d-mlx python -m pip install -r "$HY3D/requirements.txt"

WEIGHTS_DIR="$PIPELINE_ROOT/weights/hunyuan3d-mlx"
if [ ! -f "$WEIGHTS_DIR/.complete" ]; then
  log "baixando pesos Hunyuan3D-MLX (auto-seleciona quant p/ RAM)…"
  run "$CONDA" run -n hy3d-mlx python - "$WEIGHTS_DIR" <<'EOF'
import json, os, sys
from huggingface_hub import snapshot_download
dest = sys.argv[1]
os.makedirs(dest, exist_ok=True)
snapshot_download("dgrauet/hunyuan3d-2.1-mlx", local_dir=dest)
open(os.path.join(dest, ".complete"), "w").write("ok")
EOF
  # grava weights_dir no config (idempotente)
  run "$PY" - "$CONFIG" "$WEIGHTS_DIR" <<'EOF'
import json, sys
cfg = json.load(open(sys.argv[1]))
cfg["hunyuan3d"]["weights_dir"] = sys.argv[2]
json.dump(cfg, open(sys.argv[1], "w"), indent=2)
EOF
fi

# 4) LM Studio + GGUFs (critic Qwen3-VL + LLaMA-Mesh) -----------------------
if [ "$SKIP_LLM" -eq 0 ]; then
  if [ ! -d "/Applications/LM Studio.app" ]; then
    log "AVISO: LM Studio não encontrado — baixe em https://lmstudio.ai (arraste p/ Applications)."
    log "       O pipeline segue sem o crítico; LLaMA-Mesh ficará indisponível até instalar."
  fi
  LMDIR="$HOME/.lmstudio/models/ai-pipeline"
  run mkdir -p "$LMDIR"
  log "baixando GGUFs (Qwen3-VL-8B Q4 + LLaMA-Mesh Q6)…"
  run "$CONDA" run -n hy3d-mlx python - "$LMDIR" <<'EOF'
import os, sys, re, json
from huggingface_hub import hf_hub_download
dest = sys.argv[1]
jobs = [
    ("Qwen/Qwen3-VL-8B-Instruct-GGUF", [r"Qwen3-VL-8B-Instruct-Q4_K_M\.gguf$"]),
    ("bartowski/LLaMA-Mesh-GGUF", [r"LLaMA-Mesh-Q6_K\.gguf$", r"LLaMA-Mesh-Q5_K_M\.gguf$"]),
]
for repo, pats in jobs:
    try:
        files = [f.rfilename for f in __import__("huggingface_hub").list_repo_files(repo)]
    except Exception as e:
        print("repo não acessível:", repo, e); continue
    for f in files:
        for pat in pats:
            if re.search(pat, f):
                p = hf_hub_download(repo, f, local_dir=dest)
                print("ok:", p)
                break
print("LLM_DOWNLOAD_DONE")
EOF
fi

# 5) Blender ----------------------------------------------------------------
BLENDER_APP="/Applications/Blender.app/Contents/MacOS/Blender"
if [ ! -x "$BLENDER_APP" ]; then
  log "AVISO: Blender não instalado — rode: brew install --cask blender  (ou baixe em blender.org)"
  log "       Etapas 3/4 (mesh/rig/animação) exigem Blender."
fi

# 6) ComfyUI-trellis2-apple (opcional, NO ComfyUI EXISTENTE) -----------------
if [ "$WITH_TRELLIS" -eq 1 ]; then
  COMFYUI_DIR="${COMFYUI_DIR:-$HOME/Documents/ComfyUI}"
  if [ ! -d "$COMFYUI_DIR/custom_nodes" ]; then
    log "ERRO: --with-trellis exige --comfyui-dir=<pasta do ComfyUI> (não achei $COMFYUI_DIR)."
    exit 1
  fi
  NODE="$COMFYUI_DIR/custom_nodes/ComfyUI-trellis2-apple"
  if [ ! -d "$NODE" ]; then
    run git clone --recursive https://github.com/LeonardMeagher2/ComfyUI-trellis2-apple.git "$NODE"
    log "rodando setup.sh do nó trellis2 (usa o venv do próprio ComfyUI)…"
    run bash "$NODE/setup.sh"
  fi
fi

# 7) smoke test -------------------------------------------------------------
if [ "$SMOKE" -eq 1 ] && [ "$DRY" -eq 0 ]; then
  log "smoke: health-check básico…"
  "$CONDA" run -n hy3d-mlx python -c "import mlx.core, trimesh, mediapipe; print('deps OK')"
  "$PY" "$PIPELINE_ROOT/scripts/detect_env.py"
fi

# 8) manifesto de instalação (para remove.sh) --------------------------------
if [ "$DRY" -eq 0 ]; then
  cat > "$MANIFEST" <<EOF
{
  "created_at": "$(date -u +%FT%TZ)",
  "root": "$PIPELINE_ROOT",
  "items": [
    $(if [ -d "$MINIFORGE" ]; then echo "\"$MINIFORGE\","; fi)
    "$PIPELINE_ROOT/engines/hunyuan3d-2.1-mlx",
    "$PIPELINE_ROOT/weights/hunyuan3d-mlx",
    $(if [ "$SKIP_LLM" -eq 0 ]; then echo "\"$HOME/.lmstudio/models/ai-pipeline\","; fi)
    $(if [ "$WITH_TRELLIS" -eq 1 ]; then echo "\"$COMFYUI_DIR/custom_nodes/ComfyUI-trellis2-apple\","; fi)
    "$PIPELINE_ROOT/logs",
    "$PIPELINE_ROOT/tmp",
    "$PIPELINE_ROOT/.install_manifest.json"
  ],
  "conda_envs": ["hy3d-mlx"]
}
EOF
  log "manifesto escrito: $MANIFEST"
fi

log "INSTALL OK${DRY:+(dry-run)} — próximo passo:"
log "  1) generate um asset: python3 scripts/gen_asset.py --slug prop_weapon_sword_v001 --prompt \"espada de ferro medieval\"  [com --ref se tiver imagem]"
log "  2) loop completo:     python3 scripts/agent_loop.py --slug prop_weapon_sword_v001 --prompt \"...\""
log "  3) remover tudo:      bash scripts/remove.sh --yes"
