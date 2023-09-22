#ifndef GAME_GL_WIDGET_H_
#define GAME_GL_WIDGET_H_

#include <QList>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include "game_object.h"

class GameGlWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GameGlWidget(QWidget* parent = nullptr);
    ~GameGlWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QList<std::unique_ptr<GameObject>> bricks_;
    quint32 vao_;
    QOpenGLShaderProgram shaderProgram_;
};
#endif
