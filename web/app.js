import { makeRenderer } from "./renderer.js";
const $ = (id) => document.getElementById(id);
const definitions = [
  {
    name: "Swarm",
    source: "https://github.com/thaqibm/age/blob/main/web/game.cpp#L100",
    mode: 0,
    hint: "WASD / arrows / drag to move · Space to pulse · P to pause · Esc to close",
  },
  {
    name: "Breakout",
    source: "https://github.com/thaqibm/age/blob/main/web/game.cpp#L220",
    mode: 1,
    hint: "← → / A D / drag to move · P to pause · Esc to close",
  },
  {
    name: "Orbit",
    source: "https://github.com/thaqibm/age/blob/main/web/game.cpp#L334",
    mode: 3,
    hint: "Drag to move the gravity center · P to pause · Esc to close",
  },
  {
    name: "Flow",
    source: "https://github.com/thaqibm/age/blob/main/web/game.cpp#L341",
    mode: 4,
    hint: "Drag to attract particles · P to pause · Esc to close",
  },
];
const scenes = [],
  keys = new Set(),
  STEP = 1 / 120;
let active = null,
  paused = false,
  stress = false,
  showFPS = true,
  ownedFullscreen = false;
let pointer = false,
  px = 480,
  py = 300,
  pulse = false,
  last = 0,
  previewClock = 0;
let frames = 0,
  sampleStart = 0,
  simTime = 0,
  drawTime = 0;
const stats = (scene) => {
  const offset = scene.engine._game_stats() / 4;
  return scene.engine.HEAPF32.subarray(offset, offset + 10);
};
function clearInput() {
  keys.clear();
  pointer = false;
  pulse = false;
}
function resetMetrics() {
  frames = simTime = drawTime = 0;
  sampleStart = performance.now();
  $("metrics").textContent = "Measuring…";
}
function reset(stressed = false) {
  if (!active || active.failed) return;
  stress = stressed;
  paused = false;
  clearInput();
  const mode =
    active.mode === 0 && stress
      ? 2
      : stress && active.mode >= 3
        ? active.mode + 2
        : active.mode;
  active.engine._game_init(mode, 42);
  active.accumulator = 0;
  $("regular").setAttribute("aria-pressed", String(!stress));
  $("stress").setAttribute("aria-pressed", String(stress));
  $("pause").textContent = "Pause";
  $("hint").textContent = active.hint;
  $("result").hidden = true;
  resetMetrics();
  updateHUD();
  active.canvas.focus();
}
function updateHUD() {
  if (!active) return;
  const s = stats(active);
  $("hud").textContent =
    active.mode >= 3
      ? ""
      : active.mode === 1
        ? `Score ${s[0]} · ${s[1]} lives · ${s[9]} bricks`
        : `Score ${s[0]} · ${stress ? "Invulnerable" : `${s[1]} hull · ${Math.ceil(Math.max(0, 90 - s[2]))}s`}`;
  $("pulse").disabled = paused || !!s[4] || s[8] > 0;
  $("pulse").textContent = s[8] > 0 ? `Pulse ${s[8].toFixed(1)}s` : "Pulse";
}
function open(scene) {
  if (scene.failed || active) return;
  active = scene;
  $("name").textContent = scene.name;
  $("game-source").href = scene.source;
  $("game-source").setAttribute("aria-label", `${scene.name} source code`);
  $("hint").textContent = scene.hint;
  $("regular").textContent = scene.mode >= 3 ? "Run" : "Play";
  $("stress").hidden = scene.mode === 1;
  $("pulse").hidden = scene.mode !== 0;
  $("viewer")
    .querySelectorAll("button")
    .forEach((button) => (button.disabled = false));
  $("stage").prepend(scene.canvas);
  scene.canvas.tabIndex = 0;
  $("viewer").showModal();
  document.body.style.overflow = "hidden";
  reset(false);
  // Keep the viewport-filling dialog when native fullscreen is unavailable.
  if (
    !document.fullscreenElement &&
    document.documentElement.requestFullscreen
  ) {
    document.documentElement
      .requestFullscreen()
      .then(() => {
        if (active) ownedFullscreen = true;
        else document.exitFullscreen().catch(() => {});
      })
      .catch(() => {});
  }
}
function close() {
  if (!active) return;
  const scene = active;
  active = null;
  clearInput();
  paused = false;
  scene.canvas.removeAttribute("tabindex");
  scene.preview.append(scene.canvas);
  if (!scene.failed) {
    scene.engine._game_init(scene.mode, 42);
    scene.accumulator = 0;
  }
  $("viewer").close();
  document.body.style.overflow = "";
  scene.button.focus();
  const exit = ownedFullscreen;
  ownedFullscreen = false;
  if (exit && document.fullscreenElement)
    document.exitFullscreen().catch(() => {});
}
function togglePause() {
  if (!active || active.failed || stats(active)[4]) return;
  paused = !paused;
  clearInput();
  active.accumulator = 0;
  resetMetrics();
  $("pause").textContent = paused ? "Resume" : "Pause";
  $("hint").textContent = paused
    ? "Paused · P to resume · Esc to close"
    : active.hint;
  updateHUD();
}
$("close").onclick = close;
$("viewer").addEventListener("cancel", (event) => {
  event.preventDefault();
  close();
});
document.addEventListener("fullscreenchange", () => {
  if (ownedFullscreen && !document.fullscreenElement) close();
});
$("regular").onclick = () => reset(false);
$("stress").onclick = () => reset(true);
$("restart").onclick = () => reset(stress);
$("again").onclick = () => reset(stress);
$("pause").onclick = togglePause;
$("pulse").onclick = () => {
  pulse = true;
  active?.canvas.focus();
};
$("fps-toggle").onclick = () => {
  showFPS = !showFPS;
  $("metrics").hidden = !showFPS;
  $("fps-toggle").setAttribute("aria-pressed", String(showFPS));
  resetMetrics();
};
$("metrics").title =
  "Measured FPS, entity count, simulation CPU time, and draw submission CPU time per frame. Draw time is not GPU completion.";
function step(scene, dt, demo) {
  scene.accumulator += dt;
  while (scene.accumulator >= STEP) {
    let mx =
      Number(keys.has("KeyD") || keys.has("ArrowRight")) -
      Number(keys.has("KeyA") || keys.has("ArrowLeft"));
    let my =
      Number(keys.has("KeyS") || keys.has("ArrowDown")) -
      Number(keys.has("KeyW") || keys.has("ArrowUp"));
    let x = px,
      y = py,
      dragging = pointer,
      firing = pulse;
    if (demo) {
      const s = stats(scene),
        t = s[2];
      mx = Math.cos(t * 0.7);
      my = Math.sin(t * 0.7);
      dragging = false;
      firing = true;
      if (scene.mode === 1) {
        // The preview paddle follows the real ball component.
        x = scene.engine.HEAPF32[scene.engine._game_vertices() / 4 + 11];
        dragging = true;
      }
    }
    scene.engine._game_step(
      STEP,
      mx,
      my,
      x,
      y,
      Number(dragging),
      Number(firing),
    );
    if (!demo) pulse = false;
    scene.accumulator -= STEP;
  }
  if (demo && stats(scene)[4]) scene.engine._game_init(scene.mode, 42);
}
const previewInterval = matchMedia("(prefers-reduced-motion: reduce)").matches
  ? 0.2
  : 1 / 30;
function frame(now) {
  requestAnimationFrame(frame);
  const dt = last ? Math.min((now - last) / 1000, 0.05) : 0;
  last = now;
  if (document.hidden) return;
  if (active) {
    if (active.failed) return;
    const start = performance.now();
    if (!paused && !stats(active)[4]) step(active, dt, false);
    simTime += performance.now() - start;
    const drawing = performance.now();
    active.render(active.engine);
    drawTime += performance.now() - drawing;
    updateHUD();
    if (!paused) frames++;
    if (!paused && now - sampleStart >= 500) {
      $("metrics").textContent =
        `${Math.round((frames * 1000) / (now - sampleStart))} fps · ${active.engine._game_count().toLocaleString()} entities · ${(simTime / Math.max(1, frames)).toFixed(2)} ms sim · ${(drawTime / Math.max(1, frames)).toFixed(2)} ms draw`;
      frames = simTime = drawTime = 0;
      sampleStart = now;
    }
    const s = stats(active);
    if (s[4]) {
      $("result").hidden = false;
      $("result-text").textContent =
        `${s[4] === 2 ? "Complete" : "Game over"} · ${s[0]}`;
      $("pulse").disabled = true;
    }
  } else {
    previewClock += dt;
    if (previewClock < previewInterval) return;
    const elapsed = previewClock;
    previewClock = 0;
    for (const scene of scenes)
      if (scene.visible && !scene.failed) {
        step(scene, elapsed, true);
        scene.render(scene.engine);
      }
  }
}
window.addEventListener("keydown", (event) => {
  if (!active || event.ctrlKey || event.metaKey || event.altKey) return;
  if (event.code === "Escape") {
    event.preventDefault();
    close();
    return;
  }
  if (event.code === "Space" && document.activeElement !== active.canvas)
    return;
  if (
    ["Space", "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown"].includes(
      event.code,
    )
  )
    event.preventDefault();
  keys.add(event.code);
  if (event.repeat) return;
  if (event.code === "Space") pulse = true;
  if (event.code === "KeyP") togglePause();
  if (event.code === "KeyR") reset(stress);
});
window.addEventListener("keyup", (event) => keys.delete(event.code));
window.addEventListener("blur", () => {
  clearInput();
  if (active && !paused) togglePause();
});
document.addEventListener("visibilitychange", () => {
  clearInput();
  if (document.hidden && active && !paused) togglePause();
});
async function init() {
  try {
    const { default: createAGE } = await import("./age.js");
    const response = await fetch(new URL("./age.wasm", import.meta.url));
    if (!response.ok)
      throw new Error("Engine download failed. Reload to retry.");
    const wasmBinary = await response.arrayBuffer();
    for (const definition of definitions) {
      const button = document.createElement("button");
      button.className = "terminal";
      button.disabled = true;
      button.setAttribute("aria-label", `Expand ${definition.name}`);
      const bar = document.createElement("span");
      bar.className = "terminal-bar";
      const label = document.createElement("span");
      label.textContent = definition.name;
      const expand = document.createElement("span");
      expand.textContent = "↗";
      expand.setAttribute("aria-hidden", "true");
      bar.append(label, expand);
      const preview = document.createElement("span");
      preview.className = "preview";
      const canvas = document.createElement("canvas");
      canvas.width = 960;
      canvas.height = 600;
      canvas.setAttribute("aria-label", definition.name);
      preview.append(canvas);
      button.append(bar, preview);
      const card = document.createElement("article");
      card.className = "terminal-card";
      const source = document.createElement("a");
      source.className = "game-source";
      source.href = definition.source;
      source.textContent = "source";
      source.target = "_blank";
      source.rel = "noopener";
      source.setAttribute("aria-label", `${definition.name} source code`);
      card.append(button, source);
      $("gallery").append(card);
      try {
        const engine = await createAGE({ wasmBinary });
        const scene = {
          ...definition,
          engine,
          canvas,
          button,
          preview,
          render: makeRenderer(canvas),
          accumulator: 0,
          visible: true,
          failed: false,
        };
        engine._game_init(scene.mode, 42);
        scenes.push(scene);
        scene.render(engine);
        button.disabled = false;
        button.onclick = () => open(scene);
        const observer = new IntersectionObserver(
          ([entry]) => (scene.visible = entry.isIntersecting),
        );
        observer.observe(button);
        canvas.addEventListener("pointerdown", (event) => {
          if (active !== scene) return;
          pointer = true;
          point(event);
          canvas.setPointerCapture(event.pointerId);
          canvas.focus();
        });
        canvas.addEventListener("pointermove", (event) => {
          if (active === scene && pointer) point(event);
        });
        for (const type of ["pointerup", "pointercancel", "lostpointercapture"])
          canvas.addEventListener(type, () => (pointer = false));
        canvas.addEventListener("webglcontextlost", (event) => {
          event.preventDefault();
          scene.failed = true;
          button.disabled = true;
          label.textContent = scene.name + " · reload";
          if (active === scene) {
            $("result").hidden = false;
            $("result-text").textContent =
              "Graphics interrupted. Reload to retry.";
            $("viewer")
              .querySelectorAll("button:not(#close)")
              .forEach((b) => (b.disabled = true));
          }
        });
      } catch (error) {
        label.textContent = definition.name + " · unavailable";
        button.title = error.message;
        console.error(error);
      }
    }
    $("status").textContent = scenes.length
      ? "Click to expand · C++ / WebAssembly"
      : "WebGL 2 unavailable. Enable hardware acceleration and reload.";
    requestAnimationFrame(frame);
  } catch (error) {
    $("status").textContent = error.message;
    console.error(error);
  }
}
function point(event) {
  const r = active.canvas.getBoundingClientRect();
  px = ((event.clientX - r.left) / r.width) * 960;
  py = ((event.clientY - r.top) / r.height) * 600;
}
init();
