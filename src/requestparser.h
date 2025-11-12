// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <QObject>
#include "request.h"


class RequestParser : public QObject
{
    Q_OBJECT
public:
    QList<Request> parseAll(const QString &input);

private:
    Request parseSingle(const QStringList &tokens, int &index);

};

#endif // REQUESTPARSER_H
