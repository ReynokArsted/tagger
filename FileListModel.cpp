#include <iostream>
#include <string>
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

QSet<QString> get_tag_set (QString tg)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");

    if (!db.open()) {
        qWarning() << "DB open failed:" << db.lastError().text();
        return QSet<QString>();
    }

    QSqlQuery query(db);
    if (tg == "##")
        query.prepare("SELECT file.path FROM tag_file JOIN file ON tag_file.file_id = file.id GROUP BY file.path");
    else 
    {
        query.prepare("SELECT file.path FROM tag_file JOIN file ON tag_file.file_id = file.id JOIN tag ON tag_file.tag_id = tag.id WHERE tag_name = :tag");
        query.bindValue(":tag", tg.remove(0, 1));
    }

    if (!query.exec()) 
    {
        qWarning() << "SELECT tag failed:" << query.lastError().text();
        return QSet<QString>();
    }

    QSet<QString> tag_set;
    while (query.next())
        tag_set.insert(query.value(0).toString());

    qDebug() << "tag_set:" << tag_set;
    return tag_set;
}

QSet<QString> subtract_op (QString tg1, QString tg2)
{       
    QSet<QString> set1 = get_tag_set(tg1);
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.subtract(set2);
    return result;
}

QSet<QString> subtract_op (QSet<QString> set1, QString tg2)
{
    qDebug() << "tag_set:" << set1;
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.subtract(set2);
    return result;
}

QSet<QString> subtract_op (QString tg1, QSet<QString> set2)
{
    QSet<QString> set1 = get_tag_set(tg1);
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.subtract(set2);
    return result;
}

QSet<QString> subtract_op (QSet<QString> set1, QSet<QString> set2)
{
    qDebug() << "tag_set:" << set1;
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.subtract(set2);
    return result;
}

QSet<QString> unite_op (QString tg1, QString tg2)
{
    QSet<QString> set1 = get_tag_set(tg1);
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.unite(set2);
    return result;
}

QSet<QString> unite_op (QSet<QString> set1, QString tg2)
{
    qDebug() << "tag_set:" << set1;
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.unite(set2);
    return result;
}

QSet<QString> unite_op (QString tg1, QSet<QString> set2)
{
    QSet<QString> set1 = get_tag_set(tg1);
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.unite(set2);
    return result;
}

QSet<QString> unite_op (QSet<QString> set1, QSet<QString> set2)
{
    qDebug() << "tag_set:" << set1;
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.unite(set2);
    return result;
}

QSet<QString> intersect_op (QString tg1, QString tg2)
{
    QSet<QString> set1 = get_tag_set(tg1);
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.intersect(set2);  
    return result;
}

QSet<QString> intersect_op (QSet<QString> set1, QString tg2)
{
    qDebug() << "tag_set:" << set1;
    QSet<QString> set2 = get_tag_set(tg2);
    QSet<QString> result = set1.intersect(set2);
    return result;
}

QSet<QString> intersect_op (QString tg1, QSet<QString> set2)
{
    QSet<QString> set1 = get_tag_set(tg1);
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.intersect(set2);
    return result;
}

QSet<QString> intersect_op (QSet<QString> set1, QSet<QString> set2)
{
    qDebug() << "tag_set:" << set1;
    qDebug() << "tag_set:" << set2;
    QSet<QString> result = set1.intersect(set2);
    return result;
}

FileListModel::FileListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int FileListModel::rowCount(const QModelIndex& parent) const 
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant FileListModel::data(const QModelIndex& index, int role) const 
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) return {};

    const auto &it = m_items[index.row()];
    switch (role) {
        case NameRole: return it.name;
        case PathRole: return it.path;
        case IsDirRole: return it.isDir;
        default: return {};
    }
}

QHash<int, QByteArray> FileListModel::roleNames() const 
{
    return 
    {
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
        qWarning() << "ERROR: ShellExecuteExW failed. error code =" << err;
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

    if (!ok) 
    {
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

void FileListModel::setFolder(const QString &folderPath) 
{
    QString temp = folderPath;
    temp = temp.simplified();

    QString ref = "+-\\*()";
    int operation_count = 0;

    // check brace closing
    int brace_pair_count = 0;
    QVector<int> brace_balances;
    int current_brace_index;
    for (int i = 0; i < temp.length(); i++)
    {
        if (ref.contains(temp[i])) 
        {
            if (temp[i] != '(' && temp[i] != ')') operation_count++;
            if (temp[i] == '(') 
            {
                brace_balances.push_back(1);
                current_brace_index = brace_balances.length() - 1;
            }
            
            if (temp[i] == ')') 
            {
                brace_balances[current_brace_index]--;
                brace_pair_count++;

                if (current_brace_index > 0) current_brace_index--;

                while (brace_balances[current_brace_index] == 0 && current_brace_index > 0)
                    current_brace_index--;
            }

            if (i > 0 && temp[i - 1] != ' ') 
            {
                temp.insert(i, ' ');
                i++;
            }
            else if (i + 1 < temp.length() && temp[i + 1] != ' ') 
                temp.insert(i + 1, ' ');
        }
    }

    bool brace_balance_was_lost = false;
    for (int i = 0; i < brace_balances.length(); i++)
        if (brace_balances[i] != 0) 
        {
            brace_balance_was_lost = true;
            current_brace_index = i;
        }

    if (brace_balance_was_lost) 
    {
        qDebug() << "ERROR: the " + QString::number(current_brace_index + 1) + "th brace wasn't closed";
        beginResetModel();
        m_items.clear();
        endResetModel();
        return;
    }

    QStringList temp_list = temp.split(' ');
    qDebug() << "temp_list =" << temp_list;

    // check operands count 
    bool other_operand_wasnt_found = false;
    for (int i = 0; i < temp_list.length(); i++) {
        if (ref.contains(temp_list[i]) && temp_list[i] != "(" && temp_list[i] != ")") {
            if (temp_list[i] == "\\" && ref.contains(temp_list[i+1]))
                other_operand_wasnt_found = true;
            else if (temp_list[i] != "\\" && 
                (i == temp_list.length() - 1 || i == 0 || 
                (ref.contains(temp_list[i-1]) && !(QString("()").contains(temp_list[i-1]))) || 
                (ref.contains(temp_list[i+1]) && !(QString("()").contains(temp_list[i+1]))))
            )
                other_operand_wasnt_found = true;
        }
    }
    if (other_operand_wasnt_found)
    {
        qDebug() << "ERROR: one operand wasn't found";
        beginResetModel();
        m_items.clear();
        endResetModel();
        return;
    }

    qDebug() << "res:" << temp_list;
    qDebug() << "operation_count:" << operation_count;
    qDebug() << "brace_pair_count:" << brace_pair_count;

    // delete superfluous braces
    for (int i = 1; i < temp_list.length() - 1; i++)
    {
        if (temp_list[i].contains("#")) // #cat + (#iron + (#cat) ()) \ #chest ()
        {
            int k = 1;
            bool braces_is_here = true;
            while (braces_is_here && (i - k) >= 0 && (i + k) < temp_list.length())
            {
                if (temp_list[i - k] == "(" && temp_list[i + k] == ")")
                {
                    temp_list[i + k] = " ";
                    temp_list[i - k] = " ";
                }
                else braces_is_here = false;
                k++;
            }
        }
        else if (i < (temp_list.length() - 1) && temp_list[i] == "(" && temp_list[i + 1] == ")")
        {
            temp_list[i] = " ";
            temp_list[i + 1] = " ";
        }
    }
    temp_list.removeAll(" ");
    qDebug() << "brace removing result:" << temp_list;

    int last_tag_index = 0;
    bool path_indicator = false;
    bool error_was_found = false;
    for (int i = 0; i < temp_list.length(); i++)
    {
        if (check_tag(temp_list[i]) || ref.contains(temp_list[i])) 
        {
            //qDebug() << temp_list[i] << "is a tag or an operation";
            if (path_indicator) error_was_found = true;
            if (temp_list[i] == "\\" && (i == 0 || temp_list[i - 1] == "(")) 
            {
                temp_list.insert(i == 0? 0 : i - 1, "##");
                i++;
            }
            last_tag_index = i;
        }
        else path_indicator = true;
    }
    if (error_was_found)
    {
        qDebug() << "ERROR: seq has paths between tags";
        beginResetModel();
        m_items.clear();
        endResetModel();
        return;
    }
    qDebug() << "last_tag_index =" << last_tag_index;

    QVector<QVector<int>> operations_list;
    bool in_brace = false; 
    operations_list.emplace_back();
    int current_brace_pair = 0;
    //for (int i = 0; i < last_tag_index; i++)
    for (int i = 0; i < temp_list.length(); i++)
    {
        if (ref.contains(temp_list[i]))
        {
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
            else operations_list[current_brace_pair].push_back(i);
        }
    }
    qDebug() << "operations_list:" << operations_list;

    QVector<QSet<QString>> results;
    int i = operations_list.length() - 1, j = -1;
    while (i >= 0)
    {
        j = operations_list[i].length() - 1;
        while (j >= 0)
        {
            qDebug() << "operation:" << temp_list[operations_list[i][j]];
            if (temp_list[operations_list[i][j]] == "+")
            {   
                int k = 1, t = 1; 
                while (temp_list[operations_list[i][j] - k] == "") k++;
                while (temp_list[operations_list[i][j] + t] == "") t++;
                qDebug() << "op1 =" << temp_list[operations_list[i][j] - k];
                qDebug() << "op2 =" << temp_list[operations_list[i][j] + t];

                QSet<QString> result;
                if (!temp_list[operations_list[i][j] - k].contains("#") && 
                    !temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index1 = temp_list[operations_list[i][j] - k].toInt();
                    int index2 = temp_list[operations_list[i][j] + t].toInt();
                    result = unite_op(results[index1], results[index2]);
                }    
                else if (!temp_list[operations_list[i][j] - k].contains("#"))
                {
                    int index = temp_list[operations_list[i][j] - k].toInt();
                    result = unite_op(results[index], temp_list[operations_list[i][j] + t]);
                }
                else if (!temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index = temp_list[operations_list[i][j] + t].toInt();
                    result = unite_op(temp_list[operations_list[i][j] - k], results[index]);
                }    
                else
                {
                    result = unite_op(
                        temp_list[operations_list[i][j] - k], 
                        temp_list[operations_list[i][j] + t]);
                }
                qDebug() << "result:" << result;

                QString index = QString::number(results.size());
                results.push_back(result);

                temp_list.replace(operations_list[i][j], index);
                temp_list.replace(operations_list[i][j] - k, "");
                temp_list.replace(operations_list[i][j] + t, "");
                int m = 1;
                bool braces_is_here = true;
                while(braces_is_here && operations_list[i][j] - (k + m) >= 0 && 
                    operations_list[i][j] + (t + m) <= (temp_list.length() - 1))
                {
                    if (temp_list[operations_list[i][j] - (k + m)] == "(" && 
                        temp_list[operations_list[i][j] + (t + m)] == ")")
                    {
                        temp_list.replace(operations_list[i][j] - (k + m), "");
                        temp_list.replace(operations_list[i][j] + (t + m), "");
                    }
                    else braces_is_here = false;
                    m++;
                }
            }
            else if (temp_list[operations_list[i][j]] == "*")
            {
                int k = 1, t = 1; 
                while (temp_list[operations_list[i][j] - k] == "") k++;
                while (temp_list[operations_list[i][j] + t] == "") t++;
                qDebug() << "op1 =" << temp_list[operations_list[i][j] - k];
                qDebug() << "op2 =" << temp_list[operations_list[i][j] + t];

                QSet<QString> result;
                if (!temp_list[operations_list[i][j] - k].contains("#") && 
                    !temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index1 = temp_list[operations_list[i][j] - k].toInt();
                    int index2 = temp_list[operations_list[i][j] + t].toInt();
                    result = intersect_op(results[index1], results[index2]);
                }    
                else if (!temp_list[operations_list[i][j] - k].contains("#"))
                {
                    int index = temp_list[operations_list[i][j] - k].toInt();
                    result = intersect_op(results[index], temp_list[operations_list[i][j] + t]);
                }
                else if (!temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index = temp_list[operations_list[i][j] + t].toInt();
                    result = intersect_op(temp_list[operations_list[i][j] - k], results[index]);
                }    
                else
                {
                    result = intersect_op(
                        temp_list[operations_list[i][j] - k], 
                        temp_list[operations_list[i][j] + t]);
                }
                qDebug() << "result:" << result;

                QString index = QString::number(results.size());
                results.push_back(result);

                temp_list.replace(operations_list[i][j], index);
                temp_list.replace(operations_list[i][j] - k, "");
                temp_list.replace(operations_list[i][j] + t, "");
                int m = 1;
                bool braces_is_here = true;
                while(braces_is_here && operations_list[i][j] - (k + m) >= 0 && 
                    operations_list[i][j] + (t + m) <= (temp_list.length() - 1))
                {
                    if (temp_list[operations_list[i][j] - (k + m)] == "(" && 
                        temp_list[operations_list[i][j] + (t + m)] == ")")
                    {
                        temp_list.replace(operations_list[i][j] - (k + m), "");
                        temp_list.replace(operations_list[i][j] + (t + m), "");
                    }
                    else braces_is_here = false;
                    m++;
                }
            }
            else if (temp_list[operations_list[i][j]] == "\\")
            {
                int k = 1, t = 1; 
                while (temp_list[operations_list[i][j] - k] == "") k++;
                while (temp_list[operations_list[i][j] + t] == "") t++;
                qDebug() << "op1 =" << temp_list[operations_list[i][j] - k];
                qDebug() << "op2 =" << temp_list[operations_list[i][j] + t];

                QSet<QString> result;
                if (!temp_list[operations_list[i][j] - k].contains("#") && 
                    !temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index1 = temp_list[operations_list[i][j] - k].toInt();
                    int index2 = temp_list[operations_list[i][j] + t].toInt();
                    result = subtract_op(results[index1], results[index2]);
                }    
                else if (!temp_list[operations_list[i][j] - k].contains("#"))
                {                  
                    int index = temp_list[operations_list[i][j] - k].toInt();
                    result = subtract_op(results[index], temp_list[operations_list[i][j] + t]);
                }
                else if (!temp_list[operations_list[i][j] + t].contains("#"))
                {
                    int index = temp_list[operations_list[i][j] + t].toInt();
                    result = subtract_op(temp_list[operations_list[i][j] - k], results[index]);
                }    
                else
                {
                    result = subtract_op
                    (
                        temp_list[operations_list[i][j] - k], 
                        temp_list[operations_list[i][j] + t]
                    );
                }
                qDebug() << "result:" << result;
                results.push_back(result);

                QString index = QString::number(results.size() - 1);
                temp_list.replace(operations_list[i][j], index);
                temp_list.replace(operations_list[i][j] - k, "");
                temp_list.replace(operations_list[i][j] + t, "");

                int m = 1;
                bool braces_is_here = true;
                while(braces_is_here && operations_list[i][j] - (k + m) >= 0 && 
                    operations_list[i][j] + (t + m) <= (temp_list.length() - 1))
                {
                    if (temp_list[operations_list[i][j] - (k + m)] == "(" && 
                        temp_list[operations_list[i][j] + (t + m)] == ")")
                    {
                        temp_list.replace(operations_list[i][j] - (k + m), "");
                        temp_list.replace(operations_list[i][j] + (t + m), "");
                    }
                    else braces_is_here = false;
                    m++;
                }
            }
            qDebug() << "temp_list =" << temp_list;
            j--;
        }
        i--;
    }

    if (folderPath.contains("#"))
    {
        QString temp = folderPath;
        temp.replace("#", "");

        beginResetModel();
        m_items.clear();

        if (results.isEmpty())
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("test.db");

            if (!db.open()) 
            {
                qWarning() << "DB open failed:" << db.lastError().text();
                return;
            }

            QSqlQuery query(db);
            query.prepare("SELECT file.path FROM tag JOIN tag_file ON tag_file.tag_id = tag.id JOIN file ON file.id = tag_file.file_id WHERE tag.tag_name = :tag");
            query.bindValue(":tag", temp);
            if (!query.exec()) 
            {
                qWarning() << "SELECT tag failed:" << query.lastError().text();
                return;
            }

            while (query.next()) 
            {
                QString q_path = query.value(0).toString();
                QFileInfo info(q_path);
                Item query_file;
                query_file.path = q_path;
                query_file.name = info.fileName();
                query_file.path = info.absoluteFilePath();
                query_file.isDir = info.isDir();
                m_items.push_back(std::move(query_file));
            }
        }
        else
        {
            int i = 0;
            while (i < results.last().size())
            {
                QFileInfo info(results.last().values().at(i));
                Item query_file;
                query_file.path = results.last().values().at(i);
                query_file.name = info.fileName();
                query_file.path = info.absoluteFilePath();
                query_file.isDir = info.isDir();
                m_items.push_back(std::move(query_file));
                i++;
            }
        }
        endResetModel();
    }
    else 
    {
        QDir dir(folderPath);
        if (!dir.exists()) return;

        beginResetModel();
        m_items.clear();
        m_folder = dir.absolutePath();

        QFileInfoList infos = dir.entryInfoList
        (
            QDir::NoDotAndDotDot | QDir::AllEntries,
            QDir::DirsFirst | QDir::Name
        );

        for (const QFileInfo& fi : infos) 
        {
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

QString FileListModel::parentFolder() const 
{
    QDir dir(m_folder);
    if (dir.cdUp()) return dir.absolutePath();
    return {};
}