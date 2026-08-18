Game Engine Roadmap – Part 5: SDL2 Integration

Goal: Port your terminal game to SDL2, piece by piece, understanding what SDL replaces and what stays the same. Learn that SDL is just better I/O—your architecture remains intact.
Before You Start: Mental Model

What you already have:

    Game loop with fixed timestep ✅
    State machine ✅
    Object management (players, enemies, bullets) ✅
    Input abstraction ✅
    Collision detection ✅
    Event system ✅

What SDL gives you:

    Window creation (instead of terminal)
    Texture rendering (instead of printf chars)
    Input events (instead of readKey())
    Precise timing (instead of clock_gettime wrapper)
    Audio, fonts, and more (bonus features)

Critical insight: You're not rewriting your game. You're replacing the I/O layer.
Exercise 1: SDL Initialization & Window Creation

Objective: Understand SDL's initialization system and create your first window.

Concepts to learn:

    SDL subsystems (video, audio, timer, events)
    Initialization and cleanup patterns
    SDL_Window and SDL_Renderer
    Error handling in SDL

Your task:

    Understand SDL subsystems:
        SDL is modular—you initialize only what you need
        SDL_INIT_VIDEO - for graphics
        SDL_INIT_AUDIO - for sound (later)
        SDL_INIT_TIMER - for timing functions
        What happens if initialization fails?

    Study the initialization pattern:

    SDL_Init(flags) → Create Window → Create Renderer → ... → Destroy Renderer → Destroy Window → SDL_Quit()

        Why this order?
        What's the relationship between Window and Renderer?

    Add SDL context to your Game struct:
        Where should you store the window pointer?
        Where should you store the renderer?
        Should these be separate from your game logic structs?

    Replace your terminal setup:
        Instead of enableRawMode(), what do you call?
        Instead of printf("\033[H\033[J"), what do you use?

    Handle errors properly:
        Every SDL function can fail
        SDL_GetError() tells you why
        What should you do if window creation fails?

Questions to think about:

    Why separate Window and Renderer? (Hint: software vs hardware rendering)
    What window flags might you want? (Fullscreen? Resizable?)
    How do you convert your 10x10 grid to pixel dimensions?
    Should you make the window size configurable?

Key insight: Your createGame() function now needs to initialize SDL, but the rest of your game struct stays the same.

Challenge:

    Make window size configurable (from your config file!)
    Add error messages that show SDL_GetError() output
    Try creating the window without initializing SDL—understand the error
    Experiment with different window flags (borderless, resizable, etc.)

Exercise 2: The SDL Event Loop

Objective: Replace your terminal input with SDL's event system. Understand the difference between polling and blocking.

Concepts to learn:

    Event-driven architecture
    SDL_Event union
    Event types (keyboard, mouse, window, quit)
    Event polling vs event waiting

Your task:

    Understand SDL_Event:
        It's a union—what does that mean?
        Different event types store different data
        How do you check what type of event you received?

    Study the event loop pattern:

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // ...
            case SDL_KEYDOWN:
                // ...
        }
    }

        Why a while loop, not if?
        When does SDL_PollEvent return false?
        What's the difference between SDL_PollEvent and SDL_WaitEvent?

    Replace your readKey() function:
        Create handleSDLInput(struct Game *game)
        Poll for events
        Handle SDL_QUIT (clicking X button)
        Handle SDL_KEYDOWN and SDL_KEYUP

    Map SDL keys to your actions:
        Remember your input abstraction from Part 4?
        Update your mapKeyToAction() function
        SDL uses SDL_Scancode (physical keys) vs SDL_Keycode (logical keys)
        Which should you use and why?

    Handle key state properly:
        Terminal input: key is pressed this frame or not
        SDL input: key down event, key up event, or check state
        Should movement stop when you release a key?

Questions to think about:

    Why poll multiple events per frame? (Hint: what if player presses multiple keys?)
    What's the difference between event.key.keysym.scancode and event.key.keysym.sym?
    How do you handle continuous movement (hold key) vs discrete actions (tap key)?
    Should you process events before or after update?

Common mistake to avoid:

// WRONG - only processes one event per frame
if (SDL_PollEvent(&event)) { ... }

// RIGHT - processes all events in queue
while (SDL_PollEvent(&event)) { ... }

Challenge:

    Add mouse support (click to shoot in direction of cursor)
    Handle window resize events
    Add a "fullscreen toggle" key
    Print all events to console to see what SDL generates

Exercise 3: Basic Rendering with SDL_Renderer

Objective: Replace your character-based rendering with SDL's 2D rendering API. Understand render targets and the render loop.

Concepts to learn:

    Render targets (back buffer)
    Color representation (RGBA)
    SDL_Rect for positioning
    Clear, draw, present cycle
    Coordinate systems

Your task:

    Understand the rendering cycle:

    SDL_RenderClear() → Draw everything → SDL_RenderPresent()

        Why clear the screen every frame?
        What's the "back buffer"?
        When does the image actually appear?

    Replace your renderMap() function:
        Instead of building a char array, you'll draw rectangles
        Each grid cell becomes a pixel rectangle

    Calculate positions:
        Your game uses grid coordinates (0-9, 0-9)
        SDL uses pixel coordinates (0-640, 0-480, or whatever your window size is)
        How do you convert? pixel_x = grid_x * cell_width

    Draw different objects with different colors:
        Player: one color
        Enemies: another color
        Bullets: another color
        Background: another color
        Use SDL_SetRenderDrawColor() before each draw

    Implement the basic renderer:
        Clear to background color
        Draw grid cells (optional visual aid)
        Draw enemies (loop through array)
        Draw bullets (loop through array)
        Draw player
        Present

Questions to think about:

    Why draw player last? (Hint: layering/z-order)
    How big should each grid cell be in pixels?
    Should you draw the grid lines? (Helps debug positioning)
    What happens if you forget SDL_RenderPresent()?

Coordinate system gotcha:

    Your terminal: (0,0) is top-left, Y increases downward ✅
    SDL: (0,0) is top-left, Y increases downward ✅
    Good news: they match! No conversion needed.

Challenge:

    Make cell size configurable
    Add a background color that's different from empty cells
    Draw a border around the play area
    Add a simple HUD (score, wave number) using colored rectangles
    Make the player pulse/flash when hit (needs timer)

Exercise 4: Texture Loading & Sprite Rendering

Objective: Move beyond colored rectangles. Load images and render them as sprites.

Concepts to learn:

    SDL_Surface vs SDL_Texture
    Image loading (SDL_image library)
    Texture rendering with source and destination rects
    Resource management

Your task:

    Understand SDL_image:
        SDL can load BMP natively
        SDL_image adds PNG, JPG, etc.
        You need to initialize it: IMG_Init(IMG_INIT_PNG)
        And clean it up: IMG_Quit()

    Learn Surface vs Texture:
        SDL_Surface = raw pixel data in RAM
        SDL_Texture = GPU-ready image
        Flow: Load image → Surface → Texture → Destroy Surface → Render Texture
        Why this two-step process?

    Create sprite loading function:
        Takes a filename
        Returns an SDL_Texture*
        Handles errors (file not found, invalid image)
        Don't forget to free the surface!

    Add textures to your structs:
        Should each Unit have its own texture?
        Or should you have a texture cache? (One texture, many units)
        Think about memory usage

    Update rendering to use textures:
        SDL_RenderCopy() instead of SDL_RenderFillRect()
        Define destination rect (where to draw)
        Optional: define source rect (what part of image to draw)

    Create simple placeholder sprites:
        Don't worry about art yet
        Use a paint program to make 32x32 colored squares
        Player: blue square
        Enemy: red square
        Bullet: yellow square

Questions to think about:

    When should you load textures? (Init time? Or lazy load?)
    Where should you store them? (Game struct? Global? Texture manager?)
    What happens if you forget to destroy textures? (Memory leak)
    How do you scale a 32x32 image to fit your grid cell?

Common mistake:

// WRONG - creates new texture every frame
SDL_Texture *t = loadTexture("player.png");
SDL_RenderCopy(renderer, t, NULL, &rect);
// Forgot to destroy - memory leak!

// RIGHT - load once, render many times
// In init:
game->playerTexture = loadTexture("player.png");
// In render loop:
SDL_RenderCopy(renderer, game->playerTexture, NULL, &rect);
// In cleanup:
SDL_DestroyTexture(game->playerTexture);

Challenge:

    Create a texture manager (hash map of filename → texture)
    Add sprite animation (player has multiple frames)
    Add rotation (bullets point in direction of travel)
    Implement sprite flipping (player faces left/right)

Exercise 5: Timing & Frame Rate Control

Objective: Replace your custom timing with SDL's timing functions. Implement frame rate limiting and delta time properly.

Concepts to learn:

    SDL_GetTicks() vs SDL_GetPerformanceCounter()
    Frame rate limiting
    VSync vs manual delay
    Delta time precision

Your task:

    Understand SDL timing functions:
        SDL_GetTicks() - milliseconds since SDL_Init (Uint32)
        SDL_GetPerformanceCounter() - high precision counter (Uint64)
        SDL_GetPerformanceFrequency() - ticks per second
        Which should you use and why?

    Replace your getTimeInSeconds() function:
        Your Part 4 version used clock_gettime()
        SDL version uses performance counter
        How do you convert ticks to seconds?

    Implement frame rate limiting:
        Option 1: VSync (SDL_RENDERER_PRESENTVSYNC flag)
        Option 2: Manual delay (SDL_Delay())
        What are the tradeoffs?

    Maintain your fixed timestep accumulator:
        This doesn't change! Your Part 4 code still works
        You're just using SDL functions to measure time
        Verify your physics still runs at consistent speed

    Add frame timing debug info:
        Calculate actual FPS
        Measure frame time
        Print to console or draw on screen
        Is your game running at target framerate?

Questions to think about:

    Why use performance counter instead of GetTicks?
    What's the precision difference? (milliseconds vs microseconds)
    What happens if you enable VSync AND call SDL_Delay?
    Should you cap framerate if VSync is enabled?

Frame pacing patterns:

// Pattern 1: VSync (simplest)
SDL_CreateRenderer(..., SDL_RENDERER_PRESENTVSYNC);
// No delay needed, driver handles it

// Pattern 2: Fixed delay (predictable, but wastes CPU)
frameStart = SDL_GetTicks();
// ... game logic ...
frameTime = SDL_GetTicks() - frameStart;
if (frameTime < targetFrameTime) {
    SDL_Delay(targetFrameTime - frameTime);
}

// Pattern 3: Precise timing (what you built in Part 4)
// Use accumulator for updates, render as fast as possible
// This is what most engines do

Challenge:

    Implement all three patterns, let user choose via config
    Add frame time graph (visualize frame consistency)
    Measure how much time update vs render takes
    Test with different target framerates (30, 60, 120, 144 FPS)

Exercise 6: Text Rendering with SDL_ttf

Objective: Add text to display score, wave number, and game state. Learn SDL's font rendering system.

Concepts to learn:

    TTF font loading
    Rendering text to textures
    Text caching vs re-rendering
    Font sizes and styles

Your task:

    Initialize SDL_ttf:
        TTF_Init() at startup
        TTF_Quit() at shutdown
        Handle initialization failure

    Load a font:
        TTF_OpenFont(filename, size)
        You need a .ttf file (use a free one, or system font)
        What size should you use?
        Don't forget to close it with TTF_CloseFont()

    Understand text rendering flow:

    Create surface from text → Convert to texture → Render → Destroy texture

        TTF_RenderText_Solid() - fast, no anti-aliasing
        TTF_RenderText_Blended() - slow, smooth
        Which should you use?

    Create a text rendering helper:
        Takes text, position, color
        Handles the surface → texture conversion
        Cleans up properly
        Should you cache textures or create them each frame?

    Add HUD elements:
        Current wave number
        Score (if you added scoring)
        Player health
        Game state ("PAUSED", "GAME OVER", etc.)

Questions to think about:

    Should you re-render text every frame? (Expensive!)
    How do you cache text that doesn't change often?
    How do you handle text that changes every frame? (score counting up)
    What happens if text is too long for the screen?

Text rendering gotcha:

// WRONG - recreates texture every frame (slow!)
void render() {
    SDL_Texture *text = createText("Score: 100");
    SDL_RenderCopy(renderer, text, NULL, &rect);
    SDL_DestroyTexture(text);  // Creates garbage for GC
}

// BETTER - cache static text
void init() {
    game->scoreLabel = createText("Score:");
}
void render() {
    SDL_RenderCopy(renderer, game->scoreLabel, NULL, &rect);
}

// BEST - only recreate when value changes
void updateScore(int newScore) {
    if (newScore != game->score) {
        SDL_DestroyTexture(game->scoreTexture);
        char buffer[32];
        sprintf(buffer, "Score: %d", newScore);
        game->scoreTexture = createText(buffer);
        game->score = newScore;
    }
}

Challenge:

    Add a message system (display "Wave 2!" for 2 seconds then fade)
    Center text on screen (calculate position from text width/height)
    Add different font sizes for different UI elements
    Implement text wrapping for long messages

Exercise 7: Audio with SDL_mixer

Objective: Add sound effects and background music. Complete the sensory experience.

Concepts to learn:

    Audio initialization
    Sound effects vs music
    Channels and mixing
    Audio formats

Your task:

    Initialize SDL_mixer:
        Mix_OpenAudio(frequency, format, channels, chunksize)
        What do these parameters mean?
        Common values: Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)
        Don't forget Mix_CloseAudio()

    Understand the difference:
        Mix_Chunk - short sound effects (shoot, hit, explosion)
        Mix_Music - long music tracks (background music)
        Why different types?

    Load and play sound effects:
        Mix_LoadWAV() for effects
        Mix_PlayChannel() to play
        Multiple effects can play simultaneously (channels)
        Free with Mix_FreeChunk()

    Load and play music:
        Mix_LoadMUS() for music
        Mix_PlayMusic() to play
        Only one music track at a time
        Free with Mix_FreeMusic()

    Add sounds to your game:
        Shoot sound when firing bullet
        Hit sound when bullet hits enemy
        Death sound when enemy dies
        Background music during gameplay
        Different music for menu vs gameplay?

    Hook into your event system:
        Remember your event queue from Part 4?
        On EVENT_BULLET_FIRED → play shoot sound
        On EVENT_ENEMY_KILLED → play explosion sound
        This is clean decoupling!

Questions to think about:

    How many channels should you allocate?
    What happens if you try to play more sounds than channels?
    Should music loop? (Mix_PlayMusic(..., -1) for infinite loop)
    How do you control volume?

Audio file formats:

    WAV - uncompressed, large, works everywhere
    OGG - compressed, smaller, good for music
    MP3 - requires licensing, avoid for now

Creating placeholder sounds:

    Use a free tool like Audacity or Bfxr
    Or find free sounds on freesound.org
    Keep them short (under 1 second for effects)

Challenge:

    Add volume control (config file or in-game slider)
    Fade out music when transitioning states
    Add a mute toggle
    Implement 3D positional audio (sounds louder when closer)
    Add different hit sounds for different enemy types

Exercise 8: Putting It All Together

Objective: Complete the SDL port. Compare your terminal version to SDL version. Understand what changed and what didn't.

Your task:

    Side-by-side comparison:
        Create a table of what changed:
    Feature 	Terminal Version 	SDL Version
    Window 	printf 	SDL_Window
    Input 	readKey() 	SDL_PollEvent()
    Rendering 	char buffer 	SDL_RenderCopy()
    Timing 	clock_gettime() 	SDL_GetPerformanceCounter()
    Objects 	Same! 	Same!
    Physics 	Same! 	Same!
    Collisions 	Same! 	Same!
    State machine 	Same! 	Same!

    Verify your architecture survived:
        Is your game loop structure the same?
        Did your state machine change?
        Did your component system change?
        Did your event queue change?
        Answer: NO, NO, NO, NO!

    Measure the port:
        How many files did you create?
        How much code did you add?
        How much code did you delete?
        How much game logic changed? (Ideally: none!)

    Add polish:
        Smooth movement (interpolation)
        Screen shake on hits
        Particle effects (simple colored rectangles spawning/fading)
        Better sprites (download or create 16x16 pixel art)

    Performance check:
        Is your game running at 60fps?
        Use VSync or frame limiting?
        Any rendering bottlenecks?

Questions to reflect on:

    What was easier than expected?
    What was harder?
    What SDL concepts were confusing?
    What would you do differently next time?

Challenge: The ultimate test:

    Can you compile BOTH versions (terminal and SDL)?
    Can you switch between them with a compiler flag?
    This proves your abstraction worked:

#ifdef USE_SDL
    renderSDL(game);
#else
    renderTerminal(game);
#endif

If this works, you TRULY separated game logic from I/O. Congratulations.
End of Part 5 Outcome

After completing Part 5, you will have:

✅ A complete SDL2 game - Window, rendering, input, audio, text
✅ Architecture that survived the port - Proof your design was solid
✅ Understanding of SDL's role - It's I/O, not magic
✅ Reusable patterns - These work for ANY SDL project
✅ Foundation for 2D games - You can now build anything

More importantly, you understand:

    SDL doesn't make games, you do
    Good architecture is framework-agnostic
    I/O is replaceable, logic is not
    The difference between engine and game

What's Next?

You now have:

    A working game engine (your code)
    Modern I/O (SDL2)
    Clean architecture (Part 4)
    Fundamental CS skills (Parts 1-3)

Possible next steps:

Option 1: Deepen this game

    Add enemy types with different behaviors
    Add power-ups and pickups
    Add boss fights
    Add level progression
    Polish it into a complete game

Option 2: Learn advanced graphics

    Sprite animation systems
    Particle systems
    Tile-based maps
    Camera/scrolling
    Lighting effects

Option 3: Explore 3D

    OpenGL basics
    3D math (matrices, vectors)
    3D rendering pipeline
    Simple 3D shapes

Option 4: Study real engines

    Read Unity source code (what's different from yours?)
    Study Godot's architecture (similar patterns?)
    Explore ECS in depth (Bevy, flecs)

The meta-skill you've built:

You can now look at ANY game engine and understand:

    Where's the game loop?
    How do they handle input?
    What's their rendering abstraction?
    How do they manage state?
    What's their entity model?

You're not just a user of engines anymore. You're an engine programmer.

Congratulations. You've graduated from "following tutorials" to "understanding systems."
