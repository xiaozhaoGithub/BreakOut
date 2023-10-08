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
    PowerUp::Type type;
    QVector3D color;
    QString filename;

    if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_SPEED;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_speed.png";
    } else if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_STICKY;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_sticky.png";
    } else if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_PASS_THROUGH;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_passthrough.png";
    } else if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_PAD_SIZE_INCREASE;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_increase.png";
    } else if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_CONFUSE;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_confuse.png";
    } else if (NeedSpawnPowerUp(probability_of_good_)) {
        type = PowerUp::T_CHAOS;
        color = QVector3D(1.0f, 0.0f, 0.0f);
        filename = ":/res/images/powerup_chaos.png";
    }

    if (filename.isEmpty())
        return;

    powerups_.emplace_back(
        std::make_shared<PowerUp>(type, pos, QVector2D(100.0f, 20.0f), color,
                                  std::make_shared<QOpenGLTexture>(QImage(filename))));
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

bool PowerUpManager::NeedSpawnPowerUp(int probability)
{
    return rand() % probability == 0;
}
