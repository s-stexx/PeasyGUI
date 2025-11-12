// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "checkbox.h"


CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent), Control(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(this, &QCheckBox::checkStateChanged, this, &CheckBox::processClick);
}


void CheckBox::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        this->setText(value);

    } else if (prop == "State") {
        if (value == "Checked") {
            this->setCheckState(Qt::Checked);
        } else {
            this->setCheckState(Qt::Unchecked);
        }

    }
}


QString CheckBox::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = this->text();

    } else if (prop == "State") {
        if (this->checkState() == Qt::Checked) {
            value = "Checked";
        } else {
            value = "Unchecked";
        }

    }

    return value;
}


void CheckBox::processClick()
{
    if (propsDict["Events"].contains("StateChanged")) {
        QString state;

        if (this->checkState() == Qt::Checked) {
            state = "Checked";
        } else {
            state = "Unchecked";
        }

        emit checkBoxEvent(propsDict["Name"], "StateChanged`" + state);
    }
}
