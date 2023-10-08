#ifndef GAME_GL_WIDGET_H_
#define GAME_GL_WIDGET_H_

#include <QList>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>

#include "game_level.h"
#include "game_object.h"
#include "particle_generator.h"
#include "sprite_renderer.h"

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

    void keyPressEvent(QKeyEvent* event) override;

private:
    void UpdateGame();
    void DoCollision();
    void HandlePlayerMove(const QVector2D& pos);
    void CheckGameState();

private:
    QTimer* render_timer_;
    qint64 last_frame_time_ = 0;
    qint64 current_frame_time_;

    std::unique_ptr<GameLevel> game_level_;
    std::unique_ptr<GameObject> player_;
    std::unique_ptr<SphereObject> sphere_;

    std::shared_ptr<SpriteRenderer> sprite_renderer_;

    std::shared_ptr<QOpenGLTexture> bg_tex_;
    std::shared_ptr<QOpenGLTexture> paddle_tex_;
    std::shared_ptr<QOpenGLTexture> sphere_tex_;

    std::shared_ptr<QOpenGLShaderProgram> particle_shader_;
    std::shared_ptr<ParticleGenerator> particle_generator_;

    std::shared_ptr<PostProcessor> post_processor_;
    std::shared_ptr<PowerUpManager> powerup_manager_;
};
#endif
