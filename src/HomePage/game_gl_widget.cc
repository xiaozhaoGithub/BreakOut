#include "game_gl_widget.h"

#include <QDateTime>
#include <QOpenGLTexture>

#include "collision_helper.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "resource_manager.h"

constexpr float kVelocity = 35.0f;
constexpr float kSphereRadius = 12.5f;
constexpr QVector2D kPlayerSize(100.0f, 20.0f);

GameGlWidget::GameGlWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , game_level_(std::make_unique<GameLevel>(width(), height()))
    , powerup_manager_(std::make_shared<PowerUpManager>())
{
    setFocusPolicy(Qt::StrongFocus);
}

GameGlWidget::~GameGlWidget()
{
    Singleton<ResourceManager>::ReleaseInstance();
}

void GameGlWidget::initializeGL()
{
    initializeOpenGLFunctions();

    auto res_manager = Singleton<ResourceManager>::Instance();

    // sprites
    auto shader_program = std::make_shared<QOpenGLShaderProgram>();
    shader_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/shaders/sprite.vert");
    shader_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/shaders/sprite.frag");
    shader_program->link();

    sprite_renderer_ = std::make_shared<SpriteRenderer>(shader_program);
    bg_tex_ = res_manager->Texture("wall", ":/res/images/wall.png", false);
    paddle_tex_ = res_manager->Texture("paddle", ":/res/images/paddle.png", false);
    sphere_tex_ = res_manager->Texture("awesomeface", ":/res/images/awesomeface.png", false);
    sphere_tex_->setWrapMode(QOpenGLTexture::ClampToEdge);

    player_ = std::make_unique<GameObject>(QVector2D(0.0f, 0.0f), kPlayerSize,
                                           QVector3D(1.0f, 1.0f, 1.0f), paddle_tex_);

    sphere_ = std::make_unique<SphereObject>(QVector2D(0.0f, 0.0f), kSphereRadius,
                                             QVector3D(1.0f, 1.0f, 1.0f), sphere_tex_);

    // particles
    particle_shader_ = std::make_shared<QOpenGLShaderProgram>();
    particle_shader_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/shaders/particle.vert");
    particle_shader_->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                              ":/res/shaders/particle.frag");
    particle_shader_->link();

    auto particle_tex = res_manager->Texture("particle", ":/res/images/particle.png", true);
    particle_generator_ = std::make_shared<ParticleGenerator>(particle_shader_, particle_tex);

    // post-process
    auto post_shader = std::make_shared<QOpenGLShaderProgram>();
    post_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/res/shaders/post_processor.vert");
    post_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/res/shaders/post_processor.frag");
    post_shader->link();

    auto post_fbo = std::make_shared<QOpenGLFramebufferObject>(width(), height());
    post_processor_ = std::make_shared<PostProcessor>(post_shader, post_fbo);
    game_level_->SetPostProcessor(post_processor_);

    // scheduled updates
    render_timer_ = new QTimer(this);
    render_timer_->setInterval(10);
    render_timer_->start();
    connect(render_timer_, &QTimer::timeout, this, &GameGlWidget::UpdateGame);
}

void GameGlWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);

    sprite_renderer_->SetSize(QVector2D(w, h));

    game_level_->SetViewport(w, h);
    game_level_->Load();

    player_->SetPos(QVector2D(((float)w - kPlayerSize.x()) / 2, (float)h - kPlayerSize.y()));
    sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                              (float)h - kPlayerSize.y() - 2 * sphere_->Radius()));

    QMatrix4x4 proj_mat;
    proj_mat.ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    particle_shader_->bind();
    particle_shader_->setUniformValue("proj_mat", proj_mat);

    post_processor_->SetFbo(std::make_shared<QOpenGLFramebufferObject>(w, h));
}

void GameGlWidget::paintGL()
{
    post_processor_->BeginProcessor();

    sprite_renderer_->Draw(bg_tex_, QVector2D(0.0f, 0.0f), QVector2D(width(), height()), 0.0f,
                           QVector3D(0.0f, 0.0f, 0.0f));

    game_level_->Draw(sprite_renderer_);
    player_->Draw(sprite_renderer_);
    particle_generator_->Draw();
    sphere_->Draw(sprite_renderer_);
    powerup_manager_->Draw(sprite_renderer_);

    post_processor_->EndProcessor();
    post_processor_->Draw();
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

void GameGlWidget::UpdateGame()
{
    current_frame_time_ = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (last_frame_time_ == 0) {
        last_frame_time_ = current_frame_time_;
        return;
    }

    float dt = (float)(current_frame_time_ - last_frame_time_) / 1000.0f;
    last_frame_time_ = current_frame_time_;

    sphere_->Move(dt, width(), height());
    DoCollision();

    float offset = sphere_->Radius() / 2.0f;
    particle_generator_->Update(dt, 2, sphere_.get(), QVector2D(offset, offset));

    powerup_manager_->Update(dt, std::bind(&GameGlWidget::OnDeactivatePowerUp, this,
                                           std::placeholders::_1));

    post_processor_->Update(dt);

    update();
}

void GameGlWidget::DoCollision()
{
    if (!sphere_->IsStuck()) {
        // The sphere collides with the bricks.
        game_level_->DoCollision(sphere_.get(), std::bind(&PowerUpManager::SpawnPowerUp,
                                                          powerup_manager_, std::placeholders::_1));

        // The sphere collides with the player.
        if (CollisionHelper::CheckCollision(sphere_.get(), player_.get())) {
            float player_center_x = player_->Pos().x() + player_->Size().x() / 2;

            float distance = sphere_->Pos().x() + sphere_->Radius() - player_center_x;
            float percentage = distance / (player_->Size().x() / 2);

            float strength = 2.0f;
            QVector2D old_velocity = sphere_->Velocity();

            QVector2D velocity;
            velocity.setX(sphere_->DefaultVelocity().x() * percentage * strength);
            velocity.setY(-old_velocity.y());

            // Keep the speed size, only change direction.
            velocity = velocity.normalized() * old_velocity.length();
            sphere_->SetVelocity(velocity);
        }
    }

    // The player collides with the powerups.
    powerup_manager_->DoCollision(player_.get(), std::bind(&GameGlWidget::OnActivatePowerUp, this,
                                                           std::placeholders::_1));

    CheckGameState();
}

void GameGlWidget::HandlePlayerMove(const QVector2D& pos)
{
    player_->SetPos(pos);

    if (sphere_->IsStuck()) {
        sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                                  (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));
    }
}

void GameGlWidget::CheckGameState()
{
    // bottom border
    float sphere_bottom = sphere_->Pos().y() + 2 * sphere_->Radius();
    if (sphere_bottom >= height()) {
        player_->Reset(
            QVector2D(((float)width() - kPlayerSize.x()) / 2, (float)height() - kPlayerSize.y()));

        sphere_->Reset(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                                 (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));
    }
}

void GameGlWidget::OnActivatePowerUp(PowerUp::Type type)
{
    switch (type) {
    case PowerUp::T_SPEED:
        sphere_->SetVelocity(sphere_->Velocity() * 1.2f);
        break;
    case PowerUp::T_STICKY:
        sphere_->SetPos(
            QVector2D(player_->Pos().x() + (player_->Size().x() - 2 * sphere_->Radius()) / 2.0f,
                      (float)height() - player_->Size().y() - 2 * sphere_->Radius()));
        sphere_->SetStuck(true);
        break;
    case PowerUp::T_PASS_THROUGH:
        sphere_->SetPassThrough(true);
        break;
    case PowerUp::T_PAD_SIZE_INCREASE:
        player_->SetSize(QVector2D(player_->Size().x() + 50, player_->Size().y()));
        break;
    case PowerUp::T_CONFUSE:
        post_processor_->SetConfuse(true);
        break;
    case PowerUp::T_CHAOS:
        post_processor_->SetChaos(true);
        break;
    default:
        break;
    }
}

void GameGlWidget::OnDeactivatePowerUp(PowerUp::Type type)
{
    switch (type) {
    case PowerUp::T_STICKY:
        sphere_->SetPos(
            QVector2D(player_->Pos().x() + (player_->Size().x() - 2 * sphere_->Radius()) / 2.0f,
                      (float)height() - player_->Size().y() - 2 * sphere_->Radius()));
        sphere_->SetStuck(true);
        break;
    case PowerUp::T_PASS_THROUGH:
        sphere_->SetPassThrough(true);
        break;
    case PowerUp::T_CONFUSE:
        post_processor_->SetConfuse(true);
        break;
    case PowerUp::T_CHAOS:
        post_processor_->SetChaos(true);
        break;
    default:
        break;
    }
}
