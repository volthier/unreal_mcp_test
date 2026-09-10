# Ferramentas MCP — o que temos, o que existe, o que vale trocar

> Pesquisa feita em 10/09, depois de o `comfy-mcp` instalado quebrar (`ComfyCliError`) e de o trabalho no
> Unreal ser feito por **commandlet headless** — que é lento, cego para World Partition e exige permissão
> fora do workspace. Documento de decisão: o que a comunidade usa e o que eu recomendo.

## 1. O que temos hoje (e por que quebra)

| Peça | Versão | Diagnóstico |
|---|---|---|
| `comfy_mcp` (Comfy-Org) | 0.10.0 | embrulha o **`comfy_cli` 1.19.0**, ou seja: **shell-out para `comfy run --workflow`**. É essa camada que morre com `ComfyCliError` — **não é uso errado, é arquitetura**. Caiu até com o arquivo de workflow que havia funcionado minutos antes |
| Unreal | — | eu dirijo por **`UnrealEditor-Cmd -run=pythonscript`**: ~1,5 min por operação, **não vê as células do World Partition** (foi o que me escondeu o cristal que o autor via no editor), sem contexto de UI, e precisa de permissão especial |

## 2. Como a comunidade faz — ComfyUI

| Projeto | O que oferece |
|---|---|
| **artokun/comfyui-mcp** | o mais completo: **178 ferramentas**, **`generate_3d` nativo**, escreve e roda workflows, edita o grafo ao vivo em linguagem natural, roda local/LAN/VPS/Comfy Cloud |
| **halbert04/comfyui-mcp** | 40+ ferramentas: imagem, vídeo, áudio e **3D** |
| **vovchanskuy2 / ComfyUI MCP** | alternativa menor, para LM Studio |

**O ponto que importa:** todos falam **direto com a API HTTP do ComfyUI** (`/prompt`, `/history`, `/view`,
`/upload/image`). O MCP só embrulha isso. Ou seja: o cliente HTTP direto que eu escrevi e versionei
(`Tools/comfy/img2img_style.py`, `Tools/comfy/asset_3d.py`) **não é gambiarra — é o padrão**, sem a camada
que quebra.

## 3. Como a comunidade faz — Unreal (a descoberta importante)

### 3.1 O UE 5.8 traz um MCP **oficial da Epic**, e ele já está no nosso engine

`/Users/Shared/Epic Games/UE_5.8/Engine/Plugins/Experimental/ModelContextProtocol/ModelContextProtocol.uplugin`

- Nome: **Unreal MCP** 1.0 — *"Anthropic MCP (Model Context Protocol) server implementation for Unreal Engine"*
- Módulos: `ModelContextProtocol`, `ModelContextProtocolEngine`, **`ModelContextProtocolEditor`**, `ModelContextProtocolTests`
- Endpoint: `http://127.0.0.1:8000/mcp` — **local, sem autenticação** (as portas 8000 e 30010 estão livres aqui)
- Chamadas rodam **em série na game thread**
- Está **desabilitado no projeto**: o `PloidrekRPG.uproject` não tem seção `[Plugins]`

### 3.2 Alternativa: `unreal-engine-mcp` (PyPI)

**Remote Control API (:30010) + Python dentro do editor**, **162 ferramentas**, testado contra **5.8**, e
**sem plugin C++ para compilar** — usa dois plugins que já vêm na engine (Python Editor Script Plugin e
Remote Control API). Tem uma página de *"armadilhas da API descobertas na marra"* — exatamente a família de
erros que eu tomei nesta sessão (`line_trace_single` devolvendo `None`, World Partition invisível em headless,
operador de UI que não roda em background).

### 3.3 Por que isso muda o meu trabalho aqui

| Hoje (headless) | Com MCP contra o editor aberto |
|---|---|
| ~1,5 min por operação | segundos |
| **não vê o World Partition** → não achava o cristal que o autor via | vê o estado real |
| sem contexto de UI (operadores de Blender/UE falham calados) | contexto real do editor |
| precisa de permissão fora do workspace | só HTTP local |
| eu julgo por relatório de texto | **eu poderia tirar screenshot e julgar com os olhos** |

## 4. Recomendação

1. **Unreal**: habilitar o **Unreal MCP** oficial (uma entrada de plugin no `.uproject` + reiniciar o editor) e,
   se faltar ferramenta, somar o `unreal-engine-mcp`. **O editor precisa ficar aberto** durante o trabalho.
2. **ComfyUI**: **manter o cliente HTTP direto** (funciona, é versionado e é o que os MCPs fazem por baixo).
   Instalar o `artokun/comfyui-mcp` só se quisermos editar grafo em linguagem natural.
3. **Segurança** (o guia de setup insiste, e eu concordo): endpoint **só em loopback** (não tem autenticação),
   partir de uma revisão limpa do source control, **primeiro uma chamada read-only**, depois **uma mudança
   reversível por vez**, e revisar o diff. Agente com acesso destrutivo amplo é o caminho mais rápido para
   perder trabalho.

## 5. Fontes

- Epic — *Unreal MCP in Unreal Editor (UE 5.8)*: https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-mcp-in-unreal-editor
- SEELE — *Unreal Engine 5.8 MCP Setup and Safety Guide*: https://www.seeles.ai/resources/blogs/unreal-engine-mcp-ai-assistant-workflow-guide
- `unreal-engine-mcp` (PyPI): https://pypi.org/project/unreal-engine-mcp/
- `artokun/comfyui-mcp`: https://github.com/artokun/comfyui-mcp
- `halbert04/comfyui-mcp`: https://github.com/halbert04/comfyui-mcp
- `remiphilippe/mcp-unreal` (headless builds e edição de Blueprint): https://github.com/remiphilippe/mcp-unreal
