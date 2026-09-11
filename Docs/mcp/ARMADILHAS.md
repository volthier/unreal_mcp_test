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

## 8. Sessão MCP expirada parece "ferramenta que não existe"

- **Sintoma:** `Unknown session id '...' for 'tools/call'` em toda chamada.
- **Causa:** o client mantém o id de uma sessão que morreu (editor reiniciado, por exemplo).
- **Conserto:** forçar a recriação do cliente (mudança pequena na entrada do `cordis.patch.yml`). Detalhe em
  `REGISTRO.md` §4. **Nunca** contornar com cliente próprio: foi assim que este projeto passou a acreditar que
  faltavam ferramentas que sempre existiram.
