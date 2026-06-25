/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */

#ifndef ThingieLISTMODEL_H
#define ThingieLISTMODEL_H

#include "Thingie.h"
#include <QAbstractListModel>

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

private:
    QList<Thingie*> _thingies;
};

#endif // ThingieLISTMODEL_H