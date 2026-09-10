# Paleta — Nexus-7 (universo Aether Forge · jogo PloidrekRPG)

> Referência de cores para os assets (materiais, texturas, iluminação) das regiões do jogo.
> Estilo: **forja industrial × sci-fi de ação** — latão, cobre e ferro forjado + céu dusk + luzes quentes (âmbar) e frias (ciano).

## 1. Paleta Central (materiais de metal — todos os biomas)

| Material | Hex | Uso |
|---|---|---|
| **Latão polido** | `#c9a227` | Detalhes, engrenagens, ornamentos |
| **Latão escovado** | `#a8842c` | Placas, paredes, tubos |
| **Cobre** | `#b87333` | Canos, telhados, engrenagens |
| **Cobre escuro/oxidado** | `#8a4a2a` | Envelhecido, manchado |
| **Ferro forjado** | `#3a3a42` | Estruturas, vigas |
| **Aço escuro** | `#2a2a30` | Fundo industrial, sombras |
| **Rebites/parafusos** | `#d8c890` | Detalhes metálicos |
| **Vidro/âmbar** | `#ffb040` | Janelas, luzes, faróis |
| **Vapor/névoa** | `#c8c0a8` | Steam, fumaça |

## 2. Céu dusk (ambiente geral)

| Elemento | Hex |
|---|---|
| Céu topo | `#1a2a5e` (azul profundo) |
| Céu meio | `#3a4a80` |
| Horizonte (forja/brasa) | `#c86a30` / `#ff9040` |
| Névoa de vapor | `#7a6a52` |

## 3. Bioma — Cidade (Nexus-7)

| Elemento | Hex |
|---|---|
| Prédios (silhueta) | `#1a1a2e` |
| Janelas acesas (âmbar) | `#ffb040` |
| Ruas/calçadas | `#4a4038` |
| Telhados de cobre | `#b87333` |
| Chaminés/pipes | `#3a3a42` + cobre |
| Iluminação de rua | `#ffcf70` |

## 4. Bioma — Áreas Geladas

| Elemento | Hex |
|---|---|
| Gelo claro | `#a8d8e8` |
| Gelo profundo | `#4a90c0` |
| Cristais de gelo | `#80e0ff` (emissivo) |
| Metal gelado | `#b0c0c8` |
| Sombras frias | `#1a2a3a` |
| Brilho ciano | `#40e0ff` |

## 5. Bioma — Floresta Biomecânica

| Elemento | Hex |
|---|---|
| Troncos metálicos | `#3a3a30` + cobre |
| Folhas de latão | `#c9a227` |
| Veias ciano (energia) | `#40ffc0` (emissivo) |
| Névoa orgânica | `#4a5a3a` |
| Frutos/glow | `#ff8040` |
| Musgo mecânico | `#6a8a3a` |

## 6. Iluminação dusk (para a cena)

- **Sol/luz direcional**: ângulo baixo, cor âmbar suave `#ffb060`, intensidade baixa (0.8–1.5).
- **Ambiente (SkyLight)**: azul `#3050a0`, intensidade baixa (0.3–0.5).
- **Exposição**: fixa/baixa (−2 a −4 stops) para o clima escuro de forja.
- **Vapor**: fog cor `#7a6a52`, densidade moderada.

---

## Assets a gerar (ComfyUI) — `Art/generated/steampunk/`
- `city_street.png` — rua da megacidade vitoriana (fundo/ambiente)
- `brass_wall.png` — textura de latão com rebites (paredes/plataformas)
- `ice_cave.png` — caverna de gelo com cristais (área gelada)
- `bio_forest.png` — floresta biomecânica (trilha/ambiente)
- `steam_house.png` — casa vitoriana steampunk (props)
- `plants_gears.png` — engrenagens + plantas mecânicas (props)

## Aplicação (quando o editor reabrir — via MCP)
1. **Importar** as texturas (`TextureTools.import_file`).
2. **Materiais steampunk**: criar `M_Steam_Brass`, `M_Steam_Copper`, `M_Steam_Iron`, `M_Steam_Ice`, `M_Steam_Bio` com as texturas (Metallic/emissivo).
3. **Repintar** o nível atual com a paleta (latão/cobre/ferro).
4. **PCG**: gerar a cidade (prédios/chaminés), a área gelada e a floresta biomecânica proceduralmente.
