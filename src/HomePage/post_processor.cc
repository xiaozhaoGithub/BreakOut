#include "post_processor.h"

#include <QOpenGLVertexArrayObject>

// clang-format off
static constexpr float vertices[] = {
	// vertext   // texture pos	
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};

static constexpr GLfloat offset = 1.0f / 300.0f;
static constexpr GLfloat offsets[9][2] = {
    {-offset, offset},  // top-left
    {0.0f, offset},     // top-center
    {offset, offset},   // top-right
    {-offset, 0.0f},    // center-left
    {0.0f, 0.0f},       // center-center
    {offset, 0.0f},     // center - right
    {-offset, -offset}, // bottom-left
    {0.0f, -offset},    // bottom-center
    {offset, -offset}   // bottom-right
};

static constexpr GLfloat blur_kernels[9] {
	1.0 / 16, 2.0 / 16, 1.0 / 16, 
	2.0 / 16, 4.0 / 16, 2.0 / 16, 
	1.0 / 16, 2.0 / 16, 1.0 / 16
};
// clang-format on


PostProcessor::PostProcessor(std::shared_ptr<QOpenGLShaderProgram> shader,
                             std::shared_ptr<QOpenGLFramebufferObject> fbo)

    : vao_(0)
    , shader_(shader)
    , fbo_(fbo)
    , is_shake_(false)
    , duration_(0)
{
    InitRenderData();

    shader_->bind();
    shader_->setUniformValueArray("offsets", (float*)offsets, 9, 2);
    shader_->setUniformValueArray("blur_kernels", blur_kernels, 9, 1);
}

PostProcessor::~PostProcessor()
{
    glDeleteVertexArrays(1, &vao_);
}

void PostProcessor::BeginProcessor()
{
    fbo_->bind();
}

void PostProcessor::EndProcessor()
{
    auto ct = QOpenGLContext::currentContext();
    if (!ct)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, ct->defaultFramebufferObject());
}

void PostProcessor::Update(float dt)
{
    shader_->bind();
    shader_->setUniformValue("time", dt);

    if (duration_ > 0.0f) {
        duration_ = std::max(duration_ - dt, 0.0f);
    }

    if (is_shake_ && duration_ <= 0.0f) {
        is_shake_ = false;
    }
}

void PostProcessor::Draw()
{
    shader_->bind();
    shader_->setUniformValue("image", 0);
    shader_->setUniformValue("is_shake", is_shake_);

    QMatrix4x4 proj_mat;
    proj_mat.ortho(0.0f, fbo_->size().width(), fbo_->size().height(), 0.0f, -1.0f, 1.0f);
    shader_->setUniformValue("proj_mat", proj_mat);

    QMatrix4x4 model_mat;
    model_mat.translate(QVector2D(0.0f, 0.0f));
    model_mat.scale(QVector2D(fbo_->size().width(), fbo_->size().height()));
    shader_->setUniformValue("model_mat", model_mat);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_->texture());

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcessor::SetFbo(std::shared_ptr<QOpenGLFramebufferObject> fbo)
{
    fbo_ = fbo;
}

void PostProcessor::SetShake(bool is_shake)
{
    is_shake_ = is_shake;
    duration_ = is_shake_ ? 0.05f : 0.0f;
}

void PostProcessor::InitRenderData()
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
