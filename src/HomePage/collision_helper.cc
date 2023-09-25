#include "collision_helper.h"

#include <QtGlobal>

std::vector<QVector2D> CollisionHelper::directions_{QVector2D(0.0f, 1.0f), QVector2D(1.0f, 0.0f),
                                                    QVector2D(0.0f, -1.0f), QVector2D(-1.0f, 0.0f)};

CollisionHelper::CollisionHelper() {}

bool CollisionHelper::CheckCollision(GameObject* one, GameObject* two)
{
    bool is_x_axis_align = one->Pos().x() + one->Size().x() >= two->Pos().x()
                           && two->Pos().x() + two->Size().x() >= one->Pos().x();

    bool is_y_axis_align = one->Pos().y() + one->Size().y() >= two->Pos().y()
                           && two->Pos().y() + two->Size().y() >= one->Pos().y();

    return is_x_axis_align && is_y_axis_align;
}

CollisionHelper::CollisionResult CollisionHelper::CheckCollisionEx(SphereObject* one, GameObject* two)
{
    float radius = one->Radius();
    QVector2D sphere_center = one->Pos() + QVector2D(radius, radius);
    QVector2D box_center = two->Pos() + (two->Size() / 2);
    QVector2D center_diff = sphere_center - box_center;

    float x = qBound((-two->Size() / 2).x(), center_diff.x(), (two->Size() / 2).x());
    float y = qBound((-two->Size() / 2).y(), center_diff.y(), (two->Size() / 2).y());
    QVector2D clamp_diff(x, y);

    QVector2D closest_point = box_center + clamp_diff; // important

    QVector2D diff = closest_point - sphere_center;

    // Direciton based on diff.
    float max_value = 0.0f;
    size_t direction = 0;
    for (size_t i = 0; i < directions_.size(); ++i) {
        float ret = QVector2D::dotProduct(diff, directions_[i]);
        if (ret > max_value) {
            max_value = ret;
            direction = i;
        }
    }

    CollisionResult result;
    result.collision = radius >= diff.length();
    result.direction = (CollisionInputDirection)direction;
    result.diff_closest_center = diff;

    return result;
}
