// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef PATHDIALOGBUTTON_H
#define PATHDIALOGBUTTON_H

#include <QPushButton>
#include "control.h"

class PathDialogButton : public QPushButton, public Control
{
    Q_OBJECT

public:
    explicit PathDialogButton(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void pathDialogButtonEvent(const QString &name, const QString &event);

private slots:
    void processClick();

};

#endif // PATHDIALOGBUTTON_H
