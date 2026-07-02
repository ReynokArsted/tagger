#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>

#include "Translator.h"
#include "FileListModel.h"

void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file("log.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << msg << "\n";
}

int main(int argc, char *argv[])
{
    QFile::remove("log.txt");
    qInstallMessageHandler(logHandler);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    Translator translator(&engine);
    engine.rootContext()->setContextProperty("Translator", &translator);
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    qmlRegisterType<FileListModel>("untitled.files", 1, 0, "FileListModel");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("untitled", "Main");

    return app.exec();
}
