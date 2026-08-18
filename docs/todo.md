1. bullet moving
2. bullet dead on map leave
3. enemy dying
4. player cannot be move outside the map
5. input System Abstraction

Objective: Learn to separate "what button was pressed" from "what that button means in your game". Build a reusable input layer.

Concepts to learn:

    Enums for semantic actions
    Mapping/binding system
    Single Responsibility Principle
    Why abstraction reduces coupling

The problem with your current code:

You're checking for 'w', 'a', 's', 'd' directly in your game logic. What if:

    You want to add arrow keys?
    You want to support controller input (later with SDL)?
    You want to let players rebind keys?
    Different game states need different key meanings?

Your task:

    Define semantic actions - Create an enum for what the player can DO, not what keys they press:
        Moving directions
        Shooting
        Pausing
        Quitting
        Menu navigation

    Create a mapping function - This translates raw keys → actions
        Take a character as input
        Return an action enum
        This is the ONLY place that knows 'w' means "move up"

    Refactor input handling - Your game logic should only deal with actions, never raw keys

    Think about state-dependent input - Should the same key do different things in different states?

Questions to think about:

    How would you add arrow key support? (Hint: you only change one function)
    How could you let players rebind keys? (Store the mapping in a config?)
    What actions make sense in MENU vs PLAYING vs PAUSED states?

Why this matters:

When you move to SDL, you'll get SDL_Scancode and SDL_Keycode instead of chars. If your game logic depends on chars, you rewrite everything. If it depends on actions, you just rewrite the mapper.

Challenge:

    Add arrow key support alongside WASD
    Make 'p' and ESC both trigger pause
    Add a "debug mode" toggle with a different key
6. separet units and projectiles
7. units have stats
8. func that create default units with xy stats
9. Memory Audit & Cleanup

Objective: Find and fix every memory leak, invalid access, and unsafe operation before complexity explodes.

Concepts to learn:

    Stack vs heap memory
    Memory leak detection
    Valgrind basics
    Safe malloc patterns
    Resource ownership

Your task:

    Install and learn Valgrind (should be in your package manager):

    sudo apt-get install valgrind

    Compile with debug symbols:

    gcc -g -O0 hello.c -o game -lm

        -g adds debug info
        -O0 disables optimizations (easier debugging)

    Run Valgrind:

    valgrind --leak-check=full --track-origins=yes ./game

    Understand the output:
        "definitely lost" = you malloc'd but never freed
        "still reachable" = you have a pointer but didn't free (less critical)
        "invalid read/write" = you accessed memory you don't own

    Fix everything Valgrind reports:
        Are you freeing bullets->units? (Check destroyGame)
        What happens if you quit mid-game? (Ctrl+C path)
        Are you checking every malloc return value?

    Add defensive programming:
        Check malloc returns: if (!ptr) { /* handle error */ }
        NULL-check before free (though free(NULL) is safe)
        Set pointers to NULL after freeing

Questions to think about:

    Who "owns" the memory? (Player owns velocity, but who owns the player?)
    If malloc fails, what should you do? (Graceful shutdown? Error message?)
    What's the difference between a leak and an invalid access?

Why this matters:

SDL adds textures, surfaces, audio, fonts—all heap-allocated. If you can't manage memory in a simple terminal game, you'll drown in SDL. Start with clean habits NOW.

Challenge:

    Run your game for 100 waves—does memory usage grow? (Leak in your wave system?)
    Test every exit path (quit, game over, Ctrl+C)—do they all clean up?
    Add an allocation counter—how much memory does your game use?
10. Configuration System

Objective: Make your game data-driven. Separate tunable values from code so designers (or you) can experiment without recompiling.

Concepts to learn:

    Data-driven design philosophy
    File I/O in C
    Parsing text files
    String manipulation (sscanf, strcmp)
    Why hardcoding is bad

Your task:

    Identify hardcoded values in your game:
        Map size (10x10)
        Player speed (30.0f)
        Bullet speed (-5.0f)
        Shoot cooldown (30.0f threshold)
        Max bullets (100)
        Seconds per frame (0.016f)

    Create a config file format - Design something simple and readable:

    # Game Configuration
    MAP_WIDTH=20
    MAP_HEIGHT=15
    PLAYER_SPEED=30.0
    BULLET_SPEED=5.0

    Write a parser:
        Open file with fopen()
        Read line by line with fgets()
        Parse each line with sscanf() (look up the format string!)
        Handle comments (lines starting with #)
        Handle invalid lines gracefully

    Store config in Game struct - Add fields for these values

    Use config throughout your code - Replace hardcoded numbers with game->config.playerSpeed

Questions to think about:

    What happens if the config file is missing? (Default values? Error?)
    What if a value is invalid? (Negative speed? Zero bullets?)
    Should you validate ranges? (Speed between 1-100?)
    How do you handle comments and blank lines?

Why this matters:

Professional games separate code from data. Designers tweak JSON/XML files, not C code. This is your first step toward a real content pipeline.

Challenge ideas:

    Add an in-game "reload config" command
    Support multiple config files (dev.cfg vs release.cfg)
    Add config values for enemy spawn rates, wave difficulty scaling
    Write a config validator that checks for errors before the game starts
11. Simple Event System

Objective: Decouple systems by using events instead of direct function calls. Learn why loose coupling matters.

Concepts to learn:

    Observer pattern
    Event queues
    Decoupling
    Why direct calls create dependencies
    Publish/subscribe model

The problem:

Right now, when a bullet hits an enemy, you directly call:

removeEnemy(game->enemies, e);

But what if you also need to:

    Play a sound?
    Spawn a particle effect?
    Update score?
    Update kill counter?
    Check if wave is complete?

Do you add all those calls in the collision code? That's tight coupling.

Your task:

    Define event types:
        What interesting things happen in your game?
        ENEMY_KILLED, PLAYER_HIT, WAVE_COMPLETE, BULLET_FIRED, etc.

    Create an event struct:
        Type (enum)
        Optional data (void pointer? or specific data structs?)
        Timestamp?

    Build a simple event queue:
        Fixed-size array (64 events)
        Count of events
        Push and process functions

    Emit events instead of direct calls:
        In collision code: push ENEMY_KILLED event
        Don't remove the enemy yet!

    Process events in one place:
        After all updates, loop through event queue
        Handle each event type
        Clear queue for next frame

    Think about order:
        Should events be processed immediately or end-of-frame?
        Can events trigger other events? (Cascade problem)

Questions to think about:

    What's the advantage of this extra complexity?
    How do you pass data with events? (Which enemy was killed?)
    Should events persist across frames, or clear each frame?
    What if you run out of queue space?

Why this matters:

Decoupling is critical for large projects. Your collision code shouldn't know about:

    Sound system
    Particle system
    UI system
    Score system

They just listen for events and react. This is how big engines stay maintainable.

Challenge:

    Add a scoring system that listens for ENEMY_KILLED events
    Add a "combo" system (killing enemies quickly = bonus points)
    Add an event log that prints all events for debugging
    Make wave completion trigger via events instead of checking enemy count directly
