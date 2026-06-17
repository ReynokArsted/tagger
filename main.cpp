#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Translator.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    Translator translator(&engine);
    engine.rootContext()->setContextProperty("AppTranslator", &translator);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("untitled", "Main");

    return app.exec();
}
