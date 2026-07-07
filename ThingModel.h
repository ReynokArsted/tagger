#ifndef THINGMODEL_H
#define THINGMODEL_H

#include <QObject>
#include <QQmlEngine>
#include <ThingieListModel.h>

class Thingie;
class ThingModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(ThingieListModel* listOfThingies READ listOfThingies CONSTANT)
public:
    ThingModel(QObject* parent = nullptr);
    Q_INVOKABLE QString printModel() { return _listOfThingies.print(); }
    ThingieListModel* listOfThingies() { return &_listOfThingies; }

private:
    ThingieListModel _listOfThingies;
};
#endif // THINGMODEL_H