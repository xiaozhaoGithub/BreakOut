#include "particle_generator.h"

#include <time.h>

// clang-format off
static float vertices[] = {
	// vertext   // texture pos	
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};
// clang-format on

ParticleGenerator::ParticleGenerator(std::shared_ptr<QOpenGLShaderProgram> shader,
                                     std::shared_ptr<QOpenGLTexture> texture, int num)
    : shader_(shader)
    , texture_(texture)
    , vao_(0)
    , lastUnusedIndex_(0)
{
    srand((unsigned int)time(NULL));

    for (int i = 0; i < num; ++i) {
        particles_.emplace_back(Particel());
    }

    InitRenderData();
}

void ParticleGenerator::Update(float dt, int new_particle_num, GameObject* object)
{
    for (int i = 0; i < new_particle_num; ++i) {
        lastUnusedIndex_ = FirstUnusedParticleIndex();
        RespawnParticles(lastUnusedIndex_, object);
    }

    for (auto& particle : particles_) {
        particle.color.setW(particle.color.w() - dt * 2.5f);
        particle.life -= dt;
    }
}

void ParticleGenerator::Draw()
{
    texture_->bind(0);
    shader_->bind();
    for (auto& particle : particles_) {
        if (particle.life <= 0.0f)
            continue;

        shader_->setUniformValue("pos", particle.pos);
        shader_->setUniformValue("color", particle.color);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void ParticleGenerator::InitRenderData()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    quint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int ParticleGenerator::FirstUnusedParticleIndex()
{
    for (int i = lastUnusedIndex_; i < particles_.size(); ++i) {
        if (particles_[i].life <= 0.0f) {
            return i;
        }
    }

    for (int i = 0; i < lastUnusedIndex_; ++i) {
        if (particles_[i].life <= 0.0f) {
            return i;
        }
    }

    return 0;
}

void ParticleGenerator::RespawnParticles(int index, GameObject* object)
{
    particles_[index].pos = QVector2D(rand() % 10 - 5, rand() % 10 - 5);

    float color_value = (rand() % 50) / 100.0f + 0.5f;
    particles_[index].color = QVector4D(color_value, color_value, color_value, 1.0f);
    particles_[index].life = 1.0f;

    if (auto sphere = dynamic_cast<SphereObject*>(object)) {
        particles_[index].velocity = sphere->Velocity() * 0.1f;
    }
}
