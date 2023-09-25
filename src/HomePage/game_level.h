#ifndef GAME_LEVEL_H_
#define GAME_LEVEL_H_

#include "game_object.h"

class GameLevel
{
public:
    GameLevel(int w, int h);
    ~GameLevel();

    void SetViewport(int w, int h);
    void Load();
    void Draw(std::shared_ptr<SpriteRenderer> renderer);
    void DoCollision(SphereObject* sphere);

private:
    enum TileValue
    {
        TV_NON_BRICK,
        TV_HARD_BRICK,
        TV_STYLE_1_BRICK,
        TV_STYLE_2_BRICK,
        TV_STYLE_3_BRICK,
        TV_STYLE_4_BRICK,
        TV_STYLE_5_BRICK
    };

    std::vector<std::vector<int>> ReadLayersFromFile(const char* file);
    void BuildBricks(std::vector<std::vector<int>>&& level_datas);

private:
    int w_;
    int h_;
    std::list<GameObject> bricks_;
};
#endif
