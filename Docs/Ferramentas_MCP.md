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

## 3.4 **Feito:** o Unreal MCP está registrado nativamente (10/09)

O MCP do editor está **de pé e ligado ao harness**, então o agente chama o editor com as ferramentas nativas
`mcp__unreal__list_toolsets` · `mcp__unreal__describe_toolset` · `mcp__unreal__call_tool` — **sem cliente
intermediário**. Provas: `POST /mcp → 200` (protocolo 2025-06-18), 17 toolsets, e uma captura de viewport tirada
e conferida pelo próprio agente.

**Como ficou registrado:** no perfil do DSH, em `~/.dsh/profiles/web/cordis.patch.yml` (o `web` era o único perfil e
não tinha MCP nenhum; o plugin é o `dsh-mcp-client`):

```yaml
- insert:
    - id: mcp-unreal
      name: '@deepseek-ai/dsh-mcp-client'
      config:
        transport: streamable-http
        serverName: unreal
        url: 'http://127.0.0.1:8000/mcp'
        headers: {}
        toolCallTimeoutMs: 120000
        failOnStartupError: false      # o editor não está sempre ligado
        reconnect: { enabled: true, initialDelayMs: 2000, maxDelayMs: 30000, maxAttempts: 10 }
```

O perfil tem `patchReload: live`, então **não precisou reiniciar nada**: o DSH recarregou o patch, abriu conexão
com o editor (visível no `lsof -iTCP:8000`) e as ferramentas apareceram na sessão.

> **Nota de transparência:** antes disso eu havia escrito um cliente Python próprio
> (`Tools/unreal/mcp_unreal.py`) porque presumi que não alcançava o MCP pelo harness. Era **desnecessário** — o
> certo era registrar o servidor. O arquivo foi removido.

**Pegadinhas do formato, para quem for mexer** (todas resolvidas no caminho): o servidor expõe só 3 ferramentas de
entrada e quer o nome **qualificado** (`Toolset.Tool`, com `toolset_name` separado do `tool_name`); o
`CaptureViewport` **exige** o parâmetro `annotations` explícito; e a imagem volta **dentro do JSON de texto**
(`returnValue.image.data` em base64), não como bloco de imagem do MCP — para olhar, basta decodificar o base64.

**O que ainda é script, e por quê:** o toolset do editor **não** tem importar asset nem criar ator. Essas operações
continuam em `Tools/unreal/*.py`, rodando por commandlet. A alternativa melhor para elas é o **Remote Control API**
(porta 30010, que também está de pé), que executa Python **dentro do editor aberto** — mais rápido e sem a cegueira de
World Partition do commandlet.

## 3.5 **Os três MCPs registrados nativamente** (10/09)

| Servidor | Transporte | O que alcança | Ferramentas nativas |
|---|---|---|---|
| **unreal** | `streamable-http` → `http://127.0.0.1:8000/mcp` | o **editor UE aberto** (plugin oficial `ModelContextProtocol` do UE 5.8) | `mcp__unreal__list_toolsets` · `describe_toolset` · `call_tool` (→ 17 toolsets) |
| **comfy** | `stdio` → `~/.venvs/comfy-mcp/bin/comfy-mcp` | o **ComfyUI** local (porta 8188) | `mcp__comfy__*` (`server_info`, `generate_image`, `search_models`…) |
| **blender** | `stdio` → `~/.venvs/blender-mcp/bin/blender-mcp` | o **Blender aberto**, via addon escutando em `127.0.0.1:9876` | `mcp__blender__*` (`get_object_info`, `get_scene_info`, `disable_telemetry`…) |

Tudo declarado no perfil do DSH (`~/.dsh/profiles/web/cordis.patch.yml`), com `failOnStartupError: false` em
todos: **nenhum dos três pode derrubar o perfil quando estiver desligado**. O perfil tem `patchReload: live`, então
aplicar o patch já reconecta sem reiniciar nada.

### Pegadinhas de cada um (todas encontradas na prática)

**Unreal** — o servidor expõe só 3 ferramentas de entrada e quer o nome **qualificado** (`Toolset.Tool`, com
`toolset_name` separado de `tool_name`); o `CaptureViewport` **exige** `annotations` explícito; a imagem volta
**dentro do JSON de texto** (`returnValue.image.data` em base64), não como bloco de imagem do MCP.

**ComfyUI** — o `comfy-mcp` embrulha o **CLI `comfy`** por baixo, então o env precisa de `COMFY_BIN` apontando
para o executável. E é ele que quebra em `run_workflow` (`ComfyCliError`): para gerar imagem o caminho direto pela
**API HTTP do ComfyUI** (`Tools/comfy/*.py`) continua sendo o mais confiável.

**Blender** — arquitetura em **duas partes**: um addon *dentro* do Blender (`BLENDERMCP`, escutando na 9876) e um
servidor MCP em stdio que faz a ponte. Consequências: (1) o addon **só roda com o Blender em modo gráfico** — em
`blender -b` ele mesmo recusa ("cannot start server in background mode"); (2) o autor precisa clicar em
**"Connect to Claude"** no painel N do Blender; (3) servidor e addon têm **versões casadas** — o addon instalado
estava mais velho que o servidor, e o conserto é o comando que o próprio servidor sugere:
`~/.venvs/blender-mcp/bin/blender-mcp install-addon`, e depois desabilitar/habilitar o addon ou reiniciar o Blender.

> **Ainda é script:** o toolset do editor do Unreal **não** importa asset nem cria ator — essas operações continuam
> em `Tools/unreal/*.py` por commandlet. O caminho melhor para elas é o **Remote Control API (porta 30010)**, que
> executa Python **dentro do editor aberto**.

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
