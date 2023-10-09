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

    inline Type PowerUpType();

    void SetDuration(int ms);
    inline int DurationMs();

    void SetActive(bool state);
    inline bool IsActive();

    inline bool IsDone();

private:
    Type type_;
    bool is_activated_;
    int duration_ms_;
};

inline PowerUp::Type PowerUp::PowerUpType()
{
    return type_;
}

inline void PowerUp::SetDuration(int ms)
{
    duration_ms_ = ms;
}

inline int PowerUp::DurationMs()
{
    return duration_ms_;
}

inline bool PowerUp::IsActive()
{
    return is_activated_;
}

inline bool PowerUp::IsDone()
{
    return duration_ms_ == 0;
}

#endif
