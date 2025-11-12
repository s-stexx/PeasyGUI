// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef APPPATHS_H
#define APPPATHS_H

#include <QString>

class AppPaths {
public:
    static QString appRootDir();
    static QString dirOfExecutable();
    static QString cleanAbsPath(const QString &pathOrRelPath);
    static QString cleanAbsDir(const QString &DirOrRelDir);
};

#endif // APPPATHS_H

