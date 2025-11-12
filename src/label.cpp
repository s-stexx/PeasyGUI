// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "label.h"

Label::Label(QWidget *parent) : QLabel(parent), Control(parent)
{

}


void Label::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        this->setText(value);
    }
}


QString Label::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = this->text();
    }

    return value;
}
