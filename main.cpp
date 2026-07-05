#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle(QString::fromUtf8("智能监控系统"));
    window.resize(960, 680);
    window.show();

    return app.exec();
}
