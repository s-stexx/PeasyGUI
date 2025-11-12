// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef DROPDOWN_H
#define DROPDOWN_H

#include <QComboBox>
#include "control.h"

class DropDown : public QComboBox, public Control
{
    Q_OBJECT

public:
    explicit DropDown(QWidget *parent = nullptr);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void dropDownEvent(const QString &name, const QString &event);

private slots:
    void processSelectionChange();

};

#endif // DROPDOWN_H
