/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */

#include "ThingModel.h"
#include "Thingie.h"

ThingModel::ThingModel(QObject* parent) : QObject(parent)
{    
    std::vector<Thingie*> tmpV;
    tmpV.push_back(new Thingie("Coffee Bean", this));
    tmpV.push_back(new Thingie("Small Cup", this));
    tmpV.push_back(new Thingie("Remy van Elst", this));
    tmpV.push_back(new Thingie("Fire information", this));
    tmpV.push_back(new Thingie("New Products", this));
    tmpV.push_back(new Thingie("New Videos", this));
    tmpV.push_back(new Thingie("Corona Info", this));
    _listOfThingies.updateFromVector(tmpV);
}