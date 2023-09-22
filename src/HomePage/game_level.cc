#include "game_level.h"

#include <fstream>
#include <memory>
#include <sstream>

/**
 * @brief Check collision between AABB.
 */
bool CheckCollision(GameObject* one, GameObject* two)
{
    bool is_x_axis_align = one->Pos().x() + one->Size().x() >= two->Pos().x()
                           && two->Pos().x() + two->Size().x() >= one->Pos().x();

    bool is_y_axis_align = one->Pos().y() + one->Size().y() >= two->Pos().y()
                           && two->Pos().y() + two->Size().y() >= one->Pos().y();

    return is_x_axis_align && is_y_axis_align;
}

/**
 * @brief Check collision between AABB and sphere.
 */
#include <algorithm>
bool CheckCollisionEx(SphereObject* one, GameObject* two)
{
    float radius = one->Radius();
    QVector2D sphere_center = one->Pos() + QVector2D(radius, radius);
    QVector2D box_center = two->Pos() + (two->Size() / 2);
    QVector2D center_diff = sphere_center - box_center;

    QVector2D clamp_diff = std::clamp(center_diff, -two->Size() / 2, two->Size() / 2);
    QVector2D closest_point = box_center + clamp_diff; // important

    return radius >= (sphere_center - closest_point).length();
}

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

void GameLevel::Load()
{
    auto level_datas = ReadLayersFromFile("res/levels/one.lvl");
    BuildBricks(std::move(level_datas));
}

void GameLevel::Draw(std::shared_ptr<SpriteRenderer> renderer)
{
    for (auto& brick : bricks_) {
        if (!brick.IsDestroyed()) {
            brick.Draw(renderer);
        }
    }
}

void GameLevel::DoCollision(SphereObject* object)
{
    for (auto& brick : bricks_) {
        if (brick.IsDestroyed())
            continue;

        if (CheckCollisionEx(object, &brick)) {
            if (brick.IsSolid()) {
                auto sphere = dynamic_cast<SphereObject*>(object);
                auto v = sphere->Velocity();
                v.setY(-sphere->Velocity().y());
                sphere->SetVelocity(v);
            } else {
                brick.Destroy();
            }
        }
    }
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
