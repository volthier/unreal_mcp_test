#!/usr/bin/env python3
"""critic_client.py — cliente do crítico/planejador local (LM Studio).

Fala com http://localhost:1234/v1 (LM Studio, OpenAI-compatible) usando o
modelo Qwen3-VL (visão) e devolve um JSON ESTRITO:
  {"faltas": [{"regiao": str, "modo": "inpaint|regenerate|mesh_edit|rig_fix"}],
   "veredito": "continuar|aprovar|falhar", "nota": str}

Uso:
  critic_client.py --prompt work/<slug>/input/<slug>__prompt.md \
                   --images work/<slug>/tmp/turntable/*.png [--json]
"""
import argparse
import base64
import json
import mimetypes
import sys
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())

SYSTEM_PROMPT = (
    "Você é o crítico de um pipeline de geração de assets 3D. Recebe um prompt de asset, "
    "o manifesto atual e 8 renders (turntable) do modelo 3D. Liste IMPERATIVAMENTE o que "
    "falta ou está errado (regiões: silhueta, topologia visível, geometria, rig, animação) "
    "e escolha para cada falta um modo: inpaint (regenerar região via imagem), regenerate "
    "(gerar do zero), mesh_edit (ajustar no Blender), rig_fix (refazer rig). "
    "Responda SOMENTE JSON válido, sem texto antes/depois, no formato: "
    '{"faltas":[{"regiao":"...","modo":"inpaint"}],"veredito":"continuar|aprovar|falhar","nota":"..."}'
)


def _b64_image(path: Path) -> dict:
    mime = mimetypes.guess_type(str(path))[0] or "image/png"
    data = base64.b64encode(path.read_bytes()).decode("ascii")
    return {"type": "image_url", "image_url": {"url": f"data:{mime};base64,{data}"}}


def call_lm(base_url: str, model: str, prompt: str, images: list[Path], timeout: int = 300) -> dict:
    messages = [
        {"role": "system", "content": SYSTEM_PROMPT},
        {"role": "user", "content": [
            {"type": "text", "text": prompt},
            *[_b64_image(p) for p in images],
        ]},
    ]
    payload = json.dumps({"model": model, "messages": messages, "temperature": 0.2}).encode()
    req = urllib.request.Request(
        f"{base_url}/chat/completions", data=payload,
        headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        body = json.loads(resp.read().decode())
    content = body["choices"][0]["message"]["content"]
    # LM Studio pode embrulhar JSON em ```json ... ``` — extraia.
    content = content.strip()
    if "```" in content:
        content = content.split("```")[1]
        if content.startswith("json"):
            content = content[4:]
        content = content.strip()
    return json.loads(content)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--prompt", required=True, type=Path)
    ap.add_argument("--images", nargs="*", default=[], type=Path)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--model", default=CONFIG["lm_server"]["critic_model"])
    ap.add_argument("--base-url", default=CONFIG["lm_server"]["base_url"])
    args = ap.parse_args()

    prompt = args.prompt.read_text(errors="replace")[:4000]
    images = sorted(args.images)
    print(f"[critic] modelo={args.model} imagens={len(images)}", file=sys.stderr)
    try:
        verdict = call_lm(args.base_url, args.model, prompt, images)
    except (urllib.error.URLError, KeyError, json.JSONDecodeError) as exc:
        print(f"[critic] ERRO (servidor LM Studio ligado?): {exc}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps(verdict, ensure_ascii=False, indent=2))
    else:
        print(f"veredito={verdict.get('veredito')} faltas={len(verdict.get('faltas', []))}")
        print(verdict.get("nota", ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
