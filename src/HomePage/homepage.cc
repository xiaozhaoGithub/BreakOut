#include "homepage.h"

#include <QOpenGLWidget>
#include <QVBoxLayout>

#include "game_gl_widget.h"

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
{
    auto game_gl_widget = new GameGlWidget(this);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(game_gl_widget);
}

HomePage::~HomePage() {}
