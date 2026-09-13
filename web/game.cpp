// Browser games built on AGE's entity IDs and packed component storage.
#include "../src/AGE/core/components/componentDataArray.h"
#include <algorithm>
#include <cmath>
#include <memory>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define API EMSCRIPTEN_KEEPALIVE
#else
#define API
#endif
namespace {
constexpr float W = 960, H = 600, PI = 3.14159265f;
enum Kind { Ship, Enemy, Bullet, Spark, Paddle, Ball, Brick };
struct Body {
  float x{}, y{}, vx{}, vy{}, w{}, h{}, life{}, r{}, g{}, b{}, angle{};
  int kind{};
};
std::unique_ptr<EntityManager> entities;
std::unique_ptr<componentDataArray<Body>> bodies;
float vertices[MAX_ENTT * 11], stats[10];
int mode = 0, score = 0, health = 5, kills = 0, state = 0, wave = 1, brickCount = 0, drawCount = 0;
float elapsed = 0, spawnTimer = 0, shootTimer = 0, immunity = 0, pulseCooldown = 0, serve = 0;
EntityID player = 0, ball = 0;
unsigned rng = 1;
int heads[24 * 15], next[MAX_ENTT];
float random01() {
  rng ^= rng << 13;
  rng ^= rng >> 17;
  rng ^= rng << 5;
  return (rng & 0xffffff) / float(0x1000000);
}
float length(float x, float y) { return std::sqrt(x * x + y * y); }
EntityID add(Body b) {
  if (bodies->size() >= MAX_ENTT)
    return MAX_ENTT;
  auto id = entities->CreateEntity();
  bodies->attachComponent(id, b);
  return id;
}
void particles(float x, float y, float r, float g, float b, int n) {
  for (int i = 0; i < n && bodies->size() < MAX_ENTT - 64; i++) {
    float a = random01() * 2 * PI, s = 40 + random01() * 180;
    add({x, y, std::cos(a) * s, std::sin(a) * s, 2 + random01() * 3, 2, .25f + random01() * .4f, r,
         g, b, 0, Spark});
  }
}
void enemy() {
  float x, y;
  int side = int(random01() * 4);
  if (side < 2) {
    x = side ? W - 10 : 10;
    y = random01() * H;
  } else {
    x = random01() * W;
    y = side == 2 ? 10 : H - 10;
  }
  float size = 7 + random01() * 5;
  add({x, y, 0, 0, size, size, 1, 1, .34f, .26f, 0, Enemy});
}
void cleanup() {
  for (size_t i = 0; i < bodies->size();) {
    if (bodies->data()[i].life <= 0) {
      auto id = bodies->entityAt(i);
      bodies->DestroyEntity(id);
      entities->DestroyEntity(id);
    } else
      ++i;
  }
}
void pack() {
  drawCount = 0;
  for (size_t i = 0; i < bodies->size(); i++) {
    const auto &b = bodies->data()[i];
    float *v = vertices + drawCount++ * 11;
    v[0] = b.x;
    v[1] = b.y;
    v[2] = b.w;
    v[3] = b.h;
    v[4] = b.r;
    v[5] = b.g;
    v[6] = b.b;
    v[7] = (b.kind == Ship ? 2
                           : (b.kind == Enemy ? 1 : (b.kind == Brick || b.kind == Paddle ? 3 : 0)));
    v[8] = b.angle;
    v[9] = b.kind == Spark ? std::min(1.f, b.life * 2) : 1;
    v[10] = 0;
  }
  stats[0] = score;
  stats[1] = health;
  stats[2] = elapsed;
  stats[3] = wave;
  stats[4] = state;
  stats[5] = entities->size();
  stats[6] = kills;
  stats[7] = mode;
  stats[8] = pulseCooldown;
  stats[9] = brickCount;
}
void swarm(float dt, float mx, float my, float px, float py, int pointer, int pulse) {
  auto &p = bodies->getData(player);
  if (pointer) {
    mx = (px - p.x) / 20;
    my = (py - p.y) / 20;
  }
  float mag = length(mx, my);
  if (mag > 1) {
    mx /= mag;
    my /= mag;
  }
  p.x = std::clamp(p.x + mx * 245 * dt, 16.f, W - 16);
  p.y = std::clamp(p.y + my * 245 * dt, 16.f, H - 16);
  if (mag > .05f)
    p.angle = std::atan2(my, mx) + PI / 2;
  immunity = std::max(0.f, immunity - dt);
  pulseCooldown = std::max(0.f, pulseCooldown - dt);
  p.r = immunity > 0 ? 1 : .32f;
  p.g = immunity > 0 ? 1 : .88f;
  wave = 1 + int(elapsed / 15);
  spawnTimer -= dt;
  if (spawnTimer <= 0) {
    int n = mode == 2 ? 20 : 2 + wave;
    for (int i = 0; i < n && bodies->size() < MAX_ENTT - 256; i++)
      enemy();
    spawnTimer = mode == 2 ? .1f : std::max(.18f, .9f - wave * .045f);
  }
  if (pulse && pulseCooldown <= 0) {
    pulseCooldown = 5;
    for (size_t i = 0; i < bodies->size(); i++) {
      auto &b = bodies->data()[i];
      if (b.kind == Enemy && length(b.x - p.x, b.y - p.y) < 155) {
        b.life = 0;
        ++kills;
        score += 10;
      }
    }
    for (int i = 0; i < 72; i++) {
      float a = i * 2 * PI / 72;
      add({p.x, p.y, std::cos(a) * 450, std::sin(a) * 450, 3, 3, .34f, .5f, 1, 1, 0, Spark});
    }
  }
  shootTimer -= dt;
  if (shootTimer <= 0) {
    float dist = 1e20f, tx = 0, ty = -1;
    bool found = false;
    for (size_t i = 0; i < bodies->size(); i++) {
      auto &b = bodies->data()[i];
      if (b.kind != Enemy || b.life <= 0)
        continue;
      float dx = b.x - p.x, dy = b.y - p.y, d = dx * dx + dy * dy;
      if (d < dist) {
        dist = d;
        tx = dx;
        ty = dy;
        found = true;
      }
    }
    if (found) {
      float a = std::atan2(ty, tx);
      int shots = wave >= 4 ? 3 : 1;
      for (int j = 0; j < shots; j++) {
        float t = a + (j - (shots - 1) / 2.f) * .16f;
        add({p.x, p.y, std::cos(t) * 570, std::sin(t) * 570, 3, 3, 1.6f, .55f, 1, .85f, 0, Bullet});
      }
      p.angle = a + PI / 2;
    }
    shootTimer = .13f;
  }
  std::fill(std::begin(heads), std::end(heads), -1);
  for (size_t i = 0; i < bodies->size(); i++) {
    auto &b = bodies->data()[i];
    if (b.kind != Enemy || b.life <= 0)
      continue;
    float dx = p.x - b.x, dy = p.y - b.y, d = std::max(1.f, length(dx, dy));
    float speed = 35 + std::min(wave * 5, 60) + b.w * 2;
    b.x += dx / d * speed * dt;
    b.y += dy / d * speed * dt;
    b.angle += dt;
    if (d < b.w + 11 && immunity <= 0 && mode != 2) {
      --health;
      immunity = 1.3f;
      b.life = 0;
      particles(p.x, p.y, 1, .5f, .3f, 12);
      if (health <= 0)
        state = 1;
    }
    int cell = std::clamp(int(b.y / 40), 0, 14) * 24 + std::clamp(int(b.x / 40), 0, 23);
    next[i] = heads[cell];
    heads[cell] = int(i);
  }
  size_t n = bodies->size();
  for (size_t i = 0; i < n; i++) {
    auto &b = bodies->data()[i];
    if (b.kind != Bullet || b.life <= 0)
      continue;
    b.x += b.vx * dt;
    b.y += b.vy * dt;
    b.life -= dt;
    int cx = int(b.x / 40), cy = int(b.y / 40);
    for (int y = std::max(0, cy - 1); y <= std::min(14, cy + 1) && b.life > 0; y++)
      for (int x = std::max(0, cx - 1); x <= std::min(23, cx + 1) && b.life > 0; x++)
        for (int j = heads[y * 24 + x]; j >= 0; j = next[j]) {
          auto &e = bodies->data()[j];
          if (e.life <= 0)
            continue;
          float dx = e.x - b.x, dy = e.y - b.y;
          if (dx * dx + dy * dy < (e.w + 4) * (e.w + 4)) {
            e.life = 0;
            b.life = 0;
            score += 10;
            ++kills;
            particles(e.x, e.y, e.r, e.g, e.b, 6);
            break;
          }
        }
  }
  if (elapsed >= 90 && mode == 0 && state == 0)
    state = 2;
}
void breakout(float dt, float mx, float px, int pointer) {
  auto &p = bodies->getData(player);
  p.x = std::clamp(pointer ? px : p.x + mx * 500 * dt, 55.f, W - 55);
  auto &b = bodies->getData(ball);
  if (serve > 0) {
    serve -= dt;
    b.x = p.x;
    b.y = p.y - 24;
    return;
  }
  float oldX = b.x, oldY = b.y;
  b.x += b.vx * dt;
  b.y += b.vy * dt;
  if (b.x < 9) {
    b.x = 9;
    b.vx = std::abs(b.vx);
  }
  if (b.x > W - 9) {
    b.x = W - 9;
    b.vx = -std::abs(b.vx);
  }
  if (b.y < 9) {
    b.y = 9;
    b.vy = std::abs(b.vy);
  }
  if (b.vy > 0 && oldY <= p.y - 17 && b.y >= p.y - 17 && std::abs(b.x - p.x) < p.w + 7) {
    b.y = p.y - 17;
    float offset = (b.x - p.x) / p.w;
    float speed = std::min(570.f, length(b.vx, b.vy) + 8);
    b.vx = offset * speed * .8f;
    b.vy = -std::sqrt(speed * speed - b.vx * b.vx);
    particles(b.x, b.y, .5f, 1, 1, 4);
  }
  for (size_t i = 0; i < bodies->size(); i++) {
    auto &e = bodies->data()[i];
    if (e.kind != Brick || e.life <= 0)
      continue;
    if (std::abs(b.x - e.x) < e.w + 7 && std::abs(b.y - e.y) < e.h + 7) {
      e.life = 0;
      --brickCount;
      score += 25;
      particles(e.x, e.y, e.r, e.g, e.b, 12);
      if (std::abs(oldX - e.x) >= e.w + 7) {
        b.vx = -b.vx;
        b.x = oldX;
      } else {
        b.vy = -b.vy;
        b.y = oldY;
      }
      break;
    }
  }
  if (b.y > H + 12) {
    if (--health <= 0)
      state = 1;
    else {
      serve = 1;
      b.vx = 150;
      b.vy = -310;
    }
  }
  if (brickCount == 0)
    state = 2;
}
} // namespace
extern "C" {
API void game_init(int selected, unsigned seed) {
  entities = std::make_unique<EntityManager>();
  bodies = std::make_unique<componentDataArray<Body>>();
  mode = std::clamp(selected, 0, 2);
  rng = seed ? seed : 1;
  score = kills = state = brickCount = 0;
  wave = 1;
  health = mode == 1 ? 3 : 5;
  elapsed = spawnTimer = shootTimer = immunity = pulseCooldown = 0;
  serve = 1;
  if (mode == 1) {
    player = add({W / 2, H - 40, 0, 0, 52, 9, 1, .4f, .9f, 1, 0, Paddle});
    ball = add({W / 2, H - 64, 150, -310, 7, 7, 1, 1, 1, .9f, 0, Ball});
    for (int y = 0; y < 6; y++)
      for (int x = 0; x < 12; x++) {
        add({62.f + x * 76, 70.f + y * 30, 0, 0, 34, 11, 1, 1 - y * .09f, .35f + y * .095f,
             .3f + y * .12f, 0, Brick});
        ++brickCount;
      }
  } else {
    player = add({W / 2, H / 2, 0, 0, 12, 15, 1, .32f, .88f, 1, 0, Ship});
    for (int i = 0; i < (mode == 2 ? 2000 : 15); i++)
      enemy();
  }
  pack();
}
API void game_step(float dt, float mx, float my, float px, float py, int pointer, int pulse) {
  if (!bodies || state != 0)
    return;
  dt = std::clamp(dt, 0.f, 1.f / 60);
  elapsed += dt;
  if (mode == 1)
    breakout(dt, mx, px, pointer);
  else
    swarm(dt, mx, my, px, py, pointer, pulse);
  for (size_t i = 0; i < bodies->size(); i++) {
    auto &b = bodies->data()[i];
    if (b.kind == Spark) {
      b.x += b.vx * dt;
      b.y += b.vy * dt;
      b.life -= dt;
    }
  }
  cleanup();
  pack();
}
API float *game_vertices() { return vertices; }
API float *game_stats() { return stats; }
API int game_count() { return drawCount; }
}
