#include "power_up.h"

PowerUp::PowerUp(Type type, const QVector2D& pos, const QVector2D& size, const QVector3D& color,
                 std::shared_ptr<QOpenGLTexture> texture)
    : GameObject(pos, size, color, texture)
    , type_(type)
    , is_activated_(false)
    , duration_ms_(5000)
{}

void PowerUp::SetActive(bool state)
{
    is_activated_ = state;
}
