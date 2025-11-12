// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
#include "control.h"

class CheckBox : public QCheckBox, public Control
{
    Q_OBJECT

public:
    explicit CheckBox(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void checkBoxEvent(const QString &name, const QString &event);

private slots:
    void processClick();

};

#endif // CHECKBOX_H
