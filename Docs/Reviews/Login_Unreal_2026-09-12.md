# Verificação no Unreal — PloidrekRPG e tela de login

> **Esclarecimento do responsável:** a autenticação EOS está funcional, confirmado pelo usuário após
> esta verificação. A frase “não validada” abaixo descreve somente o alcance da execução do agente;
> não significa autenticação quebrada ou integração EOS ausente. O desvio observado foi a apresentação
> dos campos de desenvolvimento em relação ao formulário visual especificado.

Data: 2026-09-12. Checkout observado: a87e33c, branch cursor/megamanx-content-and-mcp.
Inspeção pelo MCP nativo do editor, sem cliente próprio e sem alterações no jogo.

## Resultado

O projeto abriu e os 15 testes Runner passaram, mas a tela de login não atende ao aceite
visual e funcional da especificação. Não declarar o projeto integralmente aprovado com
base nesses testes. Build empacotado, autenticação real e multiplayer não foram validados.

## Evidência executada

- MCP list_toolsets respondeu; mapa atual confirmado: /Game/Maps/MainMenuMap.
- DiscoverTests e RunTestsByFilter (StartsWith:Runner.): 15 Success, zero falhas,
  zero ignorados, duração agregada de 0,707731 s. Resultado completo no JSON adjacente.
- PIE começou duas vezes e registrou AMenuGameMode: menu de entrada aberto (RunnerMenuWidget).
- Capturas pelo CaptureEditorImage e Screenshot de janela Slate apresentadas na conversa.
- Primeira configuração lida: 1274×780. Segunda configuração solicitada pelo MCP: 1920×1080.
  A captura da segunda janela inclui decoração; dimensões lógicas Slate diferem por DPI.
  Não confundir dimensões da janela com medição exata da área renderizada do viewport.
- Snapshots confirmam campos Dev Auth, sete botões sociais desativados e região rolável.
- PIE encerrado; largura, altura, posição e centralização anteriores restauradas em memória.
  Observadores criados para esta inspeção removidos.

## Não conformidades

1. **Layout e recorte:** na captura maior, Steam aparece parcialmente cortado e a ação
   de criação local está fora da área visível. A moldura não envolve o conjunto de controles;
   conteúdo e ornamentos se sobrepõem. Descumpre composição, responsividade e aceite (§2,19,33,37).
   O código fixa painel interno em 620 e colapsa a barra de rolagem (RunnerMenuWidget.cpp:435,463).
   Esses valores são indícios para diagnóstico, não uma causa isolada comprovada.
2. **Foco:** o log da abertura em PIE registra:
   InputMode:UIOnly - Attempting to focus Non-Focusable widget SObjectWidget [Widget.cpp(981)]!
   MenuGameMode.cpp:51 passa Menu->TakeWidget() para SetWidgetToFocus. Requer corrigir e validar
   foco real; o teste de roteamento de botões não valida entrada física (§29,37).
3. **Login de desenvolvimento:** com EOS disponível, os campos são localhost:8081 e nome
   da credencial, com instrução de Dev Auth. O plano técnico admite esse fluxo, mas ele difere
   do formulário E-mail ou Usuário / Senha da especificação visual (§11).
4. **Social:** os sete provedores aparecem no snapshot como disabled; código confirma
   SetIsEnabled(false) para todos, inclusive Epic no grid. O portal Epic tem botão separado.
   Presença visual não comprova integração (§16). Steam ocupa a fileira inteira no código.
5. **Recuperação:** Esqueci a senha é UTextBlock informativo, sem ação de recuperação (§13).
6. **Fidelidade visual:** logo plano em vez do acabamento metálico da arte final, fundo diferente,
   moldura com aparência e escala diferentes e texto com contraste baixo em algumas regiões.
   A captura maior comprova distância visual relevante do alvo; não atribuir percentual arbitrário.
7. **Componentização:** tela montada em RunnerMenuWidget.cpp; não foi comprovada a arquitetura
   de WBP reutilizáveis nem o style system configurável pedido (§18,27,28).

## Avisos do projeto

- M_PedraRestaurada, M_AcoEscuro e M_LataoPolido: falta flag de uso Nanite; aviso alerta para
  renderização fora do editor se não forem corrigidos e salvos.
- GameplayCueNotifyPaths não configurado: busca recai em todo /Game/.
- EOS RTC relatou atraso de ticks durante a automação. Não é medição de performance em produção.
- Erros antigos de sessão MCP/resources no log não foram atribuídos à tela.

## Limites e fontes divergentes

Fontes: Docs/PloidrekRPG_GDD_v3.md §0.1; Docs/Plano_Login_Criacao_Personagem.md;
Art/Tela_login/promtp_espc.md; Art/Tela_login/TL_Tela_Login_art_final.png;
Docs/mcp/ARMADILHAS.md; código e resultados do editor.

O GDD define PloidrekRPG como jogo, Aether Forge como universo e Protocol Zero como campanha.
A especificação §8 pede AETHER FORGE X; a imagem final mostra AETHER FORGE sem o X destacado.
Há também diferença entre abas Login/Cadastro na especificação e botões verticais na arte final.
Essas divergências devem ser tratadas como decisões de fonte/prioridade, não autorização para renomear.

Não foi tentado login com credenciais reais, criação manual de conta, recuperação, nem entrada
de gamepad. Entrada sintética Slate possui limitações já documentadas; não usá-la para concluir
que toda interação do jogo está quebrada. O erro de foco observado é evidência separada e real.
2560×1440, 3440×1440 e 3840×2160 permanecem sem validação nesta rodada.

## Próxima ordem de correção

Corrigir foco e dimensionamento/recorte; validar as quatro resoluções; alinhar o modo de
login apresentado ao usuário; implementar/definir recuperação e provedores; ajustar acabamento
conforme a referência vigente; corrigir avisos de materiais e repetir testes e capturas.

Esta entrega registra a verificação. Nenhuma correção de gameplay, UI ou asset foi aplicada.
