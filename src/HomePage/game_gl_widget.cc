#include "game_gl_widget.h"

#include <QDateTime>
#include <QOpenGLTexture>
#include <mutex>

constexpr float kVelocity = 25.0f;
constexpr float kSphereRadius = 12.5f;
constexpr QVector2D kPlayerSize(100.0f, 20.0f);

GameGlWidget::GameGlWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , game_level_(std::make_unique<GameLevel>(width(), height()))
{
    setFocusPolicy(Qt::StrongFocus);
}

GameGlWidget::~GameGlWidget() {}

void GameGlWidget::initializeGL()
{
    initializeOpenGLFunctions();

    auto shader_program = std::make_shared<QOpenGLShaderProgram>();
    shader_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/shaders/sprite.vert");
    shader_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/shaders/sprite.frag");
    shader_program->link();

    sprite_renderer_ = std::make_shared<SpriteRenderer>(shader_program);
    bg_tex_ = std::make_shared<QOpenGLTexture>(QImage(":/res/images/wall.png"));
    paddle_tex_ = std::make_shared<QOpenGLTexture>(QImage(":/res/images/paddle.png"));
    sphere_tex_ = std::make_shared<QOpenGLTexture>(QImage(":/res/images/awesomeface.png"));
    sphere_tex_->setWrapMode(QOpenGLTexture::ClampToEdge);

    player_ = std::make_unique<GameObject>(QVector2D(0.0f, 0.0f), kPlayerSize,
                                           QVector3D(1.0f, 1.0f, 1.0f), paddle_tex_);

    sphere_ = std::make_unique<SphereObject>(QVector2D(0.0f, 0.0f), kSphereRadius,
                                             QVector3D(1.0f, 1.0f, 1.0f), sphere_tex_);

    render_timer_ = new QTimer(this);
    render_timer_->setInterval(10);
    render_timer_->start();
    connect(render_timer_, &QTimer::timeout, this, &GameGlWidget::UpdateParam);
}

void GameGlWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);

    game_level_->SetViewport(w, h);
    game_level_->Load();

    player_->SetPos(QVector2D(((float)w - kPlayerSize.x()) / 2, (float)h - kPlayerSize.y()));
    sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                              (float)h - kPlayerSize.y() - 2 * sphere_->Radius()));
}

void GameGlWidget::paintGL()
{
    sprite_renderer_->Draw(bg_tex_, QVector3D(0.0f, 0.0f, 0.0f), QVector3D(width(), height(), 0.0f),
                           0.0f, QVector3D(0.0f, 0.0f, 0.0f));

    game_level_->Draw(sprite_renderer_);
    player_->Draw(sprite_renderer_);
    sphere_->Draw(sprite_renderer_);
}

void GameGlWidget::keyPressEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyPressEvent(event);

    auto pos = player_->Pos();
    int key = event->key();
    switch (key) {
    case Qt::Key_Space: {
        sphere_->SetStuck(false);
        break;
    }
    case Qt::Key_Left: {
        float x = pos.x() - kVelocity;
        if (x <= 0) {
            x = 0;
        }
        HandlePlayerMove(QVector2D(x, pos.y()));
        break;
    }
    case Qt::Key_Right: {
        float x = pos.x() + kVelocity;
        if (x >= width() - player_->Size().x()) {
            x = width() - player_->Size().x();
        }
        HandlePlayerMove(QVector2D(x, pos.y()));
        break;
    }
    default:
        break;
    }

    update();
}

void GameGlWidget::UpdateParam()
{
    if (sphere_->isStuck())
        return;

    current_frame_time_ = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (last_frame_time_ == 0) {
        last_frame_time_ = current_frame_time_;
        return;
    }

    float delta_time = (float)(current_frame_time_ - last_frame_time_) / 1000.0f;
    last_frame_time_ = current_frame_time_;

    sphere_->Move(delta_time, width(), height());
    DoCollision();

    update();
}

void GameGlWidget::DoCollision()
{
    game_level_->DoCollision(sphere_.get());

    if (!sphere_->isStuck()) {
        // collision with player
        if (CheckCollision(sphere_.get(), player_.get())) {
            QVector2D velocity = sphere_->Velocity();
            velocity.setY(-velocity.y());
            sphere_->SetVelocity(velocity);
        }

        CheckGameState();
    }
}

void GameGlWidget::HandlePlayerMove(const QVector2D& pos)
{
    player_->SetPos(pos);

    if (sphere_->isStuck()) {
        sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                                  (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));
    }
}

void GameGlWidget::CheckGameState()
{
    // bottom border
    float sphere_bottom = sphere_->Pos().y() + 2 * sphere_->Radius();
    if (sphere_bottom >= height()) {
        player_->SetPos(
            QVector2D(((float)width() - kPlayerSize.x()) / 2, (float)height() - kPlayerSize.y()));

        QVector2D sphere_pos(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                             (float)height() - kPlayerSize.y() - 2 * sphere_->Radius());
        sphere_->Reset(sphere_pos);
    }
}
