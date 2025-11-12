// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "quarantinechecker.h"

#include <QString>
#include <QMessageBox>

#ifdef Q_OS_MAC
#include <sys/xattr.h>
#include <errno.h>
#include <string.h>
#endif


void QuarantineChecker::checkAndQuitIfQuarantined(QWidget *parent)
{
#ifdef Q_OS_MAC
    QString exePath = QCoreApplication::applicationFilePath();
    QByteArray path = exePath.toUtf8();

    char buf[256];
    ssize_t size = getxattr(path.constData(), "com.apple.quarantine", buf, sizeof(buf), 0, 0);

    if (size == -1) {
        if (errno == ENOATTR) {
            // Not quarantined, do nothing
            return;
        } else {
            // Unexpected error
            QString msg = QString("Quarantine check failed. errno=%1 (%2)")
                              .arg(errno)
                              .arg(strerror(errno));
            QMessageBox::warning(parent, "Quarantine Check", msg);
            QCoreApplication::quit();
        }
    } else {
        // Quarantine attribute present
        QByteArray data(buf, size);
        QString msg = QString("This app is quarantined:\n%1\n\n"
                              "Please run Unlock.command before starting the app.")
                          .arg(QString::fromUtf8(data));
        QMessageBox::warning(parent, "Quarantine Check", msg);
        QCoreApplication::quit();
    }
#else
    Q_UNUSED(parent);
#endif
}
