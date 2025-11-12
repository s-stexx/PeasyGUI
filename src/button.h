// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include "control.h"

class Button : public QPushButton, public Control
{
    Q_OBJECT

public:
    explicit Button(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void buttonEvent(const QString &name, const QString &event);

private slots:
    void processClick();

};

#endif // BUTTON_H
