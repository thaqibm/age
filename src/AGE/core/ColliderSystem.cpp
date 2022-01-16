//
// Created by thaqi on 12/14/2021.
//


#include <cmath>


#include "ColliderSystem.h"
#include "components/default/default.h"


void ColliderSystem::init() {
    ctx->registerComponent<AGE_COMPONENTS::BoxCollider>();
    ctx->registerComponent<AGE_COMPONENTS::CircleCollider>();
}

const std::set<ColliderSystem::ColliderData>& ColliderSystem::detectCollisions(){
    ComponentSignature pos_signature;
    pos_signature.set(ctx->getComponentId<AGE_COMPONENTS::Position>(), true);


    ComponentSignature box_collider_signature;
    box_collider_signature.set(ctx->getComponentId<AGE_COMPONENTS::BoxCollider>(), true);


    ComponentSignature circ_collider_signature;
    circ_collider_signature.set(ctx->getComponentId<AGE_COMPONENTS::CircleCollider>(), true);


    auto compareFloats = [](float a, float b, float epsilon = 0.01f) -> bool{
        return fabs(a - b) < epsilon;
    };
    auto floatLeq = [&compareFloats](float a, float b, float epsilon = 0.01f) -> bool{
        return compareFloats(a,b,epsilon) or a < b;
    };
    auto floatGeq = [&compareFloats](float a, float b, float epsilon = 0.01f) -> bool{
        return compareFloats(a,b,epsilon) or a > b;
    };


    const auto ss = entityManager->getEntities();
    for(const auto& e1 : ss){
        for(const auto& e2 : ss){
            if(e1 == e2) {
                continue;
            }
            /**
            * Cases:
            * Box - Box
            * Box - Circ
            * Circ - Box
            * Circ - Circ
             */
            auto e1_signature = entityManager->getSignature(e1);
            auto e2_signature = entityManager->getSignature(e2);

            if((e1_signature & pos_signature) != pos_signature or (e2_signature & pos_signature) != pos_signature){
                continue;
            }
            const auto [p1, eh1, w, l] = ctx->getComponentData<AGE_COMPONENTS::Position>(e1);
            const auto [p2, eh2, w2, lw] = ctx->getComponentData<AGE_COMPONENTS::Position>(e2);
            if(not compareFloats(eh1, eh2)){
                continue;
            }
            const auto [e1x, e1y] = p1;
            const auto [e2x, e2y] = p2;


             bool boxbox = ((e1_signature & box_collider_signature) == box_collider_signature) and ((e2_signature & box_collider_signature) == box_collider_signature);
             bool boxcirc = ((e1_signature & box_collider_signature) == box_collider_signature) and ((e2_signature & circ_collider_signature) == circ_collider_signature);
             bool circbox = ((e1_signature & circ_collider_signature) == circ_collider_signature) and ((e2_signature & box_collider_signature) == box_collider_signature);
             bool circcirc = ((e1_signature & circ_collider_signature) == circ_collider_signature) and ((e2_signature & circ_collider_signature) == circ_collider_signature);


            if(boxbox){
                const auto [h1, w1] = ctx->getComponentData<AGE_COMPONENTS::BoxCollider>(e1);
                const auto [h2, w2] = ctx->getComponentData<AGE_COMPONENTS::BoxCollider>(e2);

               bool intersect = floatGeq(e1x+w1, e2x) and floatLeq(e1x, e2x+w2) and
                        floatGeq(e1y+h1, e2y) and floatLeq(e1y, e2y + h2);

                if(intersect){
                    mCollidedEntities.insert(std::make_pair(e1, e2));
                }
             }
            if(boxcirc){
                const auto b1 = ctx->getComponentData<AGE_COMPONENTS::BoxCollider>(e1);
                const auto c2 = ctx->getComponentData<AGE_COMPONENTS::CircleCollider>(e2);


                /**
                 * Snippet from:
                 *  https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
                 *  https://stackoverflow.com/a/1879223
                 */


                float closestX = clamp(e2x, e1x, e1x+b1.width);
                float closestY = clamp(e2y, e1y+b1.height, e1y);

                float distanceX = e2x - closestX;
                float distanceY = e2y - closestY;

                float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
                if(distanceSquared < (c2.radius * c2.radius)){
                    mCollidedEntities.insert(std::make_pair(e1, e2));
                }
            }
            if(circbox){
                const auto c2 = ctx->getComponentData<AGE_COMPONENTS::CircleCollider>(e1);
                const auto b1 = ctx->getComponentData<AGE_COMPONENTS::BoxCollider>(e2);

                /**
                 * Snippet from:
                 *  https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
                 *  https://stackoverflow.com/a/1879223
                 */


                float closestX = clamp(e2x, e2x, e2x+b1.width);
                float closestY = clamp(e2y, e2y+b1.height, e2y);

                float distanceX = e2x - closestX;
                float distanceY = e2y - closestY;

                float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
                if(distanceSquared < (c2.radius * c2.radius)){
                    mCollidedEntities.insert(std::make_pair(e1, e2));
                }
            }
            if(circcirc){
                const auto c1 = ctx->getComponentData<AGE_COMPONENTS::CircleCollider>(e1);
                const auto c2 = ctx->getComponentData<AGE_COMPONENTS::CircleCollider>(e2);

                auto distSqr = (e1x - e2x)*(e1x - e2x) + (e1y - e2y)*(e1y - e2y);
                auto radiusSqr = (c1.radius + c2.radius)*(c1.radius+c2.radius);

                if(floatLeq(distSqr, radiusSqr)){
                    mCollidedEntities.insert(std::make_pair(e1, e2));
                }
            }

        }
    }
    return mCollidedEntities;
}
