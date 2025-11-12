// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef QUARANTINECHECKER_H
#define QUARANTINECHECKER_H

#include <QCoreApplication>

class QuarantineChecker
{
public:
    // Static method to check quarantine and quit if needed
    static void checkAndQuitIfQuarantined(QWidget *parent = nullptr);
};

#endif // QUARANTINECHECKER_H
