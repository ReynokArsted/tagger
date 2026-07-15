#pragma once
#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QQuickWindow>

class FileListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        IsDirRole
    };
    Q_ENUM(Roles)

    explicit FileListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    QString folder() const { return m_folder; }

    Q_INVOKABLE QString parentFolder() const;
    Q_INVOKABLE void setFolder(const QString &folderPath);
    Q_INVOKABLE void setHomeFolder();
    Q_INVOKABLE void openWith(const QString &filePath, QQuickWindow *window = nullptr);
    Q_INVOKABLE void openFile(const QString &filePath, QQuickWindow *window = nullptr);

    Q_PROPERTY(bool hasFolder READ hasFolder NOTIFY folderChanged)

    bool hasFolder() const { return !m_folder.isEmpty(); }

private:
    struct Item {
        QString name;
        QString path;
        bool isDir = false;
    };

    QVector<Item> m_items;
    QString m_folder;

signals:
    void folderChanged();
};
