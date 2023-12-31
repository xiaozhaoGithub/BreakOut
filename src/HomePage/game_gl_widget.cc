#include "game_gl_widget.h"

#include <QApplication>
#include <QDateTime>
#include <QMediaPlayer>
#include <QOpenGLTexture>

#include "audio_manager.h"
#include "collision_helper.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "resource_manager.h"

constexpr float kVelocity = 35.0f;
constexpr float kSphereRadius = 12.5f;
constexpr QVector2D kPlayerSize(100.0f, 20.0f);

GameGlWidget::GameGlWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , game_state_(std::make_unique<GameState>())
    , game_level_(std::make_unique<GameLevel>(width(), height()))
    , powerup_manager_(std::make_shared<PowerUpManager>())
{
    setFocusPolicy(Qt::StrongFocus);
    game_state_->SetLives(3);
    game_level_->SetLevelNum(4);

    InitBgMusic();
}

GameGlWidget::~GameGlWidget()
{
    Singleton<ResourceManager>::ReleaseInstance();
    Singleton<AudioManager>::Instance()->Stop();
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
    bg_tex_ = res_manager->Texture("background", ":/res/images/background.jpg", false);
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

    // texts
    text_renderer_ = std::make_unique<TextRenderer>();
    text_renderer_->Load("res/fonts/arial.ttf", 24);

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

    game_level_->Resize(w, h);

    player_->SetPos(QVector2D(((float)w - kPlayerSize.x()) / 2, (float)h - kPlayerSize.y()));
    sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                              (float)h - kPlayerSize.y() - 2 * sphere_->Radius()));

    particle_generator_->Resize(w, h);
    text_renderer_->Resize(w, h);

    post_processor_->SetFbo(std::make_shared<QOpenGLFramebufferObject>(w, h));
}

void GameGlWidget::paintGL()
{
    post_processor_->BeginProcessor();

    sprite_renderer_->Draw(bg_tex_, QVector2D(0.0f, 0.0f), QVector2D(width(), height()), 0.0f,
                           QVector3D(1.0f, 1.0f, 1.0f));

    game_level_->Draw(sprite_renderer_);
    player_->Draw(sprite_renderer_);
    particle_generator_->Draw();
    sphere_->Draw(sprite_renderer_);
    powerup_manager_->Draw(sprite_renderer_);

    post_processor_->EndProcessor();
    post_processor_->Draw();

    game_state_->Draw(text_renderer_);
}

void GameGlWidget::keyPressEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyPressEvent(event);

    auto pos = player_->Pos();
    int key = event->key();
    switch (key) {
    case Qt::Key_Space: {
        HandleSpaceInput();
        break;
    }
    case Qt::Key_Up:
    case Qt::Key_Down: {
        HandleLevelMove(key);
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
    case Qt::Key_Enter:
    case Qt::Key_Return: {
        HandleEnterInput();
        break;
    }
    case Qt::Key_Escape: {
        HandleEscInput();
        break;
    }
    default:
        break;
    }

    update();
}

void GameGlWidget::InitBgMusic()
{
    auto media_player = new QMediaPlayer(this);
    media_player->setMedia(QUrl("qrc:/res/audio/breakout.mp3"));
    media_player->setVolume(50);
    media_player->play();
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

    powerup_manager_->Update(dt, width(), height(),
                             std::bind(&GameGlWidget::OnDeactivatePowerUp, this, std::placeholders::_1));

    post_processor_->Update(dt);

    if (game_level_->IsCompleted()) {
        ResetState(GameState::SF_WIN);
    }

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
            sphere_->SetStuck(sphere_->IsSticky());

            Singleton<AudioManager>::Instance()->Play(":/res/audio/bleep_player.wav");
        }
    }

    // The player collides with the powerups.
    powerup_manager_->DoCollision(player_.get(), std::bind(&GameGlWidget::OnActivatePowerUp, this,
                                                           std::placeholders::_1));

    CheckSpherePos();
}

void GameGlWidget::HandleLevelMove(int key)
{
    if (game_state_->State() != GameState::SF_MENU)
        return;

    if (key == Qt::Key_Up) {
        game_level_->PreviousLevel();
    } else {
        game_level_->NextLevel();
    }
}

void GameGlWidget::HandlePlayerMove(const QVector2D& pos)
{
    if (game_state_->State() == GameState::SF_MENU || game_state_->State() == GameState::SF_WIN)
        return;

    player_->SetPos(pos);

    if (sphere_->IsStuck()) {
        sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                                  (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));
    }
}

void GameGlWidget::HandleEnterInput()
{
    if (game_state_->State() == GameState::SF_MENU) {
        ResetState(GameState::SF_ACTIVE);
    } else if (game_state_->State() == GameState::SF_WIN) {
        ResetState(GameState::SF_MENU);
    }
}

void GameGlWidget::HandleSpaceInput()
{
    if (game_state_->State() == GameState::SF_MENU || game_state_->State() == GameState::SF_WIN)
        return;

    sphere_->SetStuck(false);
    sphere_->SetSticky(false);
}

void GameGlWidget::HandleEscInput()
{
    if (game_state_->State() != GameState::SF_WIN)
        return;

    qApp->quit();
}

void GameGlWidget::CheckSpherePos()
{
    // bottom border
    float sphere_bottom = sphere_->Pos().y() + 2 * sphere_->Radius();
    if (sphere_bottom >= height()) {
        player_->Reset(
            QVector2D(((float)width() - kPlayerSize.x()) / 2, (float)height() - kPlayerSize.y()));
        player_->SetSize(kPlayerSize);

        sphere_->Reset(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                                 (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));

        game_state_->SetLives(game_state_->Lives() - 1);
        if (game_state_->Lives() == 0) {
            ResetState(GameState::SF_MENU);
        }
    }
}

void GameGlWidget::ResetState(GameState::StateFlag state)
{
    if (state == game_state_->State())
        return;

    game_state_->SetState(state);
    game_state_->SetLives(3);
    game_level_->Load(0);

    post_processor_->SetShake(false);
    post_processor_->SetConfuse(false);
    powerup_manager_->Clear();

    sphere_->SetVelocity(sphere_->DefaultVelocity());
    sphere_->SetPassThrough(false);
    sphere_->SetSticky(false);
    sphere_->SetStuck(true);
    player_->SetColor(QVector3D(1.0f, 1.0f, 1.0f));

    switch (state) {
    case GameState::SF_MENU: {
        post_processor_->SetChaos(false);

        player_->SetSize(kPlayerSize);
        player_->SetPos(
            QVector2D(((float)width() - kPlayerSize.x()) / 2, (float)height() - kPlayerSize.y()));

    } break;
    case GameState::SF_WIN: {
        post_processor_->SetChaos(true);
    } break;
    default:
        break;
    }

    sphere_->SetPos(QVector2D(player_->Pos().x() + (kPlayerSize.x() - 2 * sphere_->Radius()) / 2.0f,
                              (float)height() - kPlayerSize.y() - 2 * sphere_->Radius()));
}

void GameGlWidget::OnActivatePowerUp(PowerUp::Type type)
{
    Singleton<AudioManager>::Instance()->Play(":/res/audio/powerup.wav");

    switch (type) {
    case PowerUp::T_SPEED:
        sphere_->SetVelocity(sphere_->Velocity() * 1.2f);
        break;
    case PowerUp::T_STICKY:
        sphere_->SetSticky(true);
        player_->SetColor(QVector3D(1.0f, 0.5f, 1.0f));
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
        sphere_->SetSticky(false);
        player_->SetColor(QVector3D(1.0f, 1.0f, 1.0f));
        break;
    case PowerUp::T_PASS_THROUGH:
        sphere_->SetPassThrough(false);
        break;
    case PowerUp::T_CONFUSE:
        post_processor_->SetConfuse(false);
        break;
    case PowerUp::T_CHAOS:
        post_processor_->SetChaos(false);
        break;
    default:
        break;
    }
}
