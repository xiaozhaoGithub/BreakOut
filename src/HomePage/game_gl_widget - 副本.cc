#include "game_gl_widget.h"

#include <QOpenGLTexture>

// clang-format off
static float vertices[] = {
	-0.5f, -0.5f, 0.0f, 
	0.5f, -0.5f, 0.0f, 
	0.0f, 0.5f, 0.0f
};
// clang-format on

GameGlWidget::GameGlWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{}

GameGlWidget::~GameGlWidget() {}

void GameGlWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    quint32 vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shaderProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/shaders/shader.vert");
    shaderProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/shaders/shader.frag");
    shaderProgram_.link();

    QOpenGLTexture texture(QImage(":/res/images/wall.png"));
}

void GameGlWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
}

void GameGlWidget::paintGL()
{
    shaderProgram_.bind();

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
