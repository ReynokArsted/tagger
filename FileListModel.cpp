#include "FileListModel.h"
#include <QDir>

FileListModel::FileListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int FileListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant FileListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) return {};

    const auto &it = m_items[index.row()];
    switch (role) {
    case NameRole: return it.name;
    case PathRole: return it.path;
    case IsDirRole: return it.isDir;
    default: return {};
    }
}

QHash<int, QByteArray> FileListModel::roleNames() const {
    return {
        { NameRole, "name" },
        { PathRole, "path" },
        { IsDirRole, "isDir" }
    };
}

void FileListModel::setFolder(const QString& folderPath) {
    QDir dir(folderPath);

    if (!dir.exists())
        return;

    QFileInfo info(folderPath); // ????
    if (!info.exists() || !info.isDir())
        return;

    beginResetModel();
    m_items.clear();

    m_folder = dir.absolutePath();

    QFileInfoList infos = dir.entryInfoList(
        QDir::NoDotAndDotDot | QDir::AllEntries,
        QDir::DirsFirst | QDir::Name
    );

    for (const QFileInfo& fi : infos) {
        Item it;
        it.name = fi.fileName();
        it.path = fi.absoluteFilePath();
        it.isDir = fi.isDir();
        m_items.push_back(std::move(it));
    }

    endResetModel();
    emit folderChanged();
}

QString FileListModel::parentFolder() const {
    QDir dir(m_folder);
    if (dir.cdUp())
        return dir.absolutePath();
    return {};
}