#!/usr/bin/env bash
# ============================================================================
# remove.sh — remoção SEGURA do pipeline AI3D (somente o que foi instalado)
#
# Uso:
#   bash scripts/remove.sh                      # dry-run (default): lista o que apaga
#   bash scripts/remove.sh --yes                # remove de verdade
#   bash scripts/remove.sh --yes --keep-archive # preserva work/60_approved + 95_archive
#   bash scripts/remove.sh --yes --purge-ue     # remove Content/AI_Assets (confirmação)
#
# Guarda-corpos:
#   • só apaga caminhos registrados em .install_manifest.json + a raiz do pipeline
#   • nunca toca em ComfyUI (exceto o nó trellis2 registrado), LM Studio app,
#     em ~/.zshrc (só o bloco conda, com backup .bak), nem no projeto UE (sem --purge-ue)
# ============================================================================
set -Eeuo pipefail

PIPELINE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MANIFEST="$PIPELINE_ROOT/.install_manifest.json"
ZSH="$HOME/.zshrc"
CONDA="$HOME/miniforge3/bin/conda"
UE_CONTENT="$(cd "$PIPELINE_ROOT/.." && pwd)/Content/AI_Assets"

DRY=1; KEEP_ARCHIVE=0; PURGE_UE=0
for arg in "$@"; do
  case "$arg" in
    --yes) DRY=0 ;;
    --dry-run) DRY=1 ;;
    --keep-archive) KEEP_ARCHIVE=1 ;;
    --purge-ue) PURGE_UE=1 ;;
  esac
done

[ -f "$MANIFEST" ] || { echo "[remove] manifesto não encontrado: $MANIFEST"; exit 2; }

ITEMS=$(python3 - "$MANIFEST" <<'EOF'
import json, sys
m = json.load(open(sys.argv[1]))
for i in m.get("items", []):
    print(i)
for e in m.get("conda_envs", []):
    print("ENV:" + e)
EOF
)
[ -n "$ITEMS" ] || { echo "[remove] manifesto sem itens (nada a remover)."; exit 0; }

echo "[remove] planejando remoção:"
echo "$ITEMS" | sed 's/^/  - /'

delete() {
  local p="$1"
  if [ ! -e "$p" ]; then
    echo "  (já não existe) $p"
    return
  fi
  if [ "$DRY" -eq 1 ]; then
    echo "  (dry-run) rm -rf  $p"
  else
    rm -rf "$p" && echo "  removido: $p"
  fi
}

# 1) itens do manifesto (arquivos/pastas)
echo "$ITEMS" | grep -v '^ENV:' | while IFS= read -r item; do
  [ -z "$item" ] && continue
  delete "$item"
done

# 2) envs conda registrados
if [ -x "$CONDA" ]; then
  echo "$ITEMS" | grep '^ENV:' | while IFS= read -r line; do
    env="${line#ENV:}"
    [ -z "$env" ] && continue
    if [ "$DRY" -eq 1 ]; then
      echo "  (dry-run) conda env remove -n $env"
    else
      "$CONDA" env remove -n "$env" -y || echo "  aviso: env $env não removido"
    fi
  done
fi

# 3) ~/.zshrc — remove apenas o bloco 'conda initialize', com backup
if [ -f "$ZSH" ] && grep -q "conda initialize" "$ZSH"; then
  if [ "$DRY" -eq 1 ]; then
    echo "  (dry-run) removeria o bloco 'conda initialize' de $ZSH (backup .bak)"
  else
    cp "$ZSH" "$ZSH.bak"
    python3 - "$ZSH" <<'EOF'
import sys
p = sys.argv[1]
lines = open(p).read().splitlines(keepends=True)
out, skip = [], False
for ln in lines:
    if "# >>> conda initialize >>>" in ln:
        skip = True
        continue
    if "# <<< conda initialize <<<" in ln:
        skip = False
        continue
    if not skip:
        out.append(ln)
open(p, "w").writelines(out)
EOF
    echo "  bloco conda removido de $ZSH (backup em $ZSH.bak)"
  fi
fi

# 4) raiz do pipeline (config/scripts/engines/weights/logs/tmp + pergunta work)
if [ "$DRY" -eq 0 ]; then
  if [ -d "$PIPELINE_ROOT/work" ] && [ "$KEEP_ARCHIVE" -eq 0 ]; then
    read -r -p "[remove] há work/ (assets gerados). Apagar também? (y/N) " ans
    if [ "$ans" = "y" ] || [ "$ans" = "Y" ]; then
      rm -rf "$PIPELINE_ROOT/work"
    fi
  fi
  rm -rf "$PIPELINE_ROOT"/{config,scripts,engines,weights,logs,tmp}
  rm -f "$PIPELINE_ROOT/README.md" "$PIPELINE_ROOT/.install_manifest.json"
  echo "[remove] removido: $PIPELINE_ROOT (raiz)"
else
  echo "  (dry-run) rm -rf $PIPELINE_ROOT/{config,scripts,engines,weights,logs,tmp}"
fi

# 5) --purge-ue: removo do Content/AI_Assets com confirmação dupla
if [ "$PURGE_UE" -eq 1 ] && [ -d "$UE_CONTENT" ]; then
  if [ "$DRY" -eq 1 ]; then
    echo "  (dry-run) removeria $UE_CONTENT"
  else
    read -r -p "[remove] REMOVER $UE_CONTENT do projeto UE? digite SIM: " ans
    if [ "$ans" = "SIM" ]; then
      rm -rf "$UE_CONTENT" && echo "  removido: $UE_CONTENT"
    else
      echo "  cancelado."
    fi
  fi
fi

echo "[remove] concluído (${DRY:+dry-run})."
echo "[remove] ComfyUI, LM Studio app e projeto UE permanecem intactos"
echo "[remove] (LM Studio: se quiser, apague ~/.lmstudio/models/ai-pipeline manualmente)."
