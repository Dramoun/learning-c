# Necromancer Game Design Notes

*A menu of ideas, not a spec — pick, remix, or ignore freely. The goal is to give the Part 4 architecture work an actual direction to pull toward, instead of building state machines and event queues in the abstract.*

## What makes a necromancer *not* a reskinned mage

The usual tell of a "mage with a summon spell" necromancer is that raising the dead is one entry in a spellbook, and the player is still the main combatant. A design that actually commits to the theme usually leans on at least one or two of these:

1. **You're a commander, not a fighter** — fragile alone, dangerous through numbers.
2. **Corpses are a resource with real mechanical weight**, not just aftermath that disappears.
3. **Power has a cost that compounds the more you use it** (corruption, decay, sanity) rather than a clean mana bar.
4. **What you choose to raise matters as much as how much** — composition, not just quantity.

You don't need all four for a learning project — trying to cram all of them in at once will slow you down more than it'll teach you. If you want a nudge: **#1 and #2** pair naturally and map almost directly onto systems you're about to build anyway (waves, events), so they're probably the highest design-value-per-line-of-code pick to start with.

## A resource system built around death

- **Essence** (call it whatever fits — Soul Essence, Marrow, whatever): spent to raise corpses and use necromancer abilities; earned from kills, yours or your minions'.
- Optionally, a second axis — **Corruption**: rises the more you lean on raising/dark abilities. Crossing thresholds could cost you something real (lower max HP, minions occasionally acting up, changes to how the screen reads) rather than being an outright fail state. Gives the player something to push against, which is the "power at a cost" feeling this theme usually leans on.
- Start simple: a single `essence` float on `Game` or the player, spent/earned through a couple of new events, is plenty at first. Corruption can wait until Essence itself feels good.

## Corpses as a real object, not just erased-on-death

Right now, `removeEnemy()` deletes an enemy from the array the instant it dies. Consider a state in between: **alive enemy → Corpse (its own entity, its own symbol, sits on the map) → raised into a Minion, or left to decay.**

This maps directly onto the event system exercise: an `ENEMY_KILLED` event spawns a Corpse instead of just vanishing; a timer-driven `CORPSE_DECAYED` event removes an unclaimed one; a "raise" action near a Corpse fires `UNIT_RAISED`, consuming it and spawning a Minion.

It also creates a real risk/reward moment every fight: stop and raise mid-combat (costs tempo, maybe exposes you) versus keep moving and hope the corpse doesn't decay before the wave ends.

## Minions: your actual army

A small starting roster is plenty — three types, tied to what they were raised from:

| Minion | Raised from | Feel |
|---|---|---|
| Skeleton | most humanoid corpses | balanced melee, your default "line" unit |
| Zombie | a tougher corpse type | slow, tanky, maybe damages nearby things when it finally dies |
| Wraith | a corpse from something that died to magic/fire, or a boss | fast, fragile, ignores walls/terrain — a nice first "special movement rule" unit on a grid |

This is a good place to actually practice Exercise 5's component idea, rather than just filing it under "did it." Instead of one `enum MinionType` with a big `switch` scattered everywhere it's checked, a small `MinionStats` (or a tag plus a lookup table) lets movement, combat, and rendering each ask "what do I need to know about this unit" without caring what raised it. A plain enum + switch is a completely fine first pass if you're not there yet — just flagging where the ECS-lite idea actually gets to prove itself.

## Enemy design: the anti-necromancer roster

- **Regular fodder** — townsfolk, guards. Easy kills, easy corpses, teaches the raise loop.
- **A "Cleric"/"Purifier"** — its gimmick: nearby corpses can't be raised (or take longer to raise) while it's alive. Gives you a reason to prioritize targets beyond "closest," and it's an enemy idea that only makes sense *because* your core mechanic is necromancy — a generic shooter wouldn't have this enemy.
- **A boss every few waves**, themed as a direct foil (a paladin, a witch-hunter) rather than "bigger version of a regular enemy" — reinforces the living-vs-dead framing at the big beats.

## Wave design, with theme

- Rename waves if you want the flavor ("Rising I," "Rising II"...) — purely cosmetic, skip it if it's not your thing.
- Early waves: slow and weak, to teach the raise loop before anything else.
- Introduce the Cleric/Purifier once raising is second nature — the first real tactical wrinkle.
- Boss wave every N waves, as above.
- For the difficulty knob itself, the roadmap doc flags a real choice: linear growth (+2 enemies/wave) is much easier to tune well for a first pass than exponential — you can always add spice later once the base feel is right.

## Verbs / abilities

Instead of only Move and Shoot, a small set of semantic actions gives Exercise 3 something concrete to abstract toward:

- `ACTION_RAISE` — raise the nearest Corpse in range.
- `ACTION_DRAIN` — channel Essence from a nearby living enemy; maybe roots you in place while channeling, which adds real tension since you're meant to be fragile.
- `ACTION_COMMAND` — send minions toward a point or target. Turns "I have an army" into an actual decision instead of passive following.
- `ACTION_SACRIFICE` — destroy your own minion for a burst of Essence. An emergency valve, and interesting because it makes losing a minion sometimes a *choice* rather than only a loss.

You can keep the existing bullet mechanic as-is — reflavor it as a cheap, Essence-free "bone shard" the necromancer throws directly, and let raising/commanding be the deeper layer on top. That keeps most of your existing, working code intact while the new systems carry the theme.

## Win / loss framing

- **GAME_LOSE**: with a fragile, low-HP commander, losing should feel like "I got caught without minions between me and the enemy," not "I ran out of a big HP bar." Worth keeping player HP genuinely low once you wire up player damage.
- **GAME_WIN**: frame it as "the graveyard is fully cleared" rather than a generic win screen. A cheap, high-payoff touch if you build a Corruption stat: branch the win flavor text on it (a "clean" ending vs. one where "the power consumed you") — one stat, one branch at the win screen, for a surprisingly satisfying bit of replay value.

## Rendering — fits your current ASCII renderer as-is

- Player: stay with `P`, or switch to the roguelike-standard `@` — your call.
- Give minions and living enemies visually distinct symbols/casing so the battlefield reads at a glance — e.g., lowercase for your undead (`s` skeleton, `z` zombie, `w` wraith), a different case or glyph set for living threats.
- Corpses get their own symbol (`%`, `+`) distinct from empty ground and from a living unit.
- Down the line, real ANSI color is a cheap, high-impact upgrade whenever you want one — separate from all of Part 4, just a nice-to-have.

## How this maps onto Part 4, at a glance

| Part 4 exercise | Necromancer hook |
|---|---|
| State machine | `GAME_WIN`/`GAME_LOSE` flavor text branching on Corruption |
| Fixed timestep | *(no theme hook — this one just needs to be correct)* |
| Input → actions | `RAISE` / `DRAIN` / `COMMAND` / `SACRIFICE` as new action values |
| Waves | Rising I/II/III, the Cleric enemy, boss waves |
| ECS-lite components | Minion "species" as data, not a giant switch |
| Memory audit | *(no theme hook — just good hygiene)* |
| Config system | Essence costs, Corruption thresholds, minion stats as data |
| Event system | `ENEMY_KILLED` → spawn Corpse, `UNIT_RAISED`, `CORPSE_DECAYED` |

Pick one slice — probably Corpses plus a single Minion type — and get it working end-to-end before adding the rest. A small, working piece of the "true necromancer" feel will teach you more, and be more fun to keep building on, than a big pile of half-wired systems.
