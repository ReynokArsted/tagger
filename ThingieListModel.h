/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */

#ifndef ThingieLISTMODEL_H
#define ThingieLISTMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "Thingie.h"

class ThingieListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ThingieRoles
    {
        NameRole = Qt::UserRole + 1,
        ColorRole,
        ModelIndexRole,
    };
    ThingieListModel(QObject *parent = nullptr);

    void updateFromVector(std::vector<Thingie*> newThingies);
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE QString print();

///
    Q_INVOKABLE void addThing(const QString &name)
    {
        const int row = _thingies.size();
        beginInsertRows(QModelIndex(), row, row);

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("test.db");

        if (!db.open()) {
            qWarning() << "DB open failed:" << db.lastError().text();
            return;
        }

        QSqlQuery query(db);
        query.prepare("INSERT INTO tag(tag_name) VALUES(:name)");
        query.bindValue(":name", name);

        if (!query.exec()) {
            qWarning() << "SELECT tag failed:" << query.lastError().text();
            return;
        }

        _thingies.append(new Thingie(name, this));
        endInsertRows();
    }
///

private:
    QList<Thingie*> _thingies;
};

#endif // ThingieLISTMODEL_H