// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef CONTROL_H
#define CONTROL_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include "controlprototypes.h"

class Control
{

public:
    explicit Control(QObject* parent = nullptr);
    virtual ~Control() = default;

    virtual QString getProp(const QString& prop);
    virtual void setProp(const QString& prop, const QString& value);

    int getId() const;
    void setId(int newId);


    void setControlMightGetInactive(bool newControlMightGetInactive);

    int applyDefaultProperties(int typeCount, int yPos);
    QList<PropInfo> getAllPropInfos();

protected:
    int id;
    QMap<QString, QString> propsDict;

private:
    QObject* dynParent;

    bool controlMightGetInactive = true;

    virtual void setSpecificProp(const QString& prop, const QString& value) = 0;
    virtual QString getSpecificProp(const QString& prop) = 0;

};

#endif // CONTROL_H
