# AGE: Ascii game engine

A C++ entity-component engine, originally built for terminal games.
[Original design document](readme/readme.pdf).

## Setup

[Play the browser demo](https://thaqibm.github.io/age/) or build it locally.

### Browser version (macOS / Linux)

You need **Git, Bash, and Python 3.10 or newer**. On macOS, install the Xcode
Command Line Tools (`xcode-select --install`) if Git is not available. Windows
users can follow these commands inside WSL.

Clone the project and install the pinned Emscripten toolchain in a sibling folder:

```sh
git clone https://github.com/thaqibm/age.git
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
python3 emsdk.py install 4.0.14
python3 emsdk.py activate 4.0.14
source ./emsdk_env.sh
cd ../age
./scripts/build-web.sh
python3 -m http.server 8770 --bind 127.0.0.1 --directory web/dist
```

Open **http://127.0.0.1:8770/** in a browser with WebAssembly and WebGL 2 enabled.
The server stays running until you press Ctrl+C. No npm install is required;
Emscripten provides the C++ compiler and Node runtime.

For later sessions, from the `age` directory:

```sh
source ../emsdk/emsdk_env.sh
./scripts/build-web.sh
python3 -m http.server 8770 --bind 127.0.0.1 --directory web/dist
```

Rebuild after editing files in `web/`, then refresh the browser. The generated
site is in `web/dist/`. Serve it over HTTP rather than opening `index.html`
directly. If `em++` is missing, source `emsdk_env.sh` again. If the port is busy,
use another port in the server command and browser URL. `EMXX` can override the
compiler path.

### Where the code lives

- [Swarm](web/game.cpp#L100) and [Breakout](web/game.cpp#L220): gameplay and collisions.
- [Orbit](web/game.cpp#L334) and [Flow](web/game.cpp#L341): simulation updates.
- [web/app.js](web/app.js): previews, fullscreen controls, input, and FPS display.
- [web/renderer.js](web/renderer.js): instanced WebGL renderer.
- [src/AGE/core](src/AGE/core): shared C++ entity and component storage.
- [scripts/build-web.sh](scripts/build-web.sh): C++ to WebAssembly build.

## Browser demo

Live miniature terminals open into a fullscreen game or simulation. Click any preview;
the expanded view has small Play/Run, Stress, FPS, Pause, and Close controls.
Native fullscreen is used when supported, with a viewport-filling dialog fallback.

Games and simulations use AGE's `EntityManager` and `componentDataArray<T>` core:

- **Swarm** — survive 90 seconds, with automatic targeting, increasing waves,
  a three-shot upgrade at wave four, and a pulse that recharges every five seconds.
- **Breakout** — clear 72 bricks with three balls; paddle impact position controls
  the bounce angle and each return increases speed.
- **Orbit** — 600 particles under softened central gravity; drag to move the center.
- **Flow** — 600 particles following a time-varying vector field; drag to attract them.
  Both simulations offer a 4,500-particle stress mode.
- **Swarm stress test** — starts with 2,000 enemies, adds more over time, and
  makes the ship invulnerable. Total entity capacity is 5,000.

Use WASD / arrows or touch and drag. Space activates Swarm's pulse. P
pauses; R restarts; Escape closes the expanded view. The page also provides buttons. Losing focus pauses the game.

### Architecture and performance

`web/game.cpp` compiles to real WebAssembly with Emscripten. It reuses the original
entity allocator and packed component storage, with new game-specific simulation
systems. It does **not** port the old terminal renderer, `AGEManager` update loop,
or original collision system. Those native implementations and games remain.

- Four independent WASM worlds share the downloaded binary. Previews run at 30 FPS
  (5 FPS with reduced motion), pause offscreen, and stop while a scene is expanded.
- Fixed 120 Hz simulation with bounded catch-up after slow frames.
- Dense component iteration for movement and render extraction.
- Uniform 40-unit spatial grid for bullet/enemy collision candidates.
- Fixed-capacity output buffer exposed as a WASM memory view; no per-entity JS calls.
- One instanced WebGL 2 draw for ships, enemies, bullets, bricks, and particles.
- No runtime JS dependencies, backend, worker, or special headers. The font and icon
  are bundled with the static site.
- 16 MiB fixed WASM linear memory per world (64 MiB for four previews). Device pixel ratio capped at two.

The component core fixes an incorrect entity-ID/index lookup during swap removal,
initializes its size for stack construction, and exposes dense iteration. Regression
tests cover non-contiguous IDs, removal, refill, and entity ID recycling.

The page measures FPS, active entities, simulation CPU time per frame, and draw
submission CPU time per frame. Draw submission is **not GPU completion time**.
The stress test is an illustrative game workload, not a comparative ECS benchmark.

### Validate

```sh
clang++ -std=c++20 -O1 -g -fsanitize=address,undefined tests/core.cpp -o /tmp/age-core
/tmp/age-core
clang++ -std=c++20 -O1 -g -fsanitize=address,undefined tests/games.cpp -o /tmp/age-games
/tmp/age-games
node tests/wasm.mjs
```

Native sanitizer tests exercise component lifecycle, pulse cooldown, damage,
brick and paddle collisions, lives, victory, defeat, and restart. WASM tests check
seeded determinism, finite render output, capacity, and a 60-second stress run.
Their timings measure WASM simulation under Node, not browser rendering.

### GitHub Pages

`.github/workflows/pages.yml` builds and tests on GitHub-hosted Ubuntu. Pull requests
build only; pushes to `main` and manual runs on `main` also deploy `web/dist`.
Set **Settings → Pages → Source → GitHub Actions** before the first deployment.
All asset URLs are relative, so the output works under `/age/`.

## Original terminal games

Install a C++20 compiler, Make, and curses development headers first. On macOS,
the Xcode Command Line Tools provide them. On Debian/Ubuntu, install
`build-essential` and `libncurses-dev`.

From the repository root, the existing Makefile builds `output/main` and `output/main2` with C++20 and curses:

```sh
make
output/main 30
output/main2 30
```

The numeric argument is the requested frame rate. See the original PDF for the
engine design and sample game details.
