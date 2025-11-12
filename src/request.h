// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef REQUEST_H
#define REQUEST_H

#include <QString>

struct Request {
    QString action;
    QString name;
    QString prop;
    QString value;
    QString error;
};

#endif // REQUEST_H
