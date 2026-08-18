# Path to Project N

*This is `02_ROADMAP.md`'s sequel — picks up from "after Part 4, and after the Part 5 SDL port" and sketches how you actually get from where the code is today to `05_PROJECT_N_CONCEPT.md`. A suggested order, not a spec.*

## Naming the gap, honestly

Project N is a considerably bigger scope than the current terminal prototype, and bigger than the original Part 4 exercise list too — procedural rooms, multiple AI-driven unit roles, a stance system, a skill tree, an evolution/merge system, and run-to-run meta-progression is realistically a long-running project, not a next-weekend one. That's fine — roguelikes are built out of exactly this kind of "keep adding a system, keep it fun at every stage" incremental growth — but worth naming plainly so the scope is a choice, not a surprise later.

The reassuring part: none of the Part 4 work is a detour from this. State machines, a real timestep, component-style data, an event queue, config-driven values — that's the actual toolkit procedural rooms and multi-unit AI get built from. Your own notes already say as much.

## A suggested staged path

**Stage 0 — finish Part 4, and sneak in one small experiment.**
Keep going in the terminal prototype the way `02_ROADMAP.md` already lays out. While you're there, consider adding the smallest possible version of an *ally*: a single friendly unit that follows the player and auto-attacks the nearest enemy, no stances yet. Structurally this is just a second array shaped like your existing `Enemies` — a good use of Exercise 5's component work — and it's the cheapest possible way to find out whether "having something fight alongside you" feels good at all, before any of the rest of the design exists.

**Stage 1 — prototype the stance system, still cheaply.**
Once there's at least one ally to command, add Default / Aggressive / Defensive as a single enum its AI checks. "Is commanding fun" is answerable in ugly ASCII exactly as well as in a polished SDL scene, and it's much faster to iterate on a stance's *feel* before there's a renderer, camera, and sprite set built around it. This is really just your own closing advice — "if it's not fun with a few pieces, more systems won't fix it" — applied at the earliest, cheapest point it can be.

**Stage 2 — Part 5: the SDL port.**
Once the stance loop earns its keep, do the port your existing roadmap already promised — real rendering, a camera, proper input. The state machine, component structure, and event queue carry over essentially unchanged, which was always the point of doing Part 4 first.

**Stage 3 — `Project N v0.1` proper.**
Build the disciplined slice from `05_PROJECT_N_CONCEPT.md`: five hand-crafted rooms, one ally type, three stances, Bones, one boss. This is where it stops being "a prototype with an ally" and starts being the actual game.

**Stage 4 and beyond — roughly in this order:**
1. Focus Target, then Hold Position — cheapest, most immediately satisfying additions.
2. A second ally role (Archer) — the first point the stance table actually starts to matter, since Defensive means something different once your army isn't all melee.
3. Command Energy, once stances are proven fun on their own.
4. The richer corpse economy (Consume / Harvest / Raise / Explode) in place of flat Bone drops.
5. The skill tree, Commander branch first — it directly unlocks more of the command system you've already built.
6. Evolution/merging, Souls/Essence, procedural rooms, room variety, meta-progression — the long tail, added once the core loop has already earned several stages of trust.

## Open decisions worth making yourself

A few places your two source documents didn't fully agree, flagged rather than settled — you're the one synthesizing this into a final plan:

- **Corpse economy richness in `v0.1`** — full four-verb system, or flat Bone drops? Leaning flat, for the reasons in the concept doc, but it's a real judgment call.
- **Skill tree in `v0.1`** — one source included a tiny one, the other didn't.
- **Whether Hold Position ships with the first three stances or waits** — it's cheap to implement (a unit doing nothing is easy code), so this is really about whether it earns its spot in the command wheel this early.
- **Unit and resource naming** — Bones/Souls/Essence, Command Energy, the exact roster names — all placeholders in both source docs. Worth deciding early, since renaming later touches a lot of code and text.

None of this needs an answer today — it gets clearer once `v0.1`'s single ally and three stances are actually running.
