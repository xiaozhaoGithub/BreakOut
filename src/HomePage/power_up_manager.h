#ifndef POWER_UP_MANAGER_H_
#define POWER_UP_MANAGER_H_

#include "power_up.h"

#include <unordered_map>

class PowerUpManager
{
public:
    PowerUpManager();
    ~PowerUpManager() {}

    void SpawnPowerUp(const QVector2D& pos);
    void Update(float dt, int w, int h, std::function<void(PowerUp::Type)> cb);
    void Draw(std::shared_ptr<SpriteRenderer> renderer);

    void DoCollision(GameObject* object, std::function<void(PowerUp::Type)> cb);

    void Clear();

private:
    bool NeedSpawnPowerUp(int probability);
    inline void TrySpawnPowerup(const QVector2D& pos, int probability, PowerUp::Type type,
                                const QVector3D& color, const QString& filename);

    inline bool IsExistSamePowerUpActived(PowerUp::Type type);

private:
    int probability_of_good_;
    int probability_of_bad_;

    std::unordered_map<PowerUp::Type, std::vector<std::shared_ptr<PowerUp>>> powerup_map_;
};
#endif
