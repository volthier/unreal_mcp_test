# ⚠️ ARQUIVO MORTO — Mega Man X — Bíblia do Mundo

> **NÃO USAR.** Este documento cita termos e obras de terceiros (Mega Man X, Capcom) e foi **substituído** por
> **`Docs/WorldBible_Nexus7.md`**, que traz a mesma direção de arte com a nomenclatura própria do jogo
> (Runners, N.E.R.V., Fadenclyffe, Apagados, Clyffen).
> Mantido apenas como registro do material de referência original. **Nada daqui deve ir para o jogo, para o GDD ou para
> documentos de apresentação.**

> Direção de arte fiel ao universo Mega Man X: **opulência de alta tecnologia** × **decadência de guerra civil entre máquinas**. Equilibrar o **brilho tecnológico** com a **robustez mecânica pesada**.

## 1. Cidades e Prédios
- **Arquitetura vertical**: arranha-céus gigantes que tocam as nuvens, interligados por **pontes de vidro** e **cabos de alta tensão**.
- **Fachadas tecnológicas**: revestidas de **placas solares escuras**, ligas metálicas polidas, **painéis holográficos desgastados** piscando anúncios antigos.
- **Estrutura "cebola"**: topo reluzente/limpo; base (nível da rua) **escura, úmida, fiação exposta**.

## 2. Ruas e Infraestrutura
- **Rodovias suspensas**: autoestradas magnéticas flutuantes entre prédios, **destruídas/colapsadas** por combate.
- **Iluminação neon**: postes digitais + faixas de sinalização **azul, verde e laranja** sobre asfalto sintético escuro.
- **Subsolo conectado**: bueiros que expelem vapor sob pressão → esgoto tecnológico, túneis de manutenção, metrôs de carga automatizados.

## 3. Fábricas e Zonas Industriais
- **Automação brutalista**: complexos sem humanos, só braços robóticos e esteiras.
- **Metal pesado**: paredes de aço rebitado, tanques de fundição (metal líquido brilhante), geradores de plasma.
- **Perigo ambiental**: vazamentos de fluidos refrigerantes, prensas hidráulicas, trituradores de sucata ativos.

## 4. Robôs e População (Reploids)
- **Reploids**: humanos-like e mecânicos, feições expressivas, juntas esféricas, pistões nos ombros, blindagem modular (azul, vermelho, amarelo).
- **Mavericks (inimigos)**: máquinas corrompidas, **olhos vermelhos brilhantes**, armadura com ferrugem/danos de batalha.
- **Robôs de carga/sentinelas**: mecanoides pequenos, sem IA, funcionais (rodas, esteiras, hélices), patrulha/trabalho.

## 5. "Bichos" (Fauna Mecânica)
- **Animais biônicos**: robôs baseados em animais reais (artrópodes, répteis, mamíferos).
- **Adaptação hostil**: escorpiões que disparam lasers pela cauda; aves com asas de lâminas de titânio; gorilas mecânicos de carga guardando territórios.
- **Comportamento selvagem**: instinto de programação — caçam invasores ou defendem fontes de energia.

## 6. Vegetação e Natureza
- **Estufas/florestas artificiais**: plantas em biosferas controladas, tubos de nutrientes fluorescentes, luz UV.
- **Fusão orgânica-sintética**: árvores com tronco de fibra de carbono, folhas parecendo placas de circuito impresso flexíveis.
- **Natureza selvagem tecnológica**: cipós = cabos elétricos antigos com musgo real; laboratórios abandonados com vegetação descontrolada.

---

## 📋 Lista de Assets por categoria (para gerar no ComfyUI→Blender→UE)

| Categoria | Assets | Estilo |
|---|---|---|
| **Cidade** | `city_building` (arranha-céu), `city_bridge`, `city_base_street` | Vertical, placas solares, hologramas, base escura |
| **Rua** | `street_highway` (rodovia suspensa), `street_neon_signs`, `street_manhole` (vapor) | Neon azul/verde/laranja, asfalto escuro |
| **Fábrica** | `factory_wall` (aço rebitado), `factory_plasma`, `factory_vat` | Brutalista, metal pesado |
| **Robôs** | `reploid` (humanoide), `maverick` (corrompido, olhos vermelhos), `worker_robot` | Expressivo, modular azul/vermelho/amarelo |
| **Fauna** | `mech_scorpion` (laser), `mech_bird` (lâminas), `mech_gorilla` | Bionico, hostil |
| **Vegetação** | `bio_tree` (fibra de carbono), `bio_grass` (placas de circuito), `bio_vine` (cabos+musgo) | Orgânico-sintético, porões/estufas |

## 🎨 Paleta (macro)
- **Tecnologia**: azul elétrico `#2090ff`, ciano `#40e0ff`, branco fosco `#d0d8e0`.
- **Degradação/ferrugem**: cobre oxidado `#8a4a2a`, ferrugem `#a05020`, aço escuro `#2a2a30`.
- **Neon**: azul `#00c0ff`, verde `#40ff70`, laranja `#ff8040`.
- **Maverick (inimigo)**: olhos vermelhos `#ff2020`, ferrugem `#b05020`.
