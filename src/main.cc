#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QSurfaceFormat surface_format;
    surface_format.setVersion(3, 3);
    surface_format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surface_format);

    // ��ͬ���ڵ�QOpenGLWidgetʵ��֮��Ĺ���
    qApp->setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
