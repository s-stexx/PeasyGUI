#ifndef MOCKCONTROL_H
#define MOCKCONTROL_H

#include "../src/control.h"
#include <QMap>
#include <QString>

class MockControl : public Control {
public:

    void setProp(const QString& prop, const QString& value) override {
        propsDict[prop] = value;
    }

    QString getProp(const QString& prop) override {
        return propsDict[prop];
    }

    void setSpecificProp(const QString& prop, const QString& value) override {};
    QString getSpecificProp(const QString& prop) override {return "";};

private:
    QMap<QString, QString> propsDict;
};



#endif // MOCKCONTROL_H
