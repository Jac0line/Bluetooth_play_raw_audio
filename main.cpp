#include "chat.h"

#include <QtWidgets/qapplication.h>
//#include <QtCore/QLoggingCategory>

int main(int argc, char *argv[])
{
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    Chat d;
    QObject::connect(&d, &Chat::accepted, &app, &QApplication::quit);

#ifdef Q_OS_ANDROID
    d.showMaximized();
#else
    d.show();
#endif

    app.exec();

    return 0;
}

