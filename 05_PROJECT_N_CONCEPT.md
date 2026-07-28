# Project N — Concept

*Updates and expands `04_NECROMANCER_GAME_DESIGN.md` with the fuller brainstorm you just brought in — the command-system notes plus the "Project N" summary. Where `04` was scoped to fit inside the current terminal prototype as-is, this assumes the SDL port (Part 5) has happened: dungeon rooms, a camera, and several on-screen allies want real rendering, not a fixed ASCII grid. Treat this as the long-arc document; `04` still holds up as a cheap way to test pieces of this before committing to SDL — more on that in `06_PATH_TO_PROJECT_N.md`.*

## The core bet

Both rounds of brainstorming landed on the same identity independently, which is a good sign: **you are a commander, not a combatant.** Fragile alone, dangerous through the army you direct. Your "spells" are orders, not damage — this is Option B from your notes, and it's the right call: a necromancer who's secretly the strongest thing on the field is just a mage with a reskinned fireball. A necromancer who's the *weakest* unit, backed by an army that only works because you're commanding it well, is a genuinely different feeling — and a rare enough one in the genre to be worth building toward deliberately.

Most "commander" necromancer designs land in one of two bad spots: minions that are passive stat-sticks doing their own thing regardless of what you want, or minions that demand RTS-level micromanagement and turn every fight into a chore. The command system below is aimed at the middle — enough control to feel like a commander, not so much that you're managing a spreadsheet mid-fight.

## The hook: commands, not a spell bar

This is the part that actually differentiates the game, so it's worth being precise about.

**Stances** — persistent modes that apply to every current and future ally until changed:

| Stance | Tank / melee | Ranged | Caster |
|---|---|---|---|
| **Default** | engages nearest threat | maintains distance, fires at will | casts when able |
| **Aggressive** | charges in, takes risks | fires faster, closes distance | uses offensive spells freely |
| **Defensive** | steps forward to intercept / body-block | retreats, prioritizes dodging over damage | conserves resources, defensive spells only |
| **Hold Position** | stops advancing | stops advancing | stops advancing |

**One-shot orders** — momentary, targeted commands layered on top of whatever stance is active: **Focus Target** (everyone attacks one enemy — probably the first one worth building), **Protect Target** (everyone defends one ally), **Advance** (push the army forward, you hold back), **Guard Me** (everyone collapses back toward you), **Harvest Corpses** (pause fighting briefly, gather bones from nearby corpses).

Delivered through a **command wheel** (hold a key, pick an option) instead of a spell hotbar — a small mechanical choice doing real thematic work: you're ordering, not casting.

**Command Energy** — regenerates slowly, spent per command/stance-switch (their own example: Focus Target 10, an aggressive-stance switch 20, a mass-retreat order 30). Turns "which order right now" into an actual tactical decision instead of a free toggle. Sequencing note on when to introduce this is below.

## The economy: what a corpse is worth

Every dead enemy (and, thematically, every dead ally) leaves a corpse — a real, temporary object on the map, not just erased on death. Worth one of:

- **Consume** — restore HP.
- **Harvest** — bones / crafting material.
- **Raise** — a new undead ally.
- **Explode** — AoE damage where it lies.
- *(do nothing)* — it decays and the choice expires.

Five real outcomes from one death is a meaningfully richer decision than "walk over it to pick it up" — you're always deciding what a death is *for*.

## Resources, layered by what they gate

| Resource | Gates | Comes from |
|---|---|---|
| **Bones** | growing the army — summon new allies, basic upgrades | Harvest |
| **Command Energy** | directing the army *right now* — stances, one-shot orders | regenerates over time |
| **Souls / Essence** *(later)* | evolving the army — merges, deep upgrades, skill tree spend | rarer, from bosses/events |

Don't build all three on day one. Bones alone is enough to prove the core loop; Command Energy is the first thing worth adding once stances themselves feel good; Souls/Essence can wait for the skill tree and evolution system.

## Allies: roles first, names second

Thinking in roles keeps the roster open-ended (and is really just Exercise 5's component idea, applied to design instead of code — a role is a small bundle of stats/behavior, not a rigid class):

| Role | Example unit | What it wants from a stance |
|---|---|---|
| Tank / melee | Skeleton Warrior | Aggressive = charges in; Defensive = blocks/intercepts |
| Ranged | Skeleton Archer | Aggressive = fires faster, closes in; Defensive = kites, prioritizes survival |
| Fast / fragile | Spirit | ignores terrain — good for Focus Target / chase duty |
| Area / heavy | Bone Hound *(or similar)* | slow, makes Hold Position genuinely punishing at a chokepoint |
| Support caster | *(later)* | auras, debuffs — Blood/Spirit skill-tree territory |

`v0.1` needs exactly one of these (Tank/melee — Skeleton Warrior). Everything else is what makes "which stance did I pick" actually matter, per your own note: Defensive plays differently once half your army is archers.

## Skill tree — four branches, unlocking behavior, not stats

- **Commander** — new commands (Focus Target, Hold Position, Advance, Protect Ally).
- **Bone** — more summons, stronger base skeletons.
- **Spirit** — auras, debuffs, ghost-type units.
- **Blood** — sacrifice, life drain, corpse explosion.

Worth keeping as an organizing rule: every node changes what you can *do*, not just a number on a sheet. That's what makes leveling feel like it's expanding your options instead of padding a stat total.

## Rooms and runs

- `v0.1`: five hand-crafted rooms, one boss. No procedural generation yet — hand-crafted rooms let you tune how a fight *feels* without also debugging a room generator at the same time.
- Later: procedural room graphs (exactly the kind of graph/algorithm work your own notes already flagged as a good C fit), event rooms (risk/reward choices), shrines, treasure rooms.
- Later still: permanent meta-progression between runs — new starting summons, new room types unlocked. The standard roguelike "one more run" hook.

## Version 0.1 — one clean scope

Your two brainstorms proposed slightly different first slices. Here's a reconciled one, biased toward the smaller of the two on purpose — the entire point of `v0.1` is answering "is commanding undead fun" as cheaply as possible, so anything that doesn't serve that question directly can wait:

- **Player**: movement + a basic melee attack. Nothing fancier — you're not supposed to be the main damage dealer.
- **One ally**: Skeleton Warrior, simple follow-and-fight AI.
- **Three stances**: Default, Aggressive, Defensive. Skip Hold Position and the one-shot orders for now — add Focus Target first, right after `v0.1`, since it's the most immediately satisfying to test and your own notes already flag it as the natural first pick.
- **Command Energy: not yet.** Let stance-switching be free until you know the stance mechanic itself is fun. A cost added too early muddies whether a flat reaction is about the mechanic or the friction on top of it.
- **Corpse economy: simplified to Bones-only.** Enemies drop Bones directly; spend Bones to summon/upgrade. The full Consume/Harvest/Raise/Explode decision is a great `v0.2` addition once fight → grow army → fight better is already proven fun on its own.
- **No skill tree yet.** One of your two source docs included a small one in `v0.1`, the other didn't — leaving it out is the more disciplined read, since a skill tree is a meta-progression layer, a different question from "is real-time commanding fun."
- Five rooms, one boss, Bones. Done.

If that slice is fun, everything above is additive, not a rewrite. If it isn't, better to find out with a five-room prototype than after building a skill tree, three unit types, and a room generator around it.
