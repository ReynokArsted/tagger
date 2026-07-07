#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

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

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
        return -1;
    }

    // QSqlQuery query(db);
    
    // query.exec("create table tag(id integer, tag_name varchar(20), PRIMARY KEY(id AUTOINCREMENT))");
    // query.exec("insert into tag(tag_name) values('spring')");
    // query.exec("insert into tag(tag_name) values('cat')");
    // query.exec("insert into tag(tag_name) values('electronics')");

    // query.exec("create table items (id int primary key,"
    //                                          "imagefile int,"
    //                                          "itemtype varchar(20),"
    //                                          "description varchar(100))");
    // query.exec("insert into items "
    //            "values(0, 0, 'Qt',"
    //            "'Qt is a full development framework with tools designed to "
    //            "streamline the creation of stunning applications and  "
    //            "amazing user interfaces for desktop, embedded and mobile "
    //            "platforms.')");
    // query.exec("insert into items "
    //            "values(1, 1, 'Qt Quick',"
    //            "'Qt Quick is a collection of techniques designed to help "
    //            "developers create intuitive, modern-looking, and fluid "
    //            "user interfaces using a CSS & JavaScript like language.')");
    // query.exec("insert into items "
    //            "values(2, 2, 'Qt Creator',"
    //            "'Qt Creator is a powerful cross-platform integrated "
    //            "development environment (IDE), including UI design tools "
    //            "and on-device debugging.')");
    // query.exec("insert into items "
    //            "values(3, 3, 'Qt Project',"
    //            "'The Qt Project governs the open source development of Qt, "
    //            "allowing anyone wanting to contribute to join the effort "
    //            "through a meritocratic structure of approvers and "
    //            "maintainers.')");

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
