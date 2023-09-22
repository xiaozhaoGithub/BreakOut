#include "mainwindow.h"

#include <QVBoxLayout>

#include "homepage.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto home_page = new HomePage(this);

    setMinimumSize(1366, 768);
    setCentralWidget(home_page);
}

MainWindow::~MainWindow() {}
