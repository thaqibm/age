#include "../web/game.cpp"
#include <iostream>
int main() {
  game_init(0, 42);
  // A pulse removes close enemies and cannot be used twice during cooldown.
  auto &p = bodies->getData(player);
  add({p.x + 40, p.y, 0, 0, 9, 9, 1, 1, 0, 0, 0, Enemy});
  game_step(1.f / 120, 0, 0, 0, 0, 0, 1);
  assert(kills >= 1 && pulseCooldown > 4.9f);
  float cooldown = pulseCooldown;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 1);
  assert(pulseCooldown < cooldown);
  // Contact takes one hull point and grants temporary immunity.
  immunity = 0;
  add({p.x, p.y, 0, 0, 9, 9, 1, 1, 0, 0, 0, Enemy});
  int hp = health;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(health == hp - 1 && immunity > 0);
  elapsed = 89.999f;
  health = 5;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(state == 2);
  game_init(1, 42);
  assert(brickCount == 72 && health == 3);
  serve = 0;
  auto &b = bodies->getData(ball);
  b.x = 62;
  b.y = 70;
  b.vx = 0;
  b.vy = -300;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(brickCount == 71 && score == 25 && b.vy > 0);
  b.x = bodies->getData(player).x;
  b.y = 541;
  b.vy = 300;
  b.vx = 0;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(b.vy < 0);
  b.y = 620;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(health == 2 && serve > 0);
  serve = 0;
  health = 1;
  b.y = 620;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(state == 1);
  game_init(1, 42);
  brickCount = 0;
  serve = 0;
  game_step(1.f / 120, 0, 0, 0, 0, 0, 0);
  assert(state == 2);
  game_init(2, 42);
  for (int i = 0; i < 1200; i++)
    game_step(1.f / 120, 1, 0, 0, 0, 0, 0);
  assert(health == 5 && state == 0 && game_count() <= MAX_ENTT && game_count() > 1000);
  std::cout << "Game collisions, cooldown, win/loss, restart, and stress checks pass\n";
}
