#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QStringList>
#include <QRegularExpression>
#include <Windows.h>

#include "FileListModel.h"

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

void FileListModel::openWith(const QString &filePath, QQuickWindow *window)
{
    QString localPath = filePath;
    QUrl url(filePath);
    if (url.isLocalFile()) localPath = url.toLocalFile();

    localPath = QDir::toNativeSeparators(localPath);
    QFileInfo fi(localPath);
    if (!fi.exists()) return;

    HWND hwndParent = nullptr;
    if (window) hwndParent = reinterpret_cast<HWND>(window->winId());

    std::wstring wpath = QDir::toNativeSeparators(fi.absoluteFilePath()).toStdWString();

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
    sei.hwnd = hwndParent;
    sei.lpVerb = L"openas";
    sei.lpFile = wpath.c_str();
    sei.nShow = SW_SHOWNORMAL;

    BOOL ok = ShellExecuteExW(&sei);
    if (!ok) {
        DWORD err = GetLastError();
        qWarning() << "ERROR: ShellExecuteExW failed. error code = " << err;
    }
}

void FileListModel::openFile(const QString &filePath, QQuickWindow *window)
{
    QString localPath = filePath;
    QUrl url(filePath);
    if (url.isLocalFile()) localPath = url.toLocalFile();

    localPath = QDir::toNativeSeparators(localPath);
    QFileInfo fi(localPath);
    if (!fi.exists()) return;

    std::wstring wpath = QDir::toNativeSeparators(fi.absoluteFilePath()).toStdWString();

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = wpath.c_str();
    sei.nShow = SW_SHOWNORMAL;

    BOOL ok = ShellExecuteExW(&sei);

    if (!ok) {
        DWORD err = GetLastError();
        if (err == 1155)
        {
            HWND hwndParent = nullptr;
            if (window) hwndParent = reinterpret_cast<HWND>(window->winId());

            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
            sei.hwnd = hwndParent;
            sei.lpVerb = L"openas";
            sei.lpFile = wpath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        else qWarning() << "ERROR: ShellExecuteExW failed. error code = " << err;
    }
}

void FileListModel::setHomeFolder() { setFolder(""); }

void FileListModel::setFolder(const QString &folderPath) {
    QDir dir(folderPath);

    if (!dir.exists()) return;

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
    if (dir.cdUp()) return dir.absolutePath();
    return {};
}