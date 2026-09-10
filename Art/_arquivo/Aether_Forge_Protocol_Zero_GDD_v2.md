# AETHER FORGE: PROTOCOL ZERO
## Game Design Document — Revisão v2.0

> ⚠️ **DOCUMENTO SUPERADO — não usar como fonte.**
> Consolidado em **`Docs/AetherForge_GDD_v3.md`**. Onde houver divergência, **vale o v3**.
> Mantido como histórico: nomes, classes, combate, atributos e progressão daqui estão desatualizados.

---

## 1. CORE CONCEPT

**Aether Forge: Protocol Zero** é um MMORPG de mundo aberto 3D contínuo e sem costuras que funde a **progressão por aquisição de poder de chefões** (inspirado em Mega Man), a **profundidade tática de combate de MOBAs**, o **crescimento persistente de personagem de MMORPGs**, a **granularidade mecânica de D&D 5ª Edição** (sem rodadas de combate) e a **satisfação de crafting de ARPGs**.

Ambientado na megacidade fragmentada de **Nexus-7**, um mundo permanentemente envolto em névoa anômala que distorce a realidade, os jogadores controlam **Runners** — operários ciberneticamente aprimorados que exploram setores corrompidos, extraem Núcleos de Éter, derrotam Lordes de Setor e desvendam a verdade por trás do Véu.

> *"A Névoa não esconde monstros. Ela os cria. E às vezes... ela cria você também."*

---

## 2. WORLD SETTING: NEXUS-7 — MUNDO ABERTO 3D CONTÍNUO

### A Megacidade Fragmentada
Outrora o maior feito da humanidade — um arcólogo continental —, Nexus-7 foi fraturada pelo **Colapso do Éter**, um evento que liberou energia mágica bruta na infraestrutura da cidade. Hoje, a cidade existe como um mosaico de **Setores**, cada um isolado pelo **Véu de Névoa**, uma névoa senciente que obscurece a visão, corrompe a tecnologia e gera entidades hostis.

**O mundo é um open world 3D contínuo e sem costuras.** Não há telas de seleção de fase, portais de missão ou instâncias de exploração. Você atravessa a cidade a pé, veículos, trens subterrâneos ou ganchos de escalada. A transição entre zonas é fluida e orgânica.

### O Véu de Névoa (Fog of War Literal e Mecânico)
- **Visibilidade Dinâmica**: A Névoa não é apenas visual — ela consome ativamente informações. Dados do minimapa se degradam. Posições de inimigos ficam incertas. Até a localização de membros do grupo pode "driftar" no HUD.
- **Camadas de Densidade da Névoa**:
  - **Véu Leve (Shroud)**: Névoa padrão. Visibilidade reduzida, inimigos ocultos até aproximação.
  - **Véu Denso (Mist)**: Corrupção moderada. Perigos ambientais, áudio distorcido, pings falsos no radar.
  - **Véu Profundo (Void)**: Zonas extremas. Privação sensorial completa sem equipamento especializado. Alto risco, alta recompensa.
- **Reatividade da Névoa**: Ações dos jogadores (combate, habilidades, velocidade de movimento) podem atrair a atenção da Névoa, gerando **Mares de Névoa** — ondas de inimigos escalonados.

### Facções e Lore
- **Os Arquitetos**: Cientistas pré-Colapso que buscam restaurar Nexus-7. Mestres da tecnologia e da ordem.
- **Os Esvaziados (The Hollowed)**: Cultistas que abraçam a Névoa. Empunham magia corrompida e remodelam carne.
- **Os Marginais (The Fringers)**: Saqueadores e exilados que prosperam no caos. Oportunistas e engenhosos.
- **O Protocolo**: Construtos de IA enigmáticos que mantêm os sistemas falhos da cidade. Árbitros neutros com motivos inescrutáveis.

---

## 3. GAMEPLAY PILLARS

### Pilar 1: Mundo Aberto Contínuo — "Explore, Descubra, Conquiste"
Nexus-7 é um **mundo aberto 3D contínuo e persistente**. Não há seleção de fases. Você caminha, corre, escala e luta através de um único mundo coeso.
- **Descoberta Orgânica**: Lordes de Setor não são escolhidos em um menu — eles são **encontrados** no mundo através de exploração, inteligência de facções ou eventos dinâmicos.
- **Eventos de Guilda Semanal — "A Caçada"**: Cada semana, um **Lorde de Setor** emerge em uma localização dinâmica do mundo aberto. Guildas competem (ou cooperam) para localizar, invocar e derrotar o Lorde antes que outros jogadores ou facções o façam. A localização muda toda semana.
- **Sincronia de Servidor**: O mundo é persistente e compartilhado. Ações de jogadores afetam o estado do mundo (controle de território, recursos disponíveis, nível de corrupção de setores).

### Pilar 2: Boss Drops como Crafting — "Derrote, Colete, Forje"
- **Materiais de Lorde**: Derrotar um Lorde de Setor não concede uma habilidade instantânea. Em vez disso, dropa **Materiais Únicos de Lorde** — componentes raros usados para forjar armas, armaduras, implantes e consumíveis de alto nível.
- **Itens com Requisitos de Build**: Existem itens lendários e míticos que **só podem ser equipados** se o personagem possuir uma combinação específica de:
  - **Classe/Subclasse** (ex: apenas Vanguard Juggernaut)
  - **Atributos mínimos** (ex: Força 18+, Constituição 14+)
  - **Talentos/Feats específicos** (ex: ter o Feat "Aether Infusion" e o talento "Iron Will")
  - **Renome com facção** (ex: Renome 15+ com Os Arquitetos)
- **Fusão de Protocolos**: Materiais de diferentes Lordes podem ser combinados na forja para criar equipamentos híbridos. Derrotar o Lorde do Fogo dá Cinzas Eternas; derrotar o Lorde do Trovão dá Núcleos de Raios; fundir ambos cria um item com propriedades de **Plasma**.
- **Cadeias de Fraqueza**: Lordes de Setor possuem fraquezas elementais/tipológicas a materiais de outros Lordes, incentivando guildas a planejar quais chefões caçar em qual ordem estratégica.

### Pilar 3: MOBA Combat — "Skill Shots, Posicionamento, Cooldowns"
- **Combate Baseado em Habilidades**: Sem ataques automáticos. Toda ação é uma habilidade com hitbox, tempo de conjuração e cooldown.
- **Controles de MOBA**: WASD + mira com mouse. Q/W/E/R para habilidades, D/F para feitiços de invocador (habilidades utilitárias).
- **Gerenciamento de Recursos**: Em vez de mana de MOBA, usa-se **Células de Éter** — recurso regenerativo com potencial de burst, similar aos espaços de magia de D&D mas regenerando dinamicamente com base em ações de combate.
- **Hierarquia de Controle de Grupo**: Stuns, roots, silences, knockbacks — todos com retornos decrescentes e opções de contra-jogo (limpezas, stats de tenacidade).

### Pilar 4: D&D 5e — "Atributos, Proficiência, Vantagem"
- **Os Seis Atributos**: Força, Destreza, Constituição, Inteligência, Sabedoria, Carisma governam diretamente o scaling de habilidades, chance de acerto e cálculos de resistência.
- **Sistema de Proficiência**: Armas, tipos de armadura, kits de ferramentas e categorias de Protocolo têm ratings de proficiência. Ser proficiente adiciona um bônus escalonável (como o bônus de proficiência de D&D) e desbloqueia técnicas avançadas.
- **Vantagem/Desvantagem**: Rolam-se dois dados e pega-se o maior/menor — implementado como um **modificador de range crítico** em combate em tempo real. Ter Vantagem significa que seu threshold de acerto crítico cai em 5% (ex: 20 → 19-20).
- **Testes de Resistência**: Em vez de resistências planas, habilidades solicitam Testes de Resistência (salvação de Destreza para desviar de uma bola de fogo, salvação de Sabedoria para resistir a controle mental). Stats do jogador vs. CD do inimigo.
- **Sem Rodadas de Combate**: Toda mecânica de D&D é traduzida para tempo real. "Ações Bônus" tornam-se habilidades de conjuração instantânea fora do cooldown global. "Reações" tornam-se contra-habilidades disparadas por ações inimigas.

### Pilar 5: Crafting como Caminho Principal — "Forje, Comércio, Domine"
O crafting não é uma atividade secundária — é um **pilar de gameplay completo e viável**.
- **Especializações de Crafting**: Jogadores podem escolher se especializar em:
  - **Armeiro**: Forja de armas corpo-a-corpo e à distância.
  - **Armadureiro**: Criação de armaduras leves, médias e pesadas.
  - **Implantologista**: Cibernéticos e implantes que modificam atributos e habilidades.
  - **Alquimista**: Poções, granadas, buffs temporários.
  - **Runologista**: Criação e inserção de Runas de Éter.
  - **Engenheiro**: Criação de drones, torretas e veículos.
- **Progressão de Artesão**: Cada especialização possui sua própria árvore de progressão com níveis 1–20, desbloqueando receitas, técnicas avançadas e bônus de qualidade.
- **Economia de Crafting**: Artesãos de elite são essenciais para a economia do jogo. Jogadores que focam apenas em crafting podem:
  - Forjar itens sob encomenda para outros jogadores.
  - Gerenciar oficinas e contratar NPCs assistentes.
  - Dominar o mercado de itens de alto nível.
  - Participar de leilões de materiais raros.
- **Reputação de Artesão**: Sistema de reputação separado. Artesãos renomados atraem clientes, desbloqueiam receitas secretas e recebem encomendas de facções.

---

## 4. SISTEMA DE PERSONAGEM

### Classes (Runners)
Cada classe possui uma **tabela de progressão estilo D&D** (níveis 1–20) com **escolhas de Subclasse no nível 3** (como arquétipos de D&D).

#### 1. Vanguard (Análogo ao Guerreiro)
- **Função**: Tank / Bruiser
- **Atributo-Chave**: Força / Constituição
- **Mecânica Central**: **Pontos de Égide** — pool de vida secundária que regenera fora de combate. Habilidades consomem Égide para efeitos aprimorados.
- **Subclasses**:
  - **Juggernaut**: Tank imóvel. Provocações, redução de dano, imunidade a CC.
  - **Blademaster**: Duelista móvel. Aparos, ripostes, chains de crítico.
  - **Warlord**: Suporte de grupo. Estandartes que buffam aliados, habilidades de comando.

#### 2. Specter (Análogo ao Ladino)
- **Função**: Assassino / Batedor
- **Atributo-Chave**: Destreza / Carisma
- **Mecânica Central**: **Medidor de Sombra** — acumula através de ações furtivas e críticos bem-sucedidos. Gasta Sombra para finalizadores devastadores ou escapes de emergência.
- **Subclasses**:
  - **Phantom**: Especialista em furtividade. Invisibilidade, armadilhas, multiplicadores de backstab.
  - **Duelist**: Mestre 1v1. Mecânicas de riposte, lockdown de alvo único.
  - **Saboteur**: Negação de área. Minas, nuvens de veneno, manipulação ambiental.

#### 3. Channeler (Análogo ao Mago)
- **Função**: Artilharia Mágica / Controlador
- **Atributo-Chave**: Inteligência / Sabedoria
- **Mecânica Central**: **Teia de Éter** — conjuração que deixa zonas persistentes. Teias podem ser detonadas ou combinadas para efeitos combo.
- **Subclasses**:
  - **Elementalist**: Dano bruto. Fogo, gelo, relâmpago com status effects.
  - **Chronomancer**: Magia temporal. Slows, hastes, mecânicas de rewind.
  - **Voidcaller**: Magia negra. Drenagem de vida, invocações, stacking de corrupção.

#### 4. Mediator (Análogo ao Clérigo/Bardo)
- **Função**: Suporte / Buffer / Curandeiro
- **Atributo-Chave**: Sabedoria / Carisma
- **Mecânica Central**: **Harmonia** — buffs e curas geram stacks de Harmonia. Ao máximo, desencadeia uma habilidade ultimate poderosa.
- **Subclasses**:
  - **Luminary**: Curandeiro puro. Escudos, regeneração, ressurreição.
  - **Arbiter**: Buffer/debuffer. Amplificação de dano, shred de resistência, CC.
  - **Inquisitor**: Suporte de combate. Conversão dano-para-cura, habilidades de smite.

#### 5. Machinist (Análogo ao Artífice)
- **Função**: Classe de Pets / Torretas / Utilitário
- **Atributo-Chave**: Inteligência / Destreza
- **Mecânica Central**: **Pool de Sucata** — inimigos derrotados e objetos destruídos dropam Sucata. Gasta Sucata para deploy de torretas, drones ou modificação de gear em missão.
- **Subclasses**:
  - **Engineer**: Estruturas defensivas. Torretas, barreiras, estações de cura.
  - **Mechromancer**: Pets de combate. Enxames de drones, trajes mecânicos, aliados autônomos.
  - **Alchemist**: Consumíveis e granadas. Poções de buff, bombas de debuff, névoa de cura.

### Sistemas de Progressão
- **Nível Máximo**: 20 (progressão em tiers estilo D&D)
- **Pontos de Habilidade**: Cada nível concede pontos para desbloquear/aprimorar habilidades em uma árvore de habilidades estilo MOBA.
- **Sistema de Feats**: Nos níveis 4, 8, 12, 16, 19, escolha um Feat (como D&D) que concede uma passiva ou habilidade ativa maior.
- **Renome**: Reputação em nível de conta com facções, desbloqueando vendedores, cosméticos e conteúdo narrativo.
- **Leaderboards**: Rankings separados para cada modo de jogo, atualizados semanalmente.

---

## 5. SISTEMA DE COMBATE: "A ENGINE NEXUS"

### Tradução de D&D para Tempo Real
| Conceito D&D | Implementação em Tempo Real |
|-------------|---------------------------|
| **Ação** | Habilidades primárias (Q/W/E) com cooldowns |
| **Ação Bônus** | Habilidades instantâneas (botões de polegar do mouse) que não disparam cooldown global |
| **Reação** | Contra-habilidades disparadas por tells inimigos (aparar, rolamento de esquiva, reflect de magia) |
| **Movimento** | WASD com sprint/esquiva baseada em stamina. Sem grid. Livre 360° |
| **Rolagem de Ataque** | Stat de Precisão vs. stat de Evasão + rolagem RNG. Determina acerto/erro/crítico |
| **Teste de Resistência** | Stat do jogador vs. CD da habilidade. Prompt visual (indicador "SALVAÇÃO DE DES!") |
| **Iniciativa** | Não usada. Prioridade em tempo real baseada em tempos de conjuração e posicionamento |
| **Espaços de Magia** | Células de Éter — regeneram em combate via causar/receber dano |

### Mecânicas de MOBA em Contexto ARPG
- **Last Hitting**: Golpes de morte em inimigos concedem bônus de Éter e experiência.
- **Warding**: Balizas scanner deployáveis que perfuram a Névoa em uma área. Essencial para sobrevivência.
- **Objetivos**: Em modos de sessão, capturar pontos de controle ou destruir estruturas concede buffs em nível de equipe.
- **Efeitos Ativos de Itens**: Gear lendário possui habilidades ativas (como ativas de itens de MOBA) vinculadas a teclas.

### Combate Ambiental
- **Verticalidade**: Ganchos de escalada, corrida em paredes, pulos duplos. Terreno alto concede Vantagem em ataques à distância.
- **Cobertura Destrutível**: Cobertura bloqueia linha de visão mas pode ser destruída.
- **Manipulação da Névoa**: Use habilidades para limpar a Névoa temporariamente, ou a weaponize contra inimigos.

---

## 6. MODOS DE JOGO — TRÊS PILARES VIÁVEIS

O jogo é projetado para que **três caminhos de gameplay sejam igualmente viáveis e recompensadores**:

### 🛠️ PILAR CRAFTING — "O Forja-Mundo"
Jogadores que desejam focar exclusivamente em crafting têm um caminho completo de progressão:
- **Oficinas**: Adquira e upgrade uma oficina no mundo aberto. Oficinas maiores permitem mais NPCs assistentes, estações de trabalho e armazenamento.
- **Contratos e Encomendas**: Receba contratos de jogadores, guildas e facções NPCs para forjar itens específicos.
- **Exploração de Materiais**: Materiais raros são encontrados em zonas de alto risco do mundo aberto. Crafting requer exploração.
- **Eventos de Forja**: Competições semanais de crafting onde artesãos competem para criar o melhor item com materiais fornecidos.
- **Leaderboard de Artesão**: Rankings por especialização, reputação e valor total de itens forjados.
- **Receitas Secretas**: Descubra receitas ocultas através de exploração, reputação com facções ou decodificação de fragmentos.
- **Economia**: Artesãos de elite são indispensáveis. Itens de alto nível requerem um artesão com nível de especialização adequado.

### ⚔️ PILAR PvP — "O Domínio"
Jogadores focados em PvP têm múltiplos modos e progressão dedicada:
- **Zonas Contestadas**: Áreas do mundo aberto com PvP ativado e recursos raros. Guerra de facções por pontos de controle.
- **Arenas**: Combates 1v1, 2v2, 3v3 em arenas instanciadas com matchmaking baseado em rating.
- **Campos de Batalha (Battlegrounds)**: Modos MOBA-inspired competitivos (ver seção E abaixo).
- **Extração PvP**: Modo hardcore onde múltiplos esquadrões entram simultaneamente (ver seção B).
- **Recompensas PvP**: Gear exclusiva com stats voltados para PvP, títulos, montarias, cosméticos de temporada.
- **Sistema de Ranking**: Elo-based com temporadas, divisões e recompensas exclusivas.
- **Guerras de Guilda**: Batalhas semanais de grande escala por território e recursos no mundo aberto.

### 🐉 PILAR PvE — "A Caçada"
Jogadores focados em PvE têm conteúdo abundante e progressão significativa:
- **Mundo Aberto PvE**: Exploração, eventos dinâmicos, missões de facção, caçada a inimigos de elite.
- **Eventos de Crise (Dungeons)**: Instâncias desafiadoras (ver seção C).
- **Eventos de Guilda Semanal — A Caçada**: Derrote Lordes de Setor em eventos de guilda (ver seção D).
- **Solo Challenges**: Desafios individuais de alto nível.
- **Masmorras de Party**: Instâncias de grupo com mecânicas de chefão estilo MOBA.
- **Recompensas PvE**: Materiais de crafting raros, gear exclusiva, cosméticos, lore, e progressão narrativa.

---

### A. MUNDO ABERTO: A MARGEM (Hub MMORPG)
- **Zona Contínua**: Um setor persistente e de mundo aberto onde jogadores se reúnem, comercializam, formam grupos e realizam eventos dinâmicos.
- **Zonas Seguras**: Postos avançados controlados pelos Arquitetos com vendedores, estações de crafting e hubs sociais.
- **Zonas Contestadas**: Áreas com PvP ativado e recursos raros. Guerra de facções por pontos de controle.
- **Eventos Mundiais**: Batalhas massivas agendadas contra Titãs da Névoa. Centenas de jogadores cooperando.
- **Sem Timer de Sessão**: Jogue no seu próprio ritmo. É aqui que acontece a progressão persistente.

### B. INSTÂNCIAS DE EXTRAÇÃO (PvE & PvP)
*Inspirado em Escape from Tarkov / Hunt: Showdown*
- **A Preparação**: Entre em um setor corrompido com um pequeno esquadrão (1–3 jogadores). O mapa é denso de Névoa, inimigos de elite e perigos ambientais.
- **O Objetivo**: Localize e extraia Núcleos de Éter enquanto sobrevive. Núcleos vêm em tiers (Comum a Prime). Tiers mais altos = maior risco.
- **Variante PvE**: Puramente contra IA. Mares de Névoa escalam ao longo do tempo, forçando a extração.
- **Variante PvP**: Múltiplos esquadrões entram simultaneamente. Podem escolher cooperar ou caçar uns aos outros. Morte significa perder gear carregada (risco/reward hardcore).
- **Duração da Sessão**: 20–40 minutos.
- **Leaderboard**: Ranqueado por valor de extração, taxa de sobrevivência e eficiência PvP.

### C. EVENTOS DE CRISE (PvE) — "Dungeons Reimaginados"
*Substituem o conceito tradicional de dungeons*
- **O Que São**: Eventos de crise são instâncias temporárias onde a corrupção da Névoa atinge níveis críticos. Um jogador pode **invocar** um Evento de Crise usando itens específicos de invocação encontrados no mundo aberto.
- **Invocação**: Jogadores podem invocar Eventos de Crise em locais designados do mundo aberto. A invocação consome um **Catalisador de Crise** (dropado de inimigos de elite ou comprado de facções).
- **Dailies**: Todo dia, 3 Eventos de Crise estão disponíveis como missões diárias. Eles têm dificuldade ajustada e recompensas garantidas.
- **Tamanho do Grupo**: 4 jogadores (1 Tank, 1 Healer, 2 DPS — flexível mas ótimo).
- **Estrutura**: 3–4 encontros com mini-chefões levando a um Lorde de Setor corrompido.
- **Mecânicas**: Chefões possuem kits de habilidades estilo MOBA com tells claros, fases e timers de enrage.
- **Tiers de Dificuldade**: Normal → Heroic → Mythic → Nightmare. Tiers mais altos adicionam mecânicas e afixos.
- **Loot**: Drops garantidos de chefões, com lockouts semanais para recompensas de tier mais alto.
- **Duração da Sessão**: 30–60 minutos.
- **Leaderboard**: Rankings de speedrun e rankings de desafio sem morte.

### D. EVENTOS DE GUILDA SEMANAL — "A CAÇADA AO LORDE"
*Substitui o sistema de seleção de fase do Mega Man*
- **O Que São**: Cada semana, um **Lorde de Setor** emerge em uma **localização dinâmica e secreta** dentro do mundo aberto de Nexus-7.
- **A Descoberta**: Guildas devem investigar pistas no mundo aberto, interagir com NPCs de facção, decodificar fragmentos de dados ou explorar zonas de névoa densa para descobrir a localização exata do Lorde.
- **A Invocação**: Uma vez descoberto, o Lorde deve ser **invocado** através de um ritual que requer recursos coletados pela guilda. A invocação é um evento público — outras guildas podem tentar invocar o mesmo Lorde, criando competição.
- **A Batalha**: O combate contra o Lorde é um evento de mundo aberto de grande escala. Múltiplos grupos podem participar, mas a guilda que invocou receve bônus de loot e reconhecimento.
- **Recompensas**: Materiais únicos de Lorde, gear exclusiva, cosméticos de guilda, e pontos de renome.
- **Cadeia de Fraqueza**: Lordes possuem fraquezas a materiais de outros Lordes. Guildas que planejarem sua ordem de caçada estrategicamente terão vantagem.
- **Leaderboard**: Rankings de guilda por velocidade de invocação, dano causado, e eficiência de recursos.

### E. SOLO CHALLENGES (PvE)
*Desafios individuais de alto nível*
- **O Desafio**: Zonas específicas do mundo aberto onde jogadores podem enfrentar versões solo de inimigos de elite e Lordes de Setor.
- **Mestria de Build**: Complete desafios usando builds específicas para recompensas bônus.
- **Modo Ironman**: Uma vida. Sem checkpoints. Teste supremo de habilidade.
- **Duração da Sessão**: 10–20 minutos.
- **Leaderboard**: Rankings de tempo de clear e rankings baseados em pontuação (pontos de estilo por combos e runs sem dano).

### F. CAMPOS DE BATALHA (PvP)
*PvP competitivo inspirado em MOBA*
- **Tamanho do Time**: 5v5 ou 10v10.
- **Design de Mapa**: Mapas simétricos com rotas, setores de selva e objetivos centrais.
- **Variantes de Modo**:
  - **Choque de Éter**: MOBA clássico. Destrua a estrutura Nexus inimiga. Minions spawnam em rotas. Jogadores upam durante a partida (baseado em sessão, não persistente).
  - **Dominação**: Segure pontos de controle para drenar tickets inimigos.
  - **Aniquilação**: Team deathmatch com limites de respawn.
- **Regras de Personagem**: Jogadores entram com seu personagem persistente mas stats são **normalizados** para uma linha de base competitiva. Afixos de gear são desabilitados; apenas escolhas de build e habilidade importam.
- **Duração da Sessão**: 15–30 minutos.
- **Leaderboard**: Sistema ranqueado baseado em Elo com recompensas sazonais.

### G. EVENTOS SAZONAIS
- **A Maré de Névoa**: Evento mensal onde a Névoa se expande, introduzindo novos Setores, novos Lordes de Setor e Protocolos de tempo limitado.
- **Guerras de Facção**: Campanhas PvP de uma semana onde facções competem pelo controle de território.
- **Os Trials do Protocolo**: Modos de desafio diários/semanais rotativos com modificadores únicos (ex: "Todas as habilidades custam o dobro mas causam dano triplo").

---

## 7. SISTEMA DE ITENS E EQUIPAMENTOS

### Requisitos de Equipamento
Itens no jogo possuem requisitos que vão além do nível de personagem:

| Tipo de Requisito | Descrição | Exemplo |
|-------------------|-----------|---------|
| **Atributos Mínimos** | Valores base de atributos necessários | Força 18+, Destreza 12+ |
| **Classe/Subclasse** | Restrição de classe ou subclasse | Apenas Vanguard Juggernaut |
| **Talentos/Feats** | Talento ou Feat específico necessário | Requer "Aether Infusion" + "Iron Will" |
| **Proficiência** | Nível de proficiência com tipo de item | Proficiência 15 em Armaduras Pesadas |
| **Renome de Facção** | Reputação mínima com facção | Renome 20+ com Os Arquitetos |
| **Nível de Artesão** | Para itens forjados, nível do criador | Requer Artesão Nível 12+ |

### Tiers de Gear
- **Common → Uncommon → Rare → Epic → Legendary → Mythic**
- **Sistema de Afixos**: Itens rolam com afixos aleatórios (+% Dano de Fogo, +Destreza, Chance de Conjurar ao Acertar). Lendários possuem modificadores únicos que habilitam arquétipos de build.
- **Runewords**: Gear com soquetes pode ser aprimorado com Runas de Éter, criando efeitos sinérgicos poderosos.
- **Itens de Lorde**: Gear forjada com materiais de Lordes de Setor possuem afixos únicos e aparências distintas.

---

## 8. PROGRESSÃO E LEADERBOARDS

### Progressão em Múltiplas Camadas
1. **Nível de Personagem**: 1–20. Desbloqueia habilidades, feats e pontos de atributo.
2. **Gear Score**: Nível de poder médio do equipamento equipado. Determina acesso a conteúdo de tier alto.
3. **Coleção de Materiais**: Biblioteca em nível de conta de materiais de Lordes de Setor descobertos.
4. **Renome**: Reputação de facção para benefícios narrativos e econômicos.
5. **Maestria**: Experiência por arma, classe e modo que desbloqueia cosméticos e bônus menores de stat.
6. **Nível de Artesão**: 1–20 por especialização de crafting.
7. **Rating PvP**: Elo competitivo em modos PvP.

### Categorias de Leaderboard
| Categoria | Métrica | Recompensa |
|-----------|---------|------------|
| **Rei da Extração** | Valor total de Éter extraído | Cosméticos temáticos de extração |
| **Caçador de Lordes** | Lordes de Setor derrotados (solo/grupo) | Título + montaria |
| **Speedster de Crise** | Clears mais rápidos de Eventos de Crise | Estandarte de guilda + aura |
| **Campeão de Campo de Batalha** | Maior Elo PvP | Skin de armadura sazonal |
| **Sobrevivente Ironman** | Maior streak Ironman | Moldura de retrato exclusiva |
| **Andarilho da Névoa** | Maior distância explorada na Névoa | Gear temática de cartografia |
| **Mestre Artesão** | Valor total de itens forjados | Título + oficina exclusiva |
| **Magnata do Mercado** | Volume de comércio | Acesso a leilões premium |

### Temporadas
- **Temporadas de 3 Meses**: Cada temporada introduz um novo Setor, novo Lorde de Setor, novos materiais e um patch de balanceamento.
- **Personagens Sazonais**: Servidores de fresh-start opcionais onde todos começam no nível 1 para uma corrida ao nível máximo e leaderboards.
- **Passe de Batalha**: Trilhas gratuitas e premium com cosméticos, moeda e itens de conveniência (sem pay-to-win).

---

## 9. ECONOMIA E CRAFTING

### Moedas
- **Créditos**: Moeda padrão de vendedores e missões.
- **Fragmentos de Éter**: Moeda premium de extrações e conteúdo de tier alto. Usada para crafting, comércio e cosméticos.
- **Sucata**: Material de crafting de desmantelamento de gear.

### Crafting Profundo
- **Modificação**: Rerolar afixos em gear usando Fragmentos de Éter.
- **Infusão**: Upgrade de tier de gear, aumentando stats base.
- **Impressão de Protocolo**: Extraia um material de Lorde derrotado e imprima-o em gear para bônus passivos.
- **Runecrafting**: Crie e combine Runas de Éter para gear com soquetes.
- **Desmantelamento**: Quebre gear em componentes base para reutilização.

### Comércio entre Jogadores
- **O Bazar**: Casa de leilão driven por jogadores na Margem.
- **Comércio Direto**: Janelas de troca seguras com inspeção.
- **Sem Vinculação por Coleta na Maioria do Gear**: Loot de extração de alto valor pode ser vendido, criando uma economia próspera entre jogadores.
- **Contratos de Crafting**: Artesãos podem criar contratos onde jogadores fornecem materiais e pagam uma taxa pelo serviço.

---

## 10. ARTE E DIREÇÃO DE ÁUDIO

### Estilo Visual
- **Cyberpunk Neo-Noir**: Iluminação de alto contraste com sombras profundas. Acentos neon contra cinzas industriais.
- **Renderização de Névoa**: Névoa volumétrica que reage a fontes de luz e movimento do jogador. A Névoa é um personagem, não apenas um efeito.
- **Design de Personagem**: Silhuetas inspiradas em Mega Man com peças de armadura modulares. Cada classe tem um perfil distinto legível em silhueta.
- **Design de UI**: Elementos de HUD diegéticos — barras de vida projetadas de dispositivos de pulso, minimap como um drone holográfico companheiro.

### Design de Áudio
- **Soundscape Dinâmico**: A Névoa abafa sons distantes mas amplifica ameaças próximas. Áudio é uma mecânica de gameplay.
- **Áudio de Habilidades**: Cada Protocolo tem uma assinatura de áudio distinta, permitindo que jogadores habilidosos identifiquem habilidades inimigas apenas pelo som.
- **Música**: Híbrido synthwave-industrial. Intensifica durante combate e Mares de Névoa.

---

## 11. ARQUITETURA TÉCNICA

### Networking
- **Modelo de Servidor Híbrido**: Mundo aberto roda em shards dedicados (100+ jogadores). Modos de sessão usam servidores dedicados instanciados.
- **Netcode de Rollback**: Essencial para combate de precisão estilo MOBA. Predição client-side com reconciliação de servidor.
- **Sincronização de Névoa**: Sistema de visão server-authoritative. Jogadores só recebem dados sobre entidades que podem realmente ver.

### Anti-Cheat
- **Validação Server-Side**: Toda detecção de acerto, rolagem de loot e progressão são server-authoritative.
- **Névoa como Anti-Cheat**: O sistema de Névoa naturalmente limita informação disponível aos clientes, tornando wallhacks e maphacks inerentemente difíceis.

### Plataformas
- **PC (Primário)**: Otimizado para mouse e teclado. Suporte completo a mods de UI.
- **Consoles**: Suporte a controle com menus radiais para habilidades.
- **Cross-Play**: Habilitado para PvE. PvP tem matchmaking opcional baseado em input.

---

## 12. MONETIZAÇÃO

### Free-to-Play Ético
- **Grátis**: Acesso completo ao jogo, todas as classes, todos os modos, todos os níveis.
- **Premium Cosmético-Apenas**: Skins, emotes, variantes de montaria, efeitos de arma.
- **Conveniência (Não-P2W)**: Abas de stash, slots de personagem, boosters de XP (apenas para alts, não para personagens principais).
- **Passe de Batalha**: $10 por temporada. Contém cosméticos, créditos e materiais de crafting.
- **Sem Loot Boxes**: Compra direta ou obtível através de gameplay.

---

## 13. ROADMAP DE DESENVOLVIMENTO

### Fase 1: Fundação (Meses 1–12)
- Engine de combate core (Engine Nexus)
- 3 classes (Vanguard, Specter, Channeler)
- Mundo Aberto A Margem (1 zona contínua)
- Eventos de Crise (5 instâncias)
- Sistema de crafting básico

### Fase 2: Expansão (Meses 13–24)
- 2 classes adicionais (Mediator, Machinist)
- Instâncias de Extração (PvE + PvP)
- Campos de Batalha (2 modos)
- Eventos de Guilda Semanal — A Caçada
- Crafting avançado e economia
- Lançamento da Temporada 1

### Fase 3: Evolução (Meses 25+)
- Guerra de guildas e território
- Sistema de montarias e veículos
- Conteúdo de Raid (8 jogadores)
- Ferramentas de conteúdo gerado por usuários
- Lançamento em consoles

---

## 14. PONTOS DE VENDA ÚNICOS

1. **O Único Híbrido Mega Man + MOBA + D&D + ARPG Crafting**: Nenhum outro jogo combina a progressão de caça a chefões de Mega Man com a profundidade de combate de MOBA, a riqueza mecânica de D&D e a satisfação de crafting de ARPGs.
2. **Mundo Aberto 3D Contínuo**: Sem telas de seleção de fase. Nexus-7 é um mundo vivo, respirando e conectado.
3. **Névoa como Gameplay**: O Fog of War não é apenas visual — é uma ameaça sistêmica que molda cada decisão.
4. **Três Pilares Viáveis**: Crafting, PvP e PvE são caminhos de jogo completos e igualmente recompensadores. Não é necessário fazer tudo para progredir.
5. **Eventos de Guilda Semanal**: A caçada aos Lordes de Setor cria momentos sociais épicos e competição saudável entre guildas.
6. **Build Freedom Real**: A criação de personagens aberta de D&D encontra a itemização de ARPG. Nenhum personagem precisa jogar igual.
7. **Extração Risco/Recompensa**: A adrenalina de jogos de sobrevivência hardcore sem a perda permanente punidora.

---

## 15. ELEVATOR PITCH

> *Aether Forge: Protocol Zero* é o que acontece quando a progressão de caça a chefões de Mega Man, o combate de skill shots de League of Legends, o vício de loot de Diablo, a profundidade de personagem de D&D e a economia de crafting de um MMORPG colidem em uma megacidade cyberpunk engolida por névoa. Escolha seu Runner. Caçe os Lordes de Setor. Forje seu poder. Sobreviva à Névoa. Domine os leaderboards. Ou seja consumido.

---

*Documento Versão 2.0 — Iniciativa Protocol Zero*
