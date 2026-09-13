import assert from "node:assert/strict";
import createAGE from "../web/dist/age.js";
const m = await createAGE();
const stats = () =>
  Array.from(m.HEAPF32.subarray(m._game_stats() / 4, m._game_stats() / 4 + 10));
function run(mode, frames) {
  m._game_init(mode, 42);
  const times = [];
  for (let i = 0; i < frames; i++) {
    const t = performance.now();
    m._game_step(
      1 / 120,
      Math.cos(i / 180),
      Math.sin(i / 180),
      480,
      300,
      0,
      i % 600 === 0 ? 1 : 0,
    );
    times.push(performance.now() - t);
    if (i % 120 === 0) {
      assert(m._game_count() <= 5000);
      const offset = m._game_vertices() / 4;
      assert(
        m.HEAPF32.subarray(offset, offset + m._game_count() * 11).every(
          Number.isFinite,
        ),
      );
    }
  }
  times.sort((a, b) => a - b);
  console.log(
    JSON.stringify({
      mode,
      frames,
      entities: m._game_count(),
      p50: times[Math.floor(times.length * 0.5)],
      p95: times[Math.floor(times.length * 0.95)],
      stats: stats(),
    }),
  );
  return stats();
}
const a = run(0, 2400),
  b = run(0, 2400);
assert.deepEqual(a, b, "Seeded simulation must be deterministic");
const breakout = run(1, 2400);
assert(breakout[9] < 72, "Breakout must destroy bricks");
const stress = run(2, 7200);
assert.equal(stress[1], 5);
assert.equal(stress[4], 0);
assert(stress[5] > 1000);
m._game_init(0, 42);
assert.equal(stats()[0], 0);
assert.equal(stats()[1], 5);
console.log("WASM simulation tests pass");
