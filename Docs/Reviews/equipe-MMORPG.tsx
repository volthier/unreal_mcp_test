import { H1, H2, Text, useHostTheme, useState } from 'cursor/canvas';

const roles = [
 ['P0','Analista de requisitos / Product Owner','GDD v3 e backlog com critérios de aceite.','Consolidar decisões vigentes; definir jogadores simultâneos por zona, hardware, latência e escopo do primeiro incremento.','Requisito com ID, dono, dependências e teste de aceite.'],
 ['P0','Tech Lead / Integrador de agentes','AGENTS.md e playbook definem limites gerais.','Ownership por área, contratos versionados, revisão independente, isolamento de código e exclusividade do editor MCP.','Uma tarefa integrada com evidência, revisão e rollback.'],
 ['P0','DevOps / Build Engineer','Playbook exige CI; nenhum workflow .github versionado encontrado.','Build reproduzível, smoke, artefatos, gates de merge, armazenamento de binários e política de locks.','Checkout limpo produz build e relatório de testes.'],
 ['P0','QA / Automação','15 testes declarados em 4 arquivos; execução não feita nesta análise.','Testes cliente-servidor, falha de persistência, reconexão e smoke de build empacotado.','Relatório ligado ao commit e à versão do build; zero testes não conta como sucesso.'],
 ['P1','Backend / Persistência','RunnerSession grava contas e personagens em TSV local; integração EOS presente.','Identidade estável, persistência autoritativa, migrações, idempotência, inventário e economia transacional.','Personagem persiste após reinício e pedidos repetidos não duplicam recompensas.'],
 ['P1','Gameplay C++ / Networking','GAS, replicação de atributos e verificações de autoridade presentes.','Dedicated server, contratos de replicação, relevância, reconexão e orçamento de rede.','Dois clientes validam combate e persistência; depois teste de carga progressivo.'],
 ['P1','Game / Systems Designer','GDD contém combate, economia e metas de balanceamento.','Converter regras em dados verificáveis; resolver ambiguidades e simular fontes/sumidouros da economia.','Cenários de TTK, progressão e economia com faixas de aceite.'],
 ['P1','Concept Artist / Art Director','Bíblia do mundo, paleta e prompts de arte.','Briefs aprovados, vistas, materiais, escala e critérios de leitura no jogo.','Pacote de conceito aprovado e rastreável ao asset.'],
 ['P1','3D Artist','Pipeline Blender → Unreal documentado.','Contrato atual de topologia, UV, skeleton, colisão, LOD e proveniência; restaurabilidade das fontes.','Asset importado, salvo e medido no hardware alvo.'],
 ['P1','Technical Artist / Rigger / Animator','Há documentação de animação e testes de corpo.','Validar deformação, retarget, materiais, Niagara e orçamento de animação; revisar referências antigas VoltStriker.','Locomoção e combate sem deformações críticas, com custos medidos.'],
 ['P1','UI/UX + UI Engineer','Menu implementado em C++; RunnerMenuWidget.cpp tem 2.571 linhas.','Dividir por fluxos e estado; biblioteca visual, acessibilidade, localização e estados de rede.','Fluxo login → criação → mundo validado nos dispositivos alvo.'],
 ['P1','World / Level Designer','Documentação de mundo, áreas e conteúdo procedural.','Contratos por setor, navegação, densidade de encontros, streaming e HLOD medidos.','Percurso entre áreas com orçamento de memória e frame time.'],
 ['P1','Dashboard Manager / Producer','Backlog em Markdown.','Fonte única de tarefas; painel derivado de testes e builds, com bloqueios, donos e evidência.','Cada item concluído aponta para requisito, mudança, revisão e execução.'],
 ['P1','Security / Abuse Engineer','Separação de segredos no gitignore; login local explicitamente de desenvolvimento.','Revisar confiança no cliente, autorização, replay, duplicação de itens e acesso de agentes.','Cenários de abuso impedidos e auditáveis.'],
 ['P2','SRE / LiveOps / Dados','Operação online não validada nesta análise.','SLIs/SLOs, alertas, backup/restauração, rollback, telemetria e custo por jogador.','Ensaio de incidente e restauração com tempos medidos.'],
 ['P2','Narrativa / Quest Design / Áudio / Localização / Comunidade','Campanha e bestiário documentados.','Pipeline de quests testáveis, áudio espacial, conteúdo localizado e ferramentas de moderação.','Uma missão completa, compreensível, acessível e observável.']
];

export default function TeamAudit() {
 const theme = useHostTheme();
 const [priority, setPriority] = useState('Todas');
 const shown = roles.filter(r => priority === 'Todas' || r[0] === priority);
 return <main style={{background:theme.bg.editor,color:theme.text.primary,padding:24,fontFamily:'sans-serif',lineHeight:1.5}}>
  <H1>Avaliação da equipe de agentes · MMORPG</H1>
  <Text>Análise estática local · 12 setembro 2026. Papéis são responsabilidades, não agentes já configurados.</Text>
  <H2>Boa documentação; integração ainda sem garantias executáveis</H2>
  <p>Prioridade: resolver contratos e governança, automatizar a integração e provar uma pequena experiência online antes de aumentar o conteúdo.</p>
  <p style={{color:theme.text.secondary}}>Evidências: 15 testes declarados / 4 arquivos; menu com 2.571 linhas; nenhum .github workflow, CODEOWNERS, .gitattributes ou Server.Target.cs versionado encontrado. Não foram executados build, testes, auditoria de assets ou inspeção do serviço remoto.</p>
  <label>Prioridade <select value={priority} onChange={e=>setPriority(e.target.value)} style={{color:theme.text.primary,background:theme.bg.elevated}}>{['Todas','P0','P1','P2'].map(p=><option key={p}>{p}</option>)}</select></label>
  <p>P0: antes de ampliar trabalho simultâneo. P1: primeira experiência online integrada. P2: antes de operar com público; pode exigir preparação antecipada.</p>
  <div style={{overflowX:'auto'}}><table style={{width:'100%',borderCollapse:'collapse',fontSize:13}}>
   <thead><tr>{['Prioridade','Responsabilidade','Evidência atual','O que falta','Entrega verificável'].map(h=><th key={h} style={{textAlign:'left',padding:10,borderBottom:`1px solid ${theme.stroke.primary}`}}>{h}</th>)}</tr></thead>
   <tbody>{shown.map(row=><tr key={row[1]}>{row.map((cell,i)=><td key={i} style={{verticalAlign:'top',padding:10,borderBottom:`1px solid ${theme.stroke.secondary}`,fontWeight:i===1?600:400}}>{cell}</td>)}</tr>)}</tbody>
  </table></div>
  <H2>Contrato mínimo de uma tarefa</H2>
  <p>ID e objetivo → entradas e versão → dono e revisor → arquivos/assets permitidos → contratos afetados → aceite mensurável → ferramenta MCP → evidência → rollback → entrega ao próximo responsável.</p>
  <p>Um escritor por asset e uma operação mutável por editor compartilhado. Worktrees isolam código; não isolam um editor apontado ao mesmo projeto. Revisões independentes e limite de trabalho simultâneo reduzem retrabalho.</p>
  <H2>Divergências a resolver</H2>
  <ul><li>ORG-011 restringe edição de assets a humanos, enquanto AGENTS.md prevê operação por MCP.</li><li>AGENTS.md informa 14 testes; a fonte declara 15.</li><li>PROC-006 aparece duas vezes no backlog; documentos de pipeline ainda citam referências antigas.</li><li>EOSConfigurado retorna sucesso com aviso quando EOS está ausente; isso não comprova login online.</li></ul>
  <H2>Fontes e limites</H2>
  <p>Fontes locais: AGENTS.md; Docs/mcp/*; Docs/ENGINEERING_PLAYBOOK_UNREAL.md; Docs/BACKLOG_Tecnico.md; Docs/PloidrekRPG_GDD_v3.md; Docs/AssetPipeline3D.md; Source/PloidrekRPG/Private/Session/RunnerSession.cpp; Source/PloidrekRPG/Private/Tests/*; inventário Git. Ausência no checkout não comprova ausência de infraestrutura externa.</p>
  <p>Referências: <a href="https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-dedicated-servers-in-unreal-engine">Epic: dedicated servers</a> · <a href="https://dev.epicgames.com/documentation/unreal-engine/replication-graph-in-unreal-engine">Epic: Replication Graph</a> · <a href="https://git-scm.com/docs/git-worktree">Git: worktrees</a>.</p>
 </main>;
}
