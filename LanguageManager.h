#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QQmlEngine>

class LanguageManager : public QObject
{
    Q_OBJECT
public:
    explicit LanguageManager(QQmlEngine *engine, QObject *parent = nullptr);
    Q_INVOKABLE void toggleLanguage();

private:
    QQmlEngine *m_engine;
    QTranslator m_translator;
    QString m_currentLang; // "ru"/"en"
};

#endif // LANGUAGEMANAGER_H