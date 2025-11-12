// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <QObject>
#include <QCoreApplication>

#include "appconfig.h"

class Initializer : public QObject
{
    Q_OBJECT

public:
    Initializer();

    void parseCmdParamsToConfig(QCoreApplication &app, AppConfig &config);

    void readIniFile();

signals:
    void iniFileRead(const QString& content, bool replyExpected);

public slots:
    void writeToIniFile(const QString& content);

private:
    QString iniPath;
};

#endif // INITIALIZER_H
