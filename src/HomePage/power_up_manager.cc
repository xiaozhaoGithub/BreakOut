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

void PowerUpManager::Update(float dt, int w, int h, std::function<void(PowerUp::Type)> cb)
{
    for (auto& powerup_pair : powerup_map_) {
        for (auto iter = powerup_pair.second.begin(); iter != powerup_pair.second.end();) {
            QVector2D pos = QVector2D((*iter)->Pos().x(), (*iter)->Pos().y() + kVelocity * dt);
            (*iter)->SetPos(pos);

            if (!(*iter)->IsActive()) {
                if (pos.y() + (*iter)->Size().y() >= h) {
                    iter = powerup_pair.second.erase(iter);
                } else {
                    ++iter;
                }

                continue;
            }

            int ms = (*iter)->DurationMs();
            ms -= dt * 1000;
            if (ms <= 0) {
                PowerUp::Type type = (*iter)->PowerUpType();

                iter = powerup_pair.second.erase(iter);
                if (!IsExistSamePowerUpActived(type)) {
                    cb(type);
                }
            } else {
                (*iter)->SetDuration(ms);
                ++iter;
            }
        }
    }
}

void PowerUpManager::Draw(std::shared_ptr<SpriteRenderer> renderer)
{
    for (auto& powerup_pair : powerup_map_) {
        for (auto& powerup : powerup_pair.second) {
            if (powerup->IsActive())
                continue;

            powerup->Draw(renderer);
        }
    }
}

void PowerUpManager::DoCollision(GameObject* object, std::function<void(PowerUp::Type)> cb)
{
    for (auto& powerup_pair : powerup_map_) {
        for (auto& powerup : powerup_pair.second) {
            if (powerup->IsActive())
                continue;

            if (!CollisionHelper::CheckCollision(object, powerup.get()))
                continue;

            powerup->SetActive(true);
            cb(powerup->PowerUpType());
        }
    }
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

    powerup_map_[type].emplace_back(
        std::make_shared<PowerUp>(type, pos, QVector2D(100.0f, 20.0f), color,
                                  std::make_shared<QOpenGLTexture>(QImage(filename))));
}

inline bool PowerUpManager::IsExistSamePowerUpActived(PowerUp::Type type)
{
    auto iter = powerup_map_.find(type);
    if (iter == powerup_map_.end())
        return false;

    for (auto& powerup : iter->second) {
        if (powerup->IsActive()) {
            return true;
        }
    }

    return false;
}
