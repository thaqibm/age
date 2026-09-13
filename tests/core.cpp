#include "../src/AGE/core/components/componentDataArray.h"
#include <iostream>
#include <memory>
int main() {
  auto a = std::make_unique<componentDataArray<int>>();
  assert(a->size() == 0);
  a->attachComponent(400, 40);
  a->attachComponent(7, 70);
  a->attachComponent(91, 90);
  a->detachComponent(7);
  assert(a->getData(91) == 90);
  assert(a->getData(400) == 40);
  a->detachComponent(400);
  assert(a->getData(91) == 90);
  assert(a->entityAt(0) == 91);
  a->detachComponent(91);
  assert(a->size() == 0);
  for (unsigned i = 0; i < MAX_ENTT; i++)
    a->attachComponent(i, int(i * 3));
  for (unsigned i = 0; i < MAX_ENTT; i += 2)
    a->detachComponent(i);
  for (unsigned i = 1; i < MAX_ENTT; i += 2)
    assert(a->getData(i) == int(i * 3));
  for (unsigned i = 0; i < MAX_ENTT; i += 2)
    a->attachComponent(i, int(i * 7));
  assert(a->size() == MAX_ENTT);
  auto e = std::make_unique<EntityManager>();
  for (unsigned i = 0; i < MAX_ENTT; i++)
    assert(e->CreateEntity() == i);
  e->DestroyEntity(123);
  assert(e->CreateEntity() == 123);
  assert(e->size() == MAX_ENTT);
  std::cout << "AGE packed component removal, reuse, and capacity checks pass\n";
}
