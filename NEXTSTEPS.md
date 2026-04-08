1. updateUnitsPositions
What to change:
Apply dt to ALL units (player + enemies)
Move first → then handle bounds
Stop using unitInBounds() as a gate
Why:

Right now:

enemies ignore dt
bounds check is logically wrong (pre-check)

👉 This function should become:

“advance simulation by dt”

Not:

“conditionally move stuff if already valid”

2. unitInBounds
What to change:
Stop using it as a precondition
Either:
convert it into a post-check helper
OR replace with clamp/bounce logic entirely
Why:

Bounds are not a yes/no question anymore.

You now have continuous motion → things WILL cross boundaries.

So instead of:

"can I move?"

You need:

"I moved — now fix the result"
3. renderMap
What to change:
Convert all float positions → integer grid positions
(cast or round)
Ensure consistent rule for mapping float → tile
Why:

You now have:

simulation space = float
render space = int grid

👉 This function is the bridge between the two worlds

If you don’t fix this, things “disappear” forever.

4. Input handling (inside main)
What to change:
Reset velocity every frame BEFORE reading input
Treat input as “intent for this frame”, not permanent state
Why:

Right now:

press once → move forever

That’s not input handling—that’s setting a flag and forgetting it.

👉 Proper model:

each frame:
  clear intent
  read input
  apply intent
5. checkCollision
What to change:
Replace exact equality with range-based check
Why:

You switched to floats.

So this:

x == x

is basically useless now.

👉 You need:

distance < threshold

Otherwise collisions will randomly fail.

6. addEnemy
What to change:
Handle edge case when capacity == 0
Why:

Right now doubling zero = zero → permanent failure

Not urgent, but it’s a structural weakness.

7. Game loop (main)
What to change:
Enforce strict order:
process_input
update(dt)
resolve_collisions
render
Why:

Right now it's loosely mixed.

That will become a mess as soon as you add bullets or AI.

8. (Optional but important soon) renderMap → buffer system
What to change:
Stop printing per tile
Build a buffer, print once
Why:

Right now:

flickering
inefficient
tightly coupled rendering

This becomes painful fast once things get more complex.
