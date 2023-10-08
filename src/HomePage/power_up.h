#ifndef POWER_UP_H_
#define POWER_UP_H_

#include <QOpenGLTexture>

#include "game_object.h"
#include "sprite_renderer.h"

class PowerUp : public GameObject
{
public:
    enum Type
    {
        T_SPEED,
        T_STICKY,
        T_PASS_THROUGH,
        T_PAD_SIZE_INCREASE,
        T_CONFUSE,
        T_CHAOS
    };

    PowerUp(Type type, const QVector2D& pos, const QVector2D& size, const QVector3D& color,
            std::shared_ptr<QOpenGLTexture> texture);
    ~PowerUp() {}

    void Active();

private:
    Type type_;
    bool is_activated_;
};
#endif
