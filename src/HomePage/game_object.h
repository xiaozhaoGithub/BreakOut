#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <QOpenGLTexture>

#include "sprite_renderer.h"

class GameObject
{
public:
    GameObject();
    GameObject(const QVector2D& pos, const QVector2D& size, const QVector3D& color,
               std::shared_ptr<QOpenGLTexture> texture);
    virtual ~GameObject();

    void Draw(std::shared_ptr<SpriteRenderer> renderer);

    void SetPos(const QVector2D& pos);
    QVector2D Pos();

    void SetSize(const QVector2D& size);
    QVector2D Size();

    void SetColor(const QVector3D& color);

    void Destroy();
    bool IsDestroyed();

    void SetSolid(bool state);
    bool IsSolid();

protected:
    QVector2D pos_;
    QVector2D size_;
    QVector3D color_;
    std::shared_ptr<QOpenGLTexture> texture_;

    bool is_destroyed_;
    bool is_solid_;
};

class SphereObject : public GameObject
{
public:
    SphereObject(const QVector2D& pos, float radius, const QVector3D& color,
                 std::shared_ptr<QOpenGLTexture> texture);
    ~SphereObject() {}

    void SetStuck(bool state);
    bool isStuck();

    void SetRadius(float radius);
    float Radius();

    void SetVelocity(const QVector2D& velocity);
    QVector2D Velocity();

    QVector2D Move(float dt, float window_w, float window_h);
    void Reset(const QVector2D& pos);

private:
    bool is_stuck_;
    float radius_;
    QVector2D velocity_;
};
#endif
