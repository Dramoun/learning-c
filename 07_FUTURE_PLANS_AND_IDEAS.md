# Future Plans & Ideas

*Extends `05_PROJECT_N_CONCEPT.md` and `06_PATH_TO_PROJECT_N.md` with three specific threads: the skill tree's role in onboarding, a real Dark/Light fork, and moving to rooms instead of waves. Checked against a couple of real games with related mechanics (Overlord's minion-commanding, The Binding of Isaac's room system) and against the rest of this project's history for consistency.*

## The skill tree as the onboarding spine, not just another system

The instinct is a good one, and it's worth stating plainly: the skill tree doesn't have to be *a* system among several — it can be *the* delivery mechanism for nearly everything else in this doc and in `05`. New commands (already the Commander branch's job), the richer four-verb corpse economy, Command Energy itself, even the Dark/Light fork below — all of these can be things a node *unlocks*, rather than things that ship all at once in a version bump.

This is a well-tested pattern, not a novel risk: Hades' Mirror of Night means a first-time player's decision space is much smaller than a fiftieth run's; Slay the Spire's card and relic pool only grows as wins unlock more of it; Dead Cells gates entire late-game mechanics behind Boss Stem Cells. The common thread is that none of these games hand a new player the full mechanical depth on run one — depth arrives as a reward for playing, which is exactly the "don't overwhelm them" instinct already here.

Concretely: something like one new node available every few cleared rooms, or one per boss, keeps a steady drip rather than a wall of choices at the very start. The exact cadence is easy to tune later — the organizing idea from `05` (every node changes what you can *do*, not just a number) is the part worth locking in early.

## Dark vs. Light: worth making a real fork, not a stat multiplier

Framed as "raise more dumb corpses" vs. "revive fewer smart ones, for more," this maps directly onto branches that already exist in `05`: **Bone** (more summons, stronger base skeletons — this *is* the Dark path already) paired against a new branch for Light. `Blood` (sacrifice, life drain, corpse explosion) already reads as the violent, Dark-leaning end of the tree, so the natural opposite number is a new branch — call it **Grace** for now, easy to rename — covering the Light side.

One suggestion worth considering on top of what you described: instead of Light just being "the same Raise action with better numbers," give it a genuinely different verb. **Dark raises a corpse** — works on anything already dead, cheap, plentiful. **Light converts a still-living enemy** — costs more, requires *not* landing the killing blow (subduing rather than finishing them off), and produces a smarter, faster unit from the rarer Souls/Essence resource instead of Bones. That turns "which enemy do I handle which way" into an actual moment-to-moment decision, not just a shopping choice between two flavors of the same button. This isn't unprecedented — the *Overlord* games, built around commanding a horde of minions in a very similar spirit, use a comparable dominate-vs-eradicate choice when an enemy is defeated, with different rewards depending on which you pick. A related idea already working in a shipped minion-commander game is a good sign this is buildable, not just theoretical.

Worth extending to allies, too, matching your original framing: Dark treats a fallen minion as disposable — there's always another corpse. Light treats one as worth reviving at a real cost, which gives the two playstyles a different emotional shape (attrition vs. attachment), not just different numbers on a sheet.

**A room-level extension worth adding**, prompted by how well this maps onto something already proven elsewhere: The Binding of Isaac already does something similar with its own mirrored room pair — take a costly Devil Deal (trading health for a powerful item) and the game trends toward offering more Devil Rooms; refuse deals for a stretch and it starts offering the safer, free Angel Room instead. Same idea, different reskin: a **Blood Shrine** (Dark-flavored, more likely to show up if you've been sacrificing minions or exploding corpses) versus a **Grace Shrine** (Light-flavored, more likely if you've been sparing enemies and keeping minions alive) gives the Dark/Light fork a physical presence in the run, not just a menu screen — and it's a mechanic that's already been tuned and loved (if occasionally debated) in a very well-known game in this genre.

Sequencing-wise, this should follow the same rule as everything else so far: **Dark ships first.** It's simpler (raise a corpse, done) and it's already the whole `v0.1` plan. Light/Grace is a clearly later addition, once the core Raise-and-command loop has already proven itself fun on its own.

## Rooms, not waves

Worth saying plainly: this isn't a change of direction, it's confirming something already in `05` — the `v0.1` scope already called for five hand-crafted rooms instead of an open world or an endless wave counter, for exactly the reason given here: hand-built, bounded spaces are a tractable solo scope in a way procedural world generation isn't, at least not yet.

Where this touches the existing technical roadmap: Part 4's Exercise 4 was framed as "wave/level progression," and none of that work is wasted by moving to rooms — a room's combat encounter *is* a wave, just spatially contained by walls and a locked door instead of a global survival counter. Spawning a batch of enemies, tracking how many remain, detecting "encounter cleared," scaling difficulty by depth — all the same code patterns, just triggered by entering a room and unlocking a door on clear, rather than an infinite loop of waves. The container changes; the underlying system doesn't.

This is also the natural home for the Blood/Grace shrine idea above — a shrine is just another room type, alongside the treasure/event/shop rooms already sketched in `05`.

## How the three connect

These aren't three separate features bolted together: the skill tree paces *when* new tools show up, Dark/Light shapes *which* tools a given run leans on, and rooms are *where* and *how* both of those get paced and rewarded. `06`'s staged path still holds as the right order to build in — this doc mostly adds detail to steps already on that list (the skill tree, the richer corpse economy, "the long tail") rather than adding new stages. The one genuinely new addition is Grace/Light itself, which slots in as a later skill-tree branch, arriving well after Dark/Bone has already proven the core loop is fun — the same discipline `06` already recommended for Command Energy and the four-verb corpse economy.
