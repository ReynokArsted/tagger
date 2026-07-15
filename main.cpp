#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <objbase.h>

#include "Translator.h"
#include "FileListModel.h"

void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file("log.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream out(&file);
    out << msg << "\n";
}

int main(int argc, char *argv[])
{
    QFile::remove("log.txt");
    qInstallMessageHandler(logHandler);

    QGuiApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "DB open failed: " << db.lastError().text();
        return -1;
    }

    //QSqlQuery query(db);

    //query.exec("create table file(id integer, path text, PRIMARY KEY(id AUTOINCREMENT))");
    //query.exec("create table tag_file(id integer, tag_id integer, file_id integer, PRIMARY KEY(id AUTOINCREMENT))");
    
    // query.exec("create table tag(id integer, tag_name varchar(20), PRIMARY KEY(id AUTOINCREMENT))");
    // query.exec("insert into tag(tag_name) values('spring')");
    // query.exec("insert into tag(tag_name) values('cat')");
    // query.exec("insert into tag(tag_name) values('electronics')");

    // query.exec("create table items (id int primary key,"
    //                                          "imagefile int,"
    //                                          "itemtype varchar(20),"
    //                                          "description varchar(100))");

    QQmlApplicationEngine engine;
    Translator translator(&engine);
    engine.rootContext()->setContextProperty("Translator", &translator);

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
