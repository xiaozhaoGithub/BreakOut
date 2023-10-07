#ifndef POWER_UP_MANAGER_H_
#define POWER_UP_MANAGER_H_

#include "power_up.h"

class PowerUpManager
{
public:
    PowerUpManager();
    ~PowerUpManager() {}

    void SpawnPowerUp(const QVector2D& pos);

private:
    bool NeedSpawnPowerUp(int probability);

private:
    int probability_of_good_;
    int probability_of_bad_;

    std::vector<std::shared_ptr<PowerUp>> powerups_;
};
#endif
