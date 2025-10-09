#include "mainwindow.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);

    if (otc_init(nullptr) != OTC_SUCCESS) {
        std::cout << "Could not init OpenTok library" << std::endl;
    }
    otc_log_enable(OTC_LOG_LEVEL_TRACE);

    MainWindow w;
    w.show();
    return a.exec();
    otc_destroy();
}

#include "main.moc"
