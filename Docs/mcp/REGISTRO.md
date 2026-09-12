# Registro dos MCPs — como se declara, para cada agente

> **Regra:** MCP **se declara** (nome/label + host/porta ou comando). Não se escreve código para falar com
> MCP. Este documento é a fonte para registrar os servidores neste projeto e em qualquer agente novo.

## 1. Os três servidores deste projeto

| Label | O que é | Transporte | Endereço | Requisito |
|---|---|---|---|---|
| `unreal` / `unreal-mcp` | MCP **oficial da Epic**, roda **dentro do editor** (plugin `ModelContextProtocol`) | streamable-http | `http://127.0.0.1:8000/mcp` | editor aberto; **56 toolsets / 867 ferramentas** em 2026-09-12 |
| `blender` | ponte até o addon `BLENDERMCP`, que escuta no Blender gráfico | stdio | `BLENDER_HOST=127.0.0.1`, `BLENDER_PORT=9876` | addon ligado (painel do Blender) |
| `comfy` / `comfy-mcp` | servidor `comfy-mcp`, que embrulha o CLI `comfy` | stdio | comando + `COMFY_BIN` | ComfyUI em `127.0.0.1:8188` para executar |

A porta do MCP do Unreal é configurável: `Config/DefaultEditorPerProjectUserSettings.ini` →
`[/Script/ModelContextProtocolEngine.ModelContextProtocolSettings]` → `ServerPortNumber=8000`.

## 2. Onde se declara, por agente

### 2.1 DSH

Arquivo: `~/.dsh/profiles/web/cordis.patch.yml` (recarregado a quente). Três entradas `insert`, uma por
servidor, com `id`, `name: '@deepseek-ai/dsh-mcp-client'` e o `config` do transporte:

```yaml
- insert:
    - id: mcp-unreal
      name: '@deepseek-ai/dsh-mcp-client'
      config:
        transport: streamable-http
        serverName: unreal
        url: 'http://127.0.0.1:8000/mcp'
        headers: {}
        toolCallTimeoutMs: 180000
        failOnStartupError: false      # o editor não está sempre ligado
        reconnect: { enabled: true, initialDelayMs: 2000, maxDelayMs: 30000, maxAttempts: 10 }
```

As entradas de Blender e Comfy são `transport: stdio`, com `command`, `args` e `env`
(`BLENDER_HOST`/`BLENDER_PORT`, `COMFY_BIN`). `failOnStartupError: false` nos três: se o app não estiver
aberto, o perfil não pode falhar ao carregar.

### 2.2 Claude Code (e o `.mcp.json` do projeto)

Arquivo: `.mcp.json` na raiz do projeto (já existe, com os três):

```json
{ "mcpServers": {
    "unreal-mcp": { "type": "http", "url": "http://127.0.0.1:8000/mcp" },
    "blender":    { "command": ".../uvx", "args": ["--python","3.11","blender-mcp"],
                    "env": { "BLENDER_HOST": "localhost", "BLENDER_PORT": "9876" } },
    "comfy-mcp":  { "command": "~/.venvs/comfy-mcp/bin/comfy-mcp",
                    "env": { "COMFY_BIN": "~/.venvs/comfy-mcp/bin/comfy" } } } }
```

As permissões ficam em `.claude/settings.local.json` (`mcp__unreal-mcp__call_tool`,
`mcp__unreal-mcp__describe_toolset`, …).

### 2.3 Cursor e outros

Mesmo conteúdo, no arquivo do agente (`.cursor/mcp.json` e equivalentes): **label + url** para o Unreal,
**label + command/env** para Blender e Comfy. Se um agente novo entrar, ele lê este documento e registra —
não se cria nada em Python.

### 2.4 Codex — lacuna medida em 2026-09-12

Inspecionados `.codex/config.toml` do projeto e `~/.codex/config.toml`: ambos declaram Blender e Unreal;
nenhum declara Comfy. `.mcp.json` tem os três, mas sua presença não comprova carregamento no Codex.
O catálogo da sessão expõe 28 ferramentas Blender e três meta-tools Unreal; zero ferramentas Comfy.

Entrada que falta no registro Codex (proposta, não aplicada nesta auditoria):

```toml
[mcp_servers.comfy-mcp]
command = "/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp"

[mcp_servers.comfy-mcp.env]
COMFY_BIN = "/Users/volthier/.venvs/comfy-mcp/bin/comfy"
```

Depois de carregar a configuração, confirmar `server_info` e a lista real de ferramentas; então validar
um workflow local controlado antes de retirar EXC-001. Não escrever cliente MCP para contornar a falta.
Timeouts efetivos e reconexão devem ser registrados por cliente, não copiados como promessa universal.

O arquivo DSH inspecionado contém os três: Unreal timeout 180012 ms, Comfy 900000 ms e Blender 300000 ms;
`failOnStartupError: false` nos três. Isso prova configuração, não saúde da sessão DSH em execução.

## 3. Como o agente usa (e o que ele NÃO faz)

- Descobrir capacidade: `list_toolsets` e depois `describe_toolset <nome>` (é assim que este projeto
  levantou os 59 toolsets — e descobriu que material, import, cena, salvar e testes **já existem**).
- Chamar: `call_tool` com `toolset_name` + `tool_name` + `arguments`.
- **Não** abrir socket por conta própria, **não** falar JSON-RPC na mão, **não** envolver o MCP em script.

## 4. Problemas conhecidos de sessão (e o conserto)

O transporte `streamable-http` mantém um **id de sessão**. Se o editor reiniciar (ou a sessão expirar), o
client pode continuar mandando o id antigo e o servidor responde:

```text
Unknown session id '...' for 'tools/call'
```

Conserto verificado: forçar a **recriação do cliente** — uma mudança pequena na entrada do servidor no
`cordis.patch.yml` faz o loader recriar a conexão e a sessão nova é negociada (`initialize` de novo).
Depois disso as chamadas voltam. Tratar como sintoma de infraestrutura: **nunca** contornar escrevendo um
cliente próprio.
