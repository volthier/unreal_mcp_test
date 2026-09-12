# Armadilhas — sintoma, causa e conserto

> Todas foram **medidas neste projeto**. Leia antes de agir: cada uma destas já custou rodada inteira.

## 1. O editor carrega o módulo C++ no start

- **Sintoma:** código novo compilado, e o editor continua com o comportamento antigo.
- **Causa:** o editor carrega o módulo quando sobe; compilar com ele aberto **não** injeta o código novo.
- **Conserto:** fechar e reabrir o editor. Alternativa pelo próprio MCP: `LiveCodingToolset.CompileLiveCoding`
  (`CompileLiveCoding`).

## 2. Asset **alterado** não recarrega no viewport; asset **novo** aparece

- **Sintoma:** você corrige um material em disco, captura, e a imagem continua com o defeito antigo.
- **Causa:** o editor mantém o asset carregado em memória.
- **Conserto:** reabrir o asset/editor antes de capturar. E **declarar** quando a evidência é de estado velho.

## 3. O manifesto decide qual dylib carrega

- **Sintoma:** editor abre, mas commandlets rodam **zero testes** e o `pyscript` não executa — **sem erro** no log.
- **Causa:** `Binaries/Mac/UnrealEditor.modules` nomeia o arquivo do módulo; se a consolidação deixa o nome
  apontando para um arquivo inexistente, o módulo não carrega. Com o editor aberto, o UBT só escreve iterações
  numeradas (`-0015`, `-0017`…).
- **Conserto:** `Tools/unreal/consolidar_modulo.sh` mantém o arquivo com o **nome que o manifesto pede**.

## 4. Commandlet headless trava no lock do projeto

- **Sintoma:** `UnrealEditor-Cmd` sai com código 1, log só com startup, zero `LogPython`.
- **Causa:** o editor aberto segura o lock do projeto. E uma segunda instância do editor **não termina de inicializar**
  (fica escutando a porta MCP, mas não responde `initialize`).
- **Conserto:** trabalhar com o editor fechado, ou agir pelo MCP (que é o caminho preferido, de todo modo).

## 5. A MCP cria asset na memória — sem salvar, não existe

- **Sintoma:** você cria um grafo de PCG pela MCP, ele funciona na sessão, e no dia seguinte não está lá.
- **Causa:** autoria pela MCP vive na memória do editor até um save.
- **Conserto:** salvar pelo próprio MCP — `editor_toolset.toolsets.asset.AssetTools.save_assets`.

## 6. `unreal.Rotator(a, b, c)` é `(roll, pitch, yaw)`

- **Sintoma:** atores nascem **deitados** (as torres da cidade saíram como caixas horizontais).
- **Causa:** o terceiro argumento é o yaw; passar o yaw em segundo aplica **pitch**.
- **Conserto:** argumentos nomeados (`unreal.Rotator(roll=0, pitch=0, yaw=90)`) ou a ordem correta.

## 7. Medir vence olhar

- **Sintoma:** "a malha está errada" — e a malha estava certa; o **posicionamento** é que estava.
- **Causa:** captura de tela é ambígua para escala e orientação.
- **Conserto:** medir com `ActorTools.get_actor_bounds` (MCP) ou `StaticMeshTools.get_bounds`; foi o que separou
  problema de import de problema de posicionamento.

## 7d. Entrada sintética NÃO alcança a janela de PIE — medido com o log

**Sintoma:** `Click` e `PressKey` devolvem sucesso, o botão do jogo fica no estado pressionado e **nada acontece**.
O log do jogo não registra **nenhuma** reação — nem a tentativa de login, nem o status do menu.

**O que já foi testado, e o resultado de cada um:**

| Tentativa | Resultado |
|---|---|
| `Click` simples, dois e três cliques | nada |
| `Hover` antes do clique | nada |
| Clique em widget inerte antes (soltar captura de mouse) | nada |
| Sessão de PIE nova | nada |
| `playMode: InEditorFloating` (a janela do jogo vira janela própria, com refs novos) | nada |
| `PressKey` com Enter, Tab e setas (depois de o widget TOMAR FOCO de teclado) | nada |

**A prova que fecha o caso:** o log. O menu registra quando abre (`AMenuGameMode: menu de entrada aberto`) e não
registra **absolutamente nada** depois do ENTER. Se a tecla chegasse ao widget, o estado do login mudaria e o
log teria a linha — o handler está ligado e testado, o foco foi implementado, e mesmo assim a tecla não chega.

**Conclusão:** a síntese de entrada do inspetor vai para a aplicação Slate do **editor**, e o viewport de PIE (uma
janela filha) não a recebe nesta configuração. **É limitação de ferramenta, não do jogo** — e é por isso que
`Click`/`PressKey` não servem para validar fluxo de UI aqui.

**O que usar no lugar:** (1) os testes de automação, que exercitam a regra sem depender de entrada; (2)
`StartPIE` com `startTransform`, que põe o pawn onde se quer e permite capturar da câmera dele — foi assim que a
entrada no mundo aberto ficou verificada; (3) captura de tela pelo engine (`CaptureEditorImage`), que pega a janela
do editor inteira.

**E uma regra de conduta que isto me ensinou:** não capturar a tela do sistema para contornar isso. Eu tentei uma
vez, a captura pegou o navegador do usuário, e isso é invasão de privacidade — não importa o ganho de verificação.

## 7b. O clique sintético do Slate nem sempre chega ao jogo

- **Sintoma:** `SlateInspectorToolset.Click` devolve `true`, o botão do jogo fica no estado **pressionado** e a tela
  **não avança**. O mesmo vale para `PressKey` (testado com Enter, Tab e setas).
- **Onde foi medido:** rodada 22 e 23, na tela de login do `MainMenuMap`. Na **rodada 18 o mesmo caminho
  funcionou** (clique em "Criar conta nova (local)" levou à tela de conta, e o fluxo até o chassi foi percorrido).
  Ou seja: não é um defeito do jogo, e sim do caminho de entrada sintética.
- **O que já foi tentado, sem sucesso:** dois e três cliques no mesmo ref; `Hover` antes do clique; clique num
  widget inerte antes (para soltar captura presa de mouse); sessão de PIE nova; `playMode: PlayMode_InEditorFloating`
  (com a janela do jogo aparecendo como janela própria no inspetor, e refs novos).
- **O que funciona como alternativa:** `EditorAppToolset.StartPIE` aceita **`startTransform`** - dá para colocar o
  pawn do jogador em qualquer ponto do mapa e capturar **da câmera dele**. Foi assim que a entrada no mundo aberto
  foi verificada (jogador na praça, com os mostradores de cristal em volta).
- **Não declarar causa sem prova.** A hipótese mais provável é foco/roteamento de entrada do PIE no macOS, mas
  isso **não foi medido**. Se um humano clicar com o mouse e o jogo responder, o problema é do caminho sintético;
  se também não responder, é ambiente. Essa pergunta está aberta.

## 7c. Niagara: as formas exatas da API (todas descobertas na prática)

Trabalhar o FX pelo Niagara MCP funciona, mas cada ferramenta tem uma forma de referência própria — e todas
foram descobertas **pelas mensagens de erro**, uma a uma. Fica registrado para não repetir:

| Ferramenta | Argumento de referência | Extra obrigatório |
|---|---|---|
| `GetSystemSummary` / `AddEmitter` / `AddRenderer` | `system` | `AddEmitter`: + `templateEmitter` e `emitterName` |
| `GetEmitterTopology` | `emitterRef` | — |
| `GetRendererData` | `rendererRef` | — |
| `SetRendererData` | `renderer` (não `rendererRef`!) | + `rendererData` e `propertyValues` |
| `ActorTools.add_component` | `owner` | + `name` e `component_type` |
| `NiagaraToolset_Component.SetSystem` | `niagaraComponent` | + `system` e `bResetExistingOverrideParameters` |

**E a descoberta que mais importa:** *duplicar um **sistema** da engine (`AssetTools.duplicate` de um
`FountainLightweight`) traz o sistema **vazio** — `scriptName: NONE` nos quatro estágios, `modules: []`,
`renderers: []`. Já **adicionar um emissor** de template (`AddEmitter` com
`/Niagara/DefaultAssets/Templates/Emitters/SingleLoopingParticle`) traz o emissor **completo**, com
`NiagaraSpriteRendererProperties` e `EmitterSpawnScript`.*

Onde estão os templates da engine (acessíveis pelo MCP): `/Niagara/DefaultAssets/Templates` — 47 assets, com
**7 sistemas** (`FountainLightweight`, `MinimalLightweight`, `DirectionalBurst`, `SimpleExplosion`…) e
**14 emissores** (`SingleLoopingParticle`, `HangingParticulates`, `RecycleParticlesInView`…).

**Pendência conhecida:** trocar o material do renderer por `SetRendererData` falha com *"could not convert
incoming function input params Json to a UStruct"* — o campo `rendererData` espera um struct específico, e
ainda não encontrei a forma certa. O material do sprite continua o padrão do template.

**E o aviso de verificação:** Niagara **não aparece** no viewport do editor parado. Para conferir FX, capturar
**em PIE**. Foi por isso (e por ler a imagem em vez da topologia) que eu afirmei duas vezes que partículas
estavam desenhando quando não estavam.

## 8. Sessão MCP expirada parece "ferramenta que não existe"

- **Sintoma:** `Unknown session id '...' for 'tools/call'` em toda chamada.
- **Causa:** o client mantém o id de uma sessão que morreu (editor reiniciado, por exemplo).
- **Conserto:** forçar a recriação do cliente (mudança pequena na entrada do `cordis.patch.yml`). Detalhe em
  `REGISTRO.md` §4. **Nunca** contornar com cliente próprio: foi assim que este projeto passou a acreditar que
  faltavam ferramentas que sempre existiram.
