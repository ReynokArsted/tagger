#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QStringList>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
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

bool check_tag(QString tag)
{
    if (tag.length() <= 1) return false;
    if (!tag.startsWith('#') || tag.count('#') >= 2) return false;

    return true;
}

void FileListModel::setFolder(const QString &folderPath) {
    QString temp = folderPath;
    temp = temp.simplified();

    int operation_count = 0;
    int brace_pair_count = 0;
    QString ref = "+-\\*()";
    bool brace_is_open = false; 
    for (int i = temp.length() - 1; i >= 0; --i) {
        if (ref.contains(temp[i])) {
            operation_count++;
            if (temp[i] == ')') brace_is_open = true;
            if (temp[i] == '(') 
            {
                if (brace_is_open) 
                {
                    brace_is_open = false;
                    brace_pair_count++;
                }
                else brace_is_open = true;
            }

            if (i > 0 && temp[i - 1] != ' ') 
            {
                temp.insert(i, ' ');
                i++;
            }
            if (i + 1 < temp.length() && temp[i + 1] != ' ') temp.insert(i + 1, ' ');
        }
    }
    if (brace_is_open) qDebug() << "ERROR: one brace wasn't closed";

    QStringList temp_list = temp.split(' ');
    qDebug() << "res:" << temp_list;
    qDebug() << "operation_count:" << operation_count;
    qDebug() << "brace_pair_count:" << brace_pair_count;

    int last_tag_index = 0;
    bool path_indicator = false;
    for (int i = 0; i < temp_list.length(); i++)
    {
        if (check_tag(temp_list[i]) || ref.contains(temp_list[i])) {
            qDebug() << temp_list[i] << "is a tag or an operation";
            if (path_indicator) qDebug() << "ERROR: seq has paths between tags";
            last_tag_index = i;
        }
        else path_indicator = true;
    }
    qDebug() << "last_tag_index =" << last_tag_index;

    //QVector<int> operand_list;
    QVector<QVector<int>> operations_list;
    bool in_brace = false; 
    operations_list.emplace_back();
    int current_brace_pair = 0;
    //for (int i = 0; i < last_tag_index; i++)
    for (int i = 0; i < temp_list.length(); i++)
    {
        if (ref.contains(temp_list[i]))
        {
            QVector<int> temp;
            if (temp_list[i] == "(") 
            {
                in_brace = true;
                operations_list.emplace_back();
                current_brace_pair++;
            }
            else if (temp_list[i] == ")") 
            {
                if (in_brace) 
                {
                    if (current_brace_pair - 1 == 0) in_brace = false;
                    current_brace_pair--;
                }
                else in_brace = true;
            }
            else
            {
                operations_list[current_brace_pair].push_back(i);
            }
        }
    }
    qDebug() << "operations_list:" << operations_list;

    if (folderPath.contains("#"))
    {
        QString temp = folderPath;
        temp.replace("#", "");

        beginResetModel();
        m_items.clear();

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("test.db");

        if (!db.open()) {
            qWarning() << "DB open failed: " << db.lastError().text();
            return;
        }

        QSqlQuery query(db);
        query.prepare("SELECT file.path FROM tag JOIN tag_file ON tag_file.tag_id = tag.id JOIN file ON file.id = tag_file.file_id WHERE tag.tag_name = :tag");
        query.bindValue(":tag", temp);
        if (!query.exec()) {
            qWarning() << "SELECT tag failed: " << query.lastError().text();
            return;
        }

        while (query.next()) {
            QString q_path = query.value(0).toString();
            QFileInfo info(q_path);

            Item query_file;
            query_file.path = q_path;
            query_file.name = info.fileName();
            query_file.path = info.absoluteFilePath();
            query_file.isDir = info.isDir();
            m_items.push_back(std::move(query_file));
        }
        endResetModel();
    }
    else {
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
    }
    emit folderChanged();
}

QString FileListModel::parentFolder() const {
    QDir dir(m_folder);
    if (dir.cdUp()) return dir.absolutePath();
    return {};
}