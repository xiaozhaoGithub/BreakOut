#include "sprite_renderer.h"

// clang-format off
static float sprite_vertices[] = {
	// vertext   // texture pos	
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};
// clang-format on

SpriteRenderer::SpriteRenderer(std::shared_ptr<QOpenGLShaderProgram>& shader_program)
    : shader_program_(shader_program)
{
    InitRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    shader_program_->release();
}

void SpriteRenderer::Draw(std::shared_ptr<QOpenGLTexture> texture, const QVector3D& pos,
                          const QVector3D& size, float rotate, const QVector3D& color)
{
    shader_program_->bind();
    shader_program_->setUniformValue("image", 0);
    shader_program_->setUniformValue("sprite_color", color);

    QMatrix4x4 proj_mat;
    proj_mat.ortho(0.0f, 1366.0f, 768.0f, 0.0f, -1.0f, 1.0f);
    shader_program_->setUniformValue("proj_mat", proj_mat);

    QMatrix4x4 model_mat;
    model_mat.translate(pos);
    model_mat.rotate(rotate, QVector3D(0.0f, 0.0f, 1.0f)); // TODO
    model_mat.scale(size);
    shader_program_->setUniformValue("model_mat", model_mat);

    if (texture && texture->isCreated()) {
        texture->bind(0);
    }

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteRenderer::InitRenderData()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    quint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_vertices), sprite_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
