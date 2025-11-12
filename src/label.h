// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include "control.h"


class Label : public QLabel, public Control
{
    Q_OBJECT

public:
    explicit Label(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

};

#endif // LABEL_H
