#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "filereading.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<fileReading>("ReadFile", 1, 0, "ReadFile");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/FinalGui_all/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
