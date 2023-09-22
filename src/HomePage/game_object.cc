#include "game_object.h"

const QVector2D kInitVelocity(100.0f, -350.0f);

GameObject::GameObject()
    : GameObject(QVector2D(0.0f, 0.0f), QVector2D(1.0f, 1.0f), QVector3D(1.0f, 1.0f, 1.0f),
                 std::shared_ptr<QOpenGLTexture>())
{}

GameObject::GameObject(const QVector2D& pos, const QVector2D& size, const QVector3D& color,
                       std::shared_ptr<QOpenGLTexture> texture)
    : pos_(pos)
    , size_(size)
    , color_(color)
    , texture_(texture)
    , is_destroyed_(false)
    , is_solid_(false)
{}

GameObject::~GameObject() {}

void GameObject::Draw(std::shared_ptr<SpriteRenderer> renderer)
{
    renderer->Draw(texture_, pos_, size_, 0.0f, color_);
}

void GameObject::SetPos(const QVector2D& pos)
{
    pos_ = pos;
}

QVector2D GameObject::Pos()
{
    return pos_;
}

void GameObject::SetSize(const QVector2D& size)
{
    size_ = size;
}

QVector2D GameObject::Size()
{
    return size_;
}

void GameObject::SetColor(const QVector3D& color)
{
    color_ = color;
}

void GameObject::Destroy()
{
    is_destroyed_ = true;
}

bool GameObject::IsDestroyed()
{
    return is_destroyed_;
}

void GameObject::SetSolid(bool state)
{
    is_solid_ = state;
}

bool GameObject::IsSolid()
{
    return is_solid_;
}

SphereObject::SphereObject(const QVector2D& pos, float radius, const QVector3D& color,
                           std::shared_ptr<QOpenGLTexture> texture)
    : GameObject(pos, QVector2D(2 * radius, 2 * radius), color, texture)
    , is_stuck_(true)
    , radius_(radius)
    , velocity_(kInitVelocity)
{}

void SphereObject::SetRadius(float radius)
{
    radius_ = radius;
}

float SphereObject::Radius()
{
    return radius_;
}

void SphereObject::SetVelocity(const QVector2D& velocity)
{
    velocity_ = velocity;
}

QVector2D SphereObject::Velocity()
{
    return velocity_;
}

QVector2D SphereObject::Move(float dt, float window_w, float window_h)
{
    pos_ += velocity_ * dt;

    float x = qBound(0.0f, pos_.x(), window_w - 2 * radius_);
    float y = qBound(0.0f, pos_.y(), window_h - 2 * radius_);
    if (x <= 0.0f || x >= window_w - 2 * radius_) {
        velocity_.setX(-velocity_.x());
    }

    if (y <= 0.0f) {
        velocity_.setY(-velocity_.y());
    }

    pos_.setX(x);
    pos_.setY(y);

    return pos_;
}

void SphereObject::Reset(const QVector2D& pos)
{
    is_stuck_ = true;
    velocity_ = kInitVelocity;
    SetPos(pos);
}

void SphereObject::SetStuck(bool state)
{
    is_stuck_ = state;
}

bool SphereObject::isStuck()
{
    return is_stuck_;
}
