#ifndef PARTICLE_GENERATOR_H_
#define PARTICLE_GENERATOR_H_

#include <QVector2D>
#include <QVector4D>
#include <vector>

#include "game_object.h"

struct Particel
{
    QVector2D pos;
    QVector2D velocity;
    QVector4D color;
    float life = 1.0f;
};

class ParticleGenerator : protected QOpenGLFunctions_3_3_Core
{
public:
    ParticleGenerator(std::shared_ptr<QOpenGLShaderProgram> shader,
                      std::shared_ptr<QOpenGLTexture> texture, int num = 500);

    void Update(float dt, int new_particle_num, GameObject* object, const QVector2D& offset);
    void Draw();

private:
    void InitRenderData();
    int FirstUnusedParticleIndex();
    void RespawnParticles(int index, GameObject* object, const QVector2D& offset);

private:
    std::vector<Particel> particles_;
    int lastUnusedIndex_;

    quint32 vao_;
    std::shared_ptr<QOpenGLShaderProgram> shader_;
    std::shared_ptr<QOpenGLTexture> texture_;
};

#endif
