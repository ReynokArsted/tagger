#include <QDebug>

#include "ThingieListModel.h"

ThingieListModel::ThingieListModel(QObject *parent) :
    QAbstractListModel(parent) {}

void ThingieListModel::updateFromVector(std::vector<Thingie*> newThingies)
{
    beginResetModel();
    _thingies.clear();
    for (const auto &item : newThingies)
        _thingies << item;
    endResetModel();
}

QHash<int, QByteArray> ThingieListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[ModelIndexRole] = "modelIndex";
    roles[IdRole] = "id";
    return roles;
}

void ThingieListModel::addThing(const QString &name)
{
    const int row = _thingies.size();
    beginInsertRows(QModelIndex(), row, row);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed: " << db.lastError().text();
        endInsertRows();
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO tag(tag_name) VALUES(:name)");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "INSERT tag failed:" << query.lastError().text();
        endInsertRows();
        return;
    }

    const int newId = query.lastInsertId().toInt();
    _thingies.append(new Thingie(newId, name, this));
    endInsertRows();
}

bool ThingieListModel::removeThing(int tagId)
{
    const int row = std::find_if(_thingies.begin(), _thingies.end(),
        [tagId](Thingie *t) { return t->id() == tagId; }) - _thingies.begin();

    if (row < 0 || row >= _thingies.size()) return false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed: " << db.lastError().text();
        return false;
    }

    QSqlQuery deleteLinks(db);
    deleteLinks.prepare("DELETE FROM tag_file WHERE tag_id = :tagId");
    deleteLinks.bindValue(":tagId", tagId);
    if (!deleteLinks.exec())
        qWarning() << "DELETE tag_file failed: " << deleteLinks.lastError().text();

    QSqlQuery deleteTag(db);
    deleteTag.prepare("DELETE FROM tag WHERE id = :tagId");
    deleteTag.bindValue(":tagId", tagId);
    if (!deleteTag.exec()) {
        qWarning() << "DELETE tag failed: " << deleteTag.lastError().text();
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    delete _thingies.at(row);
    _thingies.removeAt(row);
    endRemoveRows();

    return true;
}

bool ThingieListModel::assignTagsToFile(const QString &path, const QVariantList &tagIds)
{
    if (path.isEmpty()) return false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed: " << db.lastError().text();
        return false;
    }

    QSqlQuery selectFile(db);
    selectFile.prepare("SELECT id FROM file WHERE path = :path");
    selectFile.bindValue(":path", path);
    if (!selectFile.exec()) {
        qWarning() << "SELECT file failed: " << selectFile.lastError().text();
        return false;
    }

    int fileId = -1;
    if (selectFile.next()) fileId = selectFile.value(0).toInt();
    else {
        QSqlQuery insertFile(db);
        insertFile.prepare("INSERT INTO file(path) VALUES(:path)");
        insertFile.bindValue(":path", path);
        if (!insertFile.exec()) {
            qWarning() << "INSERT file failed: " << insertFile.lastError().text();
            return false;
        }
        fileId = insertFile.lastInsertId().toInt();
    }

    QSet<int> desiredTagIds;
    for (const QVariant &tagIdVar : tagIds) {
        const int tagId = tagIdVar.toInt();
        if (tagId >= 0) desiredTagIds.insert(tagId);
    }

    QSqlQuery selectExisting(db);
    selectExisting.prepare("SELECT tag_id FROM tag_file WHERE file_id = :fileId");
    selectExisting.bindValue(":fileId", fileId);
    if (!selectExisting.exec()) {
        qWarning() << "SELECT existing tag_file failed: " << selectExisting.lastError().text();
        return false;
    }

    QSet<int> existingTagIds;
    while (selectExisting.next())
        existingTagIds.insert(selectExisting.value(0).toInt());

    const QSet<int> toRemove = existingTagIds - desiredTagIds;
    for (int tagId : toRemove) {
        QSqlQuery deleteLink(db);
        deleteLink.prepare("DELETE FROM tag_file WHERE tag_id = :tagId AND file_id = :fileId");
        deleteLink.bindValue(":tagId", tagId);
        deleteLink.bindValue(":fileId", fileId);
        if (!deleteLink.exec())
            qWarning() << "DELETE tag_file failed: " << deleteLink.lastError().text();
    }

    const QSet<int> toAdd = desiredTagIds - existingTagIds;
    for (int tagId : toAdd) {
        QSqlQuery insertTagFile(db);
        insertTagFile.prepare("INSERT INTO tag_file(tag_id, file_id) VALUES(:tagId, :fileId)");
        insertTagFile.bindValue(":tagId", tagId);
        insertTagFile.bindValue(":fileId", fileId);
        if (!insertTagFile.exec())
            qWarning() << "INSERT tag_file failed: " << insertTagFile.lastError().text();
    }

    return true;
}

QVariantList ThingieListModel::tagIdsForFile(const QString &path) const
{
    QVariantList result;
    if (path.isEmpty()) return result;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed: " << db.lastError().text();
        return result;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT tag_file.tag_id FROM tag_file "
        "JOIN file ON file.id = tag_file.file_id "
        "WHERE file.path = :path"
    );
    query.bindValue(":path", path);

    if (!query.exec()) {
        qWarning() << "SELECT tagIdsForFile failed: " << query.lastError().text();
        return result;
    }

    while (query.next())
        result.append(query.value(0).toInt());

    return result;
}

QVariant ThingieListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    const Thingie *thingie = _thingies[index.row()];
    switch (role)
    {
        case NameRole:
            return thingie->name();
        case ColorRole:
            return thingie->color();
        case IdRole:
            return thingie->id();
        case ModelIndexRole:
            if (std::find(_thingies.begin(), _thingies.end(), thingie) != _thingies.end()) 
                return std::distance(_thingies.begin(), std::find(_thingies.begin(), _thingies.end(), thingie));
            else return -1;
        default:
            return QVariant();
    }
}

int ThingieListModel::rowCount(const QModelIndex &) const { return _thingies.count(); }


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