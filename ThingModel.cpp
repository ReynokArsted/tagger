/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "ThingModel.h"
#include "Thingie.h"

ThingModel::ThingModel(QObject* parent) : QObject(parent)
{    
    // std::vector<Thingie*> tmpV;
    // tmpV.push_back(new Thingie("Coffee Bean", this));
    // tmpV.push_back(new Thingie("Small Cup", this));
    // tmpV.push_back(new Thingie("Remy van Elst", this));
    // tmpV.push_back(new Thingie("Fire information", this));
    // tmpV.push_back(new Thingie("New Products", this));
    // tmpV.push_back(new Thingie("New Videos", this));
    // tmpV.push_back(new Thingie("Corona Info", this));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed:" << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT id, tag_name FROM tag")) {
        qWarning() << "SELECT tag failed:" << query.lastError().text();
        return;
    }

    std::vector<Thingie*> tmpV;
    while (query.next()) {
        int id = query.value(0).toInt();
        QString tagName = query.value(1).toString();

        auto* t = new Thingie(tagName, this);
        // если у Thingie есть поле/метод для id — установите:
        // t->setId(id);

        tmpV.push_back(t);
    }
    _listOfThingies.updateFromVector(tmpV);
}
