# AETHER FORGE: PROTOCOL ZERO
## Game Design Document

> ⚠️ **DOCUMENTO SUPERADO (v1) — não usar como fonte.**
> Consolidado em **`Docs/AetherForge_GDD_v3.md`**. Onde houver divergência, **vale o v3**.
> Mantido apenas como histórico.

---

## 1. CORE CONCEPT

**Aether Forge: Protocol Zero** is a seamless open-world action RPG that fuses the **stage-select progression and boss-weapon acquisition** of classic Mega Man with the **strategic depth of MOBAs**, the **persistent character growth of MMORPGs**, and the **mechanical granularity of D&D 5th Edition**—reimagined for real-time, roundless combat.

Set in the shattered megacity of **Nexus-7**, a world perpetually shrouded in anomalous fog that warps reality, players control **Runners**—cybernetically enhanced operatives who delve into procedurally corrupted sectors to extract Aether Cores, defeat Sector Lords, and uncover the truth behind the Fog.

> *"The Fog doesn't hide monsters. It makes them. And sometimes... it makes you one too."*

---

## 2. WORLD SETTING: NEXUS-7

### The Shattered Megacity
Once humanity's greatest achievement—a continent-spanning arcology—Nexus-7 was fractured by the **Aether Collapse**, an event that unleashed raw magical energy into the city's infrastructure. Now, the city exists as a patchwork of **Sectors**, each isolated by the **Veil Fog**, a sentient mist that obscures vision, corrupts technology, and spawns hostile entities.

### The Fog of War (Literal & Mechanical)
- **Dynamic Visibility**: The Fog is not just visual—it actively consumes information. Minimap data decays. Enemy positions become uncertain. Even party member locations can drift on the HUD.
- **Fog Density Tiers**:
  - **Shroud**: Standard fog. Reduced visibility, hidden enemies until proximity.
  - **Mist**: Moderate corruption. Environmental hazards, distorted audio, false radar pings.
  - **Void**: Extreme zones. Complete sensory deprivation without specialized gear. High risk, high reward.
- **Fog Reactivity**: Player actions (combat, abilities, movement speed) can attract the Fog's attention, spawning **Fog Tides**—waves of escalating enemies.

### Factions & Lore
- **The Architects**: Pre-Collapse scientists seeking to restore Nexus-7. Masters of technology and order.
- **The Hollowed**: Cultists who embrace the Fog. They wield corrupted magic and reshape flesh.
- **The Fringers**: Scavengers and outcasts who thrive in the chaos. Opportunistic and resourceful.
- **The Protocol**: Enigmatic AI constructs that maintain the city's failing systems. Neutral arbiters with inscrutable motives.

---

## 3. GAMEPLAY PILLARS

### Pillar 1: Mega Man Progression — "Acquire, Adapt, Overcome"
- **Sector Lords**: Each major Sector is ruled by a powerful boss (the "Sector Lord"). Defeating them grants their **Signature Protocol**—a unique ability, weapon, or passive that can be equipped.
- **Protocol Fusion**: Acquired Protocols can be combined. Defeat the Pyro-Lord to get Flame Dash; defeat the Volt-Lord to get Thunder Strike; fuse them into **Plasma Surge**.
- **Weakness Chains**: Sector Lords have elemental/type weaknesses to other Lords' Protocols, encouraging strategic progression order—just like Mega Man's rock-paper-scissors boss design.

### Pillar 2: MOBA Combat — "Skill Shots, Positioning, Cooldowns"
- **Ability-Based Combat**: No auto-attacks. Every action is an ability with a hitbox, cast time, and cooldown.
- **MOBA Controls**: WASD movement + mouse aim. Q/W/E/R for abilities, D/F for summoner spells (utility skills).
- **Resource Management**: Instead of MOBA mana, use **Aether Cells**—a regenerating resource with burst potential, similar to D&D's spell slots but regenerating dynamically based on combat actions.
- **Crowd Control Hierarchy**: Stuns, roots, silences, knockbacks—all with diminishing returns and counter-play options (cleanses, tenacity stats).

### Pillar 3: D&D 5e Depth — "Stats, Proficiency, Advantage"
- **The Six Attributes**: Strength, Dexterity, Constitution, Intelligence, Wisdom, Charisma directly govern ability scaling, hit chance, and resistance calculations.
- **Proficiency System**: Weapons, armor types, tool kits, and Protocol categories have proficiency ratings. Being proficient adds a scaling bonus (like D&D's proficiency bonus) and unlocks advanced techniques.
- **Advantage/Disadvantage**: Roll two dice and take the higher/lower—implemented as a **critical range modifier** in real-time combat. Having Advantage means your critical hit threshold drops by 5% (e.g., 20 → 19-20).
- **Saving Throws**: Instead of flat resistances, abilities prompt Saving Throws (Dexterity save to dodge a fireball, Wisdom save to resist mind control). Player stats vs. enemy DC.
- **No Combat Rounds**: All D&D mechanics are translated into real-time. "Bonus Actions" become instant-cast off-global-cooldown abilities. "Reactions" become counter-abilities triggered by enemy actions.

### Pillar 4: ARPG Loot & Buildcraft — "Numbers Go Up, Builds Go Deep"
- **Gear Tiers**: Common → Uncommon → Rare → Epic → Legendary → Mythic.
- **Affix System**: Items roll with random affixes (+% Fire Damage, +Dexterity, Chance to Cast on Hit). Legendaries have unique modifiers that enable build archetypes.
- **Runewords**: Socketed gear can be enhanced with Aether Runes, creating powerful synergistic effects.
- **Build Diversity**: A "Fighter" class can be a tanky frontliner (Str/Con), a crit-based assassin (Dex/Cha), or a spellblade (Int/Str) depending on gear and Protocol choices.

---

## 4. CHARACTER SYSTEM

### Classes (Runners)
Each class has a **D&D-style progression table** (levels 1–20) with **Subclass choices at level 3** (like D&D archetypes).

#### 1. Vanguard (Fighter Analog)
- **Role**: Tank / Bruiser
- **Key Stat**: Strength / Constitution
- **Core Mechanic**: **Aegis Points** — a secondary health pool that regenerates out of combat. Abilities consume Aegis for enhanced effects.
- **Subclasses**:
  - **Juggernaut**: Immovable tank. Taunts, damage reduction, crowd control immunity.
  - **Blademaster**: Mobile duelist. Parries, ripostes, critical strike chains.
  - **Warlord**: Party support. Banners that buff allies, command abilities.

#### 2. Specter (Rogue Analog)
- **Role**: Assassin / Scout
- **Key Stat**: Dexterity / Charisma
- **Core Mechanic**: **Shadow Meter** — builds through stealth actions and successful crits. Spend Shadow for devastating finishers or emergency escapes.
- **Subclasses**:
  - **Phantom**: Stealth specialist. Invisibility, trap-setting, backstab multipliers.
  - **Duelist**: 1v1 master. Riposte mechanics, single-target lockdown.
  - **Saboteur**: Area denial. Mines, poison clouds, environmental manipulation.

#### 3. Channeler (Wizard Analog)
- **Role**: Artillery Mage / Controller
- **Key Stat**: Intelligence / Wisdom
- **Core Mechanic**: **Aether Weave** — spellcasting that leaves persistent zones. Weaves can be detonated or combined for combo effects.
- **Subclasses**:
  - **Elementalist**: Raw damage. Fire, ice, lightning with status effects.
  - **Chronomancer**: Time magic. Slows, hastes, rewind mechanics.
  - **Voidcaller**: Dark magic. Life drain, summons, corruption stacking.

#### 4. Mediator (Cleric/Bard Analog)
- **Role**: Support / Buffer / Healer
- **Key Stat**: Wisdom / Charisma
- **Core Mechanic**: **Harmony** — buffs and heals generate Harmony stacks. At max Harmony, unleash a powerful ultimate ability.
- **Subclasses**:
  - **Luminary**: Pure healer. Shields, regeneration, resurrection.
  - **Arbiter**: Buffer/debuffer. Damage amps, resistance shreds, crowd control.
  - **Inquisitor**: Battle support. Damage-to-healing conversion, smite abilities.

#### 5. Machinist (Artificer Analog)
- **Role**: Pet Class / Turret / Utility
- **Key Stat**: Intelligence / Dexterity
- **Core Mechanic**: **Scrap Pool** — defeated enemies and destroyed objects drop Scrap. Spend Scrap to deploy turrets, drones, or modify gear mid-mission.
- **Subclasses**:
  - **Engineer**: Defensive structures. Turrets, barriers, healing stations.
  - **Mechromancer**: Combat pets. Drone swarms, mech suits, autonomous allies.
  - **Alchemist**: Consumables and grenades. Buff potions, debuff bombs, healing mist.

### Progression Systems
- **Level Cap**: 20 (D&D tiered progression)
- **Ability Points**: Each level grants points to unlock/upgrade abilities in a MOBA-style skill tree.
- **Feat System**: At levels 4, 8, 12, 16, 19, choose a Feat (like D&D) that provides a major passive or active ability.
- **Renown**: Account-wide reputation with factions, unlocking vendors, cosmetics, and narrative content.
- **Leaderboards**: Separate rankings for each game mode, updated weekly.

---

## 5. COMBAT SYSTEM: "THE NEXUS ENGINE"

### Real-Time D&D Translation
| D&D Concept | Real-Time Implementation |
|-------------|-------------------------|
| **Action** | Primary abilities (Q/W/E) with cooldowns |
| **Bonus Action** | Instant abilities (mouse thumb buttons) that don't trigger global cooldown |
| **Reaction** | Counter-abilities triggered by enemy tells (parry, dodge roll, spell reflect) |
| **Movement** | WASD with stamina-based sprint/dodge. No grid. Freeform 360° |
| **Attack Roll** | Accuracy stat vs. Evasion stat + RNG roll. Determines hit/miss/crit |
| **Saving Throw** | Player stat vs. Ability DC. Prompted visually ("DEX SAVE!" indicator) |
| **Initiative** | Not used. Real-time priority based on ability cast times and positioning |
| **Spell Slots** | Aether Cells — regenerate in combat via dealing/taking damage |

### MOBA Mechanics in ARPG Context
- **Last Hitting**: Killing blows on enemies grant bonus Aether and experience.
- **Warding**: Deployable scanner beacons that pierce the Fog in an area. Essential for survival.
- **Objectives**: In session modes, capturing control points or destroying structures yields team-wide buffs.
- **Item Active Effects**: Legendary gear has active abilities (like MOBA item actives) bound to keys.

### Environmental Combat
- **Verticality**: Grapple hooks, wall-running, double jumps. High ground grants Advantage on ranged attacks.
- **Destructible Cover**: Cover blocks line of sight but can be destroyed.
- **Fog Manipulation**: Use abilities to clear Fog temporarily, or weaponize it against enemies.

---

## 6. GAME MODES

### A. OPEN WORLD: THE FRINGE (MMORPG Hub)
- **Seamless Zone**: A persistent, open-world sector where players gather, trade, form parties, and undertake dynamic events.
- **Safe Zones**: Architect-controlled outposts with vendors, crafting stations, and social hubs.
- **Contested Zones**: PvP-enabled areas with rare resources. Faction warfare for control points.
- **World Events**: Scheduled massive battles against Fog Titans. Hundreds of players cooperating.
- **No Session Timer**: Play at your own pace. This is where persistent progression happens.

### B. EXTRACTION INSTANCES (PvE & PvP)
*Inspired by Escape from Tarkov / Hunt: Showdown*
- **The Setup**: Enter a corrupted Sector with a small squad (1–3 players). The map is dense with Fog, elite enemies, and environmental hazards.
- **The Objective**: Locate and extract Aether Cores while surviving. Cores come in tiers (Common to Prime). Higher tiers = higher risk.
- **PvE Variant**: Purely against AI. Fog Tides escalate over time, forcing extraction.
- **PvP Variant**: Multiple squads enter simultaneously. Can choose to cooperate or hunt each other. Death means losing carried gear (hardcore risk/reward).
- **Session Length**: 20–40 minutes.
- **Leaderboard**: Ranked by extraction value, survival rate, and PvP efficiency.

### C. PARTY DUNGEONS (PvE)
*Classic MMORPG dungeon design with MOBA boss mechanics*
- **Group Size**: 4 players (1 Tank, 1 Healer, 2 DPS — flexible but optimal).
- **Structure**: 3–4 mini-boss encounters leading to a Sector Lord.
- **Mechanics**: Bosses have MOBA-style ability kits with clear tells, phases, and enrage timers.
- **Difficulty Tiers**: Normal → Heroic → Mythic → Nightmare. Higher tiers add mechanics and affixes.
- **Loot**: Guaranteed drops from bosses, with weekly lockouts for highest-tier rewards.
- **Session Length**: 30–60 minutes.
- **Leaderboard**: Speedrun rankings and no-death challenge rankings.

### D. SOLO CHALLENGES (PvE)
*Mega Man-style stage select*
- **The Gauntlet**: Individual Sector Lord stages designed for solo play.
- **Protocol Mastery**: Complete challenges using specific Protocols for bonus rewards.
- **Ironman Mode**: One life. No checkpoints. Ultimate test of skill.
- **Session Length**: 10–20 minutes.
- **Leaderboard**: Clear time rankings, score-based rankings (style points for combos and no-damage runs).

### E. BATTLEGROUNDS (PvP)
*MOBA-inspired competitive PvP*
- **Team Size**: 5v5 or 10v10.
- **Map Design**: Symmetrical maps with lanes, jungle sectors, and central objectives.
- **Mode Variants**:
  - **Aether Clash**: Classic MOBA. Destroy the enemy Nexus structure. Minions spawn in lanes. Players level up during the match (session-based, not persistent).
  - **Domination**: Hold control points to drain enemy tickets.
  - **Annihilation**: Team deathmatch with respawn limits.
- **Character Rules**: Players enter with their persistent character but stats are **normalized** to a competitive baseline. Gear affixes are disabled; only Protocol choices and skill matter.
- **Session Length**: 15–30 minutes.
- **Leaderboard**: Elo-based ranked system with seasonal rewards.

### F. SEASONAL EVENTS
- **The Fog Surge**: Monthly event where the Fog expands, introducing new Sectors, new Sector Lords, and limited-time Protocols.
- **Faction Wars**: Week-long PvP campaigns where factions compete for territory control.
- **The Protocol Trials**: Rotating daily/weekly challenge modes with unique modifiers (e.g., "All abilities cost double but deal triple damage").

---

## 7. PROGRESSION & LEADERBOARDS

### Multi-Layered Progression
1. **Character Level**: 1–20. Unlocks abilities, feats, and stat points.
2. **Gear Score**: Average power level of equipped items. Determines access to high-tier content.
3. **Protocol Collection**: Account-wide library of acquired Sector Lord abilities.
4. **Renown**: Faction reputation for narrative and economic benefits.
5. **Mastery**: Per-weapon, per-class, per-mode experience that unlocks cosmetic and minor stat bonuses.

### Leaderboard Categories
| Category | Metric | Reward |
|----------|--------|--------|
| **Extraction King** | Total Aether value extracted | Unique extraction-themed cosmetics |
| **Sector Lord Slayer** | Fastest solo Sector Lord kills | Title + mount |
| **Dungeon Speedster** | Fastest party dungeon clears | Guild banner + aura |
| **Battleground Champion** | Highest PvP Elo | Seasonal armor skin |
| **Ironman Survivor** | Longest Ironman streak | Exclusive portrait frame |
| **Fog Walker** | Most Fog-explored distance | Cartography-themed gear |

### Seasons
- **3-Month Seasons**: Each season introduces a new Sector, new Sector Lord, new Protocols, and a balance patch.
- **Seasonal Characters**: Optional fresh-start servers where everyone begins at level 1 for a race to max level and leaderboards.
- **Battle Pass**: Free and premium tracks with cosmetics, currency, and convenience items (no pay-to-win).

---

## 8. ECONOMY & CRAFTING

### Currency
- **Credits**: Standard currency from vendors and quests.
- **Aether Shards**: Premium currency from extractions and high-tier content. Used for crafting, trading, and cosmetics.
- **Scrap**: Crafting material from dismantling gear.

### Crafting
- **Modification**: Reroll affixes on gear using Aether Shards.
- **Infusion**: Upgrade gear tier, increasing base stats.
- **Protocol Imprinting**: Extract a Protocol from a defeated boss and imprint it onto gear for passive bonuses.
- **Runecrafting**: Create and combine Aether Runes for socketed gear.

### Player Trading
- **The Bazaar**: A player-driven auction house in the Fringe.
- **Direct Trading**: Secure trade windows with inspection.
- **No Bind-on-Pickup for Most Gear**: High-value extraction loot can be sold, creating a thriving player economy.

---

## 9. ART & AUDIO DIRECTION

### Visual Style
- **Neo-Noir Cyberpunk**: High-contrast lighting with deep shadows. Neon accents against industrial grays.
- **Fog Rendering**: Volumetric fog that reacts to light sources and player movement. The Fog is a character, not just an effect.
- **Character Design**: Mega Man-inspired silhouettes with modular armor pieces. Each class has a distinct profile readable in silhouette.
- **UI Design**: Diegetic HUD elements—health bars projected from wrist devices, minimap as a holographic drone companion.

### Audio Design
- **Dynamic Soundscape**: The Fog muffles distant sounds but amplifies close threats. Audio is a gameplay mechanic.
- **Ability Audio**: Each Protocol has a distinct audio signature, allowing skilled players to identify enemy abilities by sound alone.
- **Music**: Synthwave-industrial hybrid. Intensifies during combat and Fog Tides.

---

## 10. TECHNICAL ARCHITECTURE

### Networking
- **Hybrid Server Model**: Open world runs on dedicated shards (100+ players). Session modes use instanced dedicated servers.
- **Rollback Netcode**: Essential for MOBA-style precision combat. Client-side prediction with server reconciliation.
- **Fog Synchronization**: Server-authoritative vision system. Players only receive data about entities they can actually see.

### Anti-Cheat
- **Server-Side Validation**: All hit detection, loot rolls, and progression are server-authoritative.
- **Fog as Anti-Cheat**: The Fog system naturally limits information available to clients, making wallhacks and maphacks inherently difficult.

### Platforms
- **PC (Primary)**: Mouse and keyboard optimized. Full mod support for UI.
- **Consoles**: Controller support with radial menus for abilities.
- **Cross-Play**: Enabled for PvE. PvP has optional input-based matchmaking.

---

## 11. MONETIZATION

### Ethical Free-to-Play
- **Free**: Full game access, all classes, all modes, all levels.
- **Cosmetic-Only Premium**: Skins, emotes, mount variants, weapon effects.
- **Convenience (Non-P2W)**: Stash tabs, character slots, XP boosters (only for alt characters, not mains).
- **Battle Pass**: $10 per season. Contains cosmetics, credits, and crafting materials.
- **No Loot Boxes**: Direct purchase or earnable through gameplay.

---

## 12. DEVELOPMENT ROADMAP

### Phase 1: Foundation (Months 1–12)
- Core combat engine (Nexus Engine)
- 3 classes (Vanguard, Specter, Channeler)
- Open World Fringe (1 zone)
- Solo Challenges (5 Sector Lords)
- Party Dungeons (3 dungeons)

### Phase 2: Expansion (Months 13–24)
- 2 additional classes (Mediator, Machinist)
- Extraction Instances (PvE + PvP)
- Battlegrounds (2 modes)
- Crafting and economy
- Season 1 launch

### Phase 3: Evolution (Months 25+)
- Guild housing and warfare
- Mount and vehicle systems
- Raid content (8-player)
- User-generated content tools
- Console release

---

## 13. UNIQUE SELLING POINTS

1. **The Only Mega Man + MOBA + D&D Hybrid**: No other game combines stage-select boss progression with MOBA combat depth and D&D mechanical richness.
2. **Fog as Gameplay**: The Fog of War is not just visual—it's a systemic threat that shapes every decision.
3. **Session-Based Persistence**: Play a quick 15-minute solo challenge or a 3-hour extraction run. Your progress matters in both.
4. **True Build Freedom**: D&D's open-ended character creation meets ARPG's itemization. No two characters need play the same.
5. **Risk/Reward Extraction**: The adrenaline of hardcore survival games without the punishing permanent loss.

---

## 14. ELEVATOR PITCH

> *Aether Forge: Protocol Zero* is what happens when Mega Man's boss-rush progression, League of Legends' skill-shot combat, Diablo's loot addiction, and D&D's character depth collide in a fog-choked cyberpunk apocalypse. Choose your Runner. Hunt the Sector Lords. Steal their power. Survive the Fog. Climb the leaderboards. Or be consumed.

---

*Document Version 1.0 — Protocol Zero Initiative*
