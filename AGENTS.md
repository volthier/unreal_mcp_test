# AGENTS.md — o contrato de quem age neste repositório

> Este arquivo é a **porta de entrada** de qualquer agente ou bot. Ele não substitui a documentação: ele diz
> **onde ela está** e **qual é a regra**. Se você é um agente começando agora, leia isto e depois os dois
> documentos apontados no passo 1.

## A regra que manda

**MCP sempre em primeiro lugar.** Script só existe onde a capacidade não existe no MCP, e mesmo assim com
exceção registrada. Motivo: script é interagir por fora do padrão, e isso multiplica o risco de
incompatibilidade com o que a engine (Unreal 5.8+, Blender, ComfyUI) garante.

Ordem de decisão, nesta sequência exata:

| # | Caminho | Quando |
|---|---|---|
| 1 | **Ferramenta dedicada do MCP** | sempre que existir. Ver `Docs/mcp/CAPACIDADES.md` |
| 2 | **Ferramenta de execução de código do próprio MCP** | o Unreal tem `ProgrammaticToolset.execute_tool_script`; o Blender tem `execute_blender_code`. Rodar código **por dentro** do MCP continua sendo o caminho padrão |
| 3 | **Exceção registrada** | só quando 1 e 2 não cobrem. Precisa de ID em `Docs/mcp/EXCECOES.md`, motivo, risco e gatilho de revisão |

**Proibido:** escrever cliente MCP próprio (HTTP/JSON-RPC na mão). Não expõe schema, não lista toolsets,
ignora reconexão e timeout — e engessa o MCP no que o script já sabia fazer. Isso já aconteceu neste
repositório: 17 scripts faziam isso, e foi por causa deles que a capacidade real do MCP ficou escondida.

## Comece por aqui (o rito de entrada)

1. **`Docs/mcp/CAPACIDADES.md`** — o que é possível hoje, ferramenta por ferramenta, com quando usar e qual
   evidência produzir. **`Docs/mcp/CAPACIDADES.yml`** é o mesmo em formato de máquina.
2. **`Docs/mcp/REGISTRO.md`** — como os MCPs são declarados (label + host/porta ou comando) neste projeto e
   em cada agente. **Não se cria código para isso.**
3. **`Docs/mcp/ARMADILHAS.md`** — os erros que já custaram rodadas, com sintoma, causa e conserto. Ler antes
   de agir é mais barato do que redescobrir.
4. **`Docs/mcp/EXCECOES.md`** — o que ainda é script, e por quê.

## Regras de trabalho (do `Docs/ENGINEERING_PLAYBOOK_UNREAL.md`)

- **Um passo reversível por vez**, com escopo único. Sem "limpeza geral" junto com feature.
- **Evidência obrigatória**: número medido **ou** captura. Afirmação sem evidência é opinião.
- **Medir vence olhar** quando o assunto é geometria, posição ou escala.
- **Regra de jogo em C++; UI não aplica dano.** Sem singleton global solto (a sessão é um subsystem).
- **Nada de IP de terceiros dentro de `Content/`.**
- **Documento atualizado no mesmo PR** da mudança.

## Definição de pronto (DoD)

Uma tarefa só fecha quando: (1) o teste de automação passa; (2) há evidência anexada (número ou captura);
(3) a documentação afetada foi atualizada; (4) nenhum script novo entrou sem exceção registrada.

## Onde vive cada coisa

| Caminho | O que é |
|---|---|
| `Source/PloidrekRPG/` | o jogo em C++ (8 mil linhas, 14 testes de automação) |
| `Content/` | assets do jogo — **sem IP de terceiros** |
| `Data/` | as tabelas de origem (CSV) que alimentam as DataTables |
| `Art/` | arte gerada (fora do git) e referências |
| `Tools/` | **exceções** ao MCP (pipeline do engine) — ver `Docs/mcp/EXCECOES.md` |
| `Docs/` | o GDD, o playbook, a bíblia do mundo, os planos e `Docs/mcp/` |
