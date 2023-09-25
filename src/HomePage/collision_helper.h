#ifndef COLLISION_HELPER_H_
#define COLLISION_HELPER_H_

#include <QVector2D>
#include <vector>

#include "game_object.h"

class CollisionHelper
{
public:
    CollisionHelper();

    enum CollisionInputDirection
    {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    struct CollisionResult
    {
        bool collision;
        CollisionInputDirection direction;
        QVector2D diff_closest_center;
    };


    /**
     * @brief Check collision between AABB.
     */
    static bool CheckCollision(GameObject* one, GameObject* two);

    /**
     * @brief Check collision between AABB and sphere.
     */
    static CollisionResult CheckCollisionEx(SphereObject* one, GameObject* two);

private:
    static std::vector<QVector2D> directions_;
};

#endif
