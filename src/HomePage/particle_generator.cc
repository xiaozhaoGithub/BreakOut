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

void ParticleGenerator::Update(float dt, int new_particle_num, GameObject* object,
                               const QVector2D& offset)
{
    for (int i = 0; i < new_particle_num; ++i) {
        lastUnusedIndex_ = FirstUnusedParticleIndex();
        RespawnParticles(lastUnusedIndex_, object, offset);
    }

    for (auto& particle : particles_) {
        particle.life -= dt;

        if (particle.life >= 0.0f) {
            particle.pos -= dt * particle.velocity;
            particle.color.setW(std::max(0.0f, particle.color.w() - dt * 2.5f));
        }
    }
}

void ParticleGenerator::Draw()
{
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // color = src * src_a + dest * 1

    shader_->bind();
    for (auto& particle : particles_) {
        if (particle.life <= 0.0f)
            continue;

        shader_->setUniformValue("pos", particle.pos);
        shader_->setUniformValue("color", particle.color);

        texture_->bind(0);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend default
}

void ParticleGenerator::Resize(int w, int h)
{
    QMatrix4x4 proj_mat;
    proj_mat.ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    shader_->bind();
    shader_->setUniformValue("proj_mat", proj_mat);
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

void ParticleGenerator::RespawnParticles(int index, GameObject* object, const QVector2D& offset)
{
    float color_value = (rand() % 50) / 100.0f + 0.5f;
    particles_[index].color = QVector4D(color_value, color_value, color_value, 1.0f);
    particles_[index].life = 1.0f;

    if (auto sphere = dynamic_cast<SphereObject*>(object)) {
        float rand_value = (rand() % 100 - 50) / 10.0f;
        particles_[index].pos = object->Pos() + QVector2D(rand_value, rand_value) + offset;
        particles_[index].velocity = sphere->Velocity() * 0.1f;
    }
}
