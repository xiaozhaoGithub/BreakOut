#include "game_level.h"

#include <fstream>
#include <memory>
#include <sstream>

#include "audio_manager.h"
#include "collision_helper.h"

GameLevel::GameLevel(int w, int h)
    : w_(w)
    , h_(h)
{}

GameLevel::~GameLevel() {}

void GameLevel::SetViewport(int w, int h)
{
    w_ = w;
    h_ = h;
}

void GameLevel::Load(const char* filename)
{
    auto level_datas = ReadLayersFromFile(filename);
    BuildBricks(std::move(level_datas));
}

void GameLevel::Draw(std::shared_ptr<SpriteRenderer> renderer)
{
    // bricks
    for (auto& brick : bricks_) {
        if (!brick.IsDestroyed()) {
            brick.Draw(renderer);
        }
    }
}

void GameLevel::DoCollision(SphereObject* object, std::function<void(const QVector2D& pos)> cb)
{
    for (auto& brick : bricks_) {
        if (brick.IsDestroyed())
            continue;

        auto result = CollisionHelper::CheckCollisionEx(object, &brick);
        if (result.collision) {
            auto sphere = dynamic_cast<SphereObject*>(object);
            QVector2D v = sphere->Velocity();
            QVector2D pos = sphere->Pos();

            // collision repostioning
            QVector2D diff = result.diff_closest_center;
            QVector2D penetration = QVector2D(sphere->Radius(), sphere->Radius())
                                    - QVector2D(std::abs(diff.x()), std::abs(diff.y()));

            switch (result.direction) {
            case CollisionHelper::UP: {
                pos = QVector2D(pos.x(), pos.y() - penetration.y());
                v.setY(-sphere->Velocity().y());
                break;
            }
            case CollisionHelper::RIGHT: {
                pos = QVector2D(pos.x() - penetration.x(), pos.y());
                v.setX(-sphere->Velocity().x());
                break;
            }
            case CollisionHelper::DOWN: {
                pos = QVector2D(pos.x(), pos.y() + penetration.y());
                v.setY(-sphere->Velocity().y());
                break;
            }
            case CollisionHelper::LEFT: {
                pos = QVector2D(QVector2D(pos.x() + penetration.x(), pos.y()));
                v.setX(-sphere->Velocity().x());
                break;
            }
            default:
                break;
            }

            if (brick.IsSolid() || !sphere->IsPassThrough()) {
                sphere->SetPos(pos);
                sphere->SetVelocity(v);
            }

            if (brick.IsSolid()) {
                post_processor_->SetShake(true);
                Singleton<AudioManager>::Instance()->Play(":/res/audio/solid.wav");
            } else {
                brick.Destroy();
                Singleton<AudioManager>::Instance()->Play(":/res/audio/bleep.wav");

                cb(brick.Pos());
            }
        }
    }
}

void GameLevel::SetPostProcessor(std::shared_ptr<PostProcessor> post_processor)
{
    post_processor_ = post_processor;
}

void GameLevel::NextLevel()
{
    level_ = level_ % level_num_;
}

std::vector<std::vector<int>> GameLevel::ReadLayersFromFile(const char* file)
{
    std::ifstream ifs(file, std::ios_base::in);

    std::string line;
    std::vector<std::vector<int>> level_datas;
    int tileData;

    while (std::getline(ifs, line)) {
        std::vector<int> line_datas;

        std::istringstream iss(line);
        while (iss >> tileData) {
            line_datas.emplace_back(tileData);
        }
        level_datas.emplace_back(line_datas);
    }

    return level_datas;
}

void GameLevel::BuildBricks(std::vector<std::vector<int>>&& level_datas)
{
    bricks_.clear();

    int rows = (int)level_datas.size();
    if (rows == 0)
        return;

    int cols = (int)level_datas[0].size();

    QVector2D size(w_ / (float)cols, (h_ >> 1) / rows);
    QVector2D pos(0.0f, 0.0f);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QString file_name;
            QVector3D color;

            int tile = level_datas[row][col];
            switch (tile) {
            case TV_HARD_BRICK: {
                file_name = ":/res/images/block_solid.png";
                color = QVector3D(0.8f, 0.8f, 0.7f);
                break;
            }
            case TV_STYLE_1_BRICK: {
                file_name = ":/res/images/block.png";
                color = QVector3D(1.0f, 1.0f, 1.0f);
                break;
            }
            case TV_STYLE_2_BRICK: {
                file_name = ":/res/images/block.png";
                color = QVector3D(0.2f, 0.6f, 1.0f);
                break;
            }
            case TV_STYLE_3_BRICK: {
                file_name = ":/res/images/block.png";
                color = QVector3D(0.0f, 0.7f, 0.0f);
                break;
            }
            case TV_STYLE_4_BRICK: {
                file_name = ":/res/images/block.png";
                color = QVector3D(0.8f, 0.8f, 0.4f);
                break;
            }
            case TV_STYLE_5_BRICK: {
                file_name = ":/res/images/block.png";
                color = QVector3D(1.0f, 0.5f, 0.0f);
                break;
            }
            case TV_NON_BRICK: {
            default:
                break;
            }
            }

            if (!file_name.isEmpty()) {
                auto texture = std::make_shared<QOpenGLTexture>(QImage(file_name),
                                                                QOpenGLTexture::DontGenerateMipMaps);

                if (texture && texture->isCreated()) {
                    GameObject brick(pos, size, color, texture);

                    if (tile == TV_HARD_BRICK) {
                        brick.SetSolid(true);
                    }
                    bricks_.emplace_back(brick);
                }
            }

            pos += QVector2D(size.x(), 0);
        }
        pos.setX(0);
        pos.setY(pos.y() + size.y());
    }
}
