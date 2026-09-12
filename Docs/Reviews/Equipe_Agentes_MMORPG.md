# Avaliação da equipe de agentes — MMORPG

Data: 2026-09-12. Análise aprovada pelo responsável pelo projeto para versionamento.
O título é neutro: nomes técnicos existentes não representam aprovação da identidade comercial.

## Escopo e evidência

Análise estática de AGENTS.md, Docs/mcp/, playbook, backlog, GDD, pipelines de arte,
Source/, configuração e inventário Git. Não foram executados build, testes ou inspeção
de assets no editor. Ausência no checkout não demonstra ausência de infraestrutura externa.

- 15 testes declarados em quatro arquivos de Source/PloidrekRPG/Private/Tests/.
- RunnerMenuWidget.cpp: 2.571 linhas no momento da análise.
- Nenhum workflow .github, CODEOWNERS, .gitattributes ou Server.Target.cs versionado encontrado.
- RunnerSession.cpp persiste contas e personagens em TSV local; há integração EOS.
- EOSConfigurado retorna sucesso com aviso se o subsistema EOS estiver ausente; não prova login online.
- A criação de personagem ignora o retorno de SaveCharacters(); a identidade EOS usa nickname quando disponível.

## Diagnóstico

Há boa base documental, GAS, replicação de atributos e verificações de autoridade.
Faltam garantias executáveis de integração entre agentes e demonstração da experiência
online com persistência autoritativa. Documentação e testes declarados não comprovam
prontidão operacional de um MMORPG.

## Responsabilidades recomendadas

- Produto e análise de requisitos: decisões vigentes, IDs, dependências e aceite mensurável.
- Tech Lead / integrador: arquitetura, contratos, ownership e revisão independente.
- Gameplay C++ / networking: simulação autoritativa, replicação, relevância e reconexão.
- Backend / persistência: identidade estável, migrações, inventário, economia e idempotência.
- Game / systems designer: combate, progressão e fontes/sumidouros da economia.
- Concept / direção de arte: linguagem visual, vistas, escala e materiais aprovados.
- 3D artist: topologia, UV, texturas, skeleton, colisão e LOD conforme contrato.
- Technical artist / rigging / animação: integração, deformação e custos medidos no jogo.
- World / level designer: setores, navegação, encontros, streaming e HLOD.
- UI/UX e UI engineer: fluxos, estado, acessibilidade, localização e dispositivos alvo.
- QA / automação: regressão, multiplayer, falhas de persistência e performance.
- DevOps / build: builds reproduzíveis, CI, distribuição, artefatos e gates de integração.
- Dashboard manager / produção: painel derivado de tarefas, builds e evidências verificáveis.
- Segurança / abuso: autorização, confiança no cliente, replay e duplicação de itens.
- SRE / LiveOps / dados: observabilidade, restauração, incidentes, rollback e custos.
- Narrativa, quests, áudio, localização e comunidade/moderação: produção e operação de conteúdo.

São responsabilidades; não é necessário criar um agente permanente para cada uma agora.

## Contrato de colaboração

Cada tarefa deve informar ID, objetivo, entradas e versões, dono, revisor, escopo de
arquivos/assets, dependências, contratos afetados, aceite, ferramenta MCP, evidência,
rollback e destinatário da entrega. Separar implementação de revisão.

Isolar alterações de código em branches/worktrees quando houver concorrência.
Aplicar exclusividade por asset e coordenação das operações mutáveis em editor compartilhado.
Worktrees não isolam um editor apontado para o mesmo checkout. Definir limite de trabalho em andamento.

O painel deve ligar requisito → mudança → revisão → build/teste → evidência.
Uma resposta de agente dizendo que terminou não basta para concluir a tarefa.

## Pendências documentais e identidade autoral

- Resolver ORG-011, que restringe assets a humanos, versus AGENTS.md, que prevê operação MCP.
- Atualizar a contagem de 14 testes em AGENTS.md para a contagem vigente, após validar registro no runtime.
- Consolidar os registros duplicados de PROC-006 e revisar referências antigas de pipeline.
- IP-007 registra pendência sobre o nome: não tratar o identificador técnico como marca aprovada.
- IP-005/006 exigem proveniência e revisão de licenças; IP-008 restringe referências de estudo.
- Direção de arte, narrativa e responsável pelo produto devem assumir aprovação de originalidade
  e proveniência, com apoio especializado quando necessário.

Esta análise não verificou os assets para concluir conformidade e não estabeleceu ocorrência de plágio.

## Sequência de implementação proposta

1. Resolver regras conflitantes; definir donos, revisores e contratos de tarefa.
2. Implantar build reproduzível, smoke tests e evidências ligadas ao commit; zero testes não é sucesso.
3. Provar dois clientes → servidor dedicado → combate → recompensa → desconexão → personagem restaurado.
4. Medir carga progressiva, latência, memória e frame time antes de ampliar mundo e produção.
5. Preparar backup/restauração, observabilidade e resposta a incidentes antes de operação pública.

As propostas não foram implementadas por este commit. A DoD do jogo permanece pendente
de execução para mudanças futuras de código/assets; este registro é uma entrega documental.

## Painel e referências

[Snapshot do painel](equipe-MMORPG.tsx): cópia versionada do canvas produzido na análise,
dependente de cursor/canvas. Não integra o build Unreal nem constitui aplicação standalone.

- [Epic — Dedicated servers](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-dedicated-servers-in-unreal-engine)
- [Epic — Replication Graph](https://dev.epicgames.com/documentation/unreal-engine/replication-graph-in-unreal-engine)
- [Git — Worktrees](https://git-scm.com/docs/git-worktree)
