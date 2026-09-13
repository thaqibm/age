# AGE Engine

A C++ entity-component engine, originally built for terminal games.
[Original design document](readme/readme.pdf).

## Browser demo

Two new games use AGE's `EntityManager` and `componentDataArray<T>` core:

- **Swarm** — survive 90 seconds, with automatic targeting, increasing waves,
  a three-shot upgrade at wave four, and a pulse that recharges every five seconds.
- **Breakout** — clear 72 bricks with three balls; paddle impact position controls
  the bounce angle and each return increases speed.
- **Swarm stress test** — starts with 2,000 enemies, adds more over time, and
  makes the ship invulnerable. Total entity capacity is 5,000.

Use WASD / arrows or touch and drag. Space activates Swarm's pulse. P / Escape
pauses; R restarts. The page also provides buttons. Losing focus pauses the game.

### Architecture and performance

`web/game.cpp` compiles to real WebAssembly with Emscripten. It reuses the original
entity allocator and packed component storage, with new game-specific simulation
systems. It does **not** port the old terminal renderer, `AGEManager` update loop,
or original collision system. Those native implementations and games remain.

- Fixed 120 Hz simulation with bounded catch-up after slow frames.
- Dense component iteration for movement and render extraction.
- Uniform 40-unit spatial grid for bullet/enemy collision candidates.
- Fixed-capacity output buffer exposed as a WASM memory view; no per-entity JS calls.
- One instanced WebGL 2 draw for ships, enemies, bullets, bricks, and particles.
- No runtime JS dependencies, downloaded assets, server, worker, or special headers.
- 16 MiB fixed WASM linear memory. Device pixel ratio capped at two.

The component core fixes an incorrect entity-ID/index lookup during swap removal,
initializes its size for stack construction, and exposes dense iteration. Regression
tests cover non-contiguous IDs, removal, refill, and entity ID recycling.

The page measures FPS, active entities, simulation CPU time per frame, and draw
submission CPU time per frame. Draw submission is **not GPU completion time**.
The stress test is an illustrative game workload, not a comparative ECS benchmark.

### Build and run

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html),
version **4.0.14**, and activate its environment:

```sh
source /path/to/emsdk/emsdk_env.sh
./scripts/build-web.sh
python3 -m http.server 8770 --bind 127.0.0.1 --directory web/dist
```

Open <http://127.0.0.1:8770>. Serve over HTTP; opening the HTML as a local file
cannot load the WASM module. Requires WebAssembly and WebGL 2. JavaScript handles
input and rendering; C++ handles gameplay. `EMXX` can override the compiler path.

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

The existing Makefile builds `output/main` and `output/main2` with C++20 and curses:

```sh
make
output/main 30
output/main2 30
```

The numeric argument is the requested frame rate. See the original PDF for the
engine design and sample game details.
