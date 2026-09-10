# Meshy: Imagem → 3D completo no ComfyUI

Arquivo: **`ComfyUI_Meshy_Pipeline.json`**

Este workflow reproduz a pipeline do Meshy inteira dentro do ComfyUI, usando os **nós oficiais de parceiro** (Partner Nodes) que já vêm no núcleo do seu ComfyUI (versão 0.34.2). Não precisa instalar plugin nenhum.

## O que o fluxo faz

```
Imagem ──► Meshy: Image to Model ──► Meshy: Refine ──► Meshy: Rig ──► Meshy: Animate ──► Save/Preview
              (gera malha 3D)         (melhora mesh     (auto-rig)        (aplica animação)
                                       + textura PBR)
```

Cada etapa é o que você pediu:
- **Vê a imagem** → gera um asset 3D (`MeshyImageToModelNode`)
- **Revisa/melhora/ajusta a mesh** → `MeshyRefineNode` (geometria melhor + textura PBR 4k)
- **Textura** → junto do Refine, com `enable_pbr=true` (mapas metálico/rugosidade/normal)
- **Animações** → `MeshyRigModelNode` + `MeshyAnimateModelNode`

As saídas são salvas como GLB (e FBX) em `ComfyUI/output/`:
- `meshy/animated.glb` — resultado final animado
- `meshy/refined.glb` — malha texturizada (pré-rig)

## Como carregar no ComfyUI

1. O ComfyUI (Desktop) está rodando na porta 8188.
2. **Arraste e solte** o arquivo `ComfyUI_Meshy_Pipeline.json` para dentro do canvas do ComfyUI.
   (Alternativa: `Menu ▾ → Load → Workflow`, ou copie o arquivo para `ComfyUI/user/default/workflows/`.)
3. No nó **LoadImage**, escolha sua imagem de referência.
4. Clique em **Run** (a ordem de execução é automática).

## Ajustes úteis por nó

| Nó | Campo | O que faz |
|---|---|---|
| `Meshy: Image to Model` | `model` | `meshy-7` (mais novo), `meshy-6` ou `latest` |
| `Meshy: Image to Model` | `should_remesh` | `false` = rascunho; `true` = malha otimizada (escolha `topology` e `target_polycount`) |
| `Meshy: Image to Model` | `ultra_mode` | `true` = passada extra de refinamento (precisa de `meshy-7`/`latest`) |
| `Meshy: Refine` | `texture_resolution` | `2k`/`4k`/`8k` |
| `Meshy: Refine` | `texture_prompt` | descreva o estilo da textura (opcional) |
| `Meshy: Rig` | `height_meters` | altura do personagem (ajuda no rig e escala) |
| `Meshy: Animate` | `action_id` | id do movimento (0–696). Lista em https://docs.meshy.ai/en/api/animation-library |

## Requisitos importantes (seja sincero)

- **Login na conta ComfyUI** — os nós Meshy autenticam pela conta do Comfy (não é API key do Meshy) e **consomem créditos do Comfy**. A geração roda na nuvem deles; **não precisa de GPU local**.
- **Auto-rig funciona melhor em humanoides texturizados** com membros e corpo bem definidos. Malhas sem textura, assets não-humanoides ou anatomia ambígua ainda não são bem suportados pelo `MeshyRigModelNode` (limitação documentada pelo próprio Meshy).
- Os nós são **API nodes** (`partner/3d/Meshy`). Eles aparecem na barra de nós se o ComfyUI estiver atualizado e logado.

## Se preferir 100% local (sem ninho/créditos)

Para gerar malha **sem** o Meshy, o [ComfyUI-3D-Pack](https://github.com/Sortium-io/ComfyUI-3D-Pack) faz imagem→mesh localmente (TripoSR, InstantMesh, CRM, Zero123++, LGM...), mas ele **não** faz rig/animação — essa parte ficaria para o Blender (Auto-Rig/Rigify + Mixamo). O pacote oficial `comfyui-ai-gamedev` também traz Hunyuan 3D 2.1 para geração local (você já tem um workflow de Hunyuan 3D no seu ComfyUI).

## Observação de segurança

Não usei nenhuma brecha/exploit nem automação de navegador. O pipeline usa exclusivamente os nós oficiais de parceiro do ComfyUI, que são o jeito suportado pelo Meshy/ComfyUI.
