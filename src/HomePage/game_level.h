#ifndef GAME_LEVEL_H_
#define GAME_LEVEL_H_

#include "game_object.h"
#include "post_processor.h"
#include "power_up_manager.h"

class GameLevel
{
public:
    GameLevel(int w, int h);
    ~GameLevel();

    void SetViewport(int w, int h);
    void Load(const char* filename);
    void Draw(std::shared_ptr<SpriteRenderer> renderer);
    void DoCollision(SphereObject* object, std::function<void(const QVector2D& pos)> cb);

    void SetPostProcessor(std::shared_ptr<PostProcessor> post_processor);

    inline void SetLevelNum(int num);
    inline int Level();

    void NextLevel();

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

    int level_num_;
    int level_;

    std::list<GameObject> bricks_;

    std::shared_ptr<PostProcessor> post_processor_;
};


inline void GameLevel::SetLevelNum(int num)
{
    level_num_ = num;
}

inline int GameLevel::Level()
{
    return level_;
}

#endif
