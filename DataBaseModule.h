#pragma once

#include <QObject>
#include <QSqlDatabase>

class DataBaseModule : public QObject
{
    Q_OBJECT

public:
    explicit DataBaseModule(QObject *parent = nullptr);

    bool open();
    QSqlDatabase database() const;

private:
    QString m_connectionName = "app_connection";
};
