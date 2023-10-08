#ifndef POST_PROCESSOR_H_
#define POST_PROCESSOR_H_

#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <vector>

class PostProcessor : protected QOpenGLFunctions_3_3_Core
{
public:
    PostProcessor(std::shared_ptr<QOpenGLShaderProgram> shader,
                  std::shared_ptr<QOpenGLFramebufferObject> fbo);
    ~PostProcessor();

    void BeginProcessor();
    void EndProcessor();
    void Update(float dt);
    void Draw();

    void SetFbo(std::shared_ptr<QOpenGLFramebufferObject> fbo);
    void SetShake(bool state);
    void SetConfuse(bool state);
    void SetChaos(bool state);

private:
    void InitRenderData();

private:
    quint32 vao_;
    QSize size_;
    bool is_shake_;
    bool is_confuse_;
    bool is_chaos_;
    std::shared_ptr<QOpenGLShaderProgram> shader_;
    std::shared_ptr<QOpenGLFramebufferObject> fbo_;

    float duration_;
};

#endif
