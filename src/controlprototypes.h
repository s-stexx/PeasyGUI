// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef CONTROLPROTOTYPES_H
#define CONTROLPROTOTYPES_H

#include <QString>
#include <QList>
#include <QMap>

struct PropInfo {
    QString propName;
    QString defaultValue;
    QString flagsSetWrite;
    QString checkRule;
    QString description;
};

using Prototypes = QMap<QString, QList<PropInfo>>;
const Prototypes& getPrototypes();

inline const QString& extensionDefault()
{
    static const QString val = "CSV files (*.csv);;Excel files (*.xlsx *.xls);;All files (*)";
    return val;
}

inline const QString& dropStyleDefault()
{
    static const QString val = " border-radius: 10px; border: 1px inset gray; qproperty-alignment: AlignCenter;";
    return val;
}

#endif // CONTROLPROTOTYPES_H
