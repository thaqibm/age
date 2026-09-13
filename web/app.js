const $ = (id) => document.getElementById(id),
  canvas = $("game");
let engine,
  gl,
  program,
  buffer,
  mode = 0,
  running = false,
  paused = false,
  last = 0,
  accumulator = 0,
  pulse = false,
  pointer = false,
  px = 480,
  py = 300;
const keys = new Set(),
  STEP = 1 / 120;
let frameCount = 0,
  windowStart = performance.now(),
  simSum = 0,
  renderSum = 0;
const titles = ["Swarm", "Breakout", "Swarm / stress test"];
const intros = [
  "Survive 90 seconds. Auto-fire.",
  "Clear 72 bricks. Three lives.",
  "2,000 drones. Invulnerable. No time limit.",
];
function makeRenderer() {
  gl = canvas.getContext("webgl2", {
    alpha: true,
    antialias: false,
    preserveDrawingBuffer: false,
  });
  if (!gl)
    throw new Error(
      "WebGL 2 is unavailable. Try a browser with hardware acceleration enabled.",
    );
  const shader = (type, source) => {
    const s = gl.createShader(type);
    gl.shaderSource(s, source);
    gl.compileShader(s);
    if (!gl.getShaderParameter(s, gl.COMPILE_STATUS))
      throw new Error(gl.getShaderInfoLog(s));
    return s;
  };
  program = gl.createProgram();
  gl.attachShader(
    program,
    shader(
      gl.VERTEX_SHADER,
      `#version 300 es
 precision highp float;
 layout(location=0) in vec4 rect;
 layout(location=1) in vec4 colorShape;
 layout(location=2) in vec3 extra;
 out vec2 uv;out vec4 color;flat out int shape;
 void main(){vec2 corners[6]=vec2[6](vec2(-1,-1),vec2(1,-1),vec2(-1,1),vec2(-1,1),vec2(1,-1),vec2(1,1));uv=corners[gl_VertexID];vec2 q=uv*rect.zw;float c=cos(extra.x),s=sin(extra.x);vec2 p=rect.xy+mat2(c,s,-s,c)*q;gl_Position=vec4(p.x/480.-1.,1.-p.y/300.,0,1);color=vec4(colorShape.rgb,extra.y);shape=int(colorShape.a);}`,
    ),
  );
  gl.attachShader(
    program,
    shader(
      gl.FRAGMENT_SHADER,
      `#version 300 es
 precision highp float;
 in vec2 uv;in vec4 color;flat in int shape;out vec4 outColor;
 void main(){float edge=0.;if(shape==0)edge=length(uv)-1.;else if(shape==1)edge=abs(uv.x)+abs(uv.y)-1.;else if(shape==2)edge=max(abs(uv.x)-(uv.y+1.)*.5,-uv.y-1.);else edge=max(abs(uv.x),abs(uv.y))-1.;float a=1.-smoothstep(-.08,0.,edge);if(a<=0.)discard;outColor=vec4(color.rgb,color.a*a);}`,
    ),
  );
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS))
    throw new Error(gl.getProgramInfoLog(program));
  gl.useProgram(program);
  buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
  gl.bufferData(gl.ARRAY_BUFFER, 5000 * 44, gl.DYNAMIC_DRAW);
  for (const [i, n, offset] of [
    [0, 4, 0],
    [1, 4, 16],
    [2, 3, 32],
  ]) {
    gl.enableVertexAttribArray(i);
    gl.vertexAttribPointer(i, n, gl.FLOAT, false, 44, offset);
    gl.vertexAttribDivisor(i, 1);
  }
  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  gl.clearColor(0, 0, 0, 0);
}
function render() {
  const ratio = Math.min(devicePixelRatio || 1, 2),
    w = Math.round(canvas.clientWidth * ratio),
    h = Math.round(canvas.clientHeight * ratio);
  if (canvas.width !== w || canvas.height !== h) {
    canvas.width = w;
    canvas.height = h;
    gl.viewport(0, 0, w, h);
  }
  gl.clear(gl.COLOR_BUFFER_BIT);
  const n = engine._game_count(),
    offset = engine._game_vertices() / 4;
  gl.bufferSubData(
    gl.ARRAY_BUFFER,
    0,
    engine.HEAPF32.subarray(offset, offset + n * 11),
  );
  gl.drawArraysInstanced(gl.TRIANGLES, 0, 6, n);
}
function readStats() {
  const offset = engine._game_stats() / 4;
  return engine.HEAPF32.subarray(offset, offset + 10);
}
function hud() {
  const s = readStats();
  $("score").textContent = "SCORE " + String(s[0]).padStart(5, "0");
  $("lives").textContent =
    mode === 2
      ? "INVULNERABLE"
      : (mode === 1 ? "BALLS " : "HULL ") + "● ".repeat(Math.max(0, s[1]));
  $("time").textContent =
    mode === 0
      ? Math.ceil(Math.max(0, 90 - s[2])) + "s"
      : Math.floor(s[2]) + "s";
  $("wave").textContent =
    mode === 1
      ? s[9] + " BRICKS LEFT"
      : "WAVE " + String(s[3]).padStart(2, "0");
  $("pulse-status").textContent =
    mode === 1
      ? "CLEAR THE FIELD"
      : s[8] > 0
        ? "PULSE " + s[8].toFixed(1) + "s"
        : "PULSE READY";
  $("pulse").disabled = !running || paused || mode === 1 || s[8] > 0;
}
function select(selected) {
  if (!engine) return;
  mode = selected;
  running = false;
  paused = false;
  keys.clear();
  pointer = false;
  pulse = false;
  accumulator = 0;
  engine._game_init(mode, 42);
  document
    .querySelectorAll("[data-mode]")
    .forEach((b) =>
      b.setAttribute("aria-pressed", Number(b.dataset.mode) === mode),
    );
  $("overlay").hidden = false;
  $("title").textContent = titles[mode];
  $("intro").innerText = intros[mode];
  $("start").textContent = "Play";
  $("hint").textContent =
    mode === 1
      ? "← → / A D / drag to move · P to pause · R to restart"
      : "WASD / arrows / drag to move · Space to pulse · P to pause · R to restart";
  $("pulse").hidden = mode === 1;
  $("pause").disabled = true;
  $("pause").textContent = "Pause";
  $("restart").disabled = false;
  $("status").textContent = "Ready to play.";
  $("entities").textContent = engine._game_count().toLocaleString();
  hud();
  render();
}
function start() {
  if (!engine) return;
  if (paused) {
    togglePause();
    return;
  }
  engine._game_init(mode, 42);
  running = true;
  paused = false;
  accumulator = 0;
  last = 0;
  frameCount = 0;
  simSum = renderSum = 0;
  windowStart = performance.now();
  $("overlay").hidden = true;
  $("pause").disabled = false;
  $("pause").textContent = "Pause";
  $("status").textContent =
    mode === 2 ? "Stress test · invulnerable" : "Playing · " + titles[mode];
  canvas.focus();
}
function togglePause() {
  if (!running) return;
  paused = !paused;
  keys.clear();
  pointer = false;
  pulse = false;
  accumulator = 0;
  last = 0;
  frameCount = 0;
  simSum = renderSum = 0;
  windowStart = performance.now();
  $("pause").textContent = paused ? "Resume" : "Pause";
  $("overlay").hidden = !paused;
  if (paused) {
    $("title").textContent = "Paused";
    $("intro").textContent = "";
    $("start").textContent = "Resume";
    $("status").textContent = "Paused.";
  } else {
    $("status").textContent = "Playing · " + titles[mode];
    canvas.focus();
  }
  hud();
}
function frame(now) {
  requestAnimationFrame(frame);
  if (!engine || !running || paused) return;
  const delta = last ? Math.min((now - last) / 1000, 0.05) : 0;
  last = now;
  accumulator += delta;
  const begin = performance.now();
  while (accumulator >= STEP) {
    engine._game_step(
      STEP,
      Number(keys.has("KeyD") || keys.has("ArrowRight")) -
        Number(keys.has("KeyA") || keys.has("ArrowLeft")),
      Number(keys.has("KeyS") || keys.has("ArrowDown")) -
        Number(keys.has("KeyW") || keys.has("ArrowUp")),
      px,
      py,
      Number(pointer),
      Number(pulse),
    );
    pulse = false;
    accumulator -= STEP;
  }
  simSum += performance.now() - begin;
  const draw = performance.now();
  render();
  renderSum += performance.now() - draw;
  frameCount++;
  hud();
  if (now - windowStart >= 500) {
    $("fps").textContent = Math.round(
      (frameCount * 1000) / (now - windowStart),
    );
    $("entities").textContent = engine._game_count().toLocaleString();
    $("sim").textContent = (simSum / frameCount).toFixed(2);
    $("render").textContent = (renderSum / frameCount).toFixed(2);
    frameCount = 0;
    simSum = renderSum = 0;
    windowStart = now;
  }
  const s = readStats();
  if (s[4]) {
    running = false;
    $("overlay").hidden = false;
    $("title").textContent =
      s[4] === 2
        ? mode === 1
          ? "Field cleared"
          : "You survived"
        : "Run complete";
    $("intro").textContent =
      `Score ${s[0].toLocaleString()} · ${Math.floor(s[2])} seconds`;
    $("start").textContent = "Play again";
    $("pause").disabled = true;
    $("pulse").disabled = true;
    $("status").textContent = s[4] === 2 ? "Complete." : "Game over.";
  }
}
$("start").onclick = start;
$("restart").onclick = start;
$("pause").onclick = togglePause;
$("pulse").onclick = () => {
  pulse = true;
  canvas.focus();
};
$("stress").onclick = () => {
  select(2);
  start();
};
document
  .querySelectorAll("[data-mode]")
  .forEach((b) => (b.onclick = () => select(Number(b.dataset.mode))));
window.addEventListener("keydown", (e) => {
  if (!engine || e.ctrlKey || e.metaKey || e.altKey) return;
  const gameFocus = document.activeElement === canvas;
  if (e.code === "Space" && !gameFocus) return;
  if (
    !gameFocus &&
    /INPUT|SELECT|TEXTAREA/.test(document.activeElement.tagName)
  )
    return;
  if (
    ["ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Space"].includes(
      e.code,
    ) &&
    running
  )
    e.preventDefault();
  keys.add(e.code);
  if (e.repeat) return;
  if (e.code === "Space" && running && !paused) pulse = true;
  if (e.code === "KeyP" || e.code === "Escape") togglePause();
  if (e.code === "KeyR") start();
});
window.addEventListener("keyup", (e) => keys.delete(e.code));
function point(e) {
  const r = canvas.getBoundingClientRect();
  px = ((e.clientX - r.left) / r.width) * 960;
  py = ((e.clientY - r.top) / r.height) * 600;
}
canvas.addEventListener("pointerdown", (e) => {
  pointer = true;
  point(e);
  canvas.setPointerCapture(e.pointerId);
  canvas.focus();
});
canvas.addEventListener("pointermove", (e) => {
  if (pointer) point(e);
});
for (const name of ["pointerup", "pointercancel", "lostpointercapture"])
  canvas.addEventListener(name, () => (pointer = false));
window.addEventListener("blur", () => {
  keys.clear();
  pointer = false;
  if (running && !paused) togglePause();
});
document.addEventListener("visibilitychange", () => {
  if (document.hidden && running && !paused) togglePause();
});
canvas.addEventListener("webglcontextlost", (e) => {
  e.preventDefault();
  if (running && !paused) togglePause();
  $("status").textContent =
    "Graphics context lost. Reload to restore the game.";
  running = false;
  engine = null;
  $("overlay").hidden = false;
  $("title").textContent = "Graphics interrupted";
  $("intro").textContent = "Reload this page to restore the game.";
  document
    .querySelectorAll("button")
    .forEach((button) => (button.disabled = true));
});
try {
  makeRenderer();
  const { default: createAGE } = await import("./age.js");
  engine = await createAGE();
  document
    .querySelectorAll("[data-mode]")
    .forEach((button) => (button.disabled = false));
  $("start").disabled = false;
  $("stress").disabled = false;
  select(0);
  requestAnimationFrame(frame);
} catch (error) {
  $("title").textContent = "Unable to start";
  $("intro").textContent = error.message;
  $("start").textContent = "Unavailable";
  $("status").textContent = "Engine could not load. Reload to try again.";
  console.error(error);
}
