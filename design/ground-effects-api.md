# Ground Effects API — Playerbots

## Goal
Prevent bots from standing in harmful ground effects (fire, poison, void zones, etc.) by detecting active danger zones and pathfinding to safe positions.

## Status: Planned (not implemented)

---

## Existing Infrastructure (already in playerbots)

| Component | Location | What it does |
|---|---|---|
| `AvoidAoeAction` | `MovementActions.cpp:1856` | Detects DynamicObjects, traps, trigger units; calls `FleePosition()` |
| `AreaDebuffValue` | `AoeValues.cpp` | Checks if bot has periodic-damage aura from ground effect |
| `NearestTrapWithDamageValue` | `NearestGameObjects.cpp` | Scans for `GAMEOBJECT_TYPE_TRAP` with damaging spells |
| `FleePosition` | `MovementActions.cpp:2215` | Calculates radial escape vector from danger center |

## Gaps

1. **Missing GO types** — Only `GAMEOBJECT_TYPE_TRAP` (6) is scanned. Need types 12 (`AREADAMAGE`) and 30 (`AURA_GENERATOR`).
2. **No persistent registry** — Bots react only when already inside an effect. No awareness of zones they haven't entered yet.
3. **Radial flee is dumb** — Moves directly away from one point; can flee into other danger zones.
4. **No party coordination** — Each bot flees independently; healer may run out of range.

## Implementation Plan

### Phase 1 — Core Detection (3-4 days)

- Extend `NearestGameObjects` to scan GO types 12 and 30
- Build `DangerZonesValue` — per-map spatial index of active zones with position, radius, spell ID, caster, expiry
- Hook `DynamicObject` create/remove events to keep registry live

### Phase 2 — Smarter Flee (2-3 days)

- Replace radial flee with `PathGenerator`-based escape: find nearest navmesh point outside all active danger zones
- Party coordination: tank evaluates safe direction, shares via cross-bot value, all members flee that way

### Phase 3 — Data Entry (2-5 days)

- Classify spell IDs for ground effects per dungeon/raid. Start with leveling dungeons (~30-50 spells), expand to raids as needed (~150+ spells across MC/BWL/AQ40/NAXX).
- Spell registry format: `{ spellId, radiusOverride, priority }`

## File Locations

All new code lives in `modules/mod-playerbots/src/`:
- `Ai/Base/Value/DangerZonesValue.{h,cpp}` — new
- `Ai/Base/Actions/MovementActions.cpp` — extend existing `AvoidAoeAction`/`FleePosition`
- `Ai/Base/Value/NearestGameObjects.cpp` — extend existing scanner
- `PlayerbotAIConfig.{h,cpp}` — config options

## Out of Scope

- Boss phase transitions (Onyxia air, Ragnaros submerge)
- Interrupt prioritization
- Threat management
- Per-boss scripted mechanics (these go in instance scripts, not the AI)
