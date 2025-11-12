// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

enum class AppMode {
    Designer,
    TCP,
    FileBased
};

struct AppConfig {
    AppMode mode = AppMode::Designer;
    QString iniPath;
    QString exchangeDir;
    QString ip;
    quint16 port;
};

#endif // APPCONFIG_H
