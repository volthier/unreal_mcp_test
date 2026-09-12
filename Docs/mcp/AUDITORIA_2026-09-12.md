# Auditoria dos MCPs — Unreal, Blender e ComfyUI

Data: 2026-09-12. Escopo: registros de clientes, catálogo integral, schemas e consultas de saúde
sem alterar assets, habilitar provedores, gastar créditos ou iniciar geração.

## Conclusão

O catálogo anterior era incompleto e misturava disponibilidade, configuração e testes.
Unreal e Blender responderam por MCP. Comfy está instalado e declarado em DSH/.mcp.json,
mas não está declarado no Codex nem expõe ferramentas nesta sessão. Não é correto afirmar
que os três caminhos de execução foram validados.

## Evidência por servidor

- **Unreal:** list_toolsets retornou 56 entradas; describe_toolset foi chamado sequencialmente
  para todas as 56, com 867 ferramentas e nenhum erro de recuperação de schema. O número 59
  dos documentos não corresponde ao catálogo atual. Não inferir perda de três capacidades
  sem um snapshot anterior equivalente para comparar nomes e schemas.
- **Execução Unreal:** get_execution_environment e execute_tool_script consultaram o mapa
  /Game/Maps/MainMenuMap e confirmaram PIE desligado. O ambiente é restrito à orquestração
  de ferramentas, com seis módulos seguros; não aceita Python Unreal arbitrário.
- **Validação Unreal da rodada anterior:** 15 testes Runner executados com sucesso pelo MCP,
  com avisos preservados em Docs/Reviews/Login_Unreal_2026-09-12_tests.json. Isso não testa
  cada uma das 867 ferramentas. Captura, PIE, configurações, logs e Slate também responderam.
- **Blender:** 28 ferramentas expostas. get_addon_status: Blender 5.2.0 LTS, addon 1.6,
  protocolo atual/esperado 5, up_to_date=true. get_scene_info: três objetos, dois materiais.
  execute_blender_code com bpy leu a cena sem mutação e confirmou ausência de arquivo carregado.
- **Blender opcional:** os cinco status consultados retornaram disabled: PolyHaven, Sketchfab,
  Poly Pizza, Hyper3D Rodin, Hunyuan3D. Isso não bloqueia a capacidade geral de bpy. O addon
  reporta telemetria ativa e nove capacidades internas; não confundi-las com ferramentas MCP.
- **Comfy:** pacote instalado comfy-mcp 0.10.0; análise AST enumerou todos os 39 decorators
  mcp.tool no server.py, preservando argumentos e docstrings. comfy --version respondeu envelope/1
  ok=true, versão 1.19.0, acima do requisito 1.14.0 declarado pelo servidor. São evidências
  de instalação/catálogo, não de conexão MCP ou execução de workflow.

## Registro por cliente

Inspecionados apenas campos pertinentes de registro; valores de segredos não foram copiados.

- .mcp.json: Unreal HTTP 127.0.0.1:8000/mcp; Blender stdio uvx Python 3.11, localhost:9876;
  Comfy stdio com executável e COMFY_BIN absolutos existentes.
- .codex/config.toml e ~/.codex/config.toml: Blender e Unreal presentes; Comfy ausente.
- ~/.dsh/profiles/web/cordis.patch.yml: três entradas presentes. Timeouts: Unreal 180012 ms,
  Blender 300000 ms, Comfy 900000 ms; failOnStartupError=false nos três. Não foi provada a
  saúde da sessão DSH apenas lendo sua configuração.

Registro não basta: a conexão precisa ser carregada pelo cliente e responder a uma chamada.
A entrada proposta para Comfy foi documentada em REGISTRO.md, sem alterar configurações.
Havia alteração alheia em .codex/config.toml antes das edições documentais; foi preservada.

## Omissões e imprecisões corrigidas

- Contagem fixa 59 substituída pelo snapshot atual 56/867, preservando a noção de histórico.
- Guia de capacidades deixou de se apresentar como inventário exaustivo.
- Catálogos completos adicionados para Unreal, Blender e Comfy, com nível de evidência explícito.
- Acrescentadas pré-condição DiscoverTests, checagem de contagem e semântica dos filtros.
- Documentados limites diferentes de execução de código em Unreal, Blender e Comfy.
- Exceções históricas deixaram de ser apresentadas como prova de falha atual universal.
- Registrados scripts que invocam EXC-002/004/005 fora dos arquivos/escopos descritos.
- Removida a afirmação de que só geração Comfy tem lacuna relevante: o próprio registro contém
  também PNG, PCG, build e Blender CLI.
- Omissão de Comfy no registro Codex explicitada. Nenhum cliente MCP próprio foi criado.

## EOS — correção de interpretação

O responsável confirmou que a autenticação EOS está funcional. A ausência de login real na
verificação anterior descreve somente o que o agente não executou, não uma falha de EOS.
O relatório anterior recebeu esse esclarecimento. Configuração de MCP, autenticação EOS
e conformidade visual da tela são três avaliações distintas.

## Cobertura e próximos testes

O inventário inclui cada ferramenta da superfície coletada e preserva os contratos, inclusive
ações agrupadas de job/nodes/download/project no Comfy. Não promete exaustividade de plugins
desligados ou de outras instalações, nem funcionalidade de toda ferramenta anunciada.

Para fechar o runtime Comfy: carregar sua entrada no Codex, recuperar ferramentas live,
executar server_info e validação de workflow, e depois um smoke local controlado. Só então
decidir se EXC-001 permanece. Nesta rodada não se confirmou se o ComfyUI em 8188 está ativo.

Para Blender: a cena conectada é padrão, sem arquivo aberto; carregar o arquivo certo é
pré-condição para operar sobre assets do projeto. Não habilitar integrações opcionais por
inferência. Para Unreal: revalidar exceções com arquivos e propriedades concretos, preservando
assets e resultados. Não chamar operações destrutivas apenas para marcar uma ferramenta como testada.

## Artefatos e verificação

- CATALOGO_COMPLETO.md: lista humana integral.
- catalogos/unreal-2026-09-12.json: schemas live completos.
- catalogos/blender-2026-09-12.json: contratos expostos, addon e estados opcionais.
- catalogos/comfy-2026-09-12.json: assinaturas/docstrings da fonte instalada.
- catalogos/manifest-2026-09-12.json: tamanhos e SHA-256 dos três snapshots.
- CAPACIDADES.yml: índice de máquina e atalhos; nomes de toolsets e ferramentas dos atalhos
  comparados com o snapshot Unreal, sem referências inexistentes.

Nenhum script persistente, alteração de asset ou execução de geração foi introduzido.
