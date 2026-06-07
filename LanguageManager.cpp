#include "LanguageManager.h"
#include <QCoreApplication>
#include <QDebug>

LanguageManager::LanguageManager(QQmlEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine), m_currentLang("en"){}

void LanguageManager::toggleLanguage()
{
    QCoreApplication::removeTranslator(&m_translator);

    if (m_currentLang == "en") {
        if (m_translator.load(":/i18n/ru_RU.qm")) {
            QCoreApplication::installTranslator(&m_translator);
            m_currentLang = "ru";
            qDebug() << "Language changed to Russian";
        } else {
            qWarning() << "Failed to load Russian translation file!";
        }
    } else {
        m_currentLang = "en";
        qDebug() << "Language changed to English (Default)";
    }

    m_engine->retranslate();
}