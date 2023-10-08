#include "power_up_manager.h"

#include "collision_helper.h"

constexpr float kVelocity = 60.0f;

PowerUpManager::PowerUpManager()
    : probability_of_good_(75)
    , probability_of_bad_(15)
{
    srand((unsigned int)time(nullptr));
}

void PowerUpManager::SpawnPowerUp(const QVector2D& pos)
{
    TrySpawnPowerup(pos, probability_of_good_, PowerUp::T_SPEED, QVector3D(0.5f, 0.5f, 1.0f),
                    ":/res/images/powerup_speed.png");

    TrySpawnPowerup(pos, probability_of_good_, PowerUp::T_STICKY, QVector3D(1.0f, 0.5f, 1.0f),
                    ":/res/images/powerup_sticky.png");

    TrySpawnPowerup(pos, probability_of_good_, PowerUp::T_PASS_THROUGH, QVector3D(0.5f, 1.0f, 0.5f),
                    ":/res/images/powerup_passthrough.png");

    TrySpawnPowerup(pos, probability_of_good_, PowerUp::T_PAD_SIZE_INCREASE,
                    QVector3D(1.0f, 0.6f, 0.4f), ":/res/images/powerup_increase.png");

    TrySpawnPowerup(pos, probability_of_bad_, PowerUp::T_CONFUSE, QVector3D(1.0f, 0.3f, 0.3f),
                    ":/res/images/powerup_confuse.png");

    TrySpawnPowerup(pos, probability_of_bad_, PowerUp::T_CHAOS, QVector3D(0.9f, 0.25f, 0.25f),
                    ":/res/images/powerup_chaos.png");
}

void PowerUpManager::Update(float dt)
{
    for (auto& powerup : powerups_) {
        QVector2D pos = QVector2D(powerup->Pos().x(), powerup->Pos().y() + kVelocity * dt);
        powerup->SetPos(pos);
    }
}

void PowerUpManager::Draw(std::shared_ptr<SpriteRenderer> renderer)
{
    for (auto& powerup : powerups_) {
        powerup->Draw(renderer);
    }
}

void PowerUpManager::DoCollision(GameObject* object)
{
    for (auto& powerup : powerups_) {
        if (!CollisionHelper::CheckCollision(object, powerup.get()))
            continue;

        powerup->Active();
    }
}

const std::vector<std::shared_ptr<PowerUp>>& PowerUpManager::PowerUps()
{
    return powerups_;
}

bool PowerUpManager::NeedSpawnPowerUp(int probability)
{
    return rand() % probability == 0;
}

void PowerUpManager::TrySpawnPowerup(const QVector2D& pos, int probability, PowerUp::Type type,
                                     const QVector3D& color, const QString& filename)
{
    if (!NeedSpawnPowerUp(probability))
        return;

    powerups_.emplace_back(
        std::make_shared<PowerUp>(type, pos, QVector2D(100.0f, 20.0f), color,
                                  std::make_shared<QOpenGLTexture>(QImage(filename))));
}
