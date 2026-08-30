#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "ThingModel.h"
#include "Thingie.h"

ThingModel::ThingModel(QObject* parent) : QObject(parent)
{    
    // QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // db.setDatabaseName("test.db");

    // if (!db.open()) {
    //     qWarning() << "DB open failed: " << db.lastError().text();
    //     return;
    // }
    QSqlDatabase db = QSqlDatabase::database("app_connection");
    if (!db.isOpen()) 
    {
        qWarning() << "ERROR: database is not open";
        return;
    }
    QSqlQuery query(db);

    if (!query.exec("SELECT id, tag_name FROM tag")) 
    {
        qWarning() << "SELECT tag failed: " << query.lastError().text();
        return;
    }

    std::vector<Thingie*> tmpV;
    while (query.next()) 
    {
        int id = query.value(0).toInt();
        QString tagName = query.value(1).toString();
        auto* t = new Thingie(id, tagName, this);
        tmpV.push_back(t);
    }
    _listOfThingies.updateFromVector(tmpV);
}