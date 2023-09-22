#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <memory>

/**
 * @brief 精灵渲染者, 抽象绘制接口
 */
class SpriteRenderer : protected QOpenGLFunctions_3_3_Core
{
public:
    SpriteRenderer(std::shared_ptr<QOpenGLShaderProgram>& shader_program);
    ~SpriteRenderer();

    void Draw(std::shared_ptr<QOpenGLTexture> texture, const QVector3D& pos,
              const QVector3D& size = QVector3D(10.0f, 10.0f, 10.0f), float rotate = 0.0f,
              const QVector3D& color = QVector3D(1.0f, 1.0f, 1.0f));

private:
    void InitRenderData();

private:
    quint32 vao_;
    std::shared_ptr<QOpenGLShaderProgram> shader_program_;
};
#endif
