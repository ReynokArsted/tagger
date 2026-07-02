/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */
#include <QDebug>

#include "ThingieListModel.h"

ThingieListModel::ThingieListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void ThingieListModel::updateFromVector(std::vector<Thingie*> newThingies)
{
    beginResetModel();
    _thingies.clear();
    for (const auto &item : newThingies)
    {
        _thingies << item;
    }
    endResetModel();
}

QHash<int, QByteArray> ThingieListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[ModelIndexRole] = "modelIndex";
    return roles;
}

QVariant ThingieListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const Thingie *thingie = _thingies[index.row()];
    switch (role)
    {
    case NameRole:
        return thingie->name();

    case ColorRole:
        return thingie->color();

    case ModelIndexRole:
        if (std::find(_thingies.begin(), _thingies.end(), thingie) != _thingies.end()) {
          return std::distance(_thingies.begin(), std::find(_thingies.begin(), _thingies.end(), thingie));
        } else {
          return -1;
        }

    default:
        return QVariant();
    }
}

int ThingieListModel::rowCount(const QModelIndex &) const
{
    return _thingies.count();
}


void ThingieListModel::move(int from, int to)
{
    if (from < 0 || from >= rowCount() || to < 0 || to >= rowCount() || from == to)
        return;

    int destRow = (to > from) ? to + 1 : to;
    beginMoveRows(QModelIndex(), from, from, QModelIndex(), destRow);
    _thingies.move(from, to);
    endMoveRows();
}

QString ThingieListModel::print()
{
    QString tmp;
    for(int i = 0; i < _thingies.size(); ++i) {
        tmp.append(QString::number(i));
        tmp.append(": ");
        tmp.append(_thingies.at(i)->name());
        tmp.append("; ");
    }
    return tmp;
}