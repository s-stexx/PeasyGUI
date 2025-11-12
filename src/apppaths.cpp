// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "apppaths.h"

#include <QCoreApplication>
#include <QDir>

QString AppPaths::appRootDir()
{
    QDir dir(QCoreApplication::applicationDirPath());

    dir.cdUp();

#if defined(Q_OS_MACOS)
    dir.cdUp();
    dir.cdUp();
#endif

    return QDir::cleanPath(dir.path());
}


QString AppPaths::dirOfExecutable()
{
    QDir dir(QCoreApplication::applicationDirPath());

    return QDir::cleanPath(dir.path());
}


QString AppPaths::cleanAbsPath(const QString &pathOrRelPath)
{
    if (pathOrRelPath.trimmed().isEmpty()) {
        return QString();
    }

    QString normalized = QDir::fromNativeSeparators(pathOrRelPath);
    QFileInfo info(normalized);

    if (info.isAbsolute()) {
        return QDir::cleanPath(info.absoluteFilePath());

    } else {
        QDir base(AppPaths::appRootDir());
        QString absPath = base.filePath(normalized);

        return QDir::cleanPath(absPath);
    }
}


QString AppPaths::cleanAbsDir(const QString &pathOrRelDir)
{
    if (pathOrRelDir.trimmed().isEmpty()) {
        return QString();
    }

    QString normalized = QDir::fromNativeSeparators(pathOrRelDir);
    QFileInfo info(normalized);

    QString absDirPath;

    if (info.isAbsolute()) {
        absDirPath = normalized;
    } else {
        QDir base(AppPaths::appRootDir());
        absDirPath = base.filePath(normalized);
    }

    absDirPath = QDir::cleanPath(absDirPath);

    // QFileInfo might point to a file-like path, so ensure it ends as a dir
    if ( ! absDirPath.endsWith('/')) {
        absDirPath += '/';
    }

    return absDirPath;
}
